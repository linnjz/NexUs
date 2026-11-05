#include "NXSuggestBoxPrivate.h"

#include <QLayout>
#include <QPropertyAnimation>
#include <QUuid>

#include "DeveloperComponents/NXBaseListView.h"
#include "DeveloperComponents/NXSuggestBoxSearchViewContainer.h"
#include "DeveloperComponents/NXSuggestModel.h"
#include "NXLineEdit.h"
#include "NXSuggestBox.h"

NXSuggestion::NXSuggestion(QObject *parent)
    : QObject { parent }
    , m_SuggestText {}
    , m_SuggestKey { QUuid::createUuid().toRfc4122().toHex() }
    , m_SuggestData { QVariantMap() }
    , m_NXIcon { NXIconType::None }
{
}

NXSuggestion::~NXSuggestion()
{
}

NXSuggestBoxPrivate::NXSuggestBoxPrivate(QObject *parent)
    : QObject { parent }
{
}

NXSuggestBoxPrivate::~NXSuggestBoxPrivate()
{
}

void NXSuggestBoxPrivate::onThemeModeChanged(NXThemeType::ThemeMode themeMode)
{
  m_ThemeMode = themeMode;
  m_SearchEdit->removeAction(m_ThemeMode == NXThemeType::Light ? m_DarkSearchAction : m_LightSearchAction);
  m_SearchEdit->addAction(m_ThemeMode == NXThemeType::Light ? m_LightSearchAction : m_DarkSearchAction, QLineEdit::TrailingPosition);
  m_SearchEdit->update();
}

void NXSuggestBoxPrivate::onSearchEditTextEdit(QString const &searchText)
{
  Q_Q(NXSuggestBox);
  if (searchText.isEmpty()) {
    _startCloseAnimation();
    return;
  }
  QVector<NXSuggestion *> suggestionVector;
  for (auto const &suggest : m_SuggestionVector) {
    if (suggest->getSuggestText().contains(searchText, m_CaseSensitivity)) {
      suggestionVector.append(suggest);
    }
  }
  if (!suggestionVector.isEmpty()) {
    m_SearchModel->setSearchSuggestion(suggestionVector);
    int rowCount = suggestionVector.count();
    if (rowCount > 4) {
      rowCount = 4;
    }
    if (!m_SearchViewBaseWidget->isVisible()) {
      q->raise();
      m_SearchViewBaseWidget->show();
      m_SearchViewBaseWidget->raise();
      QPoint cyclePoint = q->mapTo(q->window(), QPoint(-5, q->height()));
      m_SearchViewBaseWidget->move(cyclePoint);
      _startSizeAnimation(QSize(q->width() + 10, 0), QSize(q->width() + 10, 40 * rowCount + 16));
      m_SearchView->move(m_SearchView->x(), -(40 * rowCount + 16));
    } else {
      _startSizeAnimation(m_SearchViewBaseWidget->size(), QSize(q->width() + 12, 40 * rowCount + 16));
    }
    _startExpandAnimation();
  } else {
    _startCloseAnimation();
  }
}

void NXSuggestBoxPrivate::onSearchViewClicked(QModelIndex const &index)
{
  Q_Q(NXSuggestBox);
  m_SearchView->clearSelection();
  if (!index.isValid()) {
    return;
  }
  NXSuggestion *suggest = m_SearchModel->getSearchSuggestion(index.row());
  m_SearchEdit->setText(suggest->getSuggestText());
  NXSuggestBox::SuggestData data(suggest->getNXIcon(), suggest->getSuggestText(), suggest->getSuggestData());
  data.setSuggestKey(suggest->getSuggestKey());
  Q_EMIT q->suggestionClicked(data);
  _startCloseAnimation();
}

void NXSuggestBoxPrivate::_startSizeAnimation(QSize oldSize, QSize newSize)
{
  if (m_LastSize.isValid() && m_LastSize == newSize) {
    return;
  }
  m_ShadowLayout->removeWidget(m_SearchView);
  QPropertyAnimation *expandAnimation = new QPropertyAnimation(m_SearchViewBaseWidget, "size");
  connect(expandAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
    m_SearchView->resize(m_SearchViewBaseWidget->size());
  });
  connect(expandAnimation, &QPropertyAnimation::finished, this, [=]() {
    m_ShadowLayout->addWidget(m_SearchView);
  });
  expandAnimation->setDuration(300);
  expandAnimation->setEasingCurve(QEasingCurve::InOutSine);
  expandAnimation->setStartValue(oldSize);
  expandAnimation->setEndValue(newSize);
  expandAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  m_LastSize = newSize;
}

void NXSuggestBoxPrivate::_startExpandAnimation()
{
  if (!m_IsExpandAnimationFinished) {
    return;
  }
  m_IsCloseAnimationFinished           = true;
  m_IsExpandAnimationFinished          = false;
  QPropertyAnimation *expandAnimation = new QPropertyAnimation(m_SearchView, "pos");
  connect(expandAnimation, &QPropertyAnimation::finished, this, [=]() {
    m_IsExpandAnimationFinished = true;
    m_SearchView->clearSelection();
  });
  expandAnimation->setDuration(300);
  expandAnimation->setEasingCurve(QEasingCurve::InOutSine);
  expandAnimation->setStartValue(m_SearchView->pos());
  expandAnimation->setEndValue(QPoint(8, 8));
  expandAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void NXSuggestBoxPrivate::_startCloseAnimation()
{
  if (!m_IsCloseAnimationFinished) {
    return;
  }
  m_IsExpandAnimationFinished               = true;
  m_IsCloseAnimationFinished                = false;
  QPropertyAnimation *baseWidgetsAnimation = new QPropertyAnimation(m_SearchViewBaseWidget, "size");
  baseWidgetsAnimation->setDuration(300);
  baseWidgetsAnimation->setEasingCurve(QEasingCurve::InOutSine);
  baseWidgetsAnimation->setStartValue(m_SearchViewBaseWidget->size());
  baseWidgetsAnimation->setEndValue(QSize(m_SearchViewBaseWidget->width(), 0));
  baseWidgetsAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  QPropertyAnimation *closeAnimation = new QPropertyAnimation(m_SearchView, "pos");
  connect(closeAnimation, &QPropertyAnimation::finished, this, [=]() {
    m_IsCloseAnimationFinished = true;
    m_SearchModel->clearSearchNode();
    m_SearchViewBaseWidget->hide();
  });
  closeAnimation->setDuration(300);
  closeAnimation->setEasingCurve(QEasingCurve::InOutSine);
  closeAnimation->setStartValue(m_SearchView->pos());
  closeAnimation->setEndValue(QPoint(m_SearchView->pos().x(), -m_SearchView->height()));
  closeAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  m_LastSize = baseWidgetsAnimation->endValue().toSize();
}
