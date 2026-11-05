#ifndef NXCUSTOMTABWIDGET_H
#define NXCUSTOMTABWIDGET_H

#include "NXCustomWidget.h"

class NXTabBar;
class NXTabWidget;

class NXCustomTabWidget : public NXCustomWidget
{
  Q_OBJECT
  Q_PROPERTY_CREATE(QPS_PAEMIT(bool), IsFinished)

public:
  explicit NXCustomTabWidget(QWidget *parent = nullptr);
  ~NXCustomTabWidget();
  void addTab(QWidget *widget, QIcon const &tabIcon, QString const &tabTitle);
  NXTabBar *getCustomTabBar() const;
  NXTabWidget *getCustomTabWidget() const;

  Q_INVOKABLE bool processHitTest();

private:
  bool m_IsAllowLeave { false };
  NXTabBar *m_CustomTabBar { nullptr };
  NXTabWidget *m_CustomTabWidget { nullptr };
};

#endif // NXCUSTOMTABWIDGET_H
