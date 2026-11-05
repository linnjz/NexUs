#include "NXDrawerContainer.h"

#include <QPainter>
#include <QPropertyAnimation>

#include "NXTheme.h"

NXDrawerContainer::NXDrawerContainer(QWidget *parent)
    : QWidget { parent }
    , m_ContainerPix {}
    , m_Opacity { 0.0 }
    , m_BorderRadius { 6 }
    , m_ThemeMode { nxTheme->getThemeMode() }
    , m_ContainerWidget { new QWidget(this) }
    , m_MainLayout { new QVBoxLayout(this) }
    , m_ContainerLayout { new QVBoxLayout(m_ContainerWidget) }
{
  setObjectName("NXDrawerContainer");
  setStyleSheet(QStringLiteral("#NXDrawerContainer{background-color:transparent;}"));

  m_MainLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
  m_MainLayout->setContentsMargins(0, 0, 0, 0);

  m_ContainerWidget->setObjectName("NXDrawerContainerWidget");
  m_ContainerWidget->setStyleSheet(QStringLiteral("#NXDrawerContainerWidget{background-color:transparent;}"));
  m_ContainerWidget->setVisible(false);

  m_ContainerLayout->setContentsMargins(0, 0, 0, 0);
  m_ContainerLayout->setSpacing(0);

  m_MainLayout->addWidget(m_ContainerWidget);

  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    m_ThemeMode = themeMode;
    update();
  });
}

NXDrawerContainer::~NXDrawerContainer()
{
}

void NXDrawerContainer::addWidget(QWidget *widget)
{
  if (!widget || m_DrawerWidgetList.contains(widget)) {
    return;
  }
  m_ContainerLayout->addWidget(widget);
  m_DrawerWidgetList.append(widget);
}

void NXDrawerContainer::removeWidget(QWidget *widget)
{
  if (!widget) {
    return;
  }
  m_ContainerLayout->removeWidget(widget);
  m_DrawerWidgetList.removeOne(widget);
}

void NXDrawerContainer::doDrawerAnimation(bool isExpand)
{
  if (m_ContainerLayout->count() == 0) {
    return;
  }
  m_ContainerWidget->setVisible(true);
  m_MainLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
  int expandHeight = height();
  m_IsShowBorder    = true;
  setFixedHeight(expandHeight);
  m_ContainerPix                       = grab(rect());
  QPropertyAnimation *opacityAnimation = new QPropertyAnimation(this, "Opacity");
  connect(opacityAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
    update();
  });
  connect(opacityAnimation, &QPropertyAnimation::finished, this, [=]() {
    m_ContainerPix = QPixmap();
    m_IsShowBorder  = isExpand;
    m_ContainerWidget->setVisible(isExpand);
    m_MainLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
  });
  opacityAnimation->setEasingCurve(QEasingCurve::OutCubic);
  opacityAnimation->setDuration(300);
  opacityAnimation->setStartValue(m_Opacity);
  opacityAnimation->setEndValue(isExpand ? 1 : 0);
  opacityAnimation->start(QPropertyAnimation::DeleteWhenStopped);
  m_ContainerWidget->setVisible(false);
  m_IsShowBorder = false;
}

void NXDrawerContainer::paintEvent(QPaintEvent *event)
{
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::Antialiasing);
  if (m_IsShowBorder) {
    // 背景绘制
    painter.setPen(NXThemeColor(m_ThemeMode, BasicBorder));
    painter.setBrush(NXThemeColor(m_ThemeMode, BasicBaseAlpha));
    QRect foregroundRect(1, 1 - 2 * m_BorderRadius, width() - 2, height() - 2 + 2 * m_BorderRadius);
    painter.drawRoundedRect(foregroundRect, m_BorderRadius, m_BorderRadius);
    // 分割线绘制
    int drawerHeight = 0;
    for (int i = 0; i < m_DrawerWidgetList.count() - 1; i++) {
      QWidget *drawerWidget = m_DrawerWidgetList[i];
      drawerHeight += drawerWidget->height();
      painter.drawLine(0, drawerHeight, width(), drawerHeight);
    }
  }
  if (!m_ContainerPix.isNull()) {
    painter.setOpacity(m_Opacity);
    painter.drawPixmap(QRect(0, -height() * (1 - m_Opacity), width(), height()), m_ContainerPix);
  }
  painter.restore();
}
