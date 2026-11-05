#ifndef NXTOOLBUTTONPRIVATE_H
#define NXTOOLBUTTONPRIVATE_H

#include <QObject>

#include "NXProperty.h"
class NXToolButton;
class NXToolButtonStyle;

class NXToolButtonPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXToolButton)

public:
  explicit NXToolButtonPrivate(QObject *parent = nullptr);
  ~NXToolButtonPrivate();

private:
  NXToolButtonStyle *m_ToolButtonStyle { nullptr };
};

#endif // NXTOOLBUTTONPRIVATE_H
