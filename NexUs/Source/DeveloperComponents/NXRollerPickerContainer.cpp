#include "NXRollerPickerContainer.h"

#include <QMouseEvent>
#include <QPainter>

#include "NXApplication.h"
#include "NXRoller.h"
#include "NXTheme.h"

NXRollerPickerContainer::NXRollerPickerContainer(QWidget *parent)
    : QWidget { parent }
    , m_AnimationPixOffsetY { 0 }
    , m_ButtonAreaHeight { 38 }
    , m_ThemeMode { nxTheme->getThemeMode() }
{
  setMouseTracking(true);
  setWindowFlags(Qt::FramelessWindowHint | Qt::Popup | Qt::NoDropShadowWindowHint);
  setAttribute(Qt::WA_TranslucentBackground);
#if defined(Q_OS_WIN) && QT_VERSION == QT_VERSION_CHECK(6, 11, 0)
  setContentsMargins(2, 2, 2, m_ButtonAreaHeight);
#else
  setContentsMargins(8, 8, 8, 6 + m_ButtonAreaHeight);
#endif
  setObjectName("NXCalendarPickerContainer");
  setStyleSheet(QStringLiteral("#NXCalendarPickerContainer{background-color:transparent}"));

  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    m_ThemeMode = themeMode;
    update();
  });
}

NXRollerPickerContainer::~NXRollerPickerContainer()
{
}

void NXRollerPickerContainer::doPickerAnimation()
{
  _handleSaveOrReset(true);
  if (!m_AnimationPix.isNull()) {
    m_AnimationPix = QPixmap();
  }
  m_AnimationPix                       = this->grab(rect());
  QPropertyAnimation *offsetAnimation = new QPropertyAnimation(this, "AnimationPixOffsetY");
  connect(offsetAnimation, &QPropertyAnimation::finished, this, [=]() {
    m_AnimationPix = QPixmap();
    update();
  });
  connect(offsetAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
    update();
  });
  offsetAnimation->setEasingCurve(QEasingCurve::OutCubic);
  offsetAnimation->setDuration(175);
  offsetAnimation->setStartValue(70);
  offsetAnimation->setEndValue(0);
  offsetAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void NXRollerPickerContainer::mouseMoveEvent(QMouseEvent *event)
{
  auto currentPos = event->pos();
  if (m_OverButtonRect.contains(currentPos)) {
    m_IsOverButtonHover   = true;
    m_IsCancelButtonHover = false;
    update();
  } else if (m_CancelButtonRect.contains(currentPos)) {
    m_IsCancelButtonHover = true;
    m_IsOverButtonHover   = false;
    update();
  } else {
    if (m_IsCancelButtonHover || m_IsOverButtonHover) {
      m_IsCancelButtonHover = false;
      m_IsOverButtonHover   = false;
      update();
    }
  }
  QWidget::mouseMoveEvent(event);
}

void NXRollerPickerContainer::mouseReleaseEvent(QMouseEvent *event)
{
  auto currentPos = event->pos();
  if (m_OverButtonRect.contains(currentPos)) {
    m_IsOverButtonClicked = true;
    Q_EMIT overButtonClicked();
    hide();
  } else if (m_CancelButtonRect.contains(currentPos)) {
    _handleSaveOrReset(false);
    Q_EMIT cancelButtonClicked();
    hide();
  }
  QWidget::mouseReleaseEvent(event);
}

void NXRollerPickerContainer::leaveEvent(QEvent *event)
{
  m_IsOverButtonHover   = false;
  m_IsCancelButtonHover = false;
  update();
  QWidget::leaveEvent(event);
}

void NXRollerPickerContainer::hideEvent(QHideEvent *event)
{
  if (m_IsOverButtonClicked) {
    m_IsOverButtonClicked = false;
  } else {
    _handleSaveOrReset(false);
  }
  QWidget::hideEvent(event);
}

