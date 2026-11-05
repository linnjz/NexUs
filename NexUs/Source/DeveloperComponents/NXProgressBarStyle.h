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
  void drawControl(ControlElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget = nullptr) const override;
  QRect subElementRect(SubElement element, QStyleOption const *option, QWidget const *widget) const override;

private:
  NXThemeType::ThemeMode m_ThemeMode;
};

#endif // NXPROGRESSBARSTYLE_H
