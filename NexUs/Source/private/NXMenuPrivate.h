#ifndef NXMENUPRIVATE_H
#define NXMENUPRIVATE_H

#include <QObject>
#include <QPixmap>
#include <QPoint>

#include "NXProperty.h"
class NXMenu;
class NXMenuStyle;

class NXMenuPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXMenu)
  Q_PROPERTY_CREATE(QPS_PAEMIT(int), AnimationImagePosY)

public:
  explicit NXMenuPrivate(QObject *parent = nullptr);
  ~NXMenuPrivate();

private:
  bool m_IsCloseAnimation { false };
  QPoint m_MousePressPoint;
  QPixmap m_AnimationPix;
  NXMenuStyle *m_MenuStyle { nullptr };
};

#endif // NXMENUPRIVATE_H
