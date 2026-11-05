#ifndef NXRIBBONTABBAR_H
#define NXRIBBONTABBAR_H

#include <QWidget>

#include "NXDef.h"
#include "NXProperty.h"

class NXRibbonTabBarPrivate;

class NX_EXPORT NXRibbonTabBar : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXRibbonTabBar)
  Q_PROPERTY_CREATE_H(int, CurrentIndex)

public:
  explicit NXRibbonTabBar(QWidget *parent = nullptr);
  ~NXRibbonTabBar();

  int appendTab(QString const &title);
  void removeTab(int index);
  void clear();

  int tabCount() const;
  QString tabText(int index) const;
  void setTabText(int index, QString const &title);

  // 禁用态:文字置灰 不可点击 无悬停反馈 用于Ribbon整页启用/禁用联动
  void setTabEnabled(int index, bool isEnable);
  bool isTabEnabled(int index) const;

  Q_SIGNAL void tabClicked(int index);
  Q_SIGNAL void tabReclicked(int index);

protected:
  void paintEvent(QPaintEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void leaveEvent(QEvent *event) override;
  QSize sizeHint() const override;
};

#endif // NXRIBBONTABBAR_H
