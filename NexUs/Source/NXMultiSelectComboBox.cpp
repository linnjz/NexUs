#include "NXMultiSelectComboBox.h"

#include <QApplication>
#include <QDebug>
#include <QLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QPropertyAnimation>

#include "DeveloperComponents/NXComboBoxView.h"
#include "DeveloperComponents/NXMultiSelectComboBoxDelegate.h"
#include "DeveloperComponents/NXComboBoxStyle.h"
#include "NXApplication.h"
#include "NXScrollBar.h"
#include "NXTheme.h"
#include "private/NXMultiSelectComboBoxPrivate.h"
Q_PROPERTY_CREATE_CPP(NXMultiSelectComboBox, QPS_PAEMIT(int), BorderRadius)
Q_PROPERTY_CREATE_CPP(NXMultiSelectComboBox, QPS_PAEMIT(bool), ShowCheckBox)

NXMultiSelectComboBox::NXMultiSelectComboBox(QWidget *parent)
    : QComboBox(parent)
    , d_ptr(new NXMultiSelectComboBoxPrivate())
{
  Q_D(NXMultiSelectComboBox);
  d->q_ptr = this;
#ifdef Q_OS_MACOS
  setAttribute(Qt::WA_Hover);
#endif
  d->m_BorderRadius     = 3;
  d->m_ShowCheckBox     = false; // 默认不显示复选框，保持原样式
  d->m_ExpandIconRotate = 0;
  d->m_ExpandMarkWidth  = 0;
  d->m_ThemeMode         = nxTheme->getThemeMode();
  setFixedHeight(30);

  d->m_ComboBoxStyle = new NXComboBoxStyle(style());
  setStyle(d->m_ComboBoxStyle);

  //调用view 让container初始化
  d->m_ComboView = new NXComboBoxView(this);
  setView(d->m_ComboView);
  QAbstractItemView *comboBoxView = this->view();
  comboBoxView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  NXScrollBar *scrollBar = new NXScrollBar(this);
  comboBoxView->setVerticalScrollBar(scrollBar);
  NXScrollBar *floatVScrollBar = new NXScrollBar(scrollBar, comboBoxView);
  floatVScrollBar->setIsAnimation(true);
  comboBoxView->setAutoScroll(false);
  comboBoxView->setSelectionMode(QAbstractItemView::NoSelection);
  comboBoxView->setSelectionBehavior(QAbstractItemView::SelectRows);
  comboBoxView->setObjectName("NXComboBoxView");
  comboBoxView->setStyleSheet(QStringLiteral("#NXComboBoxView{background-color:transparent;}"));
  comboBoxView->setStyle(d->m_ComboBoxStyle);
  QWidget *container = this->findChild<QFrame *>();
  if (container) {
    container->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    container->setAttribute(Qt::WA_TranslucentBackground);
    container->setObjectName("NXComboBoxContainer");
    container->setStyle(d->m_ComboBoxStyle);
    container->installEventFilter(d);
    QLayout *layout = container->layout();
    while (layout->count()) {
      layout->takeAt(0);
    }
    layout->addWidget(view());
#if defined(Q_OS_WIN) && QT_VERSION == QT_VERSION_CHECK(6, 11, 0)
    layout->setContentsMargins(0, 0, 0, 0);
#else
    layout->setContentsMargins(6, 0, 6, 6);
#endif
#if defined(Q_OS_LINUX) && QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    container->setStyleSheet(QStringLiteral("background-color:transparent;"));
#endif
  }
  QComboBox::setMaxVisibleItems(5);
  connect(d->m_ComboView, &NXComboBoxView::itemPressed, d, &NXMultiSelectComboBoxPrivate::onItemPressed);
  connect(this, QOverload<int>::of(&NXMultiSelectComboBox::currentIndexChanged), d, &NXMultiSelectComboBoxPrivate::_refreshCurrentIndexs);
  d->m_ItemSelection.resize(32);
  d->m_ItemSelection.fill(false);
  d->m_ItemSelection[0] = true;
  QComboBox::setMaxVisibleItems(5);
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
  });

  connect(this, &NXMultiSelectComboBox::signalShowCheckBoxChanged, this, [=]() {
    Q_D(NXMultiSelectComboBox);
    QAbstractItemView *comboBoxView = this->view();
    if (d->m_ShowCheckBox) {
      if (!d->m_Delegate) {
        d->m_Delegate = new NXMultiSelectComboBoxDelegate(this);
        connect(this, &NXMultiSelectComboBox::itemSelectionChanged, d->m_Delegate, &NXMultiSelectComboBoxDelegate::setItemSelection);
        d->m_Delegate->setItemSelection(d->m_ItemSelection);
      }
      comboBoxView->setItemDelegate(d->m_Delegate);
    } else {
      comboBoxView->setItemDelegate(nullptr);
    }
    comboBoxView->viewport()->update();
  });
}

