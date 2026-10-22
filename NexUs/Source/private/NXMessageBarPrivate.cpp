#include "NXMessageBarPrivate.h"

#include <QDateTime>
#include <QDebug>
#include <QFontMetrics>
#include <QGraphicsOpacityEffect>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QTimer>

#include "NXApplication.h"
#include "NXIconButton.h"
#include "NXMessageBar.h"

QMap<QObject *, QMap<NXMessageBarType::PositionPolicy, QList<NXMessageBar *> *>>
    NXMessageBarPrivate::_messageBarActiveMap;

NXMessageBarPrivate::NXMessageBarPrivate(QObject *parent)
    : QObject { parent }
{
  setProperty("MessageBarClosedY", 0);
  setProperty("MessageBarFinishY", 0);
  _pTimePercent = 100;
  _createSerial = _nextCreateSerial.fetch_add(1) + 1;
}

NXMessageBarPrivate::~NXMessageBarPrivate()
{
  _updateActiveMap(false);
}

void NXMessageBarPrivate::onOtherMessageBarEnd()
{
  Q_Q(NXMessageBar);
  qreal targetPosY                      = _calculateTargetPosY();
  QPropertyAnimation *closePosAnimation = new QPropertyAnimation(this, "MessageBarClosedY");
  connect(closePosAnimation, &QPropertyAnimation::valueChanged, this, [=](const QVariant &value) {
    q->move(q->pos().x(), value.toUInt());
  });
  closePosAnimation->setEasingCurve(QEasingCurve::OutCubic);
  closePosAnimation->setDuration(220);
  closePosAnimation->setStartValue(q->pos().y());
  closePosAnimation->setEndValue(targetPosY);
  closePosAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void NXMessageBarPrivate::messageBarEnd()
{
  if (_isClosing) {
    return;
  }
  _isClosing = true;
  Q_Q(NXMessageBar);
  _closeButton->setEnabled(false);
  _updateActiveMap(false);
  QPropertyAnimation *barFinishedOpacityAnimation = new QPropertyAnimation(this, "pOpacity");
  connect(barFinishedOpacityAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
    _closeButton->setOpacity(_pOpacity);
    q->update();
  });
  connect(barFinishedOpacityAnimation, &QPropertyAnimation::finished, this, [=]() {
    q->deleteLater();
  });
  barFinishedOpacityAnimation->setDuration(300);
  barFinishedOpacityAnimation->setEasingCurve(QEasingCurve::InOutSine);
  barFinishedOpacityAnimation->setStartValue(1);
  barFinishedOpacityAnimation->setEndValue(0);
  barFinishedOpacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  // 通知同类型的其他MessageBar
  auto parentWidget = q->parent();
  if (!_messageBarActiveMap.contains(parentWidget) || !_messageBarActiveMap[parentWidget].contains(_policy)) {
    return;
  }
  auto &messageBarMap = _messageBarActiveMap[parentWidget];
  for (const auto messageBar : *messageBarMap[_policy]) {
    if (messageBar->d_ptr->_isNormalDisplay) {
      messageBar->d_ptr->onOtherMessageBarEnd();
    } else {
      messageBar->d_ptr->_isOtherMessageBarEnd = true;
    }
  }
}

void NXMessageBarPrivate::onThemeChanged(NXThemeType::ThemeMode themeMode)
{
  Q_Q(NXMessageBar);
  _themeMode = themeMode;
  q->update();
}

