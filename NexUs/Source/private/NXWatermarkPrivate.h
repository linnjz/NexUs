#ifndef NXWATERMARKPRIVATE_H
#define NXWATERMARKPRIVATE_H

#include <QObject>
#include <QPixmap>

#include "NXDef.h"
#include "NXWatermark.h"

class NXWatermarkPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXWatermark)
  Q_PROPERTY_CREATE_D(QString, Text)
  Q_PROPERTY_CREATE_D(QImage, Image)
  Q_PROPERTY_CREATE_D(QColor, TextColor)
  Q_PROPERTY_CREATE_D(qreal, Opacity)
  Q_PROPERTY_CREATE_D(qreal, Rotation)
  Q_PROPERTY_CREATE_D(int, FontPixelSize)
  Q_PROPERTY_CREATE_D(int, GapX)
  Q_PROPERTY_CREATE_D(int, GapY)
  Q_PROPERTY_CREATE_D(int, ImageWidth)
  Q_PROPERTY_CREATE_D(int, ImageHeight)

public:
  explicit NXWatermarkPrivate(QObject *parent = nullptr);
  ~NXWatermarkPrivate();

  void rebuildPattern();

private:
  bool m_PatternDirty { true };
  NXThemeType::ThemeMode m_ThemeMode;
  QPixmap m_PatternPixmap;
};

#endif // NXWATERMARKPRIVATE_H
