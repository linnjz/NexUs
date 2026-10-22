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
Q_PROPERTY_CREATE_CPP(NXTransfer, QS_SET_CREF(QString), SourceTitle)
Q_PROPERTY_CREATE_CPP(NXTransfer, QS_SET_CREF(QString), TargetTitle)
Q_PROPERTY_CREATE_CPP(NXTransfer, int, BorderRadius)
Q_PROPERTY_CREATE_CPP(NXTransfer, int, ItemHeight)
Q_PROPERTY_CREATE_CPP(NXTransfer, bool, IsSearchVisible)

NXTransfer::NXTransfer(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new NXTransferPrivate())
{
  Q_D(NXTransfer);
  d->q_ptr             = this;
  d->_pBorderRadius    = 6;
  d->_pItemHeight      = 35;
  d->_pSourceTitle     = QStringLiteral("源列表");
  d->_pTargetTitle     = QStringLiteral("目标列表");
  d->_pIsSearchVisible = true;
  d->_themeMode        = nxTheme->getThemeMode();

  setMinimumSize(500, 300);

  QWidget *sourcePanel      = new QWidget(this);
  QVBoxLayout *sourceLayout = new QVBoxLayout(sourcePanel);
  sourceLayout->setContentsMargins(0, 0, 0, 0);
  sourceLayout->setSpacing(4);

  QHBoxLayout *sourceHeaderLayout = new QHBoxLayout();
  sourceHeaderLayout->setContentsMargins(8, 8, 8, 4);
  sourceHeaderLayout->setSpacing(2);
  d->_sourceSelectAll = new NXCheckBox(this);
  d->_sourceSelectAll->setFixedWidth(30);
  d->_sourceTitleText = new NXText(d->_pSourceTitle, this);
  d->_sourceTitleText->setTextStyle(NXTextType::BodyStrong);
  d->_sourceCountText = new NXText(QStringLiteral("0/0"), this);
  d->_sourceCountText->setTextStyle(NXTextType::Caption);
  sourceHeaderLayout->addWidget(d->_sourceSelectAll, 0, Qt::AlignVCenter);
  sourceHeaderLayout->addWidget(d->_sourceTitleText);
  sourceHeaderLayout->addStretch();
  sourceHeaderLayout->addWidget(d->_sourceCountText);
  sourceLayout->addLayout(sourceHeaderLayout);

  d->_sourceSearchEdit = new NXLineEdit(this);
  d->_sourceSearchEdit->setPlaceholderText(QStringLiteral("搜索"));
  d->_sourceSearchEdit->setFixedHeight(30);
  d->_sourceSearchEdit->setClearButtonEnabled(true);
  QHBoxLayout *sourceSearchLayout = new QHBoxLayout();
  sourceSearchLayout->setContentsMargins(8, 0, 8, 0);
  sourceSearchLayout->addWidget(d->_sourceSearchEdit);
  sourceLayout->addLayout(sourceSearchLayout);

  d->_sourceModel    = new QStandardItemModel(this);
  d->_sourceListView = new NXBaseListView(this);
  d->_sourceListView->setModel(d->_sourceModel);
  d->_sourceListView->setSelectionMode(QAbstractItemView::MultiSelection);
  d->_sourceListView->setObjectName("NXTransferSourceView");
  d->_sourceListView->setStyleSheet(QStringLiteral("#NXTransferSourceView{background-color:transparent;}"));
  NXScrollBar *sourceScrollBar = new NXScrollBar(d->_sourceListView->verticalScrollBar(), d->_sourceListView);
  sourceScrollBar->setIsAnimation(true);
  sourceLayout->addWidget(d->_sourceListView);

  QWidget *buttonPanel      = new QWidget(this);
  QVBoxLayout *buttonLayout = new QVBoxLayout(buttonPanel);
  buttonLayout->setContentsMargins(8, 0, 8, 0);
  buttonLayout->setSpacing(8);
  buttonLayout->addStretch();
  d->_moveToTargetButton = new NXIconButton(NXIconType::AngleRight, 14, 36, 30, this);
  d->_moveToTargetButton->setBorderRadius(4);
  d->_moveToSourceButton = new NXIconButton(NXIconType::AngleLeft, 14, 36, 30, this);
  d->_moveToSourceButton->setBorderRadius(4);
  buttonLayout->addWidget(d->_moveToTargetButton);
  buttonLayout->addWidget(d->_moveToSourceButton);
  buttonLayout->addStretch();

  QWidget *targetPanel      = new QWidget(this);
  QVBoxLayout *targetLayout = new QVBoxLayout(targetPanel);
  targetLayout->setContentsMargins(0, 0, 0, 0);
  targetLayout->setSpacing(4);

  QHBoxLayout *targetHeaderLayout = new QHBoxLayout();
  targetHeaderLayout->setContentsMargins(8, 8, 8, 4);
  targetHeaderLayout->setSpacing(2);
  d->_targetSelectAll = new NXCheckBox(this);
  d->_targetSelectAll->setFixedWidth(30);
  d->_targetTitleText = new NXText(d->_pTargetTitle, this);
  d->_targetTitleText->setTextStyle(NXTextType::BodyStrong);
  d->_targetCountText = new NXText(QStringLiteral("0/0"), this);
  d->_targetCountText->setTextStyle(NXTextType::Caption);
  targetHeaderLayout->addWidget(d->_targetSelectAll, 0, Qt::AlignVCenter);
  targetHeaderLayout->addWidget(d->_targetTitleText);
  targetHeaderLayout->addStretch();
  targetHeaderLayout->addWidget(d->_targetCountText);
  targetLayout->addLayout(targetHeaderLayout);

  d->_targetSearchEdit = new NXLineEdit(this);
  d->_targetSearchEdit->setPlaceholderText(QStringLiteral("搜索"));
  d->_targetSearchEdit->setFixedHeight(30);
  d->_targetSearchEdit->setClearButtonEnabled(true);
  QHBoxLayout *targetSearchLayout = new QHBoxLayout();
  targetSearchLayout->setContentsMargins(8, 0, 8, 0);
  targetSearchLayout->addWidget(d->_targetSearchEdit);
  targetLayout->addLayout(targetSearchLayout);

  d->_targetModel    = new QStandardItemModel(this);
  d->_targetListView = new NXBaseListView(this);
  d->_targetListView->setModel(d->_targetModel);
  d->_targetListView->setSelectionMode(QAbstractItemView::MultiSelection);
  d->_targetListView->setObjectName("NXTransferTargetView");
  d->_targetListView->setStyleSheet(QStringLiteral("#NXTransferTargetView{background-color:transparent;}"));
  NXScrollBar *targetScrollBar = new NXScrollBar(d->_targetListView->verticalScrollBar(), d->_targetListView);
  targetScrollBar->setIsAnimation(true);
  targetLayout->addWidget(d->_targetListView);

  QHBoxLayout *mainLayout = new QHBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);
  mainLayout->addWidget(sourcePanel, 1);
  mainLayout->addWidget(buttonPanel, 0);
  mainLayout->addWidget(targetPanel, 1);

  connect(d->_moveToTargetButton, &NXIconButton::clicked, d, &NXTransferPrivate::onMoveToTarget);
  connect(d->_moveToSourceButton, &NXIconButton::clicked, d, &NXTransferPrivate::onMoveToSource);
  connect(d->_sourceSearchEdit, &NXLineEdit::textChanged, d, &NXTransferPrivate::onSourceSearchTextChanged);
  connect(d->_targetSearchEdit, &NXLineEdit::textChanged, d, &NXTransferPrivate::onTargetSearchTextChanged);
  connect(d->_sourceSelectAll, &NXCheckBox::stateChanged, d, &NXTransferPrivate::onSourceSelectAllChanged);
  connect(d->_targetSelectAll, &NXCheckBox::stateChanged, d, &NXTransferPrivate::onTargetSelectAllChanged);

  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode    = themeMode;
    QColor textColor = NXThemeColor(themeMode, BasicText);
    for (int i = 0; i < d->_sourceModel->rowCount(); ++i) {
      d->_sourceModel->item(i)->setForeground(textColor);
    }
    for (int i = 0; i < d->_targetModel->rowCount(); ++i) {
      d->_targetModel->item(i)->setForeground(textColor);
    }
    update();
  });
}

