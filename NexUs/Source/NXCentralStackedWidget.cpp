#include "NXCentralStackedWidget.h"

#include <QApplication>
#include <QDebug>
#include <QGraphicsBlurEffect>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QTimer>
#include <QVBoxLayout>
#include <cmath>
#include "NXTheme.h"

#include "private/NXCentralStackedWidgetPrivate.h"

Q_PROPERTY_CREATE_CPP(NXCentralStackedWidget, QPS_PAEMIT(int), LastTargetIndex)
Q_PROPERTY_CREATE_CPP(NXCentralStackedWidget, QPS_PAEMIT(int), BlurAnimationRadius)
Q_PROPERTY_CREATE_CPP(NXCentralStackedWidget, QPS_PAEMIT(int), PopupAnimationYOffset)
Q_PROPERTY_CREATE_CPP(NXCentralStackedWidget, QPS_PAEMIT(qreal), ScaleAnimationRatio)
Q_PROPERTY_CREATE_CPP(NXCentralStackedWidget, QPS_PAEMIT(qreal), ScaleAnimationPixOpacity)
Q_PROPERTY_CREATE_CPP(NXCentralStackedWidget, QPS_PAEMIT(qreal), FlipAnimationRatio)

NXCentralStackedWidget::NXCentralStackedWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new NXCentralStackedWidgetPrivate())
{
  Q_D(NXCentralStackedWidget);
  d->q_ptr                      = this;
  d->m_LastTargetIndex          = 0;
  d->m_PopupAnimationYOffset    = 0;
  d->m_ScaleAnimationRatio      = 1;
  d->m_ScaleAnimationPixOpacity = 1;
  d->m_FlipAnimationRatio       = 1;
  d->m_BlurAnimationRadius      = 0;

  setObjectName("NXCentralStackedWidget");
  setStyleSheet(QStringLiteral("#NXCentralStackedWidget{background-color:transparent;}"));

  d->m_ContainerStackedWidget = new NXLazyStackedWidget(this);
  d->m_ContainerStackedWidget->setObjectName("NXCentralStackedWidget");
  d->m_ContainerStackedWidget->setStyleSheet(QStringLiteral("#NXCentralStackedWidget{background-color:transparent;}"));

  d->m_BlurEffect = new QGraphicsBlurEffect(d->m_ContainerStackedWidget);
  d->m_BlurEffect->setBlurHints(QGraphicsBlurEffect::BlurHint::QualityHint);
  d->m_BlurEffect->setBlurRadius(0);
  d->m_BlurEffect->setEnabled(false);
  d->m_ContainerStackedWidget->setGraphicsEffect(d->m_BlurEffect);

  d->m_MainLayout = new QVBoxLayout(this);
  d->m_MainLayout->setSpacing(0);
  d->m_MainLayout->setContentsMargins(0, 0, 0, 0);
  d->m_MainLayout->addWidget(d->m_ContainerStackedWidget);

  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, d, &NXCentralStackedWidgetPrivate::onThemeModeChanged);
}

NXCentralStackedWidget::~NXCentralStackedWidget()
{
}

NXLazyStackedWidget *NXCentralStackedWidget::getContainerStackedWidget() const
{
  return d_ptr->m_ContainerStackedWidget;
}

void NXCentralStackedWidget::setCustomWidget(QWidget *widget)
{
  Q_D(NXCentralStackedWidget);
  if (!widget) {
    return;
  }
  if (d->m_CustomWidget) {
    d->m_MainLayout->removeWidget(d->m_CustomWidget);
  }
  d->m_MainLayout->insertWidget(0, widget);
  d->m_CustomWidget = widget;
}

QWidget *NXCentralStackedWidget::getCustomWidget() const
{
  return d_ptr->m_CustomWidget;
}

void NXCentralStackedWidget::setIsTransparent(bool isTransparent)
{
  d_ptr->m_IsTransparent = isTransparent;
  update();
}

bool NXCentralStackedWidget::getIsTransparent() const
{
  return d_ptr->m_IsTransparent;
}

void NXCentralStackedWidget::setIsHasRadius(bool isHasRadius)
{
  d_ptr->m_IsHasRadius = isHasRadius;
  update();
}

