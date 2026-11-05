#ifndef NXTABLEVIEWSTYLE_H
#define NXTABLEVIEWSTYLE_H
#include <QHash>
#include <QModelIndex>
#include <QProxyStyle>

#include "NXDef.h"
class QStyleOptionViewItem;

class NXTableViewStyle : public QProxyStyle
{
  Q_OBJECT
  Q_PROPERTY_CREATE(QPS_SCREF_PAEMIT(QModelIndex), CurrentHoverIndex)
  Q_PROPERTY_CREATE(QPS_PAEMIT(int), HeaderMargin)
  Q_PROPERTY_CREATE(QPS_PAEMIT(int), BorderRadius)
  Q_PROPERTY_CREATE(QPS_PAEMIT(int), CheckIndicatorWidth)
  Q_PROPERTY_CREATE(QPS_PAEMIT(int), DefaultPadding)
  Q_PROPERTY_CREATE(QPS_PAEMIT(bool), IsHoverRowEffectEnable)
  Q_PROPERTY_CREATE(QPS_PAEMIT(bool), IsSelectedRowEffectEnable)

public:
  explicit NXTableViewStyle(QStyle *style = nullptr);
  ~NXTableViewStyle();
  void setColumnPadding(int column, int padding);
  int getColumnPadding(int column) const;
  void clearColumnPadding(int column);

  void drawPrimitive(PrimitiveElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget = nullptr) const override;
  void drawControl(ControlElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget = nullptr) const override;
  int pixelMetric(PixelMetric metric, QStyleOption const *option = nullptr, QWidget const *widget = nullptr) const override;

private:
  NXThemeType::ThemeMode m_ThemeMode;
  QHash<int, int> m_ColumnPaddingMap;

  void _drawCheckIndicator(QPainter *painter, QRect rect, Qt::CheckState state) const;
};

#endif // NXTABLEVIEWSTYLE_H
