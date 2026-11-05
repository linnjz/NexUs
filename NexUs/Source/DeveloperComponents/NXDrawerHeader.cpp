#include "NXDrawerHeader.h"

#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPropertyAnimation>

#include "NXApplication.h"
#include "NXTheme.h"

NXDrawerHeader::NXDrawerHeader(QWidget *parent)
    : QWidget(parent)
    , m_ExpandIconRotate { 0.0 }
    , m_BorderRadius { 6 }
    , m_IsExpand { false }
    , m_ThemeMode { nxTheme->getThemeMode() }
    , m_MainLayout { new QVBoxLayout(this) }
{
  setFixedHeight(75);
  setMouseTracking(true);
  setObjectName("NXDrawerHeader");
  setStyleSheet(QStringLiteral("#NXDrawerHeader{background-color:transparent;}"));

  m_MainLayout->setContentsMargins(0, 0, 0, 0);
  setContentsMargins(0, 0, 30, 0);

  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    m_ThemeMode = themeMode;
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
  if (m_HeaderWidget) {
    m_MainLayout->removeWidget(m_HeaderWidget);
    m_HeaderWidget->deleteLater();
  }
  m_MainLayout->addWidget(widget);
  m_HeaderWidget = widget;
}

void NXDrawerHeader::doExpandOrCollapseAnimation()
{
  QPropertyAnimation *rotateAnimation = new QPropertyAnimation(this, "ExpandIconRotate");
  connect(rotateAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
    update();
  });
  rotateAnimation->setDuration(300);
  rotateAnimation->setEasingCurve(QEasingCurve::InOutSine);
  rotateAnimation->setStartValue(m_ExpandIconRotate);
  rotateAnimation->setEndValue(m_IsExpand ? -180 : 0);
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
    m_IsPressed = true;
    update();
  }
  QWidget::mousePressEvent(event);
}

void NXDrawerHeader::mouseReleaseEvent(QMouseEvent *event)
{
  QWidget *posWidget = childAt(event->pos());
  if (!posWidget || (posWidget && posWidget->objectName().isEmpty())) {
    m_IsPressed = false;
    m_IsExpand = !m_IsExpand;
    //指示器动画
    doExpandOrCollapseAnimation();
    Q_EMIT drawerHeaderClicked(m_IsExpand);
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
  painter.setPen(NXThemeColor(m_ThemeMode, BasicBorder));
  painter.setBrush(m_IsPressed ? NXThemeColor(m_ThemeMode, BasicSelectedAlpha) : isUnderMouse ? NXThemeColor(m_ThemeMode, BasicPressAlpha)
                                                                                            : NXThemeColor(m_ThemeMode, BasicBaseAlpha));
  QRect foregroundRect(1, 1, width() - 2, m_IsExpand ? height() + m_BorderRadius : height() - 2);
  painter.drawRoundedRect(foregroundRect, m_BorderRadius, m_BorderRadius);
  // 底边线绘制
  if (isUnderMouse) {
    painter.setPen(NXThemeColor(m_ThemeMode, BasicBorderDeep));
  }
  if (m_IsExpand) {
    painter.drawLine(0, height() - 1, width(), height() - 1);
  } else {
    painter.drawLine(m_BorderRadius, height() - 1, width() - m_BorderRadius, height() - 1);
    painter.drawArc(QRectF(1, height() - 1 - 2 * m_BorderRadius, 2 * m_BorderRadius, 2 * m_BorderRadius), 240 * 16, 30 * 16);
    painter.drawArc(QRectF(width() - 2 * m_BorderRadius, height() - 1 - 2 * m_BorderRadius, 2 * m_BorderRadius, 2 * m_BorderRadius), -90 * 16, 30 * 16);
  }

  // 展开图标绘制
  QFont iconFont = QFont(QStringLiteral("NXAwesome"));
  iconFont.setPixelSize(nxApp->getFontPixelSize() + 4);
  painter.setFont(iconFont);
  painter.setPen(isEnabled() ? NXThemeColor(m_ThemeMode, BasicText) : NXThemeColor(m_ThemeMode, BasicTextDisable));
  QRectF expandIconRect(width() - 25, 0, 20, height());
  painter.translate(expandIconRect.x() + (qreal) expandIconRect.width() / 2 - 2, expandIconRect.y() + (qreal) expandIconRect.height() / 2);
  painter.rotate(m_ExpandIconRotate);
  painter.translate(-expandIconRect.x() - (qreal) expandIconRect.width() / 2 + 2, -expandIconRect.y() - (qreal) expandIconRect.height() / 2);
  painter.drawText(expandIconRect, Qt::AlignVCenter, QChar((unsigned short) NXIconType::AngleDown));

  painter.restore();
}
