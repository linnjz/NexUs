#include "NXScrollPagePrivate.h"

#include <QHBoxLayout>
#include <QPropertyAnimation>
#include <QStackedWidget>
#include <QTimer>

#include "NXBreadcrumbBar.h"
#include "NXScrollPage.h"

NXScrollPagePrivate::NXScrollPagePrivate(QObject *parent)
    : QObject { parent }
{
}

NXScrollPagePrivate::~NXScrollPagePrivate()
{
}

void NXScrollPagePrivate::onNavigationRoute(QVariantMap const &routeData)
{
  // 面包屑
  Q_Q(NXScrollPage);
  QString pageCheckSumKey = routeData.value(QStringLiteral("NXScrollPageCheckSumKey")).toString();
  bool isRouteBack        = routeData.value(QStringLiteral("NXRouteBackMode")).toBool();
  if (pageCheckSumKey == QStringLiteral("Navigation")) {
    QString pageTitle = isRouteBack ? routeData.value(QStringLiteral("NXBackPageTitle")).toString()
                                    : routeData.value(QStringLiteral("NXForwardPageTitle")).toString();
    q->navigation(m_CentralWidgetMap.value(pageTitle), false);
  } else if (pageCheckSumKey == QStringLiteral("BreadcrumbClicked")) {
    QStringList breadcrumbList = isRouteBack
                                   ? routeData.value(QStringLiteral("NXBackBreadcrumbList")).toStringList()
                                   : routeData.value(QStringLiteral("NXForwardBreadcrumbList")).toStringList();
    int widgetIndex            = m_CentralWidgetMap.value(breadcrumbList.last());
    _switchCentralStackIndex(widgetIndex, m_NavigationTargetIndex);
    m_NavigationTargetIndex = widgetIndex;
    m_BreadcrumbBar->setBreadcrumbList(breadcrumbList);
  }
}

void NXScrollPagePrivate::_switchCentralStackIndex(int targetIndex, int lastIndex)
{
  QWidget *currentWidget = m_CentralStackedWidget->widget(lastIndex);
  QWidget *targetWidget  = m_CentralStackedWidget->widget(targetIndex);
  targetWidget->resize(currentWidget->size());
  targetWidget->setVisible(true);

  QPropertyAnimation *currentWidgetAnimation = new QPropertyAnimation(currentWidget, "pos");
  currentWidgetAnimation->setEasingCurve(QEasingCurve::InExpo);
  currentWidgetAnimation->setDuration(300);

  QPropertyAnimation *targetWidgetAnimation = new QPropertyAnimation(targetWidget, "pos");
  connect(targetWidgetAnimation, &QPropertyAnimation::finished, this, [=]() {
    m_CentralStackedWidget->setCurrentIndex(targetIndex);
  });
  targetWidgetAnimation->setEasingCurve(QEasingCurve::InExpo);
  targetWidgetAnimation->setDuration(300);
  if (targetIndex > lastIndex) {
    //左滑
    currentWidgetAnimation->setStartValue(currentWidget->pos());
    currentWidgetAnimation->setEndValue(QPoint(-m_CentralStackedWidget->width(), 0));
    targetWidgetAnimation->setStartValue(QPoint(m_CentralStackedWidget->width(), 0));
    targetWidgetAnimation->setEndValue(QPoint(0, 0));
  } else {
    //右滑
    currentWidgetAnimation->setStartValue(currentWidget->pos());
    currentWidgetAnimation->setEndValue(QPoint(m_CentralStackedWidget->width(), 0));
    targetWidgetAnimation->setStartValue(QPoint(-m_CentralStackedWidget->width(), 0));
    targetWidgetAnimation->setEndValue(QPoint(0, 0));
  }
  currentWidgetAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  targetWidgetAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}
