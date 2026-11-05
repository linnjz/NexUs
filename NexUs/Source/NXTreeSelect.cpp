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

Q_PROPERTY_CREATE_CPP(NXTreeSelect, QPS_SCREF_PAEMIT(QString), PlaceholderText)
Q_PROPERTY_CREATE_CPP(NXTreeSelect, QPS_PAEMIT(int), BorderRadius)
Q_PROPERTY_CREATE_CPP(NXTreeSelect, QPS_PAEMIT(int), MaxVisibleItems)
Q_PROPERTY_CREATE_CPP(NXTreeSelect, QPS_PAEMIT(bool), IsSearchVisible)
Q_PROPERTY_CREATE_CPP(NXTreeSelect, QPS_PAEMIT(bool), IsEditable)

NXTreeSelect::NXTreeSelect(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new NXTreeSelectPrivate())
{
  Q_D(NXTreeSelect);
  d->q_ptr             = this;
  d->m_BorderRadius    = 3;
  d->m_ItemHeight      = 35;
  d->m_MaxVisibleItems = 8;
  d->m_PlaceholderText = QStringLiteral("");
  d->m_IsSearchVisible = true;
  d->m_IsEditable      = false;
  d->m_ThemeMode        = nxTheme->getThemeMode();
  setObjectName("NXTreeSelect");
  setFixedHeight(35);
  setMinimumWidth(180);
  setMouseTracking(true);
  setCursor(Qt::PointingHandCursor);

  d->m_PopupContainer = new NXTreeSelectPopupWidget(nullptr);
  d->m_PopupContainer->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
  d->m_PopupContainer->setAttribute(Qt::WA_TranslucentBackground);
  d->m_PopupContainer->m_ThemeMode = d->m_ThemeMode;
  d->m_PopupContainer->setObjectName("NXTreeSelectPopup");
  d->m_PopupContainer->hide();

  QVBoxLayout *popupLayout = new QVBoxLayout(d->m_PopupContainer);
#if defined(Q_OS_WIN) && QT_VERSION == QT_VERSION_CHECK(6, 11, 0)
  popupLayout->setContentsMargins(4, 4, 4, 4);
#else
  popupLayout->setContentsMargins(6, 6, 6, 6);
#endif
  popupLayout->setSpacing(4);

  d->m_SearchEdit = new NXLineEdit(d->m_PopupContainer);
  d->m_SearchEdit->setPlaceholderText(QStringLiteral("搜索..."));
  d->m_SearchEdit->setFixedHeight(30);
  d->m_SearchEdit->setIsClearButtonEnable(true);
  popupLayout->addWidget(d->m_SearchEdit);

  d->m_TreeView = new NXTreeView(d->m_PopupContainer);
  d->m_TreeView->setHeaderHidden(true);
  d->m_TreeView->setRootIsDecorated(true);
  d->m_TreeView->setSelectionMode(QAbstractItemView::SingleSelection);
  d->m_TreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  d->m_TreeView->setObjectName("NXTreeSelectView");
  d->m_TreeView->setStyleSheet(QStringLiteral("#NXTreeSelectView{background-color:transparent;}"));
  popupLayout->addWidget(d->m_TreeView);

  d->m_ProxyModel = new QSortFilterProxyModel(this);
  d->m_ProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
  d->m_ProxyModel->setRecursiveFilteringEnabled(true);

  connect(d->m_TreeView, &QTreeView::clicked, d, &NXTreeSelectPrivate::onItemClicked);
  connect(d->m_TreeView, &QTreeView::expanded, d, &NXTreeSelectPrivate::onTreeExpanded);
  connect(d->m_TreeView, &QTreeView::collapsed, d, &NXTreeSelectPrivate::onTreeCollapsed);
  connect(d->m_SearchEdit, &NXLineEdit::textChanged, d, &NXTreeSelectPrivate::onSearchTextChanged);
  connect(nxTheme, &NXTheme::themeModeChanged, d, &NXTreeSelectPrivate::onThemeChanged);
}

NXTreeSelect::~NXTreeSelect()
{
  d_ptr->m_PopupContainer->deleteLater();
}

void NXTreeSelect::setItemHeight(int itemHeight)
{
  Q_D(NXTreeSelect);
  if (itemHeight > 0) {
    d->m_ItemHeight = itemHeight;
    d->m_TreeView->setItemHeight(itemHeight);
  }
}

