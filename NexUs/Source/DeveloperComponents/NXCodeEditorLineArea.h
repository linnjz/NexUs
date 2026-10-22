#ifndef NXCODEEDITORLINEAREA_H
#define NXCODEEDITORLINEAREA_H

#include <QPainter>
#include <QPlainTextEdit>
#include <QTextBlock>
#include <QWidget>

#include "NXTheme.h"
#include "private/NXCodeEditorPrivate.h"

// Expose protected methods of QPlainTextEdit
class NXCodePlainTextEdit : public QPlainTextEdit
{
public:
  using QPlainTextEdit::QPlainTextEdit;

  QTextBlock getFirstVisibleBlock() const { return firstVisibleBlock(); }

  QRectF getBlockBoundingGeometry(const QTextBlock &block) const { return blockBoundingGeometry(block); }

  QRectF getBlockBoundingRect(const QTextBlock &block) const { return blockBoundingRect(block); }

  QPointF getContentOffset() const { return contentOffset(); }
};

class NXCodeEditorLineArea : public QWidget
{
public:
  explicit NXCodeEditorLineArea(NXCodePlainTextEdit *editor, NXCodeEditorPrivate *d, QWidget *parent = nullptr)
      : QWidget(parent)
      , _editor(editor)
      , _d(d)
  {
  }

  QSize sizeHint() const override { return QSize(lineAreaWidth(), 0); }

  int lineAreaWidth() const
  {
    int digits   = 1;
    int maxBlock = qMax(1, _editor->blockCount());
    while (maxBlock >= 10) {
      maxBlock /= 10;
      ++digits;
    }
    int space = _editor->fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits + 20;
    return space;
  }

protected:
  void paintEvent(QPaintEvent *event) override
  {
    QPainter painter(this);
    NXThemeType::ThemeMode themeMode = _d->_themeMode;
    painter.fillRect(event->rect(), NXThemeColor(themeMode, BasicBaseDeep));

    QTextBlock block = _editor->getFirstVisibleBlock();
    int blockNumber  = block.blockNumber();
    int top          = qRound(_editor->getBlockBoundingGeometry(block).translated(_editor->getContentOffset()).top());
    int bottom       = top + qRound(_editor->getBlockBoundingRect(block).height());

    int currentBlockNumber = _editor->textCursor().blockNumber();

    while (block.isValid() && top <= event->rect().bottom()) {
      if (block.isVisible() && bottom >= event->rect().top()) {
        QString number = QString::number(blockNumber + 1);
        if (blockNumber == currentBlockNumber) {
          painter.setPen(NXThemeColor(themeMode, PrimaryNormal));
        } else {
          painter.setPen(NXThemeColor(themeMode, BasicDetailsText));
        }
        painter.setFont(_editor->font());
        painter.drawText(0, top, width() - 8, _editor->fontMetrics().height(), Qt::AlignRight | Qt::AlignVCenter, number);
      }
      block  = block.next();
      top    = bottom;
      bottom = top + qRound(_editor->getBlockBoundingRect(block).height());
      ++blockNumber;
    }
  }

private:
  NXCodePlainTextEdit *_editor;
  NXCodeEditorPrivate *_d;
  friend class NXCodeEditorPrivate;
};

#endif // NXCODEEDITORLINEAREA_H
