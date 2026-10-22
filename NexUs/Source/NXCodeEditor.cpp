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
  _themeMode = themeMode;

  QPalette pal = _editor->palette();
  pal.setColor(QPalette::Base, NXThemeColor(_themeMode, BasicBase));
  pal.setColor(QPalette::Text, NXThemeColor(_themeMode, BasicText));
  _editor->setPalette(pal);

  _lineArea->update();

  QList<QTextEdit::ExtraSelection> extraSelections;
  if (!_editor->isReadOnly()) {
    QTextEdit::ExtraSelection selection;
    selection.format.setBackground(NXThemeColor(_themeMode, BasicHover));
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = _editor->textCursor();
    selection.cursor.clearSelection();
    extraSelections.append(selection);
  }
  _editor->setExtraSelections(extraSelections);

  NXCodeHighlighter *highlighter = static_cast<NXCodeHighlighter *>(_highlighter);
  if (highlighter) {
    highlighter->setThemeMode(_themeMode);
  }

  // Force stylesheet update for editor background
  _editor->setStyleSheet(QString(QStringLiteral("QPlainTextEdit { background-color: %1; color: %2; }"))
                             .arg(NXThemeColor(_themeMode, BasicBase).name(QColor::HexArgb))
                             .arg(NXThemeColor(_themeMode, BasicText).name(QColor::HexArgb)));
  _editor->update();
  _lineArea->update();
}

// --- NXCodeEditor ---
Q_PROPERTY_CREATE_CPP(NXCodeEditor, int, TabSize)
Q_PROPERTY_CREATE_CPP(NXCodeEditor, bool, IsReadOnly)

void NXCodeEditor::setCode(const QString &Code)
{
  Q_D(NXCodeEditor);
  if (d->_pCode != Code) {
    d->_pCode = Code;
    d->_editor->setPlainText(Code);
    Q_EMIT pCodeChanged();
  }
}

QString NXCodeEditor::getCode() const
{
  return d_ptr->_pCode;
}

void NXCodeEditor::setLanguage(Language lang)
{
  Q_D(NXCodeEditor);
  d->_language                   = static_cast<int>(lang);
  NXCodeHighlighter *highlighter = static_cast<NXCodeHighlighter *>(d->_highlighter);
  if (highlighter) {
    highlighter->setLanguage(d->_language);
  }
}

NXCodeEditor::Language NXCodeEditor::getLanguage() const
{
  return static_cast<Language>(d_ptr->_language);
}

NXCodeEditor::NXCodeEditor(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXCodeEditorPrivate())
{
  Q_D(NXCodeEditor);
  d->q_ptr        = this;
  d->_pIsReadOnly = false;
  d->_pTabSize    = 4;
  setObjectName("NXCodeEditor");

  d->_editor = new NXCodePlainTextEdit(this);
  d->_editor->setFrameShape(QFrame::NoFrame);

  QFont monoFont;
#if defined(Q_OS_MAC)
  monoFont = QFont(QStringLiteral("Courier New"), 13);
#elif defined(Q_OS_WIN)
  monoFont = QFont(QStringLiteral("Consolas"), 13);
#else
  monoFont = QFont(QStringLiteral("Monospace"), 13);
#endif
  d->_editor->setFont(monoFont);

  d->_editor->setTabStopDistance(d->_pTabSize * d->_editor->fontMetrics().horizontalAdvance(' '));

  NXCodeEditorLineArea *lineArea = new NXCodeEditorLineArea(d->_editor, d, this);
  d->_lineArea                   = lineArea;

  QHBoxLayout *mainLayout = new QHBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);
  mainLayout->addWidget(d->_lineArea);
  mainLayout->addWidget(d->_editor);

  d->_highlighter = new NXCodeHighlighter(d->_editor->document(), nxTheme->getThemeMode());

  d->_themeMode = nxTheme->getThemeMode();
  d->_editor->setStyleSheet(QString(QStringLiteral("QPlainTextEdit { background-color: %1; color: %2; }"))
                                .arg(NXThemeColor(d->_themeMode, BasicBase).name(QColor::HexArgb))
                                .arg(NXThemeColor(d->_themeMode, BasicText).name(QColor::HexArgb)));
  d->onThemeChanged(d->_themeMode);
  connect(nxTheme, &NXTheme::themeModeChanged, d, &NXCodeEditorPrivate::onThemeChanged);

  auto updateLineAreaWidth = [d, lineArea]() {
    d->_lineArea->setFixedWidth(lineArea->lineAreaWidth());
  };
  updateLineAreaWidth();

  connect(d->_editor, &QPlainTextEdit::blockCountChanged, this, [updateLineAreaWidth]() {
    updateLineAreaWidth();
  });

  connect(d->_editor, &QPlainTextEdit::updateRequest, this, [d](const QRect &rect, int dy) {
    if (dy)
      d->_lineArea->scroll(0, dy);
    else
      d->_lineArea->update(0, rect.y(), d->_lineArea->width(), rect.height());
  });

  connect(d->_editor, &QPlainTextEdit::cursorPositionChanged, this, [d]() {
    QList<QTextEdit::ExtraSelection> extraSelections;
    if (!d->_editor->isReadOnly()) {
      QTextEdit::ExtraSelection selection;
      selection.format.setBackground(NXThemeColor(d->_themeMode, BasicHover));
      selection.format.setProperty(QTextFormat::FullWidthSelection, true);
      selection.cursor = d->_editor->textCursor();
      selection.cursor.clearSelection();
      extraSelections.append(selection);
    }
    d->_editor->setExtraSelections(extraSelections);
    d->_lineArea->update();
  });

  connect(d->_editor, &QPlainTextEdit::textChanged, this, [d]() {
    d->_pCode = d->_editor->toPlainText();
  });

  connect(this, &NXCodeEditor::pIsReadOnlyChanged, this, [d]() {
    d->_editor->setReadOnly(d->_pIsReadOnly);
  });

  connect(this, &NXCodeEditor::pTabSizeChanged, this, [d]() {
    d->_editor->setTabStopDistance(d->_pTabSize * d->_editor->fontMetrics().horizontalAdvance(' '));
  });
}

NXCodeEditor::~NXCodeEditor()
{
}
