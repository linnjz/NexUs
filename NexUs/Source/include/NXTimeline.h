#ifndef NXTIMELINE_H
#define NXTIMELINE_H

#include <QWidget>

#include "NXDef.h"
#include "NXProperty.h"

class NXTimelinePrivate;

class NX_EXPORT NXTimeline : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXTimeline)

public:
  struct TimelineItem
  {
    QString title;
    QString content;
    QString timestamp;
    NXIconType::IconName icon = NXIconType::None;
  };

  explicit NXTimeline(QWidget *parent = nullptr);
  ~NXTimeline();

  void addItem(TimelineItem const &item);
  void clearItems();
  int getItemCount() const;

protected:
  void paintEvent(QPaintEvent *event) override;
  QSize sizeHint() const override;
};

#endif // NXTIMELINE_H
