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

Q_PROPERTY_CREATE_CPP(NXSuggestBox, int, BorderRadius)
Q_PROPERTY_CREATE_CPP(NXSuggestBox, Qt::CaseSensitivity, CaseSensitivity)

NXSuggestBox::SuggestData::SuggestData()
{
}

NXSuggestBox::SuggestData::SuggestData(NXIconType::IconName icon, const QString &suggestText, const QVariantMap &suggestData)
    : _pNXIcon(icon)
    , _pSuggestText(suggestText)
    , _pSuggestData(suggestData)
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
  d->_pBorderRadius    = 6;
  d->_pCaseSensitivity = Qt::CaseInsensitive;
  d->_searchEdit       = new NXLineEdit(this);
  d->_searchEdit->setFixedHeight(35);
  d->_searchEdit->setPlaceholderText(QStringLiteral("查找功能"));
  d->_searchEdit->setClearButtonEnabled(true);
  d->_lightSearchAction =
      new QAction(NXIcon::getInstance()->getNXIcon(NXIconType::MagnifyingGlass), QStringLiteral("Search"), this);
  d->_darkSearchAction =
      new QAction(NXIcon::getInstance()->getNXIcon(NXIconType::MagnifyingGlass, QColor(0xFF, 0xFF, 0xFF)), QStringLiteral("Search"), this);

  d->_themeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, d, &NXSuggestBoxPrivate::onThemeModeChanged);
  if (d->_themeMode == NXThemeType::Light) {
    d->_searchEdit->addAction(d->_lightSearchAction, QLineEdit::TrailingPosition);
  } else {
    d->_searchEdit->addAction(d->_darkSearchAction, QLineEdit::TrailingPosition);
  }

  connect(d->_lightSearchAction, &QAction::triggered, this, [=](bool checked) {
    //qDebug() << "Search";
  });

  connect(d->_darkSearchAction, &QAction::triggered, this, [=](bool checked) {
    //qDebug() << "Search";
  });
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setSpacing(0);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->addWidget(d->_searchEdit);
  d->_searchViewBaseWidget = new NXSuggestBoxSearchViewContainer(window());
  d->_shadowLayout         = new QVBoxLayout(d->_searchViewBaseWidget);
  d->_shadowLayout->setContentsMargins(0, 0, 0, 0);
  d->_searchView               = new NXBaseListView(d->_searchViewBaseWidget);
  NXScrollBar *floatVScrollBar = new NXScrollBar(d->_searchView->verticalScrollBar(), d->_searchView);
  floatVScrollBar->setIsAnimation(true);
  d->_searchViewBaseWidget->resize(292, 300);
  d->_shadowLayout->addWidget(d->_searchView);
  d->_searchModel    = new NXSuggestModel(this);
  d->_searchDelegate = new NXSuggestDelegate(this);
  d->_searchView->setModel(d->_searchModel);
  d->_searchView->setItemDelegate(d->_searchDelegate);
  d->_searchViewBaseWidget->hide();
  connect(d->_searchEdit, &NXLineEdit::textEdited, d, &NXSuggestBoxPrivate::onSearchEditTextEdit);
  connect(d->_searchEdit, &NXLineEdit::focusIn, d, &NXSuggestBoxPrivate::onSearchEditTextEdit);
  connect(d->_searchView, &NXBaseListView::clicked, d, &NXSuggestBoxPrivate::onSearchViewClicked);

  // 焦点事件
  connect(d->_searchEdit, &NXLineEdit::wmFocusOut, this, [=]() {
    if (!d->_searchView->underMouse()) {
      d->_startCloseAnimation();
    }
  });
}

NXSuggestBox::~NXSuggestBox()
{
}

void NXSuggestBox::setPlaceholderText(const QString &placeholderText)
{
  Q_D(NXSuggestBox);
  d->_searchEdit->setPlaceholderText(placeholderText);
}

void NXSuggestBox::setFixedSize(QSize size)
{
  Q_D(NXSuggestBox);
  d->_searchEdit->setFixedHeight(size.height());
  QWidget::setFixedSize(size);
}

void NXSuggestBox::setFixedSize(int w, int h)
{
  Q_D(NXSuggestBox);
  d->_searchEdit->setFixedHeight(h);
  QWidget::setFixedSize(w, h);
}

void NXSuggestBox::setFixedHeight(int h)
{
  Q_D(NXSuggestBox);
  d->_searchEdit->setFixedHeight(h);
  QWidget::setFixedHeight(h);
}

QString NXSuggestBox::addSuggestion(const QString &suggestText, const QVariantMap &suggestData)
{
  Q_D(NXSuggestBox);
  NXSuggestion *suggest = new NXSuggestion(this);
  suggest->setSuggestText(suggestText);
  suggest->setSuggestData(suggestData);
  d->_suggestionVector.append(suggest);
  return suggest->getSuggestKey();
}

QString NXSuggestBox::addSuggestion(NXIconType::IconName icon, const QString &suggestText, const QVariantMap &suggestData)
{
  Q_D(NXSuggestBox);
  NXSuggestion *suggest = new NXSuggestion(this);
  suggest->setNXIcon(icon);
  suggest->setSuggestText(suggestText);
  suggest->setSuggestData(suggestData);
  d->_suggestionVector.append(suggest);
  return suggest->getSuggestKey();
}

QString NXSuggestBox::addSuggestion(const NXSuggestBox::SuggestData &suggestData)
{
  Q_D(NXSuggestBox);
  NXSuggestion *suggest = new NXSuggestion(this);
  suggest->setNXIcon(suggestData.getNXIcon());
  suggest->setSuggestText(suggestData.getSuggestText());
  suggest->setSuggestData(suggestData.getSuggestData());
  d->_suggestionVector.append(suggest);
  return suggest->getSuggestKey();
}

QStringList NXSuggestBox::addSuggestion(const QList<NXSuggestBox::SuggestData> &suggestDataList)
{
  Q_D(NXSuggestBox);
  QStringList suggestKeyList;
  for (const auto &suggestData : suggestDataList) {
    NXSuggestion *suggest = new NXSuggestion(this);
    suggest->setNXIcon(suggestData.getNXIcon());
    suggest->setSuggestText(suggestData.getSuggestText());
    suggest->setSuggestData(suggestData.getSuggestData());
    d->_suggestionVector.append(suggest);
    suggestKeyList.append(suggest->getSuggestKey());
  }
  return suggestKeyList;
}

void NXSuggestBox::removeSuggestion(const QString &suggestKey)
{
  Q_D(NXSuggestBox);
  foreach(auto suggest, d->_suggestionVector)
  {
    if (suggest->getSuggestKey() == suggestKey) {
      d->_suggestionVector.removeOne(suggest);
      suggest->deleteLater();
    }
  }
}

void NXSuggestBox::removeSuggestion(int index)
{
  Q_D(NXSuggestBox);
  if (index >= d->_suggestionVector.count()) {
    return;
  }
  NXSuggestion *suggest = d->_suggestionVector[index];
  d->_suggestionVector.removeOne(suggest);
  suggest->deleteLater();
}

void NXSuggestBox::clearSuggestion()
{
  Q_D(NXSuggestBox);
  foreach(auto suggest, d->_suggestionVector)
  {
    d->_suggestionVector.removeOne(suggest);
    suggest->deleteLater();
  }
}
