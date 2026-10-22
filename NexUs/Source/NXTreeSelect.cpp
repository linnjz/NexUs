#include "NXTreeSelect.h"

#include <QApplication>
#include <QMouseEvent>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QVBoxLayout>

#include "NXLineEdit.h"
#include "NXTheme.h"
#include "NXTreeView.h"
#include "private/NXTreeSelectPrivate.h"

Q_PROPERTY_CREATE_CPP(NXTreeSelect, QS_SET_CREF(QString), PlaceholderText)
Q_PROPERTY_CREATE_CPP(NXTreeSelect, int, BorderRadius)
Q_PROPERTY_CREATE_CPP(NXTreeSelect, int, MaxVisibleItems)
Q_PROPERTY_CREATE_CPP(NXTreeSelect, bool, IsSearchVisible)
Q_PROPERTY_CREATE_CPP(NXTreeSelect, bool, IsEditable)

NXTreeSelect::NXTreeSelect(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new NXTreeSelectPrivate())
{
  Q_D(NXTreeSelect);
  d->q_ptr             = this;
  d->_pBorderRadius    = 3;
  d->_pItemHeight      = 35;
  d->_pMaxVisibleItems = 8;
  d->_pPlaceholderText = QStringLiteral("");
  d->_pIsSearchVisible = true;
  d->_pIsEditable      = false;
  d->_themeMode        = nxTheme->getThemeMode();
  setObjectName("NXTreeSelect");
  setFixedHeight(35);
  setMinimumWidth(180);
  setMouseTracking(true);
  setCursor(Qt::PointingHandCursor);

  d->_popupContainer = new NXTreeSelectPopupWidget(nullptr);
  d->_popupContainer->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
  d->_popupContainer->setAttribute(Qt::WA_TranslucentBackground);
  d->_popupContainer->_themeMode = d->_themeMode;
  d->_popupContainer->setObjectName("NXTreeSelectPopup");
  d->_popupContainer->hide();

  QVBoxLayout *popupLayout = new QVBoxLayout(d->_popupContainer);
#if defined(Q_OS_WIN) && QT_VERSION == QT_VERSION_CHECK(6, 11, 0)
  popupLayout->setContentsMargins(4, 4, 4, 4);
#else
  popupLayout->setContentsMargins(6, 6, 6, 6);
#endif
  popupLayout->setSpacing(4);

  d->_searchEdit = new NXLineEdit(d->_popupContainer);
  d->_searchEdit->setPlaceholderText(QStringLiteral("搜索..."));
  d->_searchEdit->setFixedHeight(30);
  d->_searchEdit->setIsClearButtonEnable(true);
  popupLayout->addWidget(d->_searchEdit);

  d->_treeView = new NXTreeView(d->_popupContainer);
  d->_treeView->setHeaderHidden(true);
  d->_treeView->setRootIsDecorated(true);
  d->_treeView->setSelectionMode(QAbstractItemView::SingleSelection);
  d->_treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  d->_treeView->setObjectName("NXTreeSelectView");
  d->_treeView->setStyleSheet(QStringLiteral("#NXTreeSelectView{background-color:transparent;}"));
  popupLayout->addWidget(d->_treeView);

  d->_proxyModel = new QSortFilterProxyModel(this);
  d->_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
  d->_proxyModel->setRecursiveFilteringEnabled(true);

  connect(d->_treeView, &QTreeView::clicked, d, &NXTreeSelectPrivate::onItemClicked);
  connect(d->_treeView, &QTreeView::expanded, d, &NXTreeSelectPrivate::onTreeExpanded);
  connect(d->_treeView, &QTreeView::collapsed, d, &NXTreeSelectPrivate::onTreeCollapsed);
  connect(d->_searchEdit, &NXLineEdit::textChanged, d, &NXTreeSelectPrivate::onSearchTextChanged);
  connect(nxTheme, &NXTheme::themeModeChanged, d, &NXTreeSelectPrivate::onThemeChanged);
}

NXTreeSelect::~NXTreeSelect()
{
  d_ptr->_popupContainer->deleteLater();
}

void NXTreeSelect::setItemHeight(int itemHeight)
{
  Q_D(NXTreeSelect);
  if (itemHeight > 0) {
    d->_pItemHeight = itemHeight;
    d->_treeView->setItemHeight(itemHeight);
  }
}

int NXTreeSelect::getItemHeight() const
{
  return d_ptr->_pItemHeight;
}

