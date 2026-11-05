#ifndef NX_SLIDERPRIVATE_H
#define NX_SLIDERPRIVATE_H

#include <QObject>

#include "NXProperty.h"
class NXToolTip;
class NXSlider;
class NXSliderStyle;

class NXSliderPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXSlider)

public:
  explicit NXSliderPrivate(QObject *parent = nullptr);
  ~NXSliderPrivate();

private:
  bool m_MousePressed { false };
  NXToolTip *m_ValueToolTip { nullptr };
  NXSliderStyle *m_SliderStyle { nullptr };
};


#endif //! NX_SLIDERPRIVATE_H
