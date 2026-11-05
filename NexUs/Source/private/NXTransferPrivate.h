#ifndef NXTRANSFERPRIVATE_H
#define NXTRANSFERPRIVATE_H

#include <QObject>
#include <QStringList>

#include "NXDef.h"
class QHBoxLayout;
class QVBoxLayout;
class NXCheckBox;
class NXLineEdit;
class NXIconButton;
class NXText;
class NXBaseListView;
class QStandardItemModel;
class NXTransfer;

class NXTransferPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXTransfer)
  Q_PROPERTY_CREATE_D(QString, SourceTitle)
  Q_PROPERTY_CREATE_D(QString, TargetTitle)
  Q_PROPERTY_CREATE_D(int, BorderRadius)
  Q_PROPERTY_CREATE_D(int, ItemHeight)
  Q_PROPERTY_CREATE_D(bool, IsSearchVisible)

public:
  explicit NXTransferPrivate(QObject *parent = nullptr);
  ~NXTransferPrivate();

  Q_SLOT void onMoveToTarget();
  Q_SLOT void onMoveToSource();
  Q_SLOT void onSourceSearchTextChanged(QString const &text);
  Q_SLOT void onTargetSearchTextChanged(QString const &text);
  Q_SLOT void onSourceSelectAllChanged(int state);
  Q_SLOT void onTargetSelectAllChanged(int state);

  void _refreshCountText();

private:
  friend class NXTransfer;
  NXThemeType::ThemeMode m_ThemeMode;
  NXCheckBox *m_SourceSelectAll { nullptr };
  NXText *m_SourceTitleText { nullptr };
  NXText *m_SourceCountText { nullptr };
  NXLineEdit *m_SourceSearchEdit { nullptr };
  NXBaseListView *m_SourceListView { nullptr };
  QStandardItemModel *m_SourceModel { nullptr };
  NXCheckBox *m_TargetSelectAll { nullptr };
  NXText *m_TargetTitleText { nullptr };
  NXText *m_TargetCountText { nullptr };
  NXLineEdit *m_TargetSearchEdit { nullptr };
  NXBaseListView *m_TargetListView { nullptr };
  QStandardItemModel *m_TargetModel { nullptr };
  NXIconButton *m_MoveToTargetButton { nullptr };
  NXIconButton *m_MoveToSourceButton { nullptr };
};

#endif // NXTRANSFERPRIVATE_H
