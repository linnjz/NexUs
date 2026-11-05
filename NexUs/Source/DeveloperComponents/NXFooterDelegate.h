#ifndef NXFOOTERDELEGATE_H
#define NXFOOTERDELEGATE_H

#include <QModelIndex>
#include <QStyledItemDelegate>

#include "NXDef.h"
class NXNavigationNode;
class QPropertyAnimation;
class NXBaseListView;

class NXFooterDelegate : public QStyledItemDelegate
{
  Q_OBJECT
  Q_PRIVATE_CREATE(QPS_SCREF(QModelIndex), PressIndex)
  Q_PRIVATE_CREATE(NXBaseListView *, NXListView)

public:
  explicit NXFooterDelegate(QObject *parent = nullptr);
  ~NXFooterDelegate();
  void navigationNodeStateChange(QVariantMap const &data);

protected:
  void paint(QPainter *painter, QStyleOptionViewItem const &option, QModelIndex const &index) const override;
  QSize sizeHint(QStyleOptionViewItem const &option, QModelIndex const &index) const override;

private:
  bool m_IsSelectMarkDisplay { true };
  int m_LeftPadding { 11 };
  int m_IconAreaWidth { 40 };
  int m_TextRightSpacing { 3 };
  int m_IndicatorIconAreaWidth { 24 };
  NXThemeType::ThemeMode m_ThemeMode;
  qreal m_LastSelectMarkTop { 10 };
  qreal m_LastSelectMarkBottom { 10 };
  qreal m_SelectMarkTop { 10 };
  qreal m_SelectMarkBottom { 10 };
  NXNavigationNode *m_LastSelectedNode { nullptr };
  QPropertyAnimation *m_LastSelectMarkTopAnimation { nullptr };
  QPropertyAnimation *m_LastSelectMarkBottomAnimation { nullptr };
  QPropertyAnimation *m_SelectMarkTopAnimation { nullptr };
  QPropertyAnimation *m_SelectMarkBottomAnimation { nullptr };
  bool _compareItemY(NXNavigationNode *node1, NXNavigationNode *node2);
};

#endif // NXFOOTERDELEGATE_H