void NXCentralStackedWidget::doWindowStackSwitch(NXWindowType::StackSwitchMode stackSwitchMode, int nodeIndex, bool isRouteBack)
{
  Q_D(NXCentralStackedWidget);
  if (d->m_LastTargetIndex == nodeIndex) {
    return;
  }
  d->m_LastTargetIndex = nodeIndex;
  d->m_StackSwitchMode  = stackSwitchMode;
  switch (stackSwitchMode) {
  case NXWindowType::None : {
    d->m_ContainerStackedWidget->setCurrentIndex(nodeIndex);
    break;
  }
  case NXWindowType::Popup : {
    QTimer::singleShot(180, this, [=]() {
      QWidget *targetWidget = d->m_ContainerStackedWidget->widget(nodeIndex);
      d->m_ContainerStackedWidget->setCurrentIndex(nodeIndex);
      d->_getTargetStackPix();
      targetWidget->setVisible(false);
      QPropertyAnimation *popupAnimation = new QPropertyAnimation(this, "PopupAnimationYOffset");
      connect(popupAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
        update();
      });
      connect(popupAnimation, &QPropertyAnimation::finished, this, [=]() {
        d->m_TargetStackPix = QPixmap();
        targetWidget->setVisible(true);
      });
      popupAnimation->setEasingCurve(QEasingCurve::OutCubic);
      popupAnimation->setDuration(300);
      int targetWidgetY = d->m_ContainerStackedWidget->mapToParent(QPoint(0, 0)).y();
      popupAnimation->setEndValue(targetWidgetY);
      targetWidgetY += 80;
      popupAnimation->setStartValue(targetWidgetY);
      popupAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    });
    break;
  }
  case NXWindowType::Scale : {
    QWidget *targetWidget = d->m_ContainerStackedWidget->widget(nodeIndex);
    d->_getCurrentStackPix();
    d->m_ContainerStackedWidget->setCurrentIndex(nodeIndex);
    targetWidget->setVisible(true);
    d->_getTargetStackPix();
    targetWidget->setVisible(false);
    d->m_IsDrawNewPix                            = false;
    QPropertyAnimation *currentPixZoomAnimation = new QPropertyAnimation(this, "ScaleAnimationRatio");
    connect(currentPixZoomAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
      update();
    });
    connect(currentPixZoomAnimation, &QPropertyAnimation::finished, this, [=]() {
      d->m_IsDrawNewPix                           = true;
      QPropertyAnimation *targetPixZoomAnimation = new QPropertyAnimation(this, "ScaleAnimationRatio");
      connect(targetPixZoomAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
        update();
      });
      connect(targetPixZoomAnimation, &QPropertyAnimation::finished, this, [=]() {
        d->m_TargetStackPix  = QPixmap();
        d->m_CurrentStackPix = QPixmap();
        targetWidget->setVisible(true);
      });
      if (isRouteBack) {
        targetPixZoomAnimation->setStartValue(1.5);
        targetPixZoomAnimation->setEndValue(1);
      } else {
        // 放大
        targetPixZoomAnimation->setStartValue(0.85);
        targetPixZoomAnimation->setEndValue(1);
      }
      targetPixZoomAnimation->setDuration(300);
      targetPixZoomAnimation->setEasingCurve(QEasingCurve::OutCubic);
      targetPixZoomAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    });
    if (isRouteBack) {
      // 缩小
      currentPixZoomAnimation->setStartValue(1);
      currentPixZoomAnimation->setEndValue(0.85);
    } else {
      // 放大
      currentPixZoomAnimation->setStartValue(1);
      currentPixZoomAnimation->setEndValue(1.15);
    }
    currentPixZoomAnimation->setDuration(150);
    currentPixZoomAnimation->start(QAbstractAnimation::DeleteWhenStopped);

    QPropertyAnimation *currentPixOpacityAnimation = new QPropertyAnimation(this, "ScaleAnimationPixOpacity");
    connect(currentPixZoomAnimation, &QPropertyAnimation::finished, this, [=]() {
      QPropertyAnimation *targetPixOpacityAnimation = new QPropertyAnimation(this, "ScaleAnimationPixOpacity");
      targetPixOpacityAnimation->setStartValue(0);
      targetPixOpacityAnimation->setEndValue(1);
      targetPixOpacityAnimation->setDuration(300);
      targetPixOpacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    });
    currentPixOpacityAnimation->setStartValue(1);
    currentPixOpacityAnimation->setEndValue(0);
    currentPixOpacityAnimation->setDuration(150);
    currentPixOpacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    break;
  }
  case NXWindowType::Flip : {
    QWidget *targetWidget = d->m_ContainerStackedWidget->widget(nodeIndex);
    d->_getCurrentStackPix();
    d->m_ContainerStackedWidget->setCurrentIndex(nodeIndex);
    d->_getTargetStackPix();
    targetWidget->setVisible(false);
    QPropertyAnimation *flipAnimation = new QPropertyAnimation(this, "FlipAnimationRatio");
    connect(flipAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
      update();
    });
    connect(flipAnimation, &QPropertyAnimation::finished, this, [=]() {
      d->m_TargetStackPix  = QPixmap();
      d->m_CurrentStackPix = QPixmap();
      targetWidget->setVisible(true);
    });
    flipAnimation->setEasingCurve(QEasingCurve::InOutSine);
    flipAnimation->setDuration(650);
    flipAnimation->setStartValue(0);
    flipAnimation->setEndValue(isRouteBack ? -180 : 180);
    flipAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    break;
  }
  case NXWindowType::Blur : {
    d->m_TargetStackPix = QPixmap();
    d->m_BlurEffect->setEnabled(true);
    QPropertyAnimation *blurAnimation = new QPropertyAnimation(this, "BlurAnimationRadius");
    connect(blurAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
      d->m_BlurEffect->setBlurRadius(d->m_BlurAnimationRadius);
    });
    connect(blurAnimation, &QPropertyAnimation::finished, this, [=]() {
      d->m_BlurEffect->setEnabled(false);
    });
    blurAnimation->setEasingCurve(QEasingCurve::InOutSine);
    blurAnimation->setDuration(350);
    blurAnimation->setStartValue(40);
    blurAnimation->setEndValue(2);
    blurAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    QApplication::processEvents();
    d->m_ContainerStackedWidget->setCurrentIndex(nodeIndex);
    break;
  }
  }
}