NXMultiSelectComboBox::~NXMultiSelectComboBox()
{
  Q_D(NXMultiSelectComboBox);
  delete d->m_ComboBoxStyle;
}

void NXMultiSelectComboBox::setCurrentSelection(QString const &selection)
{
  Q_D(NXMultiSelectComboBox);
  d->m_ItemSelection.fill(false);
  d->m_ComboView->selectionModel()->clearSelection();
  for (int i = 0; i < this->count(); i++) {
    if (selection == itemText(i)) {
      QModelIndex index = model()->index(i, 0);
      d->m_ComboView->selectionModel()->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
      d->m_ItemSelection[index.row()] = true;
    }
  }
  d->_refreshCurrentIndexs();
}

void NXMultiSelectComboBox::setCurrentSelection(QStringList const &selection)
{
  Q_D(NXMultiSelectComboBox);
  d->m_ComboView->selectionModel()->clearSelection();
  d->m_ItemSelection.fill(false);
  for (int i = 0; i < this->count(); i++) {
    if (selection.contains(itemText(i))) {
      QModelIndex index = model()->index(i, 0);
      d->m_ComboView->selectionModel()->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
      d->m_ItemSelection[index.row()] = true;
    }
  }
  d->_refreshCurrentIndexs();
}

void NXMultiSelectComboBox::setCurrentSelection(int index)
{
  Q_D(NXMultiSelectComboBox);
  if (index >= this->count() || index < 0) {
    return;
  }
  d->m_ComboView->selectionModel()->clearSelection();
  d->m_ItemSelection.fill(false);
  QModelIndex currentIndex = model()->index(index, 0);
  d->m_ComboView->selectionModel()->select(currentIndex, QItemSelectionModel::Select | QItemSelectionModel::Rows);
  d->m_ItemSelection[index] = true;
  d->_refreshCurrentIndexs();
}

void NXMultiSelectComboBox::setCurrentSelection(QList<int> const &selectionIndex)
{
  Q_D(NXMultiSelectComboBox);
  d->m_ItemSelection.fill(false);
  d->m_ComboView->selectionModel()->clearSelection();
  for (auto index : selectionIndex) {
    if (index >= this->count() || index < 0) {
      continue;
    }
    QModelIndex currentIndex = model()->index(index, 0);
    d->m_ComboView->selectionModel()->select(currentIndex, QItemSelectionModel::Select | QItemSelectionModel::Rows);
    d->m_ItemSelection[index] = true;
  }
  d->_refreshCurrentIndexs();
}

QStringList NXMultiSelectComboBox::getCurrentSelection() const
{
  return d_ptr->m_SelectedTextList;
}

QList<int> NXMultiSelectComboBox::getCurrentSelectionIndex() const
{
  QList<int> indexList;
  for (int i = 0; i < d_ptr->m_ItemSelection.count(); i++) {
    if (d_ptr->m_ItemSelection[i]) {
      indexList.append(i);
    }
  }
  return indexList;
}

