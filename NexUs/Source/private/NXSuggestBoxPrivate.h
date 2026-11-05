#ifndef NXSUGGESTBOXPRIVATE_H
#define NXSUGGESTBOXPRIVATE_H

#include <QAction>
#include <QObject>
#include <QSize>
#include <QVariantMap>
#include <QVector>

#include "NXDef.h"

class NXSuggestion : public QObject
{
  Q_OBJECT
  Q_PROPERTY_CREATE(QPS_SCREF_PAEMIT(QString), SuggestText)
  Q_PROPERTY_CREATE(QPS_SCREF_PAEMIT(QString), SuggestKey)
  Q_PROPERTY_CREATE(QPS_SCREF_PAEMIT(QVariantMap), SuggestData)
  Q_PROPERTY_CREATE(QPS_PAEMIT(NXIconType::IconName), NXIcon)

public:
  explicit NXSuggestion(QObject *parent = nullptr);
  ~NXSuggestion();
};

class QVBoxLayout;
class NXLineEdit;
class NXNavigationNode;
class NXSuggestModel;
class NXBaseListView;
class NXSuggestDelegate;
class NXSuggestBox;
class NXSuggestBoxSearchViewContainer;

class NXSuggestBoxPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXSuggestBox)
  Q_PROPERTY_CREATE_D(int, BorderRadius)
  Q_PROPERTY_CREATE_D(Qt::CaseSensitivity, CaseSensitivity)

public:
  explicit NXSuggestBoxPrivate(QObject *parent = nullptr);
  ~NXSuggestBoxPrivate();
  Q_SLOT void onThemeModeChanged(NXThemeType::ThemeMode themeMode);
  Q_SLOT void onSearchEditTextEdit(QString const &searchText);
  Q_SLOT void onSearchViewClicked(QModelIndex const &index);

private:
  bool m_IsExpandAnimationFinished { true };
  bool m_IsCloseAnimationFinished { true };
  NXThemeType::ThemeMode m_ThemeMode;
  QSize m_LastSize;
  QVector<NXSuggestion *> m_SuggestionVector;
  QAction *m_LightSearchAction { nullptr };
  QAction *m_DarkSearchAction { nullptr };
  NXSuggestBoxSearchViewContainer *m_SearchViewBaseWidget { nullptr };
  NXLineEdit *m_SearchEdit { nullptr };
  NXSuggestModel *m_SearchModel { nullptr };
  NXBaseListView *m_SearchView { nullptr };
  NXSuggestDelegate *m_SearchDelegate { nullptr };
  QVBoxLayout *m_ShadowLayout { nullptr };
  void _startSizeAnimation(QSize oldSize, QSize newSize);
  void _startExpandAnimation();
  void _startCloseAnimation();
};

#endif // NXSUGGESTBOXPRIVATE_H
