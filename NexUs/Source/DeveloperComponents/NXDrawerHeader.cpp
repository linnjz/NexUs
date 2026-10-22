#include "NXDrawerHeader.h"

#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPropertyAnimation>

#include "NXApplication.h"
#include "NXTheme.h"

NXDrawerHeader::NXDrawerHeader(QWidget *parent)
    : QWidget(parent)
{
  _pBorderRadius     = 6;
  _pIsExpand         = false;
  _pExpandIconRotate = 0;
  setFixedHeight(75);
  setMouseTracking(true);
  setObjectName("NXDrawerHeader");
  setStyleSheet(QStringLiteral("#NXDrawerHeader{background-color:transparent;}"));

  _mainLayout = new QVBoxLayout(this);
  _mainLayout->setContentsMargins(0, 0, 0, 0);
  setContentsMargins(0, 0, 30, 0);

  _themeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    _themeMode = themeMode;
  });
}

NXDrawerHeader::~NXDrawerHeader()
{
}

void NXDrawerHeader::setHeaderWidget(QWidget *widget)
{
  if (!widget) {
    return;
  }
  if (_headerWidget) {
    _mainLayout->removeWidget(_headerWidget);
    _headerWidget->deleteLater();
  }
  _mainLayout->addWidget(widget);
  _headerWidget = widget;
}

void NXDrawerHeader::doExpandOrCollapseAnimation()
{
  QPropertyAnimation *rotateAnimation = new QPropertyAnimation(this, "pExpandIconRotate");
  connect(rotateAnimation, &QPropertyAnimation::valueChanged, this, [=](const QVariant &value) {
    update();
  });
  rotateAnimation->setDuration(300);
  rotateAnimation->setEasingCurve(QEasingCurve::InOutSine);
  rotateAnimation->setStartValue(_pExpandIconRotate);
  rotateAnimation->setEndValue(_pIsExpand ? -180 : 0);
  rotateAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

bool NXDrawerHeader::event(QEvent *event)
{
  switch (event->type()) {
  case QEvent::Enter :
  case QEvent::Leave : {
    update();
    break;
  }
  default : {
    break;
  }
  }
  return QWidget::event(event);
}

void NXDrawerHeader::mousePressEvent(QMouseEvent *event)
{
  QWidget *posWidget = childAt(event->pos());
  if (!posWidget || (posWidget && posWidget->objectName().isEmpty())) {
    _isPressed = true;
    update();
  }
  QWidget::mousePressEvent(event);
}

void NXDrawerHeader::mouseReleaseEvent(QMouseEvent *event)
{
  QWidget *posWidget = childAt(event->pos());
  if (!posWidget || (posWidget && posWidget->objectName().isEmpty())) {
    _isPressed = false;
    _pIsExpand = !_pIsExpand;
    //指示器动画
    doExpandOrCollapseAnimation();
    Q_EMIT drawerHeaderClicked(_pIsExpand);
  }
  QWidget::mouseReleaseEvent(event);
}

void NXDrawerHeader::mouseMoveEvent(QMouseEvent *event)
{
  update();
  QWidget::mouseMoveEvent(event);
}

void NXDrawerHeader::paintEvent(QPaintEvent *event)
{
  bool isUnderMouse = underMouse();
  QPainter painter(this);
  painter.save();
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setPen(NXThemeColor(_themeMode, BasicBorder));
  painter.setBrush(_isPressed ? NXThemeColor(_themeMode, BasicSelectedAlpha) : isUnderMouse ? NXThemeColor(_themeMode, BasicPressAlpha)
                                                                                            : NXThemeColor(_themeMode, BasicBaseAlpha));
  QRect foregroundRect(1, 1, width() - 2, _pIsExpand ? height() + _pBorderRadius : height() - 2);
  painter.drawRoundedRect(foregroundRect, _pBorderRadius, _pBorderRadius);
  // 底边线绘制
  if (isUnderMouse) {
    painter.setPen(NXThemeColor(_themeMode, BasicBorderDeep));
  }
  if (_pIsExpand) {
    painter.drawLine(0, height() - 1, width(), height() - 1);
  } else {
    painter.drawLine(_pBorderRadius, height() - 1, width() - _pBorderRadius, height() - 1);
    painter.drawArc(QRectF(1, height() - 1 - 2 * _pBorderRadius, 2 * _pBorderRadius, 2 * _pBorderRadius), 240 * 16, 30 * 16);
    painter.drawArc(QRectF(width() - 2 * _pBorderRadius, height() - 1 - 2 * _pBorderRadius, 2 * _pBorderRadius, 2 * _pBorderRadius), -90 * 16, 30 * 16);
  }

  // 展开图标绘制
  QFont iconFont = QFont(QStringLiteral("NXAwesome"));
  iconFont.setPixelSize(nxApp->getFontPixelSize() + 4);
  painter.setFont(iconFont);
  painter.setPen(isEnabled() ? NXThemeColor(_themeMode, BasicText) : NXThemeColor(_themeMode, BasicTextDisable));
  QRectF expandIconRect(width() - 25, 0, 20, height());
  painter.translate(expandIconRect.x() + (qreal) expandIconRect.width() / 2 - 2, expandIconRect.y() + (qreal) expandIconRect.height() / 2);
  painter.rotate(_pExpandIconRotate);
  painter.translate(-expandIconRect.x() - (qreal) expandIconRect.width() / 2 + 2, -expandIconRect.y() - (qreal) expandIconRect.height() / 2);
  painter.drawText(expandIconRect, Qt::AlignVCenter, QChar((unsigned short) NXIconType::AngleDown));

  painter.restore();
}
