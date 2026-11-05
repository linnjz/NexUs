#include "NXCodeEditorLineArea.h"

NXCodeEditorLineArea::NXCodeEditorLineArea(NXCodePlainTextEdit *editor, NXCodeEditorPrivate *d, QWidget *parent)
    : QWidget(parent)
    , m_Editor { editor }
    , d_ptr { d }
{
}

QSize NXCodeEditorLineArea::sizeHint() const
{
  return QSize(lineAreaWidth(), 0);
}

int NXCodeEditorLineArea::lineAreaWidth() const
{
  int digits   = 1;
  int maxBlock = qMax(1, m_Editor->blockCount());
  while (maxBlock >= 10) {
    maxBlock /= 10;
    ++digits;
  }
  int space = m_Editor->fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits + 20;
  return space;
}

void NXCodeEditorLineArea::paintEvent(QPaintEvent *event)
{
  QPainter painter(this);
  NXThemeType::ThemeMode themeMode = d_ptr->m_ThemeMode;
  painter.fillRect(event->rect(), NXThemeColor(themeMode, BasicBaseDeep));

  QTextBlock block = m_Editor->getFirstVisibleBlock();
  int blockNumber  = block.blockNumber();
  int top          = qRound(m_Editor->getBlockBoundingGeometry(block).translated(m_Editor->getContentOffset()).top());
  int bottom       = top + qRound(m_Editor->getBlockBoundingRect(block).height());

  int currentBlockNumber = m_Editor->textCursor().blockNumber();

  while (block.isValid() && top <= event->rect().bottom()) {
    if (block.isVisible() && bottom >= event->rect().top()) {
      QString number = QString::number(blockNumber + 1);
      if (blockNumber == currentBlockNumber) {
        painter.setPen(NXThemeColor(themeMode, PrimaryNormal));
      } else {
        painter.setPen(NXThemeColor(themeMode, BasicDetailsText));
      }
      painter.setFont(m_Editor->font());
      painter.drawText(0, top, width() - 8, m_Editor->fontMetrics().height(), Qt::AlignRight | Qt::AlignVCenter, number);
    }
    block  = block.next();
    top    = bottom;
    bottom = top + qRound(m_Editor->getBlockBoundingRect(block).height());
    ++blockNumber;
  }
}
