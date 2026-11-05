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

Q_PROPERTY_CREATE_CPP(NXTerminalWidget, QPS_SCREF_PAEMIT(QString), Prompt)
Q_PROPERTY_CREATE_CPP(NXTerminalWidget, QPS_PAEMIT(int), MaxHistorySize)
Q_PROPERTY_CREATE_CPP(NXTerminalWidget, QPS_PAEMIT(int), MaxLineCount)

void NXTerminalWidget::setFontPixelSize(int size)
{
  Q_D(NXTerminalWidget);
  d->m_FontPixelSize = size;
  d->updateFont();
  Q_EMIT signalFontPixelSizeChanged();
}

int NXTerminalWidget::getFontPixelSize() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  return d_ptr->m_FontPixelSize;
}

NXTerminalWidget::NXTerminalWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new NXTerminalWidgetPrivate())
{
  Q_D(NXTerminalWidget);
  d->q_ptr            = this;
  d->m_Prompt         = QStringLiteral("$ ");
  d->m_MaxHistorySize = 500;
  d->m_MaxLineCount   = 5000;
  d->m_FontPixelSize  = 14;
  setObjectName("NXTerminalWidget");

  d->m_ThemeMode = nxTheme->getThemeMode();

  d->m_OutputArea = new QPlainTextEdit(this);
  d->m_OutputArea->setReadOnly(true);
  d->m_OutputArea->setFrameShape(QFrame::NoFrame);
  d->m_OutputArea->setVerticalScrollBar(new NXScrollBar(d->m_OutputArea));
  d->m_OutputArea->setHorizontalScrollBar(new NXScrollBar(d->m_OutputArea));
  d->m_OutputArea->setLineWrapMode(QPlainTextEdit::NoWrap);

  d->m_PromptLabel = new QLabel(d->m_Prompt, this);

  d->m_InputLine = new QLineEdit(this);
  d->m_InputLine->setFrame(false);

  QHBoxLayout *inputLayout = new QHBoxLayout();
  inputLayout->setContentsMargins(6, 2, 6, 6);
  inputLayout->setSpacing(4);
  inputLayout->addWidget(d->m_PromptLabel);
  inputLayout->addWidget(d->m_InputLine);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);
  mainLayout->addWidget(d->m_OutputArea, 1);
  mainLayout->addLayout(inputLayout);

  auto applyTheme = [=](NXThemeType::ThemeMode mode) {
    d->m_ThemeMode      = mode;
    QColor bgColor     = (mode == NXThemeType::Dark) ? QColor(0x1E, 0x1E, 0x2E) : QColor(0xF8, 0xF8, 0xF8);
    QColor textColor   = (mode == NXThemeType::Dark) ? QColor(0xCD, 0xD6, 0xF4) : QColor(0x2C, 0x2C, 0x2C);
    QColor inputBg     = (mode == NXThemeType::Dark) ? QColor(0x28, 0x28, 0x3A) : QColor(0xFF, 0xFF, 0xFF);
    QColor promptColor = NXThemeColor(mode, PrimaryNormal);

    QString outputStyle =
        QString(
            QStringLiteral(
                "QPlainTextEdit { background-color: %1; color: %2; selection-background-color: %3; border: none; }"))
            .arg(bgColor.name(), textColor.name(), NXThemeColor(mode, PrimaryNormal).name());
    d->m_OutputArea->setStyleSheet(outputStyle);

    QString inputStyle =
        QString(QStringLiteral("QLineEdit { background-color: %1; color: %2; border: none; padding: 4px; }"))
            .arg(inputBg.name(), textColor.name());
    d->m_InputLine->setStyleSheet(inputStyle);

    d->m_PromptLabel->setStyleSheet(QString(QStringLiteral("QLabel { color: %1; }")).arg(promptColor.name()));

    setStyleSheet(QString(QStringLiteral("QWidget#NXTerminalWidget { background-color: %1; }")).arg(bgColor.name()));
  };
  applyTheme(d->m_ThemeMode);
  connect(nxTheme, &NXTheme::themeModeChanged, this, applyTheme);

  d->updateFont();

  connect(this, &NXTerminalWidget::signalPromptChanged, this, [=]() {
    d->m_PromptLabel->setText(d->m_Prompt);
  });

  d->m_InputLine->installEventFilter(this);

  connect(d->m_InputLine, &QLineEdit::returnPressed, d, &NXTerminalWidgetPrivate::submitCommand);
}

NXTerminalWidget::~NXTerminalWidget()
{
}

