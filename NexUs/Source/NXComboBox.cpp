#include "NXComboBox.h"

#include <QAbstractItemView>
#include <QApplication>
#include <QLayout>
#include <QLineEdit>
#include <QListView>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QScreen>
#include <QVBoxLayout>

#include "DeveloperComponents/NXComboBoxStyle.h"
#include "NXScrollBar.h"
#include "NXTheme.h"
#include "private/NXComboBoxPrivate.h"
Q_PROPERTY_CREATE_CPP(NXComboBox, int, BorderRadius)

NXComboBox::NXComboBox(QWidget *parent)
    : QComboBox(parent)
    , d_ptr(new NXComboBoxPrivate())
{
  Q_D(NXComboBox);
  d->q_ptr          = this;
  d->_pBorderRadius = 3;
  d->_themeMode     = nxTheme->getThemeMode();
  setObjectName("NXComboBox");
  setFixedHeight(30);
  d->_comboBoxStyle = new NXComboBoxStyle(style());
  setStyle(d->_comboBoxStyle);

  // 调用view 让container初始化
  setView(new QListView(this));
  QAbstractItemView *comboBoxView = this->view();
  comboBoxView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  NXScrollBar *scrollBar = new NXScrollBar(this);
  comboBoxView->setVerticalScrollBar(scrollBar);
  NXScrollBar *floatVScrollBar = new NXScrollBar(scrollBar, comboBoxView);
  floatVScrollBar->setIsAnimation(true);
  comboBoxView->setAutoScroll(false);
  comboBoxView->setSelectionMode(QAbstractItemView::NoSelection);
  comboBoxView->setObjectName("NXComboBoxView");
  comboBoxView->setStyleSheet(QStringLiteral("#NXComboBoxView{background-color:transparent;}"));
  comboBoxView->setStyle(d->_comboBoxStyle);
  auto *container = qobject_cast<QWidget *>(view()->parentWidget());
  if (!container || container == this) {
    container = qobject_cast<QWidget *>(view()->window());
  }
  if (container && container != this && container != view()) {
    container->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    container->setAttribute(Qt::WA_TranslucentBackground);
    container->setObjectName("NXComboBoxContainer");
    container->setStyle(d->_comboBoxStyle);
    QLayout *layout = container->layout();
    if (!layout) {
      layout = new QVBoxLayout(container);
    }
    while (layout->count()) {
      layout->takeAt(0);
    }
    layout->addWidget(view());
    layout->setContentsMargins(6, 0, 6, 6);
#ifndef Q_OS_WIN
#  if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    container->setStyleSheet(QStringLiteral("background-color:transparent;"));
#  endif
#endif
  }
  QComboBox::setMaxVisibleItems(5);
  connect(nxTheme, &NXTheme::themeModeChanged, d, &NXComboBoxPrivate::onThemeChanged);
}

NXComboBox::~NXComboBox()
{
  Q_D(NXComboBox);
  delete d->_comboBoxStyle;
}

void NXComboBox::setEditable(bool editable)
{
  Q_D(NXComboBox);
  QComboBox::setEditable(editable);
  if (editable) {
    lineEdit()->setStyle(d->_comboBoxStyle);
    d->onThemeChanged(d->_themeMode);
  }
}

void NXComboBox::showPopup()
{
  Q_D(NXComboBox);
  bool oldAnimationEffects = qApp->isEffectEnabled(Qt::UI_AnimateCombo);
  qApp->setEffectEnabled(Qt::UI_AnimateCombo, false);
  QComboBox::showPopup();
  qApp->setEffectEnabled(Qt::UI_AnimateCombo, oldAnimationEffects);
  if (count() > 0) {
    QWidget *container = this->findChild<QFrame *>();
    if (container) {
      int containerHeight = 0;
      if (count() >= maxVisibleItems()) {
        containerHeight = maxVisibleItems() * 35 + 8;
      } else {
        containerHeight = count() * 35 + 8;
      }
      // 容器实际宽度在组合框宽度基础上两侧各扩展阴影宽度
      // 阴影绘制于外围 前景绘制宽度与组合框宽度保持一致
      const int shadowBorderWidth = d->_comboBoxStyle->getShadowBorderWidth();
      container->resize(width() + 2 * shadowBorderWidth, containerHeight);
      container->move(container->x() - shadowBorderWidth, container->y() + 3);
      view()->resize(width(), containerHeight - 8);
      QLayout *layout = container->layout();
      while (layout->count()) {
        layout->takeAt(0);
      }
      QPropertyAnimation *fixedSizeAnimation = new QPropertyAnimation(container, "maximumHeight");
      connect(fixedSizeAnimation, &QPropertyAnimation::valueChanged, this, [=](const QVariant &value) {
        container->setFixedHeight(value.toUInt());
      });
      fixedSizeAnimation->setStartValue(1);
      fixedSizeAnimation->setEndValue(containerHeight);
      fixedSizeAnimation->setEasingCurve(QEasingCurve::OutCubic);
      fixedSizeAnimation->setDuration(400);
      fixedSizeAnimation->start(QAbstractAnimation::DeleteWhenStopped);

      QPropertyAnimation *viewPosAnimation = new QPropertyAnimation(view(), "pos");
      connect(viewPosAnimation, &QPropertyAnimation::finished, this, [=]() {
        d->_isAllowHidePopup = true;
        layout->addWidget(view());
      });
      QPoint viewPos = view()->pos();
      viewPosAnimation->setStartValue(QPoint(viewPos.x(), viewPos.y() - view()->height()));
      viewPosAnimation->setEndValue(viewPos);
      viewPosAnimation->setEasingCurve(QEasingCurve::OutCubic);
      viewPosAnimation->setDuration(400);
      viewPosAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    }
    //指示器动画
    QPropertyAnimation *rotateAnimation = new QPropertyAnimation(d->_comboBoxStyle, "pExpandIconRotate");
    connect(rotateAnimation, &QPropertyAnimation::valueChanged, this, [=](const QVariant &value) {
      update();
    });
    rotateAnimation->setDuration(300);
    rotateAnimation->setEasingCurve(QEasingCurve::InOutSine);
    rotateAnimation->setStartValue(d->_comboBoxStyle->getExpandIconRotate());
    rotateAnimation->setEndValue(-180);
    rotateAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    QPropertyAnimation *markAnimation = new QPropertyAnimation(d->_comboBoxStyle, "pExpandMarkWidth");
    markAnimation->setDuration(300);
    markAnimation->setEasingCurve(QEasingCurve::InOutSine);
    markAnimation->setStartValue(d->_comboBoxStyle->getExpandMarkWidth());
    markAnimation->setEndValue(width() / 2 - d->_pBorderRadius - 6);
    markAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  }
}