void NXMultiSelectComboBox::paintEvent(QPaintEvent *e)
{
  Q_D(NXMultiSelectComboBox);
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing | QPainter::TextAntialiasing);
  painter.setPen(NXThemeColor(d->m_ThemeMode, BasicBorder));
  painter.setBrush(isEnabled() ? underMouse() ? NXThemeColor(d->m_ThemeMode, BasicHover) : NXThemeColor(d->m_ThemeMode, BasicBase) : Qt::transparent);
  QRect foregroundRect = rect();
  foregroundRect.adjust(1, 1, -1, -1);
  painter.drawRoundedRect(foregroundRect, d->m_BorderRadius, d->m_BorderRadius);
  // 底边线绘制
  painter.setPen(NXThemeColor(d->m_ThemeMode, BasicBaseLine));
  painter.drawLine(foregroundRect.x() + d->m_BorderRadius, foregroundRect.y() + foregroundRect.height(), foregroundRect.x() + foregroundRect.width() - d->m_BorderRadius, foregroundRect.y() + foregroundRect.height());

  //文字绘制
  painter.setPen(isEnabled() ? NXThemeColor(d->m_ThemeMode, BasicText) : NXThemeColor(d->m_ThemeMode, BasicTextDisable));
  QString currentText =
      painter.fontMetrics().elidedText(d->m_CurrentText, Qt::ElideRight, foregroundRect.width() - 27 - width() * 0.05);
  painter.drawText(15, height() / 2 + painter.fontMetrics().ascent() / 2 - 1, currentText);
  //展开指示器绘制
  painter.setPen(Qt::NoPen);
  painter.setBrush(d->m_ThemeMode == NXThemeType::Light ? QColor(0x0E, 0x6F, 0xC3) : QColor(0x4C, 0xA0, 0xE0));
  painter.drawRoundedRect(QRectF(width() / 2 - d->m_ExpandMarkWidth, height() - 3, d->m_ExpandMarkWidth * 2, 3), 2, 2);
  // 展开图标绘制
  if (count() > 0) {
    QFont iconFont = QFont(QStringLiteral("NXAwesome"));
    iconFont.setPixelSize(nxApp->getFontPixelSize() + 4);
    painter.setFont(iconFont);
    painter.setPen(isEnabled() ? NXThemeColor(d->m_ThemeMode, BasicText) : NXThemeColor(d->m_ThemeMode, BasicTextDisable));
    QRectF expandIconRect(width() - 25, 0, 20, height());
    painter.translate(expandIconRect.x() + (qreal) expandIconRect.width() / 2 - 2, expandIconRect.y() + (qreal) expandIconRect.height() / 2);
    painter.rotate(d->m_ExpandIconRotate);
    painter.translate(-expandIconRect.x() - (qreal) expandIconRect.width() / 2 + 2, -expandIconRect.y() - (qreal) expandIconRect.height() / 2);
    painter.drawText(expandIconRect, Qt::AlignVCenter, QChar((unsigned short) NXIconType::AngleDown));
    painter.restore();
  }
}

void NXMultiSelectComboBox::showPopup()
{
  Q_D(NXMultiSelectComboBox);
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
      int const shadowBorderWidth = d->m_ComboBoxStyle->getShadowBorderWidth();
      container->resize(width() + 2 * shadowBorderWidth, containerHeight);
      container->move(container->x() - shadowBorderWidth, container->y() + 3);
      view()->resize(width(), containerHeight - 8);
      QLayout *layout = container->layout();
      while (layout->count()) {
        layout->takeAt(0);
      }
      QPropertyAnimation *fixedSizeAnimation = new QPropertyAnimation(container, "maximumHeight");
      connect(fixedSizeAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
        container->setFixedHeight(value.toUInt());
      });
      fixedSizeAnimation->setStartValue(1);
      fixedSizeAnimation->setEndValue(containerHeight);
      fixedSizeAnimation->setEasingCurve(QEasingCurve::OutCubic);
      fixedSizeAnimation->setDuration(400);
      fixedSizeAnimation->start(QAbstractAnimation::DeleteWhenStopped);

      QPropertyAnimation *viewPosAnimation = new QPropertyAnimation(view(), "pos");
      connect(viewPosAnimation, &QPropertyAnimation::finished, this, [=]() {
        d->m_IsAllowHidePopup = true;
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
    QPropertyAnimation *rotateAnimation = new QPropertyAnimation(d, "ExpandIconRotate");
    connect(rotateAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
      update();
    });
    rotateAnimation->setDuration(300);
    rotateAnimation->setEasingCurve(QEasingCurve::InOutSine);
    rotateAnimation->setStartValue(d->m_ExpandIconRotate);
    rotateAnimation->setEndValue(-180);
    rotateAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    QPropertyAnimation *markAnimation = new QPropertyAnimation(d, "ExpandMarkWidth");
    markAnimation->setDuration(300);
    markAnimation->setEasingCurve(QEasingCurve::InOutSine);
    markAnimation->setStartValue(d->m_ExpandMarkWidth);
    qreal step = (width() / 2 - d->m_BorderRadius) / count();
    markAnimation->setEndValue(step * d->m_SelectedTextList.count());
    markAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  }
  d->_refreshCurrentIndexs();
}

