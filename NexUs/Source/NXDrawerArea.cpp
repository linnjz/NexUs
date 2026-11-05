#include "NXDrawerArea.h"

#include "NXTheme.h"
#include "private/NXDrawerAreaPrivate.h"

NXDrawerArea::NXDrawerArea(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new NXDrawerAreaPrivate())
{
  Q_D(NXDrawerArea);
  d->q_ptr = this;
  setObjectName("NXDrawerArea");
  setStyleSheet(QStringLiteral("#NXDrawerArea{background-color:transparent;}"));

  d->m_DrawerHeader    = new NXDrawerHeader(this);
  d->m_DrawerContainer = new NXDrawerContainer(this);
  connect(d->m_DrawerHeader, &NXDrawerHeader::drawerHeaderClicked, d, &NXDrawerAreaPrivate::onDrawerHeaderClicked);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);
  mainLayout->addWidget(d->m_DrawerHeader);
  mainLayout->addWidget(d->m_DrawerContainer);

  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
  });
}

NXDrawerArea::~NXDrawerArea()
{
}

void NXDrawerArea::setBorderRadius(int borderRadius)
{
  Q_D(NXDrawerArea);
  d->m_DrawerHeader->setBorderRadius(borderRadius);
  d->m_DrawerContainer->setBorderRadius(borderRadius);
  Q_EMIT signalBorderRadiusChanged();
}

int NXDrawerArea::getBorderRadius() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXDrawerArea);
  return d->m_DrawerHeader->getBorderRadius();
}

void NXDrawerArea::setHeaderHeight(int headerHeight)
{
  Q_D(NXDrawerArea);
  d->m_DrawerHeader->setFixedHeight(headerHeight);
}

int NXDrawerArea::getHeaderHeight() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXDrawerArea);
  return d->m_DrawerHeader->height();
}

void NXDrawerArea::setDrawerHeader(QWidget *widget)
{
  Q_D(NXDrawerArea);
  d->m_DrawerHeader->setHeaderWidget(widget);
}

void NXDrawerArea::addDrawer(QWidget *widget)
{
  Q_D(NXDrawerArea);
  d->m_DrawerContainer->addWidget(widget);
}

void NXDrawerArea::removeDrawer(QWidget *widget)
{
  Q_D(NXDrawerArea);
  d->m_DrawerContainer->removeWidget(widget);
}

void NXDrawerArea::expand()
{
  Q_D(NXDrawerArea);
  d->m_DrawerHeader->setIsExpand(true);
  d->m_DrawerHeader->doExpandOrCollapseAnimation();
  d->m_DrawerContainer->doDrawerAnimation(true);
  Q_EMIT expandStateChanged(true);
}

void NXDrawerArea::collapse()
{
  Q_D(NXDrawerArea);
  d->m_DrawerHeader->setIsExpand(false);
  d->m_DrawerHeader->doExpandOrCollapseAnimation();
  d->m_DrawerContainer->doDrawerAnimation(false);
  Q_EMIT expandStateChanged(false);
}

bool NXDrawerArea::getIsExpand() const
{
  Q_CONST_D(NXDrawerArea);
  return d->m_DrawerHeader->getIsExpand();
}
