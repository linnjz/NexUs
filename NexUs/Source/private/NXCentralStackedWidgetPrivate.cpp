#include "NXCentralStackedWidgetPrivate.h"

#include "NXLazyStackedWidget.h"

NXCentralStackedWidgetPrivate::NXCentralStackedWidgetPrivate(QObject *parent)
    : QObject { parent }
{
}

NXCentralStackedWidgetPrivate::~NXCentralStackedWidgetPrivate()
{
}

void NXCentralStackedWidgetPrivate::onThemeModeChanged(NXThemeType::ThemeMode themeMode)
{
  m_ThemeMode = themeMode;
}

void NXCentralStackedWidgetPrivate::_getCurrentStackPix()
{
  m_TargetStackPix        = QPixmap();
  bool isTransparent     = m_IsTransparent;
  m_IsTransparent         = true;
  QWidget *currentWidget = m_ContainerStackedWidget->currentWidget();
  if (!currentWidget) {
    m_IsTransparent = isTransparent;
    return;
  }
  currentWidget->setVisible(true);
  m_CurrentStackPix = m_ContainerStackedWidget->grab();
  currentWidget->setVisible(false);
  m_IsTransparent = isTransparent;
}

void NXCentralStackedWidgetPrivate::_getTargetStackPix()
{
  m_TargetStackPix    = QPixmap();
  bool isTransparent = m_IsTransparent;
  m_IsTransparent     = true;
  m_TargetStackPix    = m_ContainerStackedWidget->grab();
  m_IsTransparent     = isTransparent;
}
