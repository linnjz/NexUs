#ifndef NXUTOCOMPLETEDELEGATE_H
#define NXUTOCOMPLETEDELEGATE_H

#include <QStyledItemDelegate>

#include "NXAutoComplete.h"
#include "NXDef.h"

class NXAutoCompleteDelegate : public QStyledItemDelegate
{
  Q_OBJECT

public:
  explicit NXAutoCompleteDelegate(QObject *parent = nullptr);
  ~NXAutoCompleteDelegate();

  void setHighlightText(QString const &text);
  void setMatchMode(NXAutoComplete::MatchMode mode);

protected:
  void paint(QPainter *painter, QStyleOptionViewItem const &option, QModelIndex const &index) const override;
  QSize sizeHint(QStyleOptionViewItem const &option, QModelIndex const &index) const override;

private:
  NXThemeType::ThemeMode m_ThemeMode;
  QString m_HighlightText;
  NXAutoComplete::MatchMode m_MatchMode { NXAutoComplete::Contains };
};

#endif // NXUTOCOMPLETEDELEGATE_H