void NXMessageBarPrivate::_messageBarCreate(int displayMsec)
{
  const int fontPixelSize = nxApp->getFontPixelSize();
  Q_Q(NXMessageBar);
  q->show();
  QFont font = q->font();
  font.setPixelSize(fontPixelSize + 3);
  font.setWeight(QFont::Bold);
  q->setFont(font);
  int titleWidth = q->fontMetrics().horizontalAdvance(_title);
  font.setPixelSize(fontPixelSize + 1);
  font.setWeight(QFont::Medium);
  q->setFont(font);
  int textWidth  = q->fontMetrics().horizontalAdvance(_text);
  int fixedWidth = _closeButtonLeftRightMargin + _leftPadding + _titleLeftSpacing + _textLeftSpacing +
                   _closeButtonWidth + titleWidth + textWidth + 2 * _shadowBorderWidth;
  q->setFixedWidth(fixedWidth > 500 ? 500 : fixedWidth);
  q->setFixedHeight(qMax(50, _getTextHeight()));
  int startX = 0;
  int startY = 0;
  int endX   = 0;
  int endY   = 0;
  _updateActiveMap(true);
  _calculateInitialPos(startX, startY, endX, endY);
  // 滑入动画
  QPropertyAnimation *barPosAnimation = new QPropertyAnimation(q, "pos");
  connect(barPosAnimation, &QPropertyAnimation::finished, q, [=]() {
    _isNormalDisplay = true;
    if (_isOtherMessageBarEnd) {
      onOtherMessageBarEnd();
    }
    QTimer::singleShot(displayMsec, q, [=]() {
      messageBarEnd();
    });
    QPropertyAnimation *timePercentAnimation = new QPropertyAnimation(this, "pTimePercent");
    connect(timePercentAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
      q->update();
    });
    timePercentAnimation->setStartValue(100);
    timePercentAnimation->setEndValue(0);
    timePercentAnimation->setEasingCurve(QEasingCurve::Linear);
    timePercentAnimation->setDuration(displayMsec);
    timePercentAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  });
  switch (_policy) {
  case NXMessageBarType::Top :
  case NXMessageBarType::Bottom : {
    barPosAnimation->setDuration(250);
    break;
  }
  default : {
    barPosAnimation->setDuration(350);
    break;
  }
  }
  barPosAnimation->setStartValue(QPoint(startX, startY));
  barPosAnimation->setEndValue(QPoint(endX, endY));
  barPosAnimation->setEasingCurve(QEasingCurve::InOutSine);
  barPosAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void NXMessageBarPrivate::_calculateInitialPos(int &startX, int &startY, int &endX, int &endY)
{
  Q_Q(NXMessageBar);
  QList<int> resultList  = _getOtherMessageBarTotalData();
  int minimumHeightTotal = resultList[0];
  int indexLessCount     = resultList[1];
  int lastEndY           = endY;
  switch (_policy) {
  case NXMessageBarType::Top : {
    // 25动画距离
    startX = q->parentWidget()->width() / 2 - q->minimumWidth() / 2;
    startY = minimumHeightTotal + _messageBarSpacing * indexLessCount + _messageBarVerticalTopMargin - 25;
    endX   = startX;
    endY   = minimumHeightTotal + _messageBarSpacing * indexLessCount + _messageBarVerticalTopMargin;
    break;
  }
  case NXMessageBarType::Left : {
    startX = -q->minimumWidth();
    startY = minimumHeightTotal + _messageBarSpacing * indexLessCount + q->parentWidget()->height() / 2;
    endX   = _messageBarHorizontalMargin;
    endY   = startY;
    break;
  }
  case NXMessageBarType::Bottom : {
    startX = q->parentWidget()->width() / 2 - q->minimumWidth() / 2;
    startY = q->parentWidget()->height() - q->minimumHeight() - minimumHeightTotal -
             _messageBarSpacing * indexLessCount - _messageBarVerticalBottomMargin - 25;
    endX   = startX;
    endY   = q->parentWidget()->height() - q->minimumHeight() - minimumHeightTotal - _messageBarSpacing * indexLessCount -
             _messageBarVerticalBottomMargin;
    break;
  }
  case NXMessageBarType::Right : {
    startX = q->parentWidget()->width();
    startY = minimumHeightTotal + _messageBarSpacing * indexLessCount + q->parentWidget()->height() / 2;
    endX   = q->parentWidget()->width() - q->minimumWidth() - _messageBarHorizontalMargin;
    endY   = startY;
    break;
  }
  case NXMessageBarType::TopRight : {
    startX = q->parentWidget()->width();
    startY = minimumHeightTotal + _messageBarSpacing * indexLessCount + _messageBarVerticalTopMargin;
    endX   = q->parentWidget()->width() - q->minimumWidth() - _messageBarHorizontalMargin;
    endY   = startY;
    break;
  }
  case NXMessageBarType::TopLeft : {
    startX = -q->minimumWidth();
    startY = minimumHeightTotal + _messageBarSpacing * indexLessCount + _messageBarVerticalTopMargin;
    endX   = _messageBarHorizontalMargin;
    endY   = startY;
    break;
  }
  case NXMessageBarType::BottomRight : {
    startX = q->parentWidget()->width();
    startY = q->parentWidget()->height() - q->minimumHeight() - minimumHeightTotal -
             _messageBarSpacing * indexLessCount - _messageBarVerticalBottomMargin;
    endX   = q->parentWidget()->width() - q->minimumWidth() - _messageBarHorizontalMargin;
    endY   = startY;
    break;
  }
  case NXMessageBarType::BottomLeft : {
    startX = -q->minimumWidth();
    startY = q->parentWidget()->height() - q->minimumHeight() - minimumHeightTotal -
             _messageBarSpacing * indexLessCount - _messageBarVerticalBottomMargin;
    endX   = _messageBarHorizontalMargin;
    endY   = startY;
    break;
  }
  }
  if (endY == lastEndY) {
    return;
  }
  if (endY < _messageBarVerticalTopMargin ||
      endY > q->parentWidget()->height() - _messageBarVerticalBottomMargin - q->minimumHeight()) {
    auto parentWidget   = q->parent();
    auto &messageBarMap = _messageBarActiveMap[parentWidget];
    (*messageBarMap[_policy])[0]->d_ptr->messageBarEnd();
    _calculateInitialPos(startX, startY, endX, endY);
  }
}

