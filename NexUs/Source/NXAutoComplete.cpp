#include "NXAutoComplete.h"

#include <QHBoxLayout>
#include <QSortFilterProxyModel>
#include <QStringListModel>

#include "DeveloperComponents/NXAutoCompleteContainer.h"
#include "DeveloperComponents/NXAutoCompleteDelegate.h"
#include "DeveloperComponents/NXBaseListView.h"
#include "NXLineEdit.h"
#include "NXScrollBar.h"
#include "NXTheme.h"
#include "private/NXAutoCompletePrivate.h"

Q_PROPERTY_CREATE_CPP(NXAutoComplete, int, BorderRadius)
Q_PROPERTY_CREATE_CPP(NXAutoComplete, int, MaxVisibleItems)
Q_PROPERTY_CREATE_CPP(NXAutoComplete, Qt::CaseSensitivity, CaseSensitivity)

NXAutoComplete::NXAutoComplete(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new NXAutoCompletePrivate())
{
  Q_D(NXAutoComplete);
  d->q_ptr             = this;
  d->_pBorderRadius    = 6;
  d->_pMaxVisibleItems = 6;
  d->_pCaseSensitivity = Qt::CaseInsensitive;

  setObjectName("NXAutoComplete");

  d->_lineEdit = new NXLineEdit(this);
  d->_lineEdit->setFixedHeight(35);
  setFixedHeight(35);
  QHBoxLayout *mainLayout = new QHBoxLayout(this);
  mainLayout->setSpacing(0);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->addWidget(d->_lineEdit);

  d->_sourceModel = new QStringListModel(this);
  d->_filterModel = new QSortFilterProxyModel(this);
  d->_filterModel->setSourceModel(d->_sourceModel);
  d->_filterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

  d->_popup = new NXAutoCompleteContainer(nullptr);
  d->_popup->setForwardTarget(d->_lineEdit);
  d->_popup->hide();

  d->_popupLayout = new QVBoxLayout(d->_popup);
  d->_popupLayout->setContentsMargins(4, 4, 4, 4);

  d->_listView = new NXBaseListView(d->_popup);
  d->_listView->setModel(d->_filterModel);
  d->_listView->setItemDelegate(new NXAutoCompleteDelegate(this));
  d->_listView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  NXScrollBar *scrollBar = new NXScrollBar(d->_listView->verticalScrollBar(), d->_listView);
  scrollBar->setIsAnimation(true);
  d->_popupLayout->addWidget(d->_listView);

  d->_themeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, d, &NXAutoCompletePrivate::onThemeModeChanged);

  connect(d->_lineEdit, &NXLineEdit::textEdited, d, &NXAutoCompletePrivate::onTextEdited);
  connect(d->_lineEdit, &NXLineEdit::textChanged, this, &NXAutoComplete::textChanged);
  connect(d->_listView, &NXBaseListView::clicked, d, &NXAutoCompletePrivate::onItemClicked);
  connect(d->_lineEdit, &NXLineEdit::returnPressed, this, [=]() {
    Q_EMIT returnPressed(d->_lineEdit->text());
  });
  connect(d->_popup, &NXAutoCompleteContainer::popupClosed, this, [=]() {
    d->_isPopupVisible   = false;
    d->_isAllowHidePopup = false;
  });
}

NXAutoComplete::~NXAutoComplete()
{
  if (d_ptr->_popup) {
    delete d_ptr->_popup;
    d_ptr->_popup = nullptr;
  }
}

void NXAutoComplete::setPlaceholderText(const QString &placeholderText)
{
  Q_D(NXAutoComplete);
  d->_lineEdit->setPlaceholderText(placeholderText);
}

QString NXAutoComplete::placeholderText() const
{
  return d_ptr->_lineEdit->placeholderText();
}

void NXAutoComplete::setText(const QString &text)
{
  Q_D(NXAutoComplete);
  d->_lineEdit->setText(text);
}

QString NXAutoComplete::text() const
{
  return d_ptr->_lineEdit->text();
}

void NXAutoComplete::setCompletions(const QStringList &completions)
{
  Q_D(NXAutoComplete);
  d->_completions = completions;
  d->_sourceModel->setStringList(completions);
}

QStringList NXAutoComplete::completions() const
{
  return d_ptr->_completions;
}

void NXAutoComplete::setMatchMode(MatchMode mode)
{
  Q_D(NXAutoComplete);
  d->_matchMode = mode;
}

NXAutoComplete::MatchMode NXAutoComplete::matchMode() const
{
  return d_ptr->_matchMode;
}

void NXAutoComplete::setFixedHeight(int h)
{
  QWidget::setFixedHeight(h);
  Q_D(NXAutoComplete);
  d->_lineEdit->setFixedHeight(h);
}