NXTransfer::~NXTransfer()
{
}

void NXTransfer::setSourceItems(const QStringList &items)
{
  Q_D(NXTransfer);
  d->_sourceModel->clear();
  QColor textColor = NXThemeColor(d->_themeMode, BasicText);
  for (const QString &text : items) {
    QStandardItem *item = new QStandardItem(text);
    item->setForeground(textColor);
    item->setFlags(item->flags() | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    d->_sourceModel->appendRow(item);
  }
  d->_refreshCountText();
}

void NXTransfer::addSourceItem(const QString &text)
{
  Q_D(NXTransfer);
  QStandardItem *item = new QStandardItem(text);
  item->setForeground(NXThemeColor(d->_themeMode, BasicText));
  item->setFlags(item->flags() | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
  d->_sourceModel->appendRow(item);
  d->_refreshCountText();
}

void NXTransfer::addSourceItems(const QStringList &items)
{
  Q_D(NXTransfer);
  QColor textColor = NXThemeColor(d->_themeMode, BasicText);
  for (const QString &text : items) {
    QStandardItem *item = new QStandardItem(text);
    item->setForeground(textColor);
    item->setFlags(item->flags() | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    d->_sourceModel->appendRow(item);
  }
  d->_refreshCountText();
}

QStringList NXTransfer::getSourceItems() const
{
  QStringList items;
  for (int i = 0; i < d_ptr->_sourceModel->rowCount(); ++i) {
    items.append(d_ptr->_sourceModel->item(i)->text());
  }
  return items;
}

QStringList NXTransfer::getTargetItems() const
{
  QStringList items;
  for (int i = 0; i < d_ptr->_targetModel->rowCount(); ++i) {
    items.append(d_ptr->_targetModel->item(i)->text());
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
  d->_sourceListView->selectAll();
  d->onMoveToTarget();
}

void NXTransfer::moveAllToSource()
{
  Q_D(NXTransfer);
  d->_targetListView->selectAll();
  d->onMoveToSource();
}

void NXTransfer::paintEvent(QPaintEvent *event)
{
  Q_D(NXTransfer);
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::Antialiasing);
  painter.setPen(NXThemeColor(d->_themeMode, BasicBorder));
  painter.setBrush(Qt::NoBrush);

  int panelWidth   = (width() - 52) / 2;
  int buttonPanelX = panelWidth;

  QPainterPath sourcePath;
  QRectF sourceRect(0, 0, panelWidth, height());
  sourcePath.addRoundedRect(sourceRect, d->_pBorderRadius, d->_pBorderRadius);
  painter.drawPath(sourcePath);

  QPainterPath targetPath;
  QRectF targetRect(width() - panelWidth, 0, panelWidth, height());
  targetPath.addRoundedRect(targetRect, d->_pBorderRadius, d->_pBorderRadius);
  painter.drawPath(targetPath);

  painter.restore();
}
