#include "NXTerminalWidget.h"

#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QScrollBar>
#include <QTextBlock>
#include <QTextDocument>
#include <QVBoxLayout>

#include "NXScrollBar.h"
#include "NXTheme.h"
#include "private/NXTerminalWidgetPrivate.h"

Q_PROPERTY_CREATE_CPP(NXTerminalWidget, QS_SET_CREF(QString), Prompt)
Q_PROPERTY_CREATE_CPP(NXTerminalWidget, int, MaxHistorySize)
Q_PROPERTY_CREATE_CPP(NXTerminalWidget, int, MaxLineCount)

void NXTerminalWidget::setFontPixelSize(int size)
{
  Q_D(NXTerminalWidget);
  d->_pFontPixelSize = size;
  d->updateFont();
  Q_EMIT pFontPixelSizeChanged();
}

int NXTerminalWidget::getFontPixelSize() const
{
  return d_ptr->_pFontPixelSize;
}

NXTerminalWidget::NXTerminalWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new NXTerminalWidgetPrivate())
{
  Q_D(NXTerminalWidget);
  d->q_ptr            = this;
  d->_pPrompt         = QStringLiteral("$ ");
  d->_pMaxHistorySize = 500;
  d->_pMaxLineCount   = 5000;
  d->_pFontPixelSize  = 14;
  setObjectName("NXTerminalWidget");

  d->_themeMode = nxTheme->getThemeMode();

  d->_outputArea = new QPlainTextEdit(this);
  d->_outputArea->setReadOnly(true);
  d->_outputArea->setFrameShape(QFrame::NoFrame);
  d->_outputArea->setVerticalScrollBar(new NXScrollBar(d->_outputArea));
  d->_outputArea->setHorizontalScrollBar(new NXScrollBar(d->_outputArea));
  d->_outputArea->setLineWrapMode(QPlainTextEdit::NoWrap);

  d->_promptLabel = new QLabel(d->_pPrompt, this);

  d->_inputLine = new QLineEdit(this);
  d->_inputLine->setFrame(false);

  QHBoxLayout *inputLayout = new QHBoxLayout();
  inputLayout->setContentsMargins(6, 2, 6, 6);
  inputLayout->setSpacing(4);
  inputLayout->addWidget(d->_promptLabel);
  inputLayout->addWidget(d->_inputLine);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);
  mainLayout->addWidget(d->_outputArea, 1);
  mainLayout->addLayout(inputLayout);

  auto applyTheme = [=](NXThemeType::ThemeMode mode) {
    d->_themeMode      = mode;
    QColor bgColor     = (mode == NXThemeType::Dark) ? QColor(0x1E, 0x1E, 0x2E) : QColor(0xF8, 0xF8, 0xF8);
    QColor textColor   = (mode == NXThemeType::Dark) ? QColor(0xCD, 0xD6, 0xF4) : QColor(0x2C, 0x2C, 0x2C);
    QColor inputBg     = (mode == NXThemeType::Dark) ? QColor(0x28, 0x28, 0x3A) : QColor(0xFF, 0xFF, 0xFF);
    QColor promptColor = NXThemeColor(mode, PrimaryNormal);

    QString outputStyle =
        QString(
            QStringLiteral(
                "QPlainTextEdit { background-color: %1; color: %2; selection-background-color: %3; border: none; }"))
            .arg(bgColor.name(), textColor.name(), NXThemeColor(mode, PrimaryNormal).name());
    d->_outputArea->setStyleSheet(outputStyle);

    QString inputStyle =
        QString(QStringLiteral("QLineEdit { background-color: %1; color: %2; border: none; padding: 4px; }"))
            .arg(inputBg.name(), textColor.name());
    d->_inputLine->setStyleSheet(inputStyle);

    d->_promptLabel->setStyleSheet(QString(QStringLiteral("QLabel { color: %1; }")).arg(promptColor.name()));

    setStyleSheet(QString(QStringLiteral("QWidget#NXTerminalWidget { background-color: %1; }")).arg(bgColor.name()));
  };
  applyTheme(d->_themeMode);
  connect(nxTheme, &NXTheme::themeModeChanged, this, applyTheme);

  d->updateFont();

  connect(this, &NXTerminalWidget::pPromptChanged, this, [=]() {
    d->_promptLabel->setText(d->_pPrompt);
  });

  d->_inputLine->installEventFilter(this);

  connect(d->_inputLine, &QLineEdit::returnPressed, d, &NXTerminalWidgetPrivate::submitCommand);
}

NXTerminalWidget::~NXTerminalWidget()
{
}

void NXTerminalWidget::appendOutput(const QString &text, const QColor &color)
{
  Q_D(NXTerminalWidget);
  if (color.isValid()) {
    d->_outputArea->appendHtml(
        QString(QStringLiteral("<span style=\"color:%1;\">%2</span>"))
            .arg(color.name(), text.toHtmlEscaped().replace(QStringLiteral("\n"), QStringLiteral("<br>"))));
  } else {
    d->_outputArea->appendPlainText(text);
  }
  d->trimOutput();
  d->_outputArea->verticalScrollBar()->setValue(d->_outputArea->verticalScrollBar()->maximum());
}

