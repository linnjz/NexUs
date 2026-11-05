#ifndef NXSCROLLPAGE_H
#define NXSCROLLPAGE_H

#include <QWidget>

#include "NXProperty.h"
class NXScrollArea;
class NXText;
class QHBoxLayout;
class NXScrollPagePrivate;

class NX_EXPORT NXScrollPage : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXScrollPage)

public:
  explicit NXScrollPage(QWidget *parent = nullptr);
  ~NXScrollPage();

  void setTopCustomWidget(QWidget *customWidget);
  QWidget *getTopCustomWidget() const;

  void setBottomCustomWidget(QWidget *customWidget);
  QWidget *getBottomCustomWidget() const;

  void addCentralWidget(QWidget *centralWidget, bool isWidgetResizeable = true, bool isVerticalGrabGesture = true, qreal mousePressEventDelay = 0.0, Qt::ScrollBarPolicy vScrollBarPolicy = Qt::ScrollBarAsNeeded, Qt::ScrollBarPolicy hScrollBarPolicy = Qt::ScrollBarAlwaysOff);

  void setPageTitle(QString const &title);

  void navigation(int widgetIndex, bool isLogRoute = true);

  void setPageTitleSpacing(int spacing);
  int getPageTitleSpacing() const;
  void setTitleVisible(bool isVisible);

  void setPageTitlePixelSize(int pixelSize);
  int getPageTitlePixelSize() const;
};

#endif // NXSCROLLPAGE_H