void NXTerminalWidget::appendOutput(QString const &text, QColor const &color)
{
  Q_D(NXTerminalWidget);
  if (color.isValid()) {
    d->m_OutputArea->appendHtml(
        QString(QStringLiteral("<span style=\"color:%1;\">%2</span>"))
            .arg(color.name(), text.toHtmlEscaped().replace(QStringLiteral("\n"), QStringLiteral("<br>"))));
  } else {
    d->m_OutputArea->appendPlainText(text);
  }
  d->trimOutput();
  d->m_OutputArea->verticalScrollBar()->setValue(d->m_OutputArea->verticalScrollBar()->maximum());
}

void NXTerminalWidget::appendHtml(QString const &html)
{
  Q_D(NXTerminalWidget);
  d->m_OutputArea->appendHtml(html);
  d->trimOutput();
  d->m_OutputArea->verticalScrollBar()->setValue(d->m_OutputArea->verticalScrollBar()->maximum());
}

void NXTerminalWidget::appendError(QString const &text)
{
  appendOutput(text, NXThemeColor(d_ptr->m_ThemeMode, StatusDanger));
}

void NXTerminalWidget::appendSuccess(QString const &text)
{
  QColor green = (d_ptr->m_ThemeMode == NXThemeType::Dark) ? QColor(0xA6, 0xE3, 0xA1) : QColor(0x0F, 0x7B, 0x0F);
  appendOutput(text, green);
}

void NXTerminalWidget::clear()
{
  Q_D(NXTerminalWidget);
  d->m_OutputArea->clear();
}

QStringList NXTerminalWidget::getCommandHistory() const
{
  return d_ptr->m_CommandHistory;
}

bool NXTerminalWidget::eventFilter(QObject *watched, QEvent *event)
{
  Q_D(NXTerminalWidget);
  if (watched == d->m_InputLine && event->type() == QEvent::KeyPress) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
    if (keyEvent->key() == Qt::Key_Up) {
      d->navigateHistory(-1);
      return true;
    } else if (keyEvent->key() == Qt::Key_Down) {
      d->navigateHistory(1);
      return true;
    } else if (keyEvent->key() == Qt::Key_Tab) {
      Q_EMIT tabPressed(d->m_InputLine->text());
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
  QString command = m_InputLine->text().trimmed();
  if (command.isEmpty()) {
    return;
  }

  QColor promptColor = NXThemeColor(m_ThemeMode, PrimaryNormal);
  m_OutputArea->appendHtml(QString(QStringLiteral("<span style=\"color:%1;\">%2</span>%3"))
                              .arg(promptColor.name(), m_Prompt.toHtmlEscaped(), command.toHtmlEscaped()));

  if (m_CommandHistory.isEmpty() || m_CommandHistory.last() != command) {
    m_CommandHistory.append(command);
    if (m_CommandHistory.size() > m_MaxHistorySize) {
      m_CommandHistory.removeFirst();
    }
  }
  m_HistoryIndex = -1;
  m_SavedInput.clear();

  m_InputLine->clear();
  trimOutput();
  m_OutputArea->verticalScrollBar()->setValue(m_OutputArea->verticalScrollBar()->maximum());

  Q_EMIT q->commandSubmitted(command);
}

void NXTerminalWidgetPrivate::navigateHistory(int direction)
{
  if (m_CommandHistory.isEmpty()) {
    return;
  }

  if (m_HistoryIndex == -1) {
    m_SavedInput = m_InputLine->text();
  }

  if (direction < 0) {
    if (m_HistoryIndex == -1) {
      m_HistoryIndex = m_CommandHistory.size() - 1;
    } else if (m_HistoryIndex > 0) {
      m_HistoryIndex--;
    }
    m_InputLine->setText(m_CommandHistory.at(m_HistoryIndex));
  } else {
    if (m_HistoryIndex == -1) {
      return;
    }
    if (m_HistoryIndex < m_CommandHistory.size() - 1) {
      m_HistoryIndex++;
      m_InputLine->setText(m_CommandHistory.at(m_HistoryIndex));
    } else {
      m_HistoryIndex = -1;
      m_InputLine->setText(m_SavedInput);
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
  monoFont.setPixelSize(m_FontPixelSize);
  m_OutputArea->setFont(monoFont);
  m_InputLine->setFont(monoFont);
  m_PromptLabel->setFont(monoFont);
}

void NXTerminalWidgetPrivate::trimOutput()
{
  QTextDocument *doc = m_OutputArea->document();
  while (doc->blockCount() > m_MaxLineCount) {
    QTextCursor cursor(doc->begin());
    cursor.select(QTextCursor::BlockUnderCursor);
    cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
    cursor.removeSelectedText();
  }
}
