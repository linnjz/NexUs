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
  Q_PROPERTY_CREATE(QS_SET_CREF(QModelIndex), CurrentHoverIndex)
  Q_PROPERTY_CREATE(int, HeaderMargin)
  Q_PROPERTY_CREATE(int, BorderRadius)
  Q_PROPERTY_CREATE(int, CheckIndicatorWidth)
  Q_PROPERTY_CREATE(int, DefaultPadding)
  Q_PROPERTY_CREATE(bool, IsHoverRowEffectEnable)
  Q_PROPERTY_CREATE(bool, IsSelectedRowEffectEnable)

public:
  explicit NXTableViewStyle(QStyle *style = nullptr);
  ~NXTableViewStyle();
  void setColumnPadding(int column, int padding);
  int getColumnPadding(int column) const;
  void clearColumnPadding(int column);

  void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
  void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
  int pixelMetric(PixelMetric metric, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const override;

private:
  NXThemeType::ThemeMode _themeMode;
  QHash<int, int> _columnPaddingMap;

  void _drawCheckIndicator(QPainter *painter, QRect rect, Qt::CheckState state) const;
};

#endif // NXTABLEVIEWSTYLE_H
