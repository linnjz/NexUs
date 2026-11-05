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

Q_PROPERTY_CREATE_CPP(NXAutoComplete, QPS_PAEMIT(int), BorderRadius)
Q_PROPERTY_CREATE_CPP(NXAutoComplete, QPS_PAEMIT(int), MaxVisibleItems)
Q_PROPERTY_CREATE_CPP(NXAutoComplete, QPS_PAEMIT(Qt::CaseSensitivity), CaseSensitivity)

NXAutoComplete::NXAutoComplete(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new NXAutoCompletePrivate())
{
  Q_D(NXAutoComplete);
  d->q_ptr             = this;
  d->m_BorderRadius    = 6;
  d->m_MaxVisibleItems = 6;
  d->m_CaseSensitivity = Qt::CaseInsensitive;

  setObjectName("NXAutoComplete");

  d->m_LineEdit = new NXLineEdit(this);
  d->m_LineEdit->setFixedHeight(35);
  setFixedHeight(35);
  QHBoxLayout *mainLayout = new QHBoxLayout(this);
  mainLayout->setSpacing(0);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->addWidget(d->m_LineEdit);

  d->m_SourceModel = new QStringListModel(this);
  d->m_FilterModel = new QSortFilterProxyModel(this);
  d->m_FilterModel->setSourceModel(d->m_SourceModel);
  d->m_FilterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

  d->m_Popup = new NXAutoCompleteContainer(nullptr);
  d->m_Popup->setForwardTarget(d->m_LineEdit);
  d->m_Popup->hide();

  d->m_PopupLayout = new QVBoxLayout(d->m_Popup);
  d->m_PopupLayout->setContentsMargins(4, 4, 4, 4);

  d->m_ListView = new NXBaseListView(d->m_Popup);
  d->m_ListView->setModel(d->m_FilterModel);
  d->m_ListView->setItemDelegate(new NXAutoCompleteDelegate(this));
  d->m_ListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  NXScrollBar *scrollBar = new NXScrollBar(d->m_ListView->verticalScrollBar(), d->m_ListView);
  scrollBar->setIsAnimation(true);
  d->m_PopupLayout->addWidget(d->m_ListView);

  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, d, &NXAutoCompletePrivate::onThemeModeChanged);

  connect(d->m_LineEdit, &NXLineEdit::textEdited, d, &NXAutoCompletePrivate::onTextEdited);
  connect(d->m_LineEdit, &NXLineEdit::textChanged, this, &NXAutoComplete::textChanged);
  connect(d->m_ListView, &NXBaseListView::clicked, d, &NXAutoCompletePrivate::onItemClicked);
  connect(d->m_LineEdit, &NXLineEdit::returnPressed, this, [=]() {
    Q_EMIT returnPressed(d->m_LineEdit->text());
  });
  connect(d->m_Popup, &NXAutoCompleteContainer::popupClosed, this, [=]() {
    d->m_IsPopupVisible   = false;
    d->m_IsAllowHidePopup = false;
  });
}

NXAutoComplete::~NXAutoComplete()
{
  if (d_ptr->m_Popup) {
    delete d_ptr->m_Popup;
    d_ptr->m_Popup = nullptr;
  }
}

void NXAutoComplete::setPlaceholderText(QString const &placeholderText)
{
  Q_D(NXAutoComplete);
  d->m_LineEdit->setPlaceholderText(placeholderText);
}

QString NXAutoComplete::placeholderText() const
{
  return d_ptr->m_LineEdit->placeholderText();
}

void NXAutoComplete::setText(QString const &text)
{
  Q_D(NXAutoComplete);
  d->m_LineEdit->setText(text);
}

QString NXAutoComplete::text() const
{
  return d_ptr->m_LineEdit->text();
}

void NXAutoComplete::setCompletions(QStringList const &completions)
{
  Q_D(NXAutoComplete);
  d->m_Completions = completions;
  d->m_SourceModel->setStringList(completions);
}

QStringList NXAutoComplete::completions() const
{
  return d_ptr->m_Completions;
}

void NXAutoComplete::setMatchMode(MatchMode mode)
{
  Q_D(NXAutoComplete);
  d->m_MatchMode = mode;
}

NXAutoComplete::MatchMode NXAutoComplete::matchMode() const
{
  return d_ptr->m_MatchMode;
}

void NXAutoComplete::setFixedHeight(int h)
{
  QWidget::setFixedHeight(h);
  Q_D(NXAutoComplete);
  d->m_LineEdit->setFixedHeight(h);
}