QList<int> NXMessageBarPrivate::_getOtherMessageBarTotalData()
{
  Q_Q(NXMessageBar);
  QList<int> resultList;
  int minimumHeightTotal                = 0;
  int indexLessCount                    = 0;
  auto parentWidget                     = q->parent();
  QList<NXMessageBar *> *messageBarList = nullptr;
  if (_messageBarActiveMap.contains(parentWidget)) {
    messageBarList = _messageBarActiveMap[parentWidget].value(_policy, nullptr);
  }
  if (!messageBarList) {
    resultList.append(0);
    resultList.append(0);
    return resultList;
  }
  for (const auto messageBar : *messageBarList) {
    if (messageBar == q) {
      continue;
    }
    if (_judgeCreateOrder(messageBar)) {
      indexLessCount++;
      minimumHeightTotal += messageBar->minimumHeight();
    }
  }
  resultList.append(minimumHeightTotal);
  resultList.append(indexLessCount);
  return resultList;
}

qreal NXMessageBarPrivate::_calculateTargetPosY()
{
  Q_Q(NXMessageBar);
  QList<int> resultList  = _getOtherMessageBarTotalData();
  int minimumHeightTotal = resultList[0];
  int indexLessCount     = resultList[1];
  switch (_policy) {
  case NXMessageBarType::Top :
  case NXMessageBarType::TopRight :
  case NXMessageBarType::TopLeft  : {
    return minimumHeightTotal + _messageBarSpacing * indexLessCount + _messageBarVerticalTopMargin;
  }
  case NXMessageBarType::Left :
  case NXMessageBarType::Right : {
    return minimumHeightTotal + _messageBarSpacing * indexLessCount + q->parentWidget()->height() / 2;
  }
  case NXMessageBarType::Bottom :
  case NXMessageBarType::BottomRight :
  case NXMessageBarType::BottomLeft  : {
    return q->parentWidget()->height() - q->minimumHeight() - minimumHeightTotal - _messageBarSpacing * indexLessCount -
           _messageBarVerticalBottomMargin;
  }
  }
  return 0;
}

bool NXMessageBarPrivate::_judgeCreateOrder(NXMessageBar *otherMessageBar)
{
  return otherMessageBar->d_ptr->_createSerial < _createSerial;
}

void NXMessageBarPrivate::_updateActiveMap(bool isActive)
{
  Q_Q(NXMessageBar);
  NXMessageBarType::PositionPolicy policy = _policy;
  auto parentWidget                       = q->parent();
  auto &messageBarMap                     = _messageBarActiveMap[parentWidget];
  if (auto it = messageBarMap.find(policy); isActive) {
    if (it != messageBarMap.end()) {
      it.value()->append(q);
    } else {
      QList<NXMessageBar *> *messageBarList = new QList<NXMessageBar *>();
      messageBarList->append(q);
      messageBarMap.insert(policy, messageBarList);
    }
  } else {
    if (it != messageBarMap.end()) {
      if (!it.value()->isEmpty()) {
        it.value()->removeOne(q);
      } else {
        delete messageBarMap.take(policy);
      }
    }
    if (messageBarMap.isEmpty()) {
      _messageBarActiveMap.remove(parentWidget);
    }
  }
}

