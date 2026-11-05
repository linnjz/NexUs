#ifndef NXFRAMEWORK_NEXUS_DEVELOPERCOMPONENTS_NXDRAWERCONTAINER_H_
#define NXFRAMEWORK_NEXUS_DEVELOPERCOMPONENTS_NXDRAWERCONTAINER_H_

#include <QGraphicsOpacityEffect>
#include <QVBoxLayout>
#include <QWidget>

#include "NXDef.h"

class NXDrawerContainer : public QWidget
{
  Q_OBJECT
  Q_PRIVATE_CREATE(QPS_SCREF(QPixmap), ContainerPix)
  Q_PROPERTY_CREATE(QPS_PAEMIT(qreal), Opacity)
  Q_PRIVATE_CREATE(int, BorderRadius)

public:
  explicit NXDrawerContainer(QWidget *parent = nullptr);
  ~NXDrawerContainer();

  void addWidget(QWidget *widget);
  void removeWidget(QWidget *widget);

  void doDrawerAnimation(bool isExpand);

protected:
  void paintEvent(QPaintEvent *event) override;

private:
  bool m_IsShowBorder { true };
  NXThemeType::ThemeMode m_ThemeMode;
  QList<QWidget *> m_DrawerWidgetList;
  QWidget *m_ContainerWidget { nullptr };
  QVBoxLayout *m_MainLayout { nullptr };
  QVBoxLayout *m_ContainerLayout { nullptr };
};

#endif //NXFRAMEWORK_NEXUS_DEVELOPERCOMPONENTS_NXDRAWERCONTAINER_H_
