#ifndef NXFRAMEWORK_NEXUS_DEVELOPERCOMPONENTS_NXDRAWERHEADER_H_
#define NXFRAMEWORK_NEXUS_DEVELOPERCOMPONENTS_NXDRAWERHEADER_H_

#include <QVBoxLayout>
#include <QWidget>

#include "NXDef.h"

class NXDrawerHeader : public QWidget
{
  Q_OBJECT
  Q_PROPERTY_CREATE(QPS_PAEMIT(qreal), ExpandIconRotate)
  Q_PRIVATE_CREATE(int, BorderRadius)
  Q_PRIVATE_CREATE(bool, IsExpand)

public:
  explicit NXDrawerHeader(QWidget *parent = nullptr);
  ~NXDrawerHeader();

  void setHeaderWidget(QWidget *widget);
  void doExpandOrCollapseAnimation();
  Q_SIGNAL void drawerHeaderClicked(bool isExpand);

protected:
  bool event(QEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void paintEvent(QPaintEvent *event) override;

private:
  bool m_IsPressed { false };
  NXThemeType::ThemeMode m_ThemeMode;
  QWidget *m_HeaderWidget { nullptr };
  QVBoxLayout *m_MainLayout { nullptr };
};

#endif //NXFRAMEWORK_NEXUS_DEVELOPERCOMPONENTS_NXDRAWERHEADER_H_