void NXRollerPickerContainer::paintEvent(QPaintEvent *event)
{
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::Antialiasing);
  if (!m_AnimationPix.isNull()) {
    painter.setClipRect(QRect(0, m_AnimationPixOffsetY, width(), height() - 2 * m_AnimationPixOffsetY));
    painter.drawPixmap(rect(), m_AnimationPix);
  } else {
#if defined(Q_OS_WIN) && QT_VERSION == QT_VERSION_CHECK(6, 11, 0)
    QRect foregroundRect = rect();
#else
    nxTheme->drawEffectShadow(&painter, rect(), 6, 5);
    QRect foregroundRect(6, 6, rect().width() - 2 * 6, rect().height() - 2 * 6);
#endif
    painter.setPen(NXThemeColor(m_ThemeMode, PopupBorder));
    painter.setBrush(NXThemeColor(m_ThemeMode, PopupBase));
    painter.drawRoundedRect(foregroundRect, 8, 8);
    // 纵向分割线
    painter.setPen(NXThemeColor(m_ThemeMode, BasicBorder));
    int rollerXOffset = 8;
    for (int i = 0; i < m_RollerList.count(); i++) {
      rollerXOffset += m_RollerList[i]->width();
      if (i != m_RollerList.count() - 1) {
        painter.setPen(NXThemeColor(m_ThemeMode, BasicBorder));
        painter.drawLine(rollerXOffset, foregroundRect.y(), rollerXOffset, foregroundRect.bottom() - m_ButtonAreaHeight);
      }
    }
    // 横向分割线
    painter.drawLine(foregroundRect.x(), foregroundRect.bottom() - m_ButtonAreaHeight, foregroundRect.right(), foregroundRect.bottom() - m_ButtonAreaHeight);
    // 按钮区域绘制
    m_OverButtonRect = QRect(
        foregroundRect.x() + m_ButtonMargin, foregroundRect.bottom() - m_ButtonAreaHeight + m_ButtonMargin,
        (foregroundRect.width() - 2 * m_ButtonMargin - m_ButtonSpacing) / 2, m_ButtonAreaHeight - 2 * m_ButtonMargin + 1);
    m_CancelButtonRect = QRect(m_OverButtonRect.right() + m_ButtonSpacing, m_OverButtonRect.y(), m_OverButtonRect.width(), m_OverButtonRect.height());
    // 按钮覆盖绘制
    painter.setPen(Qt::NoPen);
    painter.setBrush(NXThemeColor(m_ThemeMode, BasicHover));
    if (m_IsOverButtonHover) {
      painter.drawRoundedRect(m_OverButtonRect, 5, 5);
    } else if (m_IsCancelButtonHover) {
      painter.drawRoundedRect(m_CancelButtonRect, 5, 5);
    }
    QFont iconFont = QFont(QStringLiteral("NXAwesome"));
    iconFont.setPixelSize(nxApp->getFontPixelSize() + 4);
    painter.setFont(iconFont);
    painter.setPen(NXThemeColor(m_ThemeMode, BasicText));
    // 确定
    painter.drawText(m_OverButtonRect, Qt::AlignCenter, QChar((unsigned short) NXIconType::Check));
    // 取消
    painter.drawText(m_CancelButtonRect, Qt::AlignCenter, QChar((unsigned short) NXIconType::Xmark));
  }
  painter.restore();
}

void NXRollerPickerContainer::_handleSaveOrReset(bool isSave)
{
  if (isSave) {
    // 保存历史数据
    m_HistoryIndexList.clear();
    for (auto roller : m_RollerList) {
      m_HistoryIndexList.append(roller->getCurrentIndex());
    }
  } else {
    // 恢复数据
    for (int i = 0; i < m_RollerList.count(); i++) {
      auto roller = m_RollerList[i];
      roller->setCurrentIndex(m_HistoryIndexList[i]);
    }
  }
}
