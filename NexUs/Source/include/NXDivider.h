#ifndef NXDIVIDER_H
#define NXDIVIDER_H

#include <QWidget>

#include "NXDef.h"
#include "NXProperty.h"
class NXDividerPrivate;

class NX_EXPORT NXDivider : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXDivider)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), Text)
  Q_PROPERTY_CREATE_H(Qt::Orientation, Orientation)
  Q_PROPERTY_CREATE_H(int, ContentPosition)

public:
  enum ContentPositionType
  {
    Left   = 0,
    Center = 1,
    Right  = 2
  };
  Q_ENUM(ContentPositionType)

  explicit NXDivider(QWidget *parent = nullptr);
  explicit NXDivider(QString const &text, QWidget *parent = nullptr);
  ~NXDivider();

protected:
  void paintEvent(QPaintEvent *event) override;
  QSize sizeHint() const override;
};

#endif // NXDIVIDER_H
