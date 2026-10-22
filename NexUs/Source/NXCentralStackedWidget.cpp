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

Q_PROPERTY_CREATE_CPP(NXCentralStackedWidget, int, LastTargetIndex)
Q_PROPERTY_CREATE_CPP(NXCentralStackedWidget, int, BlurAnimationRadius)
Q_PROPERTY_CREATE_CPP(NXCentralStackedWidget, int, PopupAnimationYOffset)
Q_PROPERTY_CREATE_CPP(NXCentralStackedWidget, qreal, ScaleAnimationRatio)
Q_PROPERTY_CREATE_CPP(NXCentralStackedWidget, qreal, ScaleAnimationPixOpacity)
Q_PROPERTY_CREATE_CPP(NXCentralStackedWidget, qreal, FlipAnimationRatio)

NXCentralStackedWidget::NXCentralStackedWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new NXCentralStackedWidgetPrivate())
{
  Q_D(NXCentralStackedWidget);
  d->q_ptr                      = this;
  d->_pLastTargetIndex          = 0;
  d->_pPopupAnimationYOffset    = 0;
  d->_pScaleAnimationRatio      = 1;
  d->_pScaleAnimationPixOpacity = 1;
  d->_pFlipAnimationRatio       = 1;
  d->_pBlurAnimationRadius      = 0;

  setObjectName("NXCentralStackedWidget");
  setStyleSheet(QStringLiteral("#NXCentralStackedWidget{background-color:transparent;}"));

  d->_containerStackedWidget = new NXLazyStackedWidget(this);
  d->_containerStackedWidget->setObjectName("NXCentralStackedWidget");
  d->_containerStackedWidget->setStyleSheet(QStringLiteral("#NXCentralStackedWidget{background-color:transparent;}"));

  d->_blurEffect = new QGraphicsBlurEffect(d->_containerStackedWidget);
  d->_blurEffect->setBlurHints(QGraphicsBlurEffect::BlurHint::QualityHint);
  d->_blurEffect->setBlurRadius(0);
  d->_blurEffect->setEnabled(false);
  d->_containerStackedWidget->setGraphicsEffect(d->_blurEffect);

  d->_mainLayout = new QVBoxLayout(this);
  d->_mainLayout->setSpacing(0);
  d->_mainLayout->setContentsMargins(0, 0, 0, 0);
  d->_mainLayout->addWidget(d->_containerStackedWidget);

  d->_themeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, d, &NXCentralStackedWidgetPrivate::onThemeModeChanged);
}

NXCentralStackedWidget::~NXCentralStackedWidget()
{
}

NXLazyStackedWidget * NXCentralStackedWidget::getContainerStackedWidget() const
{
  return d_ptr->_containerStackedWidget;
}

void NXCentralStackedWidget::setCustomWidget(QWidget *widget)
{
  Q_D(NXCentralStackedWidget);
  if (!widget) {
    return;
  }
  if (d->_customWidget) {
    d->_mainLayout->removeWidget(d->_customWidget);
  }
  d->_mainLayout->insertWidget(0, widget);
  d->_customWidget = widget;
}

QWidget * NXCentralStackedWidget::getCustomWidget() const
{
  return d_ptr->_customWidget;
}

void NXCentralStackedWidget::setIsTransparent(bool isTransparent)
{
  d_ptr->_isTransparent = isTransparent;
  update();
}

bool NXCentralStackedWidget::getIsTransparent() const
{
  return d_ptr->_isTransparent;
}

void NXCentralStackedWidget::setIsHasRadius(bool isHasRadius)
{
  d_ptr->_isHasRadius = isHasRadius;
  update();
}

