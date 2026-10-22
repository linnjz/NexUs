#ifndef NXPROGRESSBARSTYLE_H
#define NXPROGRESSBARSTYLE_H

#include <QProxyStyle>

#include "NXDef.h"

class NXProgressBarStyle : public QProxyStyle
{
  Q_OBJECT

public:
  explicit NXProgressBarStyle(QStyle *style = nullptr);
  ~NXProgressBarStyle();
  void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
  QRect subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget) const override;

private:
  NXThemeType::ThemeMode _themeMode;
};

#endif // NXPROGRESSBARSTYLE_H