int NXTreeSelect::getItemHeight() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  return d_ptr->m_ItemHeight;
}

void NXTreeSelect::setModel(QStandardItemModel *model)
{
  Q_D(NXTreeSelect);
  d->m_Model = model;
  if (model) {
    d->m_ProxyModel->setSourceModel(model);
    d->m_TreeView->setModel(d->m_ProxyModel);
  }
}

QStandardItemModel *NXTreeSelect::model() const
{
  return d_ptr->m_Model;
}

void NXTreeSelect::setCurrentIndex(QModelIndex const &index)
{
  Q_D(NXTreeSelect);
  d->m_CurrentIndex = index;
  d->_updateDisplayText();
  if (d->m_ProxyModel && index.isValid()) {
    QModelIndex proxyIndex = d->m_ProxyModel->mapFromSource(index);
    d->m_TreeView->setCurrentIndex(proxyIndex);
  }
  update();
}

QModelIndex NXTreeSelect::currentIndex() const
{
  return d_ptr->m_CurrentIndex;
}

QString NXTreeSelect::currentText() const
{
  return d_ptr->m_CurrentText;
}

void NXTreeSelect::expandAll()
{
  Q_D(NXTreeSelect);
  d->m_TreeView->expandAll();
}

void NXTreeSelect::collapseAll()
{
  Q_D(NXTreeSelect);
  d->m_TreeView->collapseAll();
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
  bgPath.addRoundedRect(bgRect, d->m_BorderRadius, d->m_BorderRadius);

  if (d->m_IsPressed) {
    painter.setPen(NXThemeColor(d->m_ThemeMode, BasicBorderDeep));
    painter.setBrush(NXThemeColor(d->m_ThemeMode, BasicPress));
  } else if (d->m_IsHover) {
    painter.setPen(NXThemeColor(d->m_ThemeMode, BasicBorderHover));
    painter.setBrush(NXThemeColor(d->m_ThemeMode, BasicHover));
  } else {
    painter.setPen(NXThemeColor(d->m_ThemeMode, BasicBorder));
    painter.setBrush(NXThemeColor(d->m_ThemeMode, BasicBase));
  }
  painter.drawPath(bgPath);

  if (d->m_IsPopupVisible) {
    painter.setPen(Qt::NoPen);
    painter.setBrush(NXThemeColor(d->m_ThemeMode, PrimaryNormal));
    QRectF indicatorRect(d->m_BorderRadius + 6, height() - 3, width() - 2 * (d->m_BorderRadius + 6), 3);
    painter.drawRoundedRect(indicatorRect, 1.5, 1.5);
  }

  QRectF textRect(10, 0, width() - 35, height());
  if (d->m_CurrentText.isEmpty()) {
    painter.setPen(NXThemeColor(d->m_ThemeMode, BasicTextNoFocus));
    painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, d->m_PlaceholderText);
  } else {
    painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
    QString elidedText = painter.fontMetrics().elidedText(d->m_CurrentText, Qt::ElideRight, textRect.width());
    painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, elidedText);
  }

  painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
  QFont iconFont = QFont(QStringLiteral("NXAwesome"));
  iconFont.setPixelSize(13);
  painter.setFont(iconFont);
  QRectF iconRect(width() - 25, 0, 20, height());
  painter.translate(iconRect.center());
  painter.rotate(d->m_ExpandIconRotate);
  painter.translate(-iconRect.center());
  painter.drawText(iconRect, Qt::AlignCenter, QChar((unsigned short) NXIconType::ChevronDown));

  painter.restore();
}

void NXTreeSelect::mousePressEvent(QMouseEvent *event)
{
  Q_D(NXTreeSelect);
  d->m_IsPressed = true;
  update();
  QWidget::mousePressEvent(event);
}

void NXTreeSelect::mouseReleaseEvent(QMouseEvent *event)
{
  Q_D(NXTreeSelect);
  d->m_IsPressed = false;
  if (rect().contains(event->pos())) {
    if (d->m_IsPopupVisible) {
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
  d->m_IsHover = true;
  update();
  QWidget::enterEvent(event);
}

void NXTreeSelect::leaveEvent(QEvent *event)
{
  Q_D(NXTreeSelect);
  d->m_IsHover = false;
  update();
  QWidget::leaveEvent(event);
}