void NXMessageBarPrivate::_drawSuccess(QPainter *painter)
{
  Q_Q(NXMessageBar);
  painter->save();
  painter->setBrush(_themeMode == NXThemeType::Light ? QColor(0xE0, 0xF6, 0xDD) : QColor(0x39, 0x4D, 0x37));
  QRect foregroundRect(_shadowBorderWidth, _shadowBorderWidth, q->width() - 2 * _shadowBorderWidth, q->height() - 2 * _shadowBorderWidth);
  painter->drawRoundedRect(foregroundRect, _borderRadius, _borderRadius);
  // 图标绘制
  painter->setPen(Qt::white);
  QPainterPath textPath;
  textPath.addEllipse(QPoint(_leftPadding + 6, q->height() / 2), 9, 9);
  painter->setClipPath(textPath);
  painter->fillPath(textPath, _themeMode == NXThemeType::Light ? QColor(0x11, 0x77, 0x10) : QColor(0x4C, 0xAF, 0x50));
  QFont iconFont = QFont(QStringLiteral("NXAwesome"));
  iconFont.setPixelSize(nxApp->getFontPixelSize() - 1);
  painter->setFont(iconFont);
  painter->drawText(QRect(_leftPadding + 6 - 9, q->height() / 2 - 9, 18, 18), Qt::AlignCenter, QChar((unsigned short) NXIconType::Check));
  // 时间进度条绘制
  QPainterPath clipPath;
  clipPath.addRoundedRect(foregroundRect, _borderRadius, _borderRadius);
  painter->setClipPath(clipPath);
  painter->setPen(Qt::NoPen);
  painter->setBrush(QColor(0x3C, 0x96, 0x4B));
  painter->drawRoundedRect(QRectF(foregroundRect.x(), foregroundRect.bottom() - _timePercentHeight, foregroundRect.width() * _pTimePercent / 100.0, _timePercentHeight + 1), 2, 2);
  painter->restore();
  // 文字颜色
  painter->setPen(_themeMode == NXThemeType::Light ? QPen(Qt::black) : QPen(QColor(0xE0, 0xF6, 0xDD)));
}

void NXMessageBarPrivate::_drawWarning(QPainter *painter)
{
  Q_Q(NXMessageBar);
  painter->save();
  painter->setBrush(_themeMode == NXThemeType::Light ? QColor(0x6B, 0x56, 0x27) : QColor(0x5A, 0x4A, 0x1F));
  QRect foregroundRect(_shadowBorderWidth, _shadowBorderWidth, q->width() - 2 * _shadowBorderWidth, q->height() - 2 * _shadowBorderWidth);
  painter->drawRoundedRect(foregroundRect, _borderRadius, _borderRadius);
  // 图标绘制
  // exclamation
  painter->setPen(Qt::black);
  QPainterPath textPath;
  textPath.addEllipse(QPoint(_leftPadding + 6, q->height() / 2), 9, 9);
  painter->setClipPath(textPath);
  painter->fillPath(textPath, _themeMode == NXThemeType::Light ? QColor(0xF8, 0xE2, 0x23) : QColor(0xFF, 0xEB, 0x3B));
  painter->drawText(QRect(_leftPadding + 6 - 9, q->height() / 2 - 9, 18, 18), Qt::AlignCenter, QStringLiteral("!"));
  // 时间进度条绘制
  QPainterPath clipPath;
  clipPath.addRoundedRect(foregroundRect, _borderRadius, _borderRadius);
  painter->setClipPath(clipPath);
  painter->setPen(Qt::NoPen);
  painter->setBrush(QColor(0xC4, 0xAD, 0x59));
  painter->drawRoundedRect(QRectF(foregroundRect.x(), foregroundRect.bottom() - _timePercentHeight, foregroundRect.width() * _pTimePercent / 100.0, _timePercentHeight + 1), 2, 2);
  painter->restore();
  // 文字颜色
  painter->setPen(_themeMode == NXThemeType::Light ? QColor(0xFA, 0xFA, 0xFA) : QColor(0xFF, 0xF3, 0xCD));
}

