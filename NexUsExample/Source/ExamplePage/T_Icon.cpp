#include "T_Icon.h"

#include <QApplication>
#include <QClipboard>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QVBoxLayout>

#include "NXLineEdit.h"
#include "NXListView.h"
#include "NXMessageBar.h"
#include "ModelView/T_IconDelegate.h"
#include "ModelView/T_IconModel.h"

T_Icon::T_Icon(QWidget *parent)
    : T_BasePage(parent)
{
  // 预览窗口标题
  setWindowTitle(QStringLiteral("NXIcon"));
  // 顶部元素
  createCustomWidget(QStringLiteral("一堆常用图标被放置于此，左键单击以复制其枚举"));

  _metaEnum                  = QMetaEnum::fromType<NXIconType::IconName>();
  QWidget *centralWidget     = new QWidget(this);
  QVBoxLayout *centerVLayout = new QVBoxLayout(centralWidget);
  centerVLayout->setContentsMargins(0, 0, 5, 0);
  centralWidget->setWindowTitle(QStringLiteral("NXIcon"));
  // ListView
  _iconView = new NXListView(this);
  _iconView->setIsTransparent(true);
  _iconView->setFlow(QListView::LeftToRight);
  _iconView->setViewMode(QListView::IconMode);
  _iconView->setResizeMode(QListView::Adjust);
  connect(_iconView, &NXListView::clicked, this, [=](QModelIndex const &index) {
    QString iconName = _iconModel->getIconNameFromModelIndex(index);
    if (iconName.isEmpty()) {
      return;
    }
    qApp->clipboard()->setText(iconName);
    NXMessageBar::success(NXMessageBarType::Top, QStringLiteral("复制完成"), iconName + QStringLiteral("已被复制到剪贴板"), 1000, this);
  });
  _iconModel    = new T_IconModel(this);
  _iconDelegate = new T_IconDelegate(this);
  _iconView->setModel(_iconModel);
  _iconView->setItemDelegate(_iconDelegate);
  _iconView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  _searchEdit = new NXLineEdit(this);
  _searchEdit->setPlaceholderText(QStringLiteral("搜索图标"));
  _searchEdit->setFixedSize(300, 35);
  connect(_searchEdit, &NXLineEdit::textEdited, this, &T_Icon::onSearchEditTextEdit);
  connect(_searchEdit, &NXLineEdit::focusIn, this, &T_Icon::onSearchEditTextEdit);

  centerVLayout->addSpacing(13);
  centerVLayout->addWidget(_searchEdit);
  centerVLayout->addWidget(_iconView);
  this->addCentralWidget(centralWidget, true, true, 0);
}

T_Icon::~T_Icon()
{
}

void T_Icon::onSearchEditTextEdit(QString const &searchText)
{
  if (searchText.isEmpty()) {
    _iconModel->setIsSearchMode(false);
    _iconModel->setSearchKeyList(QStringList());
    _iconView->clearSelection();
    _iconView->viewport()->update();
    return;
  }
  QStringList searchKeyList;
  for (int i = 1; i < _metaEnum.keyCount(); i++) {
    QString key = QString(_metaEnum.key(i));
    if (key.contains(searchText, Qt::CaseInsensitive)) {
      searchKeyList.append(key);
    }
  }
  _iconModel->setIsSearchMode(true);
  _iconModel->setSearchKeyList(searchKeyList);
  _iconView->clearSelection();
  _iconView->scrollTo(_iconModel->index(0, 0));
  _iconView->viewport()->update();
}
