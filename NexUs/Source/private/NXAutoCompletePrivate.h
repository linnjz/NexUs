#ifndef NXUTOCOMPLETEPRIVATE_H
#define NXUTOCOMPLETEPRIVATE_H

#include <QObject>
#include <QStringList>

#include "NXAutoComplete.h"
#include "NXDef.h"

class NXLineEdit;
class NXBaseListView;
class QVBoxLayout;
class QStringListModel;
class QSortFilterProxyModel;
class NXAutoCompleteContainer;

class NXAutoCompletePrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXAutoComplete)
  Q_PROPERTY_CREATE_D(int, BorderRadius)
  Q_PROPERTY_CREATE_D(int, MaxVisibleItems)
  Q_PROPERTY_CREATE_D(Qt::CaseSensitivity, CaseSensitivity)

public:
  explicit NXAutoCompletePrivate(QObject *parent = nullptr);
  ~NXAutoCompletePrivate();

  Q_SLOT void onThemeModeChanged(NXThemeType::ThemeMode themeMode);
  Q_SLOT void onTextEdited(QString const &text);
  Q_SLOT void onItemClicked(QModelIndex const &index);

private:
  bool m_IsAllowHidePopup { false };
  bool m_IsPopupVisible { false };
  NXThemeType::ThemeMode m_ThemeMode;
  NXAutoComplete::MatchMode m_MatchMode { NXAutoComplete::Contains };
  QStringList m_Completions;
  NXLineEdit *m_LineEdit { nullptr };
  NXAutoCompleteContainer *m_Popup { nullptr };
  NXBaseListView *m_ListView { nullptr };
  QVBoxLayout *m_PopupLayout { nullptr };
  QStringListModel *m_SourceModel { nullptr };
  QSortFilterProxyModel *m_FilterModel { nullptr };

  void _showPopup();
  void _hidePopup();
  void _updateFilter(QString const &text);
};

#endif // NXUTOCOMPLETEPRIVATE_H
