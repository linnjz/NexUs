#ifndef NXTERMINALWIDGET_H
#define NXTERMINALWIDGET_H

#include <QWidget>

#include "NXDef.h"
#include "NXProperty.h"

class NXTerminalWidgetPrivate;

class NX_EXPORT NXTerminalWidget : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXTerminalWidget)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), Prompt)
  Q_PROPERTY_CREATE_H(int, MaxHistorySize)
  Q_PROPERTY_CREATE_H(int, MaxLineCount)
  Q_PROPERTY_CREATE_H(int, FontPixelSize)

public:
  explicit NXTerminalWidget(QWidget *parent = nullptr);
  ~NXTerminalWidget();

  void appendOutput(QString const &text, QColor const &color = QColor());
  void appendHtml(QString const &html);
  void appendError(QString const &text);
  void appendSuccess(QString const &text);
  void clear();

  QStringList getCommandHistory() const;
  Q_SIGNAL void commandSubmitted(QString const &command);
  Q_SIGNAL void tabPressed(QString const &currentInput);

protected:
  bool eventFilter(QObject *watched, QEvent *event) override;
};

#endif // NXTERMINALWIDGET_H
