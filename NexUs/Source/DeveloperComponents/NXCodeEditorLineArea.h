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

  QRectF getBlockBoundingGeometry(QTextBlock const &block) const { return blockBoundingGeometry(block); }

  QRectF getBlockBoundingRect(QTextBlock const &block) const { return blockBoundingRect(block); }

  QPointF getContentOffset() const { return contentOffset(); }
};

class NXCodeEditorLineArea : public QWidget
{
public:
  explicit NXCodeEditorLineArea(NXCodePlainTextEdit *editor, NXCodeEditorPrivate *d, QWidget *parent = nullptr);

  QSize sizeHint() const override;

  int lineAreaWidth() const;

protected:
  void paintEvent(QPaintEvent *event) override;

private:
  NXCodePlainTextEdit *m_Editor;
  NXCodeEditorPrivate *d_ptr;
  friend class NXCodeEditorPrivate;
};

#endif // NXCODEEDITORLINEAREA_H