void NXCentralStackedWidget::paintEvent(QPaintEvent *event)
{
  Q_D(NXCentralStackedWidget);
  QRect targetRect = this->rect();
  targetRect.adjust(1, 1, 10, 10);
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
  if (!d->m_IsTransparent) {
    painter.setPen(QPen(NXThemeColor(d->m_ThemeMode, BasicBaseLine), 1.5));
    painter.setBrush(NXThemeColor(d->m_ThemeMode, WindowCentralStackBase));
    if (d->m_IsHasRadius) {
      painter.drawRoundedRect(targetRect, 10, 10);
    } else {
      painter.drawRect(targetRect);
    }
  }
  // 切换动画
  if (!d->m_TargetStackPix.isNull()) {
    QPoint centralStackPos = d->m_ContainerStackedWidget->mapToParent(QPoint(0, 0));
    QRect centralStackRect = QRect(centralStackPos.x(), centralStackPos.y(), d->m_ContainerStackedWidget->width(), d->m_ContainerStackedWidget->height());
    QPainterPath clipPath;
    clipPath.addRoundedRect(centralStackRect, 10, 10);
    painter.setClipPath(clipPath);
    switch (d->m_StackSwitchMode) {
    case NXWindowType::None : {
      break;
    }
    case NXWindowType::Popup : {
      painter.drawPixmap(QRect(0, d->m_PopupAnimationYOffset, width(), d->m_ContainerStackedWidget->height()), d->m_TargetStackPix);
      break;
    }
    case NXWindowType::Scale : {
      painter.setOpacity(d->m_ScaleAnimationPixOpacity);
      painter.translate(d->m_ContainerStackedWidget->rect().center());
      painter.scale(d->m_ScaleAnimationRatio, d->m_ScaleAnimationRatio);
      painter.translate(-d->m_ContainerStackedWidget->rect().center());
      painter.drawPixmap(centralStackRect, d->m_IsDrawNewPix ? d->m_TargetStackPix : d->m_CurrentStackPix);
      break;
    }
    case NXWindowType::Flip : {
      QTransform transform;
      transform.translate(centralStackRect.center().x(), 0);
      if (abs(d->m_FlipAnimationRatio) >= 90) {
        transform.rotate(-180 + d->m_FlipAnimationRatio, Qt::YAxis);
      } else {
        transform.rotate(d->m_FlipAnimationRatio, Qt::YAxis);
      }
      transform.translate(-centralStackRect.center().x(), 0);
      painter.setTransform(transform);
      if (abs(d->m_FlipAnimationRatio) >= 90) {
        painter.drawPixmap(centralStackRect, d->m_TargetStackPix);
      } else {
        painter.drawPixmap(centralStackRect, d->m_CurrentStackPix);
      }
      break;
    }
    case NXWindowType::Blur : {
      break;
    }
    }
  }
  painter.restore();
}