void NXCentralStackedWidget::doWindowStackSwitch(NXWindowType::StackSwitchMode stackSwitchMode, int nodeIndex, bool isRouteBack)
{
  Q_D(NXCentralStackedWidget);
  if (d->_pLastTargetIndex == nodeIndex) {
    return;
  }
  d->_pLastTargetIndex = nodeIndex;
  d->_stackSwitchMode  = stackSwitchMode;
  switch (stackSwitchMode) {
  case NXWindowType::None : {
    d->_containerStackedWidget->setCurrentIndex(nodeIndex);
    break;
  }
  case NXWindowType::Popup : {
    QTimer::singleShot(180, this, [=]() {
      QWidget *targetWidget = d->_containerStackedWidget->widget(nodeIndex);
      d->_containerStackedWidget->setCurrentIndex(nodeIndex);
      d->_getTargetStackPix();
      targetWidget->setVisible(false);
      QPropertyAnimation *popupAnimation = new QPropertyAnimation(this, "pPopupAnimationYOffset");
      connect(popupAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
        update();
      });
      connect(popupAnimation, &QPropertyAnimation::finished, this, [=]() {
        d->_targetStackPix = QPixmap();
        targetWidget->setVisible(true);
      });
      popupAnimation->setEasingCurve(QEasingCurve::OutCubic);
      popupAnimation->setDuration(300);
      int targetWidgetY = d->_containerStackedWidget->mapToParent(QPoint(0, 0)).y();
      popupAnimation->setEndValue(targetWidgetY);
      targetWidgetY += 80;
      popupAnimation->setStartValue(targetWidgetY);
      popupAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    });
    break;
  }
  case NXWindowType::Scale : {
    QWidget *targetWidget = d->_containerStackedWidget->widget(nodeIndex);
    d->_getCurrentStackPix();
    d->_containerStackedWidget->setCurrentIndex(nodeIndex);
    targetWidget->setVisible(true);
    d->_getTargetStackPix();
    targetWidget->setVisible(false);
    d->_isDrawNewPix                            = false;
    QPropertyAnimation *currentPixZoomAnimation = new QPropertyAnimation(this, "pScaleAnimationRatio");
    connect(currentPixZoomAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
      update();
    });
    connect(currentPixZoomAnimation, &QPropertyAnimation::finished, this, [=]() {
      d->_isDrawNewPix                           = true;
      QPropertyAnimation *targetPixZoomAnimation = new QPropertyAnimation(this, "pScaleAnimationRatio");
      connect(targetPixZoomAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
        update();
      });
      connect(targetPixZoomAnimation, &QPropertyAnimation::finished, this, [=]() {
        d->_targetStackPix  = QPixmap();
        d->_currentStackPix = QPixmap();
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

    QPropertyAnimation *currentPixOpacityAnimation = new QPropertyAnimation(this, "pScaleAnimationPixOpacity");
    connect(currentPixZoomAnimation, &QPropertyAnimation::finished, this, [=]() {
      QPropertyAnimation *targetPixOpacityAnimation = new QPropertyAnimation(this, "pScaleAnimationPixOpacity");
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
    QWidget *targetWidget = d->_containerStackedWidget->widget(nodeIndex);
    d->_getCurrentStackPix();
    d->_containerStackedWidget->setCurrentIndex(nodeIndex);
    d->_getTargetStackPix();
    targetWidget->setVisible(false);
    QPropertyAnimation *flipAnimation = new QPropertyAnimation(this, "pFlipAnimationRatio");
    connect(flipAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
      update();
    });
    connect(flipAnimation, &QPropertyAnimation::finished, this, [=]() {
      d->_targetStackPix  = QPixmap();
      d->_currentStackPix = QPixmap();
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
    d->_targetStackPix = QPixmap();
    d->_blurEffect->setEnabled(true);
    QPropertyAnimation *blurAnimation = new QPropertyAnimation(this, "pBlurAnimationRadius");
    connect(blurAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
      d->_blurEffect->setBlurRadius(d->_pBlurAnimationRadius);
    });
    connect(blurAnimation, &QPropertyAnimation::finished, this, [=]() {
      d->_blurEffect->setEnabled(false);
    });
    blurAnimation->setEasingCurve(QEasingCurve::InOutSine);
    blurAnimation->setDuration(350);
    blurAnimation->setStartValue(40);
    blurAnimation->setEndValue(2);
    blurAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    QApplication::processEvents();
    d->_containerStackedWidget->setCurrentIndex(nodeIndex);
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
  if (!d->_isTransparent) {
    painter.setPen(QPen(NXThemeColor(d->_themeMode, BasicBaseLine), 1.5));
    painter.setBrush(NXThemeColor(d->_themeMode, WindowCentralStackBase));
    if (d->_isHasRadius) {
      painter.drawRoundedRect(targetRect, 10, 10);
    } else {
      painter.drawRect(targetRect);
    }
  }
  // 切换动画
  if (!d->_targetStackPix.isNull()) {
    QPoint centralStackPos = d->_containerStackedWidget->mapToParent(QPoint(0, 0));
    QRect centralStackRect = QRect(centralStackPos.x(), centralStackPos.y(), d->_containerStackedWidget->width(), d->_containerStackedWidget->height());
    QPainterPath clipPath;
    clipPath.addRoundedRect(centralStackRect, 10, 10);
    painter.setClipPath(clipPath);
    switch (d->_stackSwitchMode) {
    case NXWindowType::None : {
      break;
    }
    case NXWindowType::Popup : {
      painter.drawPixmap(QRect(0, d->_pPopupAnimationYOffset, width(), d->_containerStackedWidget->height()), d->_targetStackPix);
      break;
    }
    case NXWindowType::Scale : {
      painter.setOpacity(d->_pScaleAnimationPixOpacity);
      painter.translate(d->_containerStackedWidget->rect().center());
      painter.scale(d->_pScaleAnimationRatio, d->_pScaleAnimationRatio);
      painter.translate(-d->_containerStackedWidget->rect().center());
      painter.drawPixmap(centralStackRect, d->_isDrawNewPix ? d->_targetStackPix : d->_currentStackPix);
      break;
    }
    case NXWindowType::Flip : {
      QTransform transform;
      transform.translate(centralStackRect.center().x(), 0);
      if (abs(d->_pFlipAnimationRatio) >= 90) {
        transform.rotate(-180 + d->_pFlipAnimationRatio, Qt::YAxis);
      } else {
        transform.rotate(d->_pFlipAnimationRatio, Qt::YAxis);
      }
      transform.translate(-centralStackRect.center().x(), 0);
      painter.setTransform(transform);
      if (abs(d->_pFlipAnimationRatio) >= 90) {
        painter.drawPixmap(centralStackRect, d->_targetStackPix);
      } else {
        painter.drawPixmap(centralStackRect, d->_currentStackPix);
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