void NXTerminalWidget::appendHtml(const QString &html)
{
  Q_D(NXTerminalWidget);
  d->_outputArea->appendHtml(html);
  d->trimOutput();
  d->_outputArea->verticalScrollBar()->setValue(d->_outputArea->verticalScrollBar()->maximum());
}

void NXTerminalWidget::appendError(const QString &text)
{
  appendOutput(text, NXThemeColor(d_ptr->_themeMode, StatusDanger));
}

void NXTerminalWidget::appendSuccess(const QString &text)
{
  QColor green = (d_ptr->_themeMode == NXThemeType::Dark) ? QColor(0xA6, 0xE3, 0xA1) : QColor(0x0F, 0x7B, 0x0F);
  appendOutput(text, green);
}

void NXTerminalWidget::clear()
{
  Q_D(NXTerminalWidget);
  d->_outputArea->clear();
}

QStringList NXTerminalWidget::getCommandHistory() const
{
  return d_ptr->_commandHistory;
}

bool NXTerminalWidget::eventFilter(QObject *watched, QEvent *event)
{
  Q_D(NXTerminalWidget);
  if (watched == d->_inputLine && event->type() == QEvent::KeyPress) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
    if (keyEvent->key() == Qt::Key_Up) {
      d->navigateHistory(-1);
      return true;
    } else if (keyEvent->key() == Qt::Key_Down) {
      d->navigateHistory(1);
      return true;
    } else if (keyEvent->key() == Qt::Key_Tab) {
      Q_EMIT tabPressed(d->_inputLine->text());
      return true;
    }
  }
  return QWidget::eventFilter(watched, event);
}

NXTerminalWidgetPrivate::NXTerminalWidgetPrivate(QObject *parent)
    : QObject(parent)
{
}

NXTerminalWidgetPrivate::~NXTerminalWidgetPrivate()
{
}

void NXTerminalWidgetPrivate::submitCommand()
{
  Q_Q(NXTerminalWidget);
  QString command = _inputLine->text().trimmed();
  if (command.isEmpty()) {
    return;
  }

  QColor promptColor = NXThemeColor(_themeMode, PrimaryNormal);
  _outputArea->appendHtml(QString(QStringLiteral("<span style=\"color:%1;\">%2</span>%3"))
                              .arg(promptColor.name(), _pPrompt.toHtmlEscaped(), command.toHtmlEscaped()));

  if (_commandHistory.isEmpty() || _commandHistory.last() != command) {
    _commandHistory.append(command);
    if (_commandHistory.size() > _pMaxHistorySize) {
      _commandHistory.removeFirst();
    }
  }
  _historyIndex = -1;
  _savedInput.clear();

  _inputLine->clear();
  trimOutput();
  _outputArea->verticalScrollBar()->setValue(_outputArea->verticalScrollBar()->maximum());

  Q_EMIT q->commandSubmitted(command);
}

void NXTerminalWidgetPrivate::navigateHistory(int direction)
{
  if (_commandHistory.isEmpty()) {
    return;
  }

  if (_historyIndex == -1) {
    _savedInput = _inputLine->text();
  }

  if (direction < 0) {
    if (_historyIndex == -1) {
      _historyIndex = _commandHistory.size() - 1;
    } else if (_historyIndex > 0) {
      _historyIndex--;
    }
    _inputLine->setText(_commandHistory.at(_historyIndex));
  } else {
    if (_historyIndex == -1) {
      return;
    }
    if (_historyIndex < _commandHistory.size() - 1) {
      _historyIndex++;
      _inputLine->setText(_commandHistory.at(_historyIndex));
    } else {
      _historyIndex = -1;
      _inputLine->setText(_savedInput);
    }
  }
}

void NXTerminalWidgetPrivate::updateFont()
{
#ifdef Q_OS_MAC
  QFont monoFont(QStringLiteral("Menlo"));
#elif defined(Q_OS_WIN)
  QFont monoFont(QStringLiteral("Consolas"));
#else
  QFont monoFont(QStringLiteral("DejaVu Sans Mono"));
#endif
  monoFont.setPixelSize(_pFontPixelSize);
  _outputArea->setFont(monoFont);
  _inputLine->setFont(monoFont);
  _promptLabel->setFont(monoFont);
}

void NXTerminalWidgetPrivate::trimOutput()
{
  QTextDocument *doc = _outputArea->document();
  while (doc->blockCount() > _pMaxLineCount) {
    QTextCursor cursor(doc->begin());
    cursor.select(QTextCursor::BlockUnderCursor);
    cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
    cursor.removeSelectedText();
  }
}
