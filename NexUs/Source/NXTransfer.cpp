#include "NXTransfer.h"

#include <QPainter>
#include <QPainterPath>
#include <QStandardItemModel>
#include <QVBoxLayout>

#include "DeveloperComponents/NXBaseListView.h"
#include "NXCheckBox.h"
#include "NXIconButton.h"
#include "NXLineEdit.h"
#include "NXScrollBar.h"
#include "NXText.h"
#include "NXTheme.h"
#include "private/NXTransferPrivate.h"
Q_PROPERTY_CREATE_CPP(NXTransfer, QPS_SCREF_PAEMIT(QString), SourceTitle)
Q_PROPERTY_CREATE_CPP(NXTransfer, QPS_SCREF_PAEMIT(QString), TargetTitle)
Q_PROPERTY_CREATE_CPP(NXTransfer, QPS_PAEMIT(int), BorderRadius)
Q_PROPERTY_CREATE_CPP(NXTransfer, QPS_PAEMIT(int), ItemHeight)
Q_PROPERTY_CREATE_CPP(NXTransfer, QPS_PAEMIT(bool), IsSearchVisible)

NXTransfer::NXTransfer(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new NXTransferPrivate())
{
  Q_D(NXTransfer);
  d->q_ptr             = this;
  d->m_BorderRadius    = 6;
  d->m_ItemHeight      = 35;
  d->m_SourceTitle     = QStringLiteral("源列表");
  d->m_TargetTitle     = QStringLiteral("目标列表");
  d->m_IsSearchVisible = true;
  d->m_ThemeMode        = nxTheme->getThemeMode();

  setMinimumSize(500, 300);

  QWidget *sourcePanel      = new QWidget(this);
  QVBoxLayout *sourceLayout = new QVBoxLayout(sourcePanel);
  sourceLayout->setContentsMargins(0, 0, 0, 0);
  sourceLayout->setSpacing(4);

  QHBoxLayout *sourceHeaderLayout = new QHBoxLayout();
  sourceHeaderLayout->setContentsMargins(8, 8, 8, 4);
  sourceHeaderLayout->setSpacing(2);
  d->m_SourceSelectAll = new NXCheckBox(this);
  d->m_SourceSelectAll->setFixedWidth(30);
  d->m_SourceTitleText = new NXText(d->m_SourceTitle, this);
  d->m_SourceTitleText->setTextStyle(NXTextType::BodyStrong);
  d->m_SourceCountText = new NXText(QStringLiteral("0/0"), this);
  d->m_SourceCountText->setTextStyle(NXTextType::Caption);
  sourceHeaderLayout->addWidget(d->m_SourceSelectAll, 0, Qt::AlignVCenter);
  sourceHeaderLayout->addWidget(d->m_SourceTitleText);
  sourceHeaderLayout->addStretch();
  sourceHeaderLayout->addWidget(d->m_SourceCountText);
  sourceLayout->addLayout(sourceHeaderLayout);

  d->m_SourceSearchEdit = new NXLineEdit(this);
  d->m_SourceSearchEdit->setPlaceholderText(QStringLiteral("搜索"));
  d->m_SourceSearchEdit->setFixedHeight(30);
  d->m_SourceSearchEdit->setClearButtonEnabled(true);
  QHBoxLayout *sourceSearchLayout = new QHBoxLayout();
  sourceSearchLayout->setContentsMargins(8, 0, 8, 0);
  sourceSearchLayout->addWidget(d->m_SourceSearchEdit);
  sourceLayout->addLayout(sourceSearchLayout);

  d->m_SourceModel    = new QStandardItemModel(this);
  d->m_SourceListView = new NXBaseListView(this);
  d->m_SourceListView->setModel(d->m_SourceModel);
  d->m_SourceListView->setSelectionMode(QAbstractItemView::MultiSelection);
  d->m_SourceListView->setObjectName("NXTransferSourceView");
  d->m_SourceListView->setStyleSheet(QStringLiteral("#NXTransferSourceView{background-color:transparent;}"));
  NXScrollBar *sourceScrollBar = new NXScrollBar(d->m_SourceListView->verticalScrollBar(), d->m_SourceListView);
  sourceScrollBar->setIsAnimation(true);
  sourceLayout->addWidget(d->m_SourceListView);

  QWidget *buttonPanel      = new QWidget(this);
  QVBoxLayout *buttonLayout = new QVBoxLayout(buttonPanel);
  buttonLayout->setContentsMargins(8, 0, 8, 0);
  buttonLayout->setSpacing(8);
  buttonLayout->addStretch();
  d->m_MoveToTargetButton = new NXIconButton(NXIconType::AngleRight, 14, 36, 30, this);
  d->m_MoveToTargetButton->setBorderRadius(4);
  d->m_MoveToSourceButton = new NXIconButton(NXIconType::AngleLeft, 14, 36, 30, this);
  d->m_MoveToSourceButton->setBorderRadius(4);
  buttonLayout->addWidget(d->m_MoveToTargetButton);
  buttonLayout->addWidget(d->m_MoveToSourceButton);
  buttonLayout->addStretch();

  QWidget *targetPanel      = new QWidget(this);
  QVBoxLayout *targetLayout = new QVBoxLayout(targetPanel);
  targetLayout->setContentsMargins(0, 0, 0, 0);
  targetLayout->setSpacing(4);

  QHBoxLayout *targetHeaderLayout = new QHBoxLayout();
  targetHeaderLayout->setContentsMargins(8, 8, 8, 4);
  targetHeaderLayout->setSpacing(2);
  d->m_TargetSelectAll = new NXCheckBox(this);
  d->m_TargetSelectAll->setFixedWidth(30);
  d->m_TargetTitleText = new NXText(d->m_TargetTitle, this);
  d->m_TargetTitleText->setTextStyle(NXTextType::BodyStrong);
  d->m_TargetCountText = new NXText(QStringLiteral("0/0"), this);
  d->m_TargetCountText->setTextStyle(NXTextType::Caption);
  targetHeaderLayout->addWidget(d->m_TargetSelectAll, 0, Qt::AlignVCenter);
  targetHeaderLayout->addWidget(d->m_TargetTitleText);
  targetHeaderLayout->addStretch();
  targetHeaderLayout->addWidget(d->m_TargetCountText);
  targetLayout->addLayout(targetHeaderLayout);

  d->m_TargetSearchEdit = new NXLineEdit(this);
  d->m_TargetSearchEdit->setPlaceholderText(QStringLiteral("搜索"));
  d->m_TargetSearchEdit->setFixedHeight(30);
  d->m_TargetSearchEdit->setClearButtonEnabled(true);
  QHBoxLayout *targetSearchLayout = new QHBoxLayout();
  targetSearchLayout->setContentsMargins(8, 0, 8, 0);
  targetSearchLayout->addWidget(d->m_TargetSearchEdit);
  targetLayout->addLayout(targetSearchLayout);

  d->m_TargetModel    = new QStandardItemModel(this);
  d->m_TargetListView = new NXBaseListView(this);
  d->m_TargetListView->setModel(d->m_TargetModel);
  d->m_TargetListView->setSelectionMode(QAbstractItemView::MultiSelection);
  d->m_TargetListView->setObjectName("NXTransferTargetView");
  d->m_TargetListView->setStyleSheet(QStringLiteral("#NXTransferTargetView{background-color:transparent;}"));
  NXScrollBar *targetScrollBar = new NXScrollBar(d->m_TargetListView->verticalScrollBar(), d->m_TargetListView);
  targetScrollBar->setIsAnimation(true);
  targetLayout->addWidget(d->m_TargetListView);

  QHBoxLayout *mainLayout = new QHBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);
  mainLayout->addWidget(sourcePanel, 1);
  mainLayout->addWidget(buttonPanel, 0);
  mainLayout->addWidget(targetPanel, 1);

  connect(d->m_MoveToTargetButton, &NXIconButton::clicked, d, &NXTransferPrivate::onMoveToTarget);
  connect(d->m_MoveToSourceButton, &NXIconButton::clicked, d, &NXTransferPrivate::onMoveToSource);
  connect(d->m_SourceSearchEdit, &NXLineEdit::textChanged, d, &NXTransferPrivate::onSourceSearchTextChanged);
  connect(d->m_TargetSearchEdit, &NXLineEdit::textChanged, d, &NXTransferPrivate::onTargetSearchTextChanged);
  connect(d->m_SourceSelectAll, &NXCheckBox::stateChanged, d, &NXTransferPrivate::onSourceSelectAllChanged);
  connect(d->m_TargetSelectAll, &NXCheckBox::stateChanged, d, &NXTransferPrivate::onTargetSelectAllChanged);

  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode    = themeMode;
    QColor textColor = NXThemeColor(themeMode, BasicText);
    for (int i = 0; i < d->m_SourceModel->rowCount(); ++i) {
      d->m_SourceModel->item(i)->setForeground(textColor);
    }
    for (int i = 0; i < d->m_TargetModel->rowCount(); ++i) {
      d->m_TargetModel->item(i)->setForeground(textColor);
    }
    update();
  });
}

