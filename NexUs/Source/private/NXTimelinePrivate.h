#ifndef NXTIMELINEPRIVATE_H
#define NXTIMELINEPRIVATE_H

#include <QObject>

#include "NXDef.h"
#include "NXTimeline.h"

class NXTimelinePrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXTimeline)

public:
  explicit NXTimelinePrivate(QObject *parent = nullptr);
  ~NXTimelinePrivate();

private:
  NXThemeType::ThemeMode m_ThemeMode;
  QList<NXTimeline::TimelineItem> m_Items;
};

#endif // NXTIMELINEPRIVATE_H
