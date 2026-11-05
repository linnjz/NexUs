#include "NXSuggestBox.h"

#include <QAction>
#include <QDebug>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QListView>
#include <QMap>
#include <QPainter>
#include <QPainterPath>
#include <QVBoxLayout>

#include "DeveloperComponents/NXBaseListView.h"
#include "DeveloperComponents/NXSuggestBoxSearchViewContainer.h"
#include "DeveloperComponents/NXSuggestDelegate.h"
#include "DeveloperComponents/NXSuggestModel.h"
#include "NXIcon.h"
#include "NXLineEdit.h"
#include "NXScrollBar.h"
#include "NXTheme.h"
#include "private/NXSuggestBoxPrivate.h"

Q_PROPERTY_CREATE_CPP(NXSuggestBox, QPS_PAEMIT(int), BorderRadius)
Q_PROPERTY_CREATE_CPP(NXSuggestBox, QPS_PAEMIT(Qt::CaseSensitivity), CaseSensitivity)

NXSuggestBox::SuggestData::SuggestData()
{
}

NXSuggestBox::SuggestData::SuggestData(NXIconType::IconName icon, QString const &suggestText, QVariantMap const &suggestData)
    : m_NXIcon(icon)
    , m_SuggestText(suggestText)
    , m_SuggestData(suggestData)
{
}

NXSuggestBox::SuggestData::~SuggestData()
{
}

NXSuggestBox::NXSuggestBox(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXSuggestBoxPrivate())
{
  Q_D(NXSuggestBox);
  QWidget::setFixedSize(280, 35);
  d->q_ptr             = this;
  d->m_BorderRadius    = 6;
  d->m_CaseSensitivity = Qt::CaseInsensitive;
  d->m_SearchEdit       = new NXLineEdit(this);
  d->m_SearchEdit->setFixedHeight(35);
  d->m_SearchEdit->setPlaceholderText(QStringLiteral("查找功能"));
  d->m_SearchEdit->setClearButtonEnabled(true);
  d->m_LightSearchAction =
      new QAction(NXIcon::getInstance()->getNXIcon(NXIconType::MagnifyingGlass), QStringLiteral("Search"), this);
  d->m_DarkSearchAction =
      new QAction(NXIcon::getInstance()->getNXIcon(NXIconType::MagnifyingGlass, QColor(0xFF, 0xFF, 0xFF)), QStringLiteral("Search"), this);

  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, d, &NXSuggestBoxPrivate::onThemeModeChanged);
  if (d->m_ThemeMode == NXThemeType::Light) {
    d->m_SearchEdit->addAction(d->m_LightSearchAction, QLineEdit::TrailingPosition);
  } else {
    d->m_SearchEdit->addAction(d->m_DarkSearchAction, QLineEdit::TrailingPosition);
  }

  connect(d->m_LightSearchAction, &QAction::triggered, this, [=](bool checked) {
    //qDebug() << "Search";
  });

  connect(d->m_DarkSearchAction, &QAction::triggered, this, [=](bool checked) {
    //qDebug() << "Search";
  });
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setSpacing(0);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->addWidget(d->m_SearchEdit);
  d->m_SearchViewBaseWidget = new NXSuggestBoxSearchViewContainer(window());
  d->m_ShadowLayout         = new QVBoxLayout(d->m_SearchViewBaseWidget);
  d->m_ShadowLayout->setContentsMargins(0, 0, 0, 0);
  d->m_SearchView               = new NXBaseListView(d->m_SearchViewBaseWidget);
  NXScrollBar *floatVScrollBar = new NXScrollBar(d->m_SearchView->verticalScrollBar(), d->m_SearchView);
  floatVScrollBar->setIsAnimation(true);
  d->m_SearchViewBaseWidget->resize(292, 300);
  d->m_ShadowLayout->addWidget(d->m_SearchView);
  d->m_SearchModel    = new NXSuggestModel(this);
  d->m_SearchDelegate = new NXSuggestDelegate(this);
  d->m_SearchView->setModel(d->m_SearchModel);
  d->m_SearchView->setItemDelegate(d->m_SearchDelegate);
  d->m_SearchViewBaseWidget->hide();
  connect(d->m_SearchEdit, &NXLineEdit::textEdited, d, &NXSuggestBoxPrivate::onSearchEditTextEdit);
  connect(d->m_SearchEdit, &NXLineEdit::focusIn, d, &NXSuggestBoxPrivate::onSearchEditTextEdit);
  connect(d->m_SearchView, &NXBaseListView::clicked, d, &NXSuggestBoxPrivate::onSearchViewClicked);

  // 焦点事件
  connect(d->m_SearchEdit, &NXLineEdit::wmFocusOut, this, [=]() {
    if (!d->m_SearchView->underMouse()) {
      d->_startCloseAnimation();
    }
  });
}

