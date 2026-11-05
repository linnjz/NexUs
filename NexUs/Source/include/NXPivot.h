#ifndef NXPIVOT_H
#define NXPIVOT_H
#include <QPixmap>
#include <QStringList>
#include <QWidget>

#include "NXDef.h"

class NXPivotPrivate;

class NX_EXPORT NXPivot : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXPivot)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QColor), TextNormalColor)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QColor), TextFocusColor)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QColor), TextPressedColor)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QColor), IndicatorColor)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QColor), ItemBackgroundColor)
  Q_PROPERTY_CREATE_H(QPoint, OverlayTextOffsetFromCenterAnchor)
  Q_PROPERTY_CREATE_H(QSize, ItemFixedSize)
  Q_PROPERTY_CREATE_H(NXPivotType::DisplayMode, DisplayMode)
  Q_PROPERTY_CREATE_H(NXPivotType::IndicatorTypeFlags, IndicatorTypeFlags)
  Q_PROPERTY_CREATE_H(Qt::CursorShape, ItemCursor)
  Q_PROPERTY_CREATE_H(int, ItemHorizontalSpacing)
  Q_PROPERTY_CREATE_H(int, ItemVerticalSpacing)
  Q_PROPERTY_CREATE_H(int, TextIconSpacing)
  Q_PROPERTY_CREATE_H(int, TextPixelSize)
  Q_PROPERTY_CREATE_H(int, CurrentIndex)
  Q_PROPERTY_CREATE_H(int, IndicatorWidth)
  Q_PROPERTY_CREATE_H(bool, IsAutoAdaptiveItemHeight)
  Q_PROPERTY_CREATE_H(bool, IsItemFixedSize)
  Q_PROPERTY_CREATE_H(bool, IsHoverItemBackgroundEnable)

public:
  explicit NXPivot(QWidget *parent = nullptr);
  ~NXPivot();

  void appendPivot(QPixmap const &pixmap);
  void appendPivot(QString const &pivotTitle, QPixmap const &pixmap = {});

  void insertPivot(int index, QPixmap const &pixmap);
  void insertPivot(int index, QString const &pivotTitle, QPixmap const &pixmap = {});

  void removePivot(int index);
  void removePivot(QString const &pivotTitle);

  void setPivot(int index, QPixmap const &pixmap);
  void setPivot(int index, QString const &pivotTitle, QPixmap const &pixmap = {});

  void clearPivot();
  QStringList getPivotTitle() const;
  void setCurrentTitle(QString const &pivotTitle);

  Q_SIGNAL void pivotChanged(int index);
  Q_SIGNAL void pivotInserted(int index);
  Q_SIGNAL void pivotRemoved(int index);
  Q_SIGNAL void pivotClicked(int index);
  Q_SIGNAL void pivotDoubleClicked(int index);
};

#endif // NXPIVOT_H
