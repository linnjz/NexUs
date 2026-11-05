#include "NXCodeEditor.h"

#include <QHBoxLayout>
#include <QScrollBar>
#include <QTextBlock>

#include "DeveloperComponents/NXCodeEditorLineArea.h"
#include "DeveloperComponents/NXCodeHighlighter.h"
#include "NXTheme.h"
#include "private/NXCodeEditorPrivate.h"

// --- NXCodeEditorPrivate ---
NXCodeEditorPrivate::NXCodeEditorPrivate(QObject *parent)
    : QObject { parent }
{
}

NXCodeEditorPrivate::~NXCodeEditorPrivate()
{
}

void NXCodeEditorPrivate::onThemeChanged(NXThemeType::ThemeMode themeMode)
{
  Q_Q(NXCodeEditor);
  m_ThemeMode = themeMode;

  QPalette pal = m_Editor->palette();
  pal.setColor(QPalette::Base, NXThemeColor(m_ThemeMode, BasicBase));
  pal.setColor(QPalette::Text, NXThemeColor(m_ThemeMode, BasicText));
  m_Editor->setPalette(pal);

  m_LineArea->update();

  QList<QTextEdit::ExtraSelection> extraSelections;
  if (!m_Editor->isReadOnly()) {
    QTextEdit::ExtraSelection selection;
    selection.format.setBackground(NXThemeColor(m_ThemeMode, BasicHover));
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = m_Editor->textCursor();
    selection.cursor.clearSelection();
    extraSelections.append(selection);
  }
  m_Editor->setExtraSelections(extraSelections);

  NXCodeHighlighter *highlighter = static_cast<NXCodeHighlighter *>(m_Highlighter);
  if (highlighter) {
    highlighter->setThemeMode(m_ThemeMode);
  }

  // Force stylesheet update for editor background
  m_Editor->setStyleSheet(QString(QStringLiteral("QPlainTextEdit { background-color: %1; color: %2; }"))
                             .arg(NXThemeColor(m_ThemeMode, BasicBase).name(QColor::HexArgb))
                             .arg(NXThemeColor(m_ThemeMode, BasicText).name(QColor::HexArgb)));
  m_Editor->update();
  m_LineArea->update();
}

// --- NXCodeEditor ---
Q_PROPERTY_CREATE_CPP(NXCodeEditor, QPS_PAEMIT(int), TabSize)
Q_PROPERTY_CREATE_CPP(NXCodeEditor, QPS_PAEMIT(bool), IsReadOnly)

void NXCodeEditor::setCode(QString const &Code)
{
  Q_D(NXCodeEditor);
  if (d->m_Code != Code) {
    d->m_Code = Code;
    d->m_Editor->setPlainText(Code);
    Q_EMIT signalCodeChanged();
  }
}

QString NXCodeEditor::getCode() const noexcept(std::is_nothrow_copy_constructible_v<QString>)
{
  return d_ptr->m_Code;
}

void NXCodeEditor::setLanguage(Language lang)
{
  Q_D(NXCodeEditor);
  d->m_Language                   = static_cast<int>(lang);
  NXCodeHighlighter *highlighter = static_cast<NXCodeHighlighter *>(d->m_Highlighter);
  if (highlighter) {
    highlighter->setLanguage(d->m_Language);
  }
}

NXCodeEditor::Language NXCodeEditor::getLanguage() const
{
  return static_cast<Language>(d_ptr->m_Language);
}

NXCodeEditor::NXCodeEditor(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXCodeEditorPrivate())
{
  Q_D(NXCodeEditor);
  d->q_ptr        = this;
  d->m_IsReadOnly = false;
  d->m_TabSize    = 4;
  setObjectName("NXCodeEditor");

  d->m_Editor = new NXCodePlainTextEdit(this);
  d->m_Editor->setFrameShape(QFrame::NoFrame);

  QFont monoFont;
#if defined(Q_OS_MAC)
  monoFont = QFont(QStringLiteral("Courier New"), 13);
#elif defined(Q_OS_WIN)
  monoFont = QFont(QStringLiteral("Consolas"), 13);
#else
  monoFont = QFont(QStringLiteral("Monospace"), 13);
#endif
  d->m_Editor->setFont(monoFont);

  d->m_Editor->setTabStopDistance(d->m_TabSize * d->m_Editor->fontMetrics().horizontalAdvance(' '));

  NXCodeEditorLineArea *lineArea = new NXCodeEditorLineArea(d->m_Editor, d, this);
  d->m_LineArea                   = lineArea;

  QHBoxLayout *mainLayout = new QHBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);
  mainLayout->addWidget(d->m_LineArea);
  mainLayout->addWidget(d->m_Editor);

  d->m_Highlighter = new NXCodeHighlighter(d->m_Editor->document(), nxTheme->getThemeMode());

  d->m_ThemeMode = nxTheme->getThemeMode();
  d->m_Editor->setStyleSheet(QString(QStringLiteral("QPlainTextEdit { background-color: %1; color: %2; }"))
                                .arg(NXThemeColor(d->m_ThemeMode, BasicBase).name(QColor::HexArgb))
                                .arg(NXThemeColor(d->m_ThemeMode, BasicText).name(QColor::HexArgb)));
  d->onThemeChanged(d->m_ThemeMode);
  connect(nxTheme, &NXTheme::themeModeChanged, d, &NXCodeEditorPrivate::onThemeChanged);

  auto updateLineAreaWidth = [d, lineArea]() {
    d->m_LineArea->setFixedWidth(lineArea->lineAreaWidth());
  };
  updateLineAreaWidth();

  connect(d->m_Editor, &QPlainTextEdit::blockCountChanged, this, [updateLineAreaWidth]() {
    updateLineAreaWidth();
  });

  connect(d->m_Editor, &QPlainTextEdit::updateRequest, this, [d](QRect const &rect, int dy) {
    if (dy)
      d->m_LineArea->scroll(0, dy);
    else
      d->m_LineArea->update(0, rect.y(), d->m_LineArea->width(), rect.height());
  });

  connect(d->m_Editor, &QPlainTextEdit::cursorPositionChanged, this, [d]() {
    QList<QTextEdit::ExtraSelection> extraSelections;
    if (!d->m_Editor->isReadOnly()) {
      QTextEdit::ExtraSelection selection;
      selection.format.setBackground(NXThemeColor(d->m_ThemeMode, BasicHover));
      selection.format.setProperty(QTextFormat::FullWidthSelection, true);
      selection.cursor = d->m_Editor->textCursor();
      selection.cursor.clearSelection();
      extraSelections.append(selection);
    }
    d->m_Editor->setExtraSelections(extraSelections);
    d->m_LineArea->update();
  });

  connect(d->m_Editor, &QPlainTextEdit::textChanged, this, [d]() {
    d->m_Code = d->m_Editor->toPlainText();
  });

  connect(this, &NXCodeEditor::signalIsReadOnlyChanged, this, [d]() {
    d->m_Editor->setReadOnly(d->m_IsReadOnly);
  });

  connect(this, &NXCodeEditor::signalTabSizeChanged, this, [d]() {
    d->m_Editor->setTabStopDistance(d->m_TabSize * d->m_Editor->fontMetrics().horizontalAdvance(' '));
  });
}

NXCodeEditor::~NXCodeEditor()
{
}
