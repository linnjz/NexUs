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

  d->_drawerHeader    = new NXDrawerHeader(this);
  d->_drawerContainer = new NXDrawerContainer(this);
  connect(d->_drawerHeader, &NXDrawerHeader::drawerHeaderClicked, d, &NXDrawerAreaPrivate::onDrawerHeaderClicked);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);
  mainLayout->addWidget(d->_drawerHeader);
  mainLayout->addWidget(d->_drawerContainer);

  d->_themeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
  });
}

NXDrawerArea::~NXDrawerArea()
{
}

void NXDrawerArea::setBorderRadius(int borderRadius)
{
  Q_D(NXDrawerArea);
  d->_drawerHeader->setBorderRadius(borderRadius);
  d->_drawerContainer->setBorderRadius(borderRadius);
  Q_EMIT pBorderRadiusChanged();
}

int NXDrawerArea::getBorderRadius() const
{
  Q_D(const NXDrawerArea);
  return d->_drawerHeader->getBorderRadius();
}

void NXDrawerArea::setHeaderHeight(int headerHeight)
{
  Q_D(NXDrawerArea);
  d->_drawerHeader->setFixedHeight(headerHeight);
}

int NXDrawerArea::getHeaderHeight() const
{
  Q_D(const NXDrawerArea);
  return d->_drawerHeader->height();
}

void NXDrawerArea::setDrawerHeader(QWidget *widget)
{
  Q_D(NXDrawerArea);
  d->_drawerHeader->setHeaderWidget(widget);
}

void NXDrawerArea::addDrawer(QWidget *widget)
{
  Q_D(NXDrawerArea);
  d->_drawerContainer->addWidget(widget);
}

void NXDrawerArea::removeDrawer(QWidget *widget)
{
  Q_D(NXDrawerArea);
  d->_drawerContainer->removeWidget(widget);
}

void NXDrawerArea::expand()
{
  Q_D(NXDrawerArea);
  d->_drawerHeader->setIsExpand(true);
  d->_drawerHeader->doExpandOrCollapseAnimation();
  d->_drawerContainer->doDrawerAnimation(true);
  Q_EMIT expandStateChanged(true);
}

void NXDrawerArea::collapse()
{
  Q_D(NXDrawerArea);
  d->_drawerHeader->setIsExpand(false);
  d->_drawerHeader->doExpandOrCollapseAnimation();
  d->_drawerContainer->doDrawerAnimation(false);
  Q_EMIT expandStateChanged(false);
}

bool NXDrawerArea::getIsExpand() const
{
  Q_D(const NXDrawerArea);
  return d->_drawerHeader->getIsExpand();
}