NXTransfer::~NXTransfer()
{
}

void NXTransfer::setSourceItems(QStringList const &items)
{
  Q_D(NXTransfer);
  d->m_SourceModel->clear();
  QColor textColor = NXThemeColor(d->m_ThemeMode, BasicText);
  for (QString const &text : items) {
    QStandardItem *item = new QStandardItem(text);
    item->setForeground(textColor);
    item->setFlags(item->flags() | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    d->m_SourceModel->appendRow(item);
  }
  d->_refreshCountText();
}

void NXTransfer::addSourceItem(QString const &text)
{
  Q_D(NXTransfer);
  QStandardItem *item = new QStandardItem(text);
  item->setForeground(NXThemeColor(d->m_ThemeMode, BasicText));
  item->setFlags(item->flags() | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
  d->m_SourceModel->appendRow(item);
  d->_refreshCountText();
}

void NXTransfer::addSourceItems(QStringList const &items)
{
  Q_D(NXTransfer);
  QColor textColor = NXThemeColor(d->m_ThemeMode, BasicText);
  for (QString const &text : items) {
    QStandardItem *item = new QStandardItem(text);
    item->setForeground(textColor);
    item->setFlags(item->flags() | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    d->m_SourceModel->appendRow(item);
  }
  d->_refreshCountText();
}

QStringList NXTransfer::getSourceItems() const
{
  QStringList items;
  for (int i = 0; i < d_ptr->m_SourceModel->rowCount(); ++i) {
    items.append(d_ptr->m_SourceModel->item(i)->text());
  }
  return items;
}

QStringList NXTransfer::getTargetItems() const
{
  QStringList items;
  for (int i = 0; i < d_ptr->m_TargetModel->rowCount(); ++i) {
    items.append(d_ptr->m_TargetModel->item(i)->text());
  }
  return items;
}

void NXTransfer::moveToTarget()
{
  Q_D(NXTransfer);
  d->onMoveToTarget();
}

void NXTransfer::moveToSource()
{
  Q_D(NXTransfer);
  d->onMoveToSource();
}

void NXTransfer::moveAllToTarget()
{
  Q_D(NXTransfer);
  d->m_SourceListView->selectAll();
  d->onMoveToTarget();
}

void NXTransfer::moveAllToSource()
{
  Q_D(NXTransfer);
  d->m_TargetListView->selectAll();
  d->onMoveToSource();
}

void NXTransfer::paintEvent(QPaintEvent *event)
{
  Q_D(NXTransfer);
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::Antialiasing);
  painter.setPen(NXThemeColor(d->m_ThemeMode, BasicBorder));
  painter.setBrush(Qt::NoBrush);

  int panelWidth   = (width() - 52) / 2;
  int buttonPanelX = panelWidth;

  QPainterPath sourcePath;
  QRectF sourceRect(0, 0, panelWidth, height());
  sourcePath.addRoundedRect(sourceRect, d->m_BorderRadius, d->m_BorderRadius);
  painter.drawPath(sourcePath);

  QPainterPath targetPath;
  QRectF targetRect(width() - panelWidth, 0, panelWidth, height());
  targetPath.addRoundedRect(targetRect, d->m_BorderRadius, d->m_BorderRadius);
  painter.drawPath(targetPath);

  painter.restore();
}
