#include "NXDrawerContainer.h"

#include <QPainter>
#include <QPropertyAnimation>

#include "NXTheme.h"

NXDrawerContainer::NXDrawerContainer(QWidget *parent)
    : QWidget(parent)
{
  _pBorderRadius = 6;
  _pOpacity      = 0;
  _pContainerPix = QPixmap();
  setObjectName("NXDrawerContainer");
  setStyleSheet(QStringLiteral("#NXDrawerContainer{background-color:transparent;}"));

  _mainLayout = new QVBoxLayout(this);
  _mainLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
  _mainLayout->setContentsMargins(0, 0, 0, 0);

  _containerWidget = new QWidget(this);
  _containerWidget->setObjectName("NXDrawerContainerWidget");
  _containerWidget->setStyleSheet(QStringLiteral("#NXDrawerContainerWidget{background-color:transparent;}"));
  _containerWidget->setVisible(false);

  _containerLayout = new QVBoxLayout(_containerWidget);
  _containerLayout->setContentsMargins(0, 0, 0, 0);
  _containerLayout->setSpacing(0);

  _mainLayout->addWidget(_containerWidget);

  _themeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    _themeMode = themeMode;
    update();
  });
}

NXDrawerContainer::~NXDrawerContainer()
{
}

void NXDrawerContainer::addWidget(QWidget *widget)
{
  if (!widget || _drawerWidgetList.contains(widget)) {
    return;
  }
  _containerLayout->addWidget(widget);
  _drawerWidgetList.append(widget);
}

void NXDrawerContainer::removeWidget(QWidget *widget)
{
  if (!widget) {
    return;
  }
  _containerLayout->removeWidget(widget);
  _drawerWidgetList.removeOne(widget);
}

void NXDrawerContainer::doDrawerAnimation(bool isExpand)
{
  if (_containerLayout->count() == 0) {
    return;
  }
  _containerWidget->setVisible(true);
  _mainLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
  int expandHeight = height();
  _isShowBorder    = true;
  setFixedHeight(expandHeight);
  _pContainerPix                       = grab(rect());
  QPropertyAnimation *opacityAnimation = new QPropertyAnimation(this, "pOpacity");
  connect(opacityAnimation, &QPropertyAnimation::valueChanged, this, [=](const QVariant &value) {
    update();
  });
  connect(opacityAnimation, &QPropertyAnimation::finished, this, [=]() {
    _pContainerPix = QPixmap();
    _isShowBorder  = isExpand;
    _containerWidget->setVisible(isExpand);
    _mainLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
  });
  opacityAnimation->setEasingCurve(QEasingCurve::OutCubic);
  opacityAnimation->setDuration(300);
  opacityAnimation->setStartValue(_pOpacity);
  opacityAnimation->setEndValue(isExpand ? 1 : 0);
  opacityAnimation->start(QPropertyAnimation::DeleteWhenStopped);
  _containerWidget->setVisible(false);
  _isShowBorder = false;
}

void NXDrawerContainer::paintEvent(QPaintEvent *event)
{
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::Antialiasing);
  if (_isShowBorder) {
    // 背景绘制
    painter.setPen(NXThemeColor(_themeMode, BasicBorder));
    painter.setBrush(NXThemeColor(_themeMode, BasicBaseAlpha));
    QRect foregroundRect(1, 1 - 2 * _pBorderRadius, width() - 2, height() - 2 + 2 * _pBorderRadius);
    painter.drawRoundedRect(foregroundRect, _pBorderRadius, _pBorderRadius);
    // 分割线绘制
    int drawerHeight = 0;
    for (int i = 0; i < _drawerWidgetList.count() - 1; i++) {
      QWidget *drawerWidget = _drawerWidgetList[i];
      drawerHeight += drawerWidget->height();
      painter.drawLine(0, drawerHeight, width(), drawerHeight);
    }
  }
  if (!_pContainerPix.isNull()) {
    painter.setOpacity(_pOpacity);
    painter.drawPixmap(QRect(0, -height() * (1 - _pOpacity), width(), height()), _pContainerPix);
  }
  painter.restore();
}