void NXMessageBarPrivate::_drawInformation(QPainter *painter)
{
  Q_Q(NXMessageBar);
  painter->save();
  painter->setBrush(_themeMode == NXThemeType::Light ? QColor(0xF4, 0xF4, 0xF4) : QColor(0x37, 0x47, 0x4F));
  QRect foregroundRect(_shadowBorderWidth, _shadowBorderWidth, q->width() - 2 * _shadowBorderWidth, q->height() - 2 * _shadowBorderWidth);
  painter->drawRoundedRect(foregroundRect, _borderRadius, _borderRadius);
  // 图标绘制
  painter->setPen(Qt::white);
  QPainterPath textPath;
  textPath.addEllipse(QPoint(_leftPadding + 6, q->height() / 2), 9, 9);
  painter->setClipPath(textPath);
  painter->fillPath(textPath, _themeMode == NXThemeType::Light ? QColor(0x00, 0x66, 0xB4) : QColor(0x42, 0xA5, 0xF5));
  painter->drawText(QRect(_leftPadding + 6 - 9, q->height() / 2 - 9, 18, 18), Qt::AlignCenter, QStringLiteral("i"));
  // 时间进度条绘制
  QPainterPath clipPath;
  clipPath.addRoundedRect(foregroundRect, _borderRadius, _borderRadius);
  painter->setClipPath(clipPath);
  painter->setPen(Qt::NoPen);
  painter->setBrush(QColor(0x00, 0x66, 0xB4));
  painter->drawRoundedRect(QRectF(foregroundRect.x(), foregroundRect.bottom() - _timePercentHeight, foregroundRect.width() * _pTimePercent / 100.0, _timePercentHeight + 1), 2, 2);
  painter->restore();
  // 文字颜色
  painter->setPen(_themeMode == NXThemeType::Light ? Qt::black : QColor(0xE1, 0xF5, 0xFE));
}

void NXMessageBarPrivate::_drawError(QPainter *painter)
{
  Q_Q(NXMessageBar);
  painter->save();
  painter->setBrush(_themeMode == NXThemeType::Light ? QColor(0xFE, 0xE7, 0xEA) : QColor(0x4E, 0x34, 0x2E));
  QRect foregroundRect(_shadowBorderWidth, _shadowBorderWidth, q->width() - 2 * _shadowBorderWidth, q->height() - 2 * _shadowBorderWidth);
  painter->drawRoundedRect(foregroundRect, _borderRadius, _borderRadius);
  // 图标绘制
  painter->setPen(Qt::white);
  QPainterPath textPath;
  textPath.addEllipse(QPoint(_leftPadding + 6, q->height() / 2), 9, 9);
  painter->setClipPath(textPath);
  painter->fillPath(textPath, _themeMode == NXThemeType::Light ? QColor(0xBA, 0x2D, 0x20) : QColor(0xEF, 0x53, 0x50));
  QFont iconFont = QFont(QStringLiteral("NXAwesome"));
  iconFont.setPixelSize(nxApp->getFontPixelSize());
  painter->setFont(iconFont);
  painter->drawText(QRect(_leftPadding + 6 - 9, q->height() / 2 - 9, 18, 18), Qt::AlignCenter, QChar((unsigned short) NXIconType::Xmark));
  // 时间进度条绘制
  QPainterPath clipPath;
  clipPath.addRoundedRect(foregroundRect, _borderRadius, _borderRadius);
  painter->setClipPath(clipPath);
  painter->setPen(Qt::NoPen);
  painter->setBrush(QColor(0xCC, 0x5C, 0x65));
  painter->drawRoundedRect(QRectF(foregroundRect.x(), foregroundRect.bottom() - _timePercentHeight, foregroundRect.width() * _pTimePercent / 100.0, _timePercentHeight + 1), 2, 2);
  painter->restore();
  // 文字颜色
  painter->setPen(_themeMode == NXThemeType::Light ? Qt::black : QColor(0xFF, 0xCD, 0xD2));
}

int NXMessageBarPrivate::_getTextHeight()
{
  Q_Q(NXMessageBar);
  int fontPixelSize = nxApp->getFontPixelSize();
  QFont font        = q->font();
  font.setPixelSize(fontPixelSize);
  font.setWeight(QFont::Bold);
  QFontMetrics titleFontMetrics(font);
  int titleTextWidth = titleFontMetrics.horizontalAdvance(_title) + 1;
  if (titleTextWidth > 100) {
    titleTextWidth = 100;
  }
  font.setWeight(QFont::Light);
  QFontMetrics textFontMetrics(font);
  int textRectWidth = q->minimumWidth() - (_leftPadding + _titleLeftSpacing + titleTextWidth + _textLeftSpacing +
                                           _closeButtonWidth + _closeButtonLeftRightMargin);
  int textFlags     = Qt::AlignLeft | Qt::AlignVCenter | Qt::TextWordWrap | Qt::TextWrapAnywhere;
  int textHeight    = textFontMetrics.boundingRect(QRect(0, 0, textRectWidth, 10000), textFlags, _text).height();
  return textHeight + 20;
}
