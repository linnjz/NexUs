#ifndef NXTERMINALWIDGETPRIVATE_H
#define NXTERMINALWIDGETPRIVATE_H

#include <QObject>
#include <QStringList>

#include "NXDef.h"
#include "NXProperty.h"

class QPlainTextEdit;
class QLineEdit;
class QLabel;
class NXTerminalWidget;

class NXTerminalWidgetPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXTerminalWidget)
  Q_PROPERTY_CREATE_D(QString, Prompt)
  Q_PROPERTY_CREATE_D(int, MaxHistorySize)
  Q_PROPERTY_CREATE_D(int, MaxLineCount)
  Q_PROPERTY_CREATE_D(int, FontPixelSize)

public:
  explicit NXTerminalWidgetPrivate(QObject *parent = nullptr);
  ~NXTerminalWidgetPrivate();

  void submitCommand();
  void navigateHistory(int direction);
  void updateFont();
  void trimOutput();

private:
  NXThemeType::ThemeMode m_ThemeMode;
  int m_HistoryIndex { -1 };
  QStringList m_CommandHistory;
  QString m_SavedInput;
  QPlainTextEdit *m_OutputArea { nullptr };
  QLineEdit *m_InputLine { nullptr };
  QLabel *m_PromptLabel { nullptr };
};

#endif // NXTERMINALWIDGETPRIVATE_H
