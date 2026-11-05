#include <QApplication>
#include <QScreen>

#include "NXApplication.h"
#include "NXLog.h"
#include "mainwindow.h"
#ifdef Q_OS_WIN
#  include <Windows.h>
#endif
int main(int argc, char *argv[])
{
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#  if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
  QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#  else
  //根据实际屏幕缩放比例更改
  qputenv(QStringLiteral("QT_SCALE_FACTOR"), QStringLiteral("1.5"));
#  endif
#endif
  QApplication a(argc, argv);
  nxApp->init();
  //NXLog::getInstance()->initMessageLog(true);
  MainWindow w;
  w.show();
#ifdef Q_OS_WIN
//    HWND handle = FindWindowA(NULL, "NXWidgetTool");
//    if (handle != NULL)
//    {
//        SetWindowDisplayAffinity(handle, WDA_EXCLUDEFROMCAPTURE);
//    }
#endif
  return a.exec();
}