NXSuggestBox::~NXSuggestBox()
{
}

void NXSuggestBox::setPlaceholderText(QString const &placeholderText)
{
  Q_D(NXSuggestBox);
  d->m_SearchEdit->setPlaceholderText(placeholderText);
}

void NXSuggestBox::setFixedSize(QSize size)
{
  Q_D(NXSuggestBox);
  d->m_SearchEdit->setFixedHeight(size.height());
  QWidget::setFixedSize(size);
}

void NXSuggestBox::setFixedSize(int w, int h)
{
  Q_D(NXSuggestBox);
  d->m_SearchEdit->setFixedHeight(h);
  QWidget::setFixedSize(w, h);
}

void NXSuggestBox::setFixedHeight(int h)
{
  Q_D(NXSuggestBox);
  d->m_SearchEdit->setFixedHeight(h);
  QWidget::setFixedHeight(h);
}

QString NXSuggestBox::addSuggestion(QString const &suggestText, QVariantMap const &suggestData)
{
  Q_D(NXSuggestBox);
  NXSuggestion *suggest = new NXSuggestion(this);
  suggest->setSuggestText(suggestText);
  suggest->setSuggestData(suggestData);
  d->m_SuggestionVector.append(suggest);
  return suggest->getSuggestKey();
}

QString NXSuggestBox::addSuggestion(NXIconType::IconName icon, QString const &suggestText, QVariantMap const &suggestData)
{
  Q_D(NXSuggestBox);
  NXSuggestion *suggest = new NXSuggestion(this);
  suggest->setNXIcon(icon);
  suggest->setSuggestText(suggestText);
  suggest->setSuggestData(suggestData);
  d->m_SuggestionVector.append(suggest);
  return suggest->getSuggestKey();
}

QString NXSuggestBox::addSuggestion(NXSuggestBox::SuggestData const &suggestData)
{
  Q_D(NXSuggestBox);
  NXSuggestion *suggest = new NXSuggestion(this);
  suggest->setNXIcon(suggestData.getNXIcon());
  suggest->setSuggestText(suggestData.getSuggestText());
  suggest->setSuggestData(suggestData.getSuggestData());
  d->m_SuggestionVector.append(suggest);
  return suggest->getSuggestKey();
}

QStringList NXSuggestBox::addSuggestion(QList<NXSuggestBox::SuggestData> const &suggestDataList)
{
  Q_D(NXSuggestBox);
  QStringList suggestKeyList;
  for (auto const &suggestData : suggestDataList) {
    NXSuggestion *suggest = new NXSuggestion(this);
    suggest->setNXIcon(suggestData.getNXIcon());
    suggest->setSuggestText(suggestData.getSuggestText());
    suggest->setSuggestData(suggestData.getSuggestData());
    d->m_SuggestionVector.append(suggest);
    suggestKeyList.append(suggest->getSuggestKey());
  }
  return suggestKeyList;
}

void NXSuggestBox::removeSuggestion(QString const &suggestKey)
{
  Q_D(NXSuggestBox);
  foreach(auto suggest, d->m_SuggestionVector)
  {
    if (suggest->getSuggestKey() == suggestKey) {
      d->m_SuggestionVector.removeOne(suggest);
      suggest->deleteLater();
    }
  }
}

void NXSuggestBox::removeSuggestion(int index)
{
  Q_D(NXSuggestBox);
  if (index >= d->m_SuggestionVector.count()) {
    return;
  }
  NXSuggestion *suggest = d->m_SuggestionVector[index];
  d->m_SuggestionVector.removeOne(suggest);
  suggest->deleteLater();
}

void NXSuggestBox::clearSuggestion()
{
  Q_D(NXSuggestBox);
  foreach(auto suggest, d->m_SuggestionVector)
  {
    d->m_SuggestionVector.removeOne(suggest);
    suggest->deleteLater();
  }
}