void NXTreeSelect::setModel(QStandardItemModel *model)
{
  Q_D(NXTreeSelect);
  d->_model = model;
  if (model) {
    d->_proxyModel->setSourceModel(model);
    d->_treeView->setModel(d->_proxyModel);
  }
}

QStandardItemModel * NXTreeSelect::model() const
{
  return d_ptr->_model;
}

void NXTreeSelect::setCurrentIndex(const QModelIndex &index)
{
  Q_D(NXTreeSelect);
  d->_currentIndex = index;
  d->_updateDisplayText();
  if (d->_proxyModel && index.isValid()) {
    QModelIndex proxyIndex = d->_proxyModel->mapFromSource(index);
    d->_treeView->setCurrentIndex(proxyIndex);
  }
  update();
}

QModelIndex NXTreeSelect::currentIndex() const
{
  return d_ptr->_currentIndex;
}

QString NXTreeSelect::currentText() const
{
  return d_ptr->_currentText;
}

void NXTreeSelect::expandAll()
{
  Q_D(NXTreeSelect);
  d->_treeView->expandAll();
}

void NXTreeSelect::collapseAll()
{
  Q_D(NXTreeSelect);
  d->_treeView->collapseAll();
}

void NXTreeSelect::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event)
  Q_D(NXTreeSelect);
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  QRectF bgRect(0, 0, width(), height());
  QPainterPath bgPath;
  bgPath.addRoundedRect(bgRect, d->_pBorderRadius, d->_pBorderRadius);

  if (d->_isPressed) {
    painter.setPen(NXThemeColor(d->_themeMode, BasicBorderDeep));
    painter.setBrush(NXThemeColor(d->_themeMode, BasicPress));
  } else if (d->_isHover) {
    painter.setPen(NXThemeColor(d->_themeMode, BasicBorderHover));
    painter.setBrush(NXThemeColor(d->_themeMode, BasicHover));
  } else {
    painter.setPen(NXThemeColor(d->_themeMode, BasicBorder));
    painter.setBrush(NXThemeColor(d->_themeMode, BasicBase));
  }
  painter.drawPath(bgPath);

  if (d->_isPopupVisible) {
    painter.setPen(Qt::NoPen);
    painter.setBrush(NXThemeColor(d->_themeMode, PrimaryNormal));
    QRectF indicatorRect(d->_pBorderRadius + 6, height() - 3, width() - 2 * (d->_pBorderRadius + 6), 3);
    painter.drawRoundedRect(indicatorRect, 1.5, 1.5);
  }

  QRectF textRect(10, 0, width() - 35, height());
  if (d->_currentText.isEmpty()) {
    painter.setPen(NXThemeColor(d->_themeMode, BasicTextNoFocus));
    painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, d->_pPlaceholderText);
  } else {
    painter.setPen(NXThemeColor(d->_themeMode, BasicText));
    QString elidedText = painter.fontMetrics().elidedText(d->_currentText, Qt::ElideRight, textRect.width());
    painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, elidedText);
  }

  painter.setPen(NXThemeColor(d->_themeMode, BasicText));
  QFont iconFont = QFont(QStringLiteral("NXAwesome"));
  iconFont.setPixelSize(13);
  painter.setFont(iconFont);
  QRectF iconRect(width() - 25, 0, 20, height());
  painter.translate(iconRect.center());
  painter.rotate(d->_expandIconRotate);
  painter.translate(-iconRect.center());
  painter.drawText(iconRect, Qt::AlignCenter, QChar((unsigned short) NXIconType::ChevronDown));

  painter.restore();
}

void NXTreeSelect::mousePressEvent(QMouseEvent *event)
{
  Q_D(NXTreeSelect);
  d->_isPressed = true;
  update();
  QWidget::mousePressEvent(event);
}

void NXTreeSelect::mouseReleaseEvent(QMouseEvent *event)
{
  Q_D(NXTreeSelect);
  d->_isPressed = false;
  if (rect().contains(event->pos())) {
    if (d->_isPopupVisible) {
      d->_hidePopup();
    } else {
      d->_showPopup();
    }
  }
  update();
  QWidget::mouseReleaseEvent(event);
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void NXTreeSelect::enterEvent(QEnterEvent *event)
#else
void NXTreeSelect::enterEvent(QEvent *event)
#endif
{
  Q_D(NXTreeSelect);
  d->_isHover = true;
  update();
  QWidget::enterEvent(event);
}

void NXTreeSelect::leaveEvent(QEvent *event)
{
  Q_D(NXTreeSelect);
  d->_isHover = false;
  update();
  QWidget::leaveEvent(event);
}
