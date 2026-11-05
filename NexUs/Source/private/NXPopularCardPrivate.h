#ifndef NXPOPULARCARDPRIVATE_H
#define NXPOPULARCARDPRIVATE_H

#include <QObject>
#include <QPixmap>

#include "NXDef.h"
class NXPopularCard;
class NXPopularCardFloater;

class NXPopularCardPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXPopularCard)
  Q_PROPERTY_CREATE_D(QPixmap, CardPixmap)
  Q_PROPERTY_CREATE_D(QPixmap, CardFloatPixmap)
  Q_PROPERTY_CREATE_D(QString, Title)
  Q_PROPERTY_CREATE_D(QString, SubTitle)
  Q_PROPERTY_CREATE_D(QString, InteractiveTips)
  Q_PROPERTY_CREATE_D(QString, DetailedText)
  Q_PROPERTY_CREATE_D(QString, CardButtonText)
  Q_PROPERTY_CREATE_D(QWidget *, CardFloatArea)
  Q_PROPERTY_CREATE(QPS_PAEMIT(qreal), HoverYOffset)
  Q_PROPERTY_CREATE(QPS_PAEMIT(qreal), HoverOpacity)
  Q_PROPERTY_CREATE_D(int, BorderRadius)

public:
  explicit NXPopularCardPrivate(QObject *parent = nullptr);
  ~NXPopularCardPrivate();

private:
  friend class NXPopularCardFloater;
  bool m_IsFloating { false };
  NXThemeType::ThemeMode m_ThemeMode;
  int m_ShadowBorderWidth { 6 };
  int m_TextHSpacing { 20 };
  int m_TextVSpacing { 5 };
  QRect m_ButtonTargetRect;
  QRectF m_InteractiveTipsBaseRect;
  QTimer *m_FloatTimer { nullptr };
  NXPopularCardFloater *m_Floater { nullptr };

  void _showFloater();
};

#endif // NXPOPULARCARDPRIVATE_H