void NXMultiSelectComboBox::_resetIndicatorAnimations()
{
  Q_D(NXMultiSelectComboBox);
  if (qFuzzyIsNull(d->m_ExpandIconRotate) && d->m_ExpandMarkWidth == 0) {
    return;
  }
  QPropertyAnimation *rotateAnimation = new QPropertyAnimation(d, "ExpandIconRotate");
  connect(rotateAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
    update();
  });
  rotateAnimation->setDuration(300);
  rotateAnimation->setEasingCurve(QEasingCurve::InOutSine);
  rotateAnimation->setStartValue(d->m_ExpandIconRotate);
  rotateAnimation->setEndValue(0);
  rotateAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  QPropertyAnimation *markAnimation = new QPropertyAnimation(d, "ExpandMarkWidth");
  markAnimation->setDuration(300);
  markAnimation->setEasingCurve(QEasingCurve::InOutSine);
  markAnimation->setStartValue(d->m_ExpandMarkWidth);
  markAnimation->setEndValue(0);
  markAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void NXMultiSelectComboBox::hidePopup()
{
  Q_D(NXMultiSelectComboBox);
  if (d->m_IsFirstPopup && !this->view()->underMouse()) {
    d->m_IsFirstPopup = false;
    return;
  }
  if (nxApp->containsCursorToItem(d->m_ComboView)) {
    return;
  } else {
    if (d->m_IsAllowHidePopup) {
      QWidget *container  = this->findChild<QFrame *>();
      int containerHeight = container->height();
      if (container) {
        QLayout *layout = container->layout();
        while (layout->count()) {
          layout->takeAt(0);
        }
        QPropertyAnimation *viewPosAnimation = new QPropertyAnimation(view(), "pos");
        connect(viewPosAnimation, &QPropertyAnimation::finished, this, [=]() {
          layout->addWidget(view());
          QMouseEvent focusEvent(QEvent::MouseButtonPress, QPoint(-1, -1), QPoint(-1, -1), Qt::NoButton, Qt::NoButton, Qt::NoModifier);
          QApplication::sendEvent(parentWidget(), &focusEvent);
          QComboBox::hidePopup();
          container->setFixedHeight(containerHeight);
        });
        QPoint viewPos = view()->pos();
        connect(viewPosAnimation, &QPropertyAnimation::finished, this, [=]() {
          view()->move(viewPos);
        });
        viewPosAnimation->setStartValue(viewPos);
        viewPosAnimation->setEndValue(QPoint(viewPos.x(), viewPos.y() - view()->height()));
        viewPosAnimation->setEasingCurve(QEasingCurve::InCubic);
        viewPosAnimation->start(QAbstractAnimation::DeleteWhenStopped);

        QPropertyAnimation *fixedSizeAnimation = new QPropertyAnimation(container, "maximumHeight");
        connect(fixedSizeAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
          container->setFixedHeight(value.toUInt());
        });
        fixedSizeAnimation->setStartValue(container->height());
        fixedSizeAnimation->setEndValue(1);
        fixedSizeAnimation->setEasingCurve(QEasingCurve::InCubic);
        fixedSizeAnimation->start(QAbstractAnimation::DeleteWhenStopped);
        d->m_IsAllowHidePopup = false;
      }
      _resetIndicatorAnimations();
    }
  }
}
