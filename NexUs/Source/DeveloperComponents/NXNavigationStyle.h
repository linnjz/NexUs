#ifndef NXNAVIGATIONSTYLE_H
#define NXNAVIGATIONSTYLE_H
#include <QModelIndex>
#include <QProxyStyle>

#include "NXDef.h"
class NXNavigationNode;
class NXNavigationView;
class QPropertyAnimation;

class NXNavigationStyle : public QProxyStyle
{
  Q_OBJECT
  Q_PRIVATE_CREATE(QPS_SCREF(QModelIndex), PressIndex)
  Q_PROPERTY_CREATE(QPS_PAEMIT(qreal), Opacity)
  Q_PROPERTY_CREATE(QPS_PAEMIT(qreal), Rotate)
  Q_PROPERTY_CREATE(QPS_PAEMIT(qreal), LastSelectMarkTop)
  Q_PROPERTY_CREATE(QPS_PAEMIT(qreal), LastSelectMarkBottom)
  Q_PROPERTY_CREATE(QPS_PAEMIT(qreal), SelectMarkTop)
  Q_PROPERTY_CREATE(QPS_PAEMIT(qreal), SelectMarkBottom)
  Q_PRIVATE_CREATE(NXNavigationView *, NavigationView)
  Q_PROPERTY_CREATE(QPS_PAEMIT(int), ItemHeight)

public:
  explicit NXNavigationStyle(QStyle *style = nullptr);
  ~NXNavigationStyle();
  void drawPrimitive(PrimitiveElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget = nullptr) const override;
  void drawControl(ControlElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget = nullptr) const override;
  QSize sizeFromContents(ContentsType type, QStyleOption const *option, QSize const &size, QWidget const *widget) const override;

  void navigationNodeStateChange(QVariantMap const &data);

private:
  bool m_IsSelectMarkDisplay { true };
  NXThemeType::ThemeMode m_ThemeMode;
  int m_LeftPadding { 11 };
  int m_IconAreaWidth { 40 };
  int m_TextRightSpacing { 3 };
  int m_IndicatorIconAreaWidth { 34 };
  NXNavigationNode *m_OpacityAnimationTargetNode { nullptr };
  NXNavigationNode *m_ExpandAnimationTargetNode { nullptr };
  NXNavigationNode *m_LastSelectedNode { nullptr };
  QPropertyAnimation *m_LastSelectMarkTopAnimation { nullptr };
  QPropertyAnimation *m_LastSelectMarkBottomAnimation { nullptr };
  QPropertyAnimation *m_SelectMarkTopAnimation { nullptr };
  QPropertyAnimation *m_SelectMarkBottomAnimation { nullptr };
  bool _compareItemY(NXNavigationNode *node1, NXNavigationNode *node2);
};

#endif // NXNAVIGATIONSTYLE_H