void NXComboBox::_resetIndicatorAnimations()
{
  Q_D(NXComboBox);
  if (qFuzzyIsNull(d->_comboBoxStyle->getExpandIconRotate()) && d->_comboBoxStyle->getExpandMarkWidth() == 0) {
    return;
  }
  QPropertyAnimation *rotateAnimation = new QPropertyAnimation(d->_comboBoxStyle, "pExpandIconRotate");
  connect(rotateAnimation, &QPropertyAnimation::valueChanged, this, [=](const QVariant &value) {
    update();
  });
  rotateAnimation->setDuration(300);
  rotateAnimation->setEasingCurve(QEasingCurve::InOutSine);
  rotateAnimation->setStartValue(d->_comboBoxStyle->getExpandIconRotate());
  rotateAnimation->setEndValue(0);
  rotateAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  QPropertyAnimation *markAnimation = new QPropertyAnimation(d->_comboBoxStyle, "pExpandMarkWidth");
  markAnimation->setDuration(300);
  markAnimation->setEasingCurve(QEasingCurve::InOutSine);
  markAnimation->setStartValue(d->_comboBoxStyle->getExpandMarkWidth());
  markAnimation->setEndValue(0);
  markAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void NXComboBox::hidePopup()
{
  Q_D(NXComboBox);
  _resetIndicatorAnimations();

  if (d->_isAllowHidePopup) {
    QWidget *container  = this->findChild<QFrame *>();
    int containerHeight = container->height();
    if (container) {
      QLayout *layout = container->layout();
      while (layout->count()) {
        layout->takeAt(0);
      }
      QPropertyAnimation *viewPosAnimation = new QPropertyAnimation(view(), "pos");
      QPoint viewPos                       = view()->pos();
      connect(viewPosAnimation, &QPropertyAnimation::finished, this, [=]() {
        layout->addWidget(view());
        QMouseEvent focusEvent(QEvent::MouseButtonPress, QPoint(-1, -1), QPoint(-1, -1), Qt::NoButton, Qt::NoButton, Qt::NoModifier);
        QApplication::sendEvent(parentWidget(), &focusEvent);
        QComboBox::hidePopup();
        container->setFixedHeight(containerHeight);
        view()->move(viewPos);
      });
      viewPosAnimation->setStartValue(viewPos);
      viewPosAnimation->setEndValue(QPoint(viewPos.x(), viewPos.y() - view()->height()));
      viewPosAnimation->setEasingCurve(QEasingCurve::InCubic);
      viewPosAnimation->start(QAbstractAnimation::DeleteWhenStopped);

      QPropertyAnimation *fixedSizeAnimation = new QPropertyAnimation(container, "maximumHeight");
      connect(fixedSizeAnimation, &QPropertyAnimation::valueChanged, this, [=](const QVariant &value) {
        container->setFixedHeight(value.toUInt());
      });
      fixedSizeAnimation->setStartValue(container->height());
      fixedSizeAnimation->setEndValue(1);
      fixedSizeAnimation->setEasingCurve(QEasingCurve::InCubic);
      fixedSizeAnimation->start(QAbstractAnimation::DeleteWhenStopped);
      d->_isAllowHidePopup = false;
    }
  } else {
    QComboBox::hidePopup();
  }
}

void NXComboBox::paintEvent(QPaintEvent *event)
{
  Q_D(NXComboBox);
  if (lineEdit() && lineEdit()->palette().color(QPalette::Text) != NXThemeColor(d->_themeMode, BasicText)) {
    d->onThemeChanged(d->_themeMode);
  }
  QComboBox::paintEvent(event);
}
