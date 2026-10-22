#include "NXSplitInput.h"

#include <QApplication>
#include <QClipboard>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QPainterPath>
#include <QVariantAnimation>

#include "NXTheme.h"
#include "private/NXSplitInputPrivate.h"

NXSplitInput::NXSplitInput(QWidget *parent)
    : NXSplitInput(1, QStringLiteral("."), parent)
{
}

NXSplitInput::NXSplitInput(int segmentCount, const QString &separator, QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXSplitInputPrivate())
{
  Q_D(NXSplitInput);
  d->q_ptr                   = this;
  d->_segmentCount           = qMax(1, segmentCount);
  d->_pSegmentSpacing        = 4;
  d->_pBorderRadius          = 6;
  d->_pIsClearButtonEnable   = true;
  d->_pIsKeepMarkOnCompleted = true;
  d->_separatorText          = separator;
  setObjectName(QStringLiteral("NXSplitInput"));
  setStyleSheet(QStringLiteral("#NXSplitInput{background-color:transparent;}"));
  setFocusPolicy(Qt::StrongFocus);
  setMouseTracking(true);
  setFixedHeight(33);
  d->_boxLayout = new QHBoxLayout(this);
  d->_boxLayout->setContentsMargins(0, 0, 0, 0);
  d->_boxLayout->setSpacing(d->_pSegmentSpacing);

  d->_themeMode = nxTheme->getThemeMode();
  d->_buildWidgets();

  connect(nxTheme, &NXTheme::themeModeChanged, d, &NXSplitInputPrivate::onThemeChanged);
}

NXSplitInput::~NXSplitInput()
{
}

int NXSplitInput::getSegmentCount() const
{
  Q_D(const NXSplitInput);
  return d->_segmentCount;
}

QString NXSplitInput::getSegmentText(int index) const
{
  Q_D(const NXSplitInput);
  if (index < 0 || index >= d->_segmentCount) {
    return {};
  }
  return d->_lineEdits[index]->text();
}

void NXSplitInput::setSegmentText(int index, const QString &text)
{
  Q_D(NXSplitInput);
  if (index < 0 || index >= d->_segmentCount) {
    return;
  }
  d->_lineEdits[index]->setText(text.left(d->_maxLengths[index]));
}

void NXSplitInput::setSegmentMaxLength(int index, int maxLength)
{
  Q_D(NXSplitInput);
  if (index < 0 || index >= d->_segmentCount) {
    return;
  }
  d->_maxLengths[index] = qMax(1, maxLength);
  d->_lineEdits[index]->setMaxLength(d->_maxLengths[index]);
}

int NXSplitInput::getSegmentMaxLength(int index) const
{
  Q_D(const NXSplitInput);
  if (index < 0 || index >= d->_segmentCount) {
    return 0;
  }
  return d->_maxLengths[index];
}

void NXSplitInput::setMaxLength(int maxLength)
{
  Q_D(NXSplitInput);
  d->_defaultMaxLength = qMax(1, maxLength);
  for (int i = 0; i < d->_segmentCount; ++i) {
    d->_maxLengths[i] = d->_defaultMaxLength;
    d->_lineEdits[i]->setMaxLength(d->_defaultMaxLength);
  }
}

void NXSplitInput::setSegmentPlaceholderText(int index, const QString &text)
{
  Q_D(NXSplitInput);
  if (index < 0 || index >= d->_segmentCount) {
    return;
  }
  d->_lineEdits[index]->setPlaceholderText(text);
}

QString NXSplitInput::getSegmentPlaceholderText(int index) const
{
  Q_D(const NXSplitInput);
  if (index < 0 || index >= d->_segmentCount) {
    return {};
  }
  return d->_lineEdits[index]->placeholderText();
}

void NXSplitInput::setSegmentValidator(int index, QValidator *validator)
{
  Q_D(NXSplitInput);
  if (index < 0 || index >= d->_segmentCount) {
    return;
  }
  d->_lineEdits[index]->setValidator(validator);
}

const QValidator * NXSplitInput::getSegmentValidator(int index) const
{
  Q_D(const NXSplitInput);
  if (index < 0 || index >= d->_segmentCount) {
    return nullptr;
  }
  return d->_lineEdits[index]->validator();
}

void NXSplitInput::setText(const QString &Text)
{
  Q_D(NXSplitInput);
  QStringList parts = Text.split(d->_separatorText);
  if (parts.size() != d->_segmentCount) {
    return;
  }
  for (int i = 0; i < d->_segmentCount; ++i) {
    d->_lineEdits[i]->blockSignals(true);
    d->_lineEdits[i]->setText(parts[i].left(d->_maxLengths[i]));
    d->_lineEdits[i]->blockSignals(false);
  }
  d->_updateCompletedState();
  QString fullText;
  for (int i = 0; i < d->_segmentCount; ++i) {
    if (i > 0) {
      fullText += d->_separatorText;
    }
    fullText += d->_lineEdits[i]->text();
  }
  Q_EMIT pTextChanged();
  Q_EMIT textChanged(fullText);
  update();
}

QString NXSplitInput::getText() const
{
  Q_D(const NXSplitInput);
  QString result;
  for (int i = 0; i < d->_segmentCount; ++i) {
    if (i > 0) {
      result += d->_separatorText;
    }
    result += d->_lineEdits[i]->text();
  }
  return result;
}

void NXSplitInput::setSeparator(const QString &Separator)
{
  Q_D(NXSplitInput);
  if (d->_separatorText == Separator) {
    return;
  }
  d->_separatorText = Separator;
  for (QLabel *label : d->_separators) {
    label->setText(Separator);
    label->setFixedWidth(label->fontMetrics().horizontalAdvance(Separator) + 4);
  }
  Q_EMIT pSeparatorChanged();
}

QString NXSplitInput::getSeparator() const
{
  Q_D(const NXSplitInput);
  return d->_separatorText;
}

void NXSplitInput::setSegmentTexts(const QStringList &SegmentTexts)
{
  Q_D(NXSplitInput);
  if (SegmentTexts.size() != d->_segmentCount) {
    return;
  }
  for (int i = 0; i < d->_segmentCount; ++i) {
    d->_lineEdits[i]->blockSignals(true);
    d->_lineEdits[i]->setText(SegmentTexts[i].left(d->_maxLengths[i]));
    d->_lineEdits[i]->blockSignals(false);
  }
  d->_updateCompletedState();
  QString fullText;
  for (int i = 0; i < d->_segmentCount; ++i) {
    if (i > 0) {
      fullText += d->_separatorText;
    }
    fullText += d->_lineEdits[i]->text();
  }
  Q_EMIT pSegmentTextsChanged();
  Q_EMIT textChanged(fullText);
  update();
}

QStringList NXSplitInput::getSegmentTexts() const
{
  Q_D(const NXSplitInput);
  QStringList result;
  for (QLineEdit *box : d->_lineEdits) {
    result.append(box->text());
  }
  return result;
}

void NXSplitInput::setSegmentSpacing(qreal SegmentSpacing)
{
  Q_D(NXSplitInput);
  d->_pSegmentSpacing = SegmentSpacing;
  d->_boxLayout->setSpacing(static_cast<int>(SegmentSpacing));
  update();
}

qreal NXSplitInput::getSegmentSpacing() const
{
  Q_D(const NXSplitInput);
  return d->_pSegmentSpacing;
}

void NXSplitInput::setBorderRadius(int BorderRadius)
{
  Q_D(NXSplitInput);
  d->_pBorderRadius = BorderRadius;
  update();
}

int NXSplitInput::getBorderRadius() const
{
  Q_D(const NXSplitInput);
  return d->_pBorderRadius;
}

void NXSplitInput::setIsClearButtonEnable(bool IsClearButtonEnable)
{
  Q_D(NXSplitInput);
  d->_pIsClearButtonEnable = IsClearButtonEnable;
  for (QLineEdit *box : d->_lineEdits) {
    box->setClearButtonEnabled(IsClearButtonEnable);
  }
  Q_EMIT pIsClearButtonEnableChanged();
}

bool NXSplitInput::getIsClearButtonEnable() const
{
  Q_D(const NXSplitInput);
  return d->_pIsClearButtonEnable;
}

void NXSplitInput::setIsKeepMarkOnCompleted(bool IsKeepMarkOnCompleted)
{
  Q_D(NXSplitInput);
  d->_pIsKeepMarkOnCompleted = IsKeepMarkOnCompleted;
  Q_EMIT pIsKeepMarkOnCompletedChanged();
}

bool NXSplitInput::getIsKeepMarkOnCompleted() const
{
  Q_D(const NXSplitInput);
  return d->_pIsKeepMarkOnCompleted;
}

void NXSplitInput::paintEvent(QPaintEvent *event)
{
  Q_D(NXSplitInput);
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing);

  QRect r = rect();

  // Step 1: 整widget基底（覆盖所有区域，包括分隔符/间距）
  painter.setPen(Qt::NoPen);
  painter.setBrush(NXThemeColor(d->_themeMode, BasicBaseAlpha));
  painter.drawRect(r);

  // Step 2: 各 segment 圆角背景覆盖层（三态 per segment）
  for (int i = 0; i < d->_segmentCount; ++i) {
    QRect segRect = d->_lineEdits[i]->geometry();
    bool focused  = d->_lineEdits[i]->hasFocus();
    bool hovered  = (i == d->_hoveredIndex);

    if (focused) {
      painter.setBrush(NXThemeColor(d->_themeMode, DialogBase));
      painter.drawRoundedRect(segRect, d->_pBorderRadius, d->_pBorderRadius);
    } else if (hovered) {
      painter.setBrush(NXThemeColor(d->_themeMode, BasicHoverAlpha));
      painter.drawRoundedRect(segRect, d->_pBorderRadius, d->_pBorderRadius);
    }
  }

  // Step 3: 外框（后绘制，叠加在背景之上）
  painter.setPen(NXThemeColor(d->_themeMode, BasicBorder));
  painter.setBrush(Qt::NoBrush);
  painter.drawRoundedRect(r, d->_pBorderRadius, d->_pBorderRadius);

  // 底边 hemline（参考 NXLineEditStyle）
  painter.setBrush(NXThemeColor(d->_themeMode, BasicHemline));
  painter.setPen(Qt::NoPen);
  {
    QPainterPath path;
    qreal y = r.bottom();
    path.moveTo(6, y);
    path.lineTo(r.width() - 6, y);
    path.arcTo(QRectF(r.width() - 12, y - 12, 12, 12), -90, 45);
    path.lineTo(6 - 3 * std::sqrt(2), y - (6 - 3 * std::sqrt(2)));
    path.arcTo(QRectF(0, y - 12, 12, 12), 225, 45);
    path.closeSubpath();
    painter.drawPath(path);
  }

  // 蓝色底部标识
  painter.setBrush(NXThemeColor(d->_themeMode, PrimaryNormal));
  qreal markY = r.bottom() - 2.5;
  qreal markH = 2.5;

  if (d->_allCompleted) {
    QRect firstRect = d->_lineEdits.first()->geometry();
    QRect lastRect  = d->_lineEdits.last()->geometry();
    qreal fullLeft  = firstRect.left();
    qreal fullRight = lastRect.right();
    painter.drawRoundedRect(QRectF(fullLeft, markY, fullRight - fullLeft, markH), 2, 2);
  } else {
    for (int i = 0; i < d->_segmentCount; ++i) {
      qreal markW = d->_segmentMarks[i];
      if (markW <= 0) {
        continue;
      }
      QRect boxRect = d->_lineEdits[i]->geometry();
      qreal cx      = boxRect.center().x();
      painter.drawRoundedRect(QRectF(cx - markW, markY, markW * 2, markH), 2, 2);
    }
  }
}

bool NXSplitInput::eventFilter(QObject *watched, QEvent *event)
{
  Q_D(NXSplitInput);
  QLineEdit *box = qobject_cast<QLineEdit *>(watched);
  if (!box) {
    return QWidget::eventFilter(watched, event);
  }
  int index = d->_lineEdits.indexOf(box);
  if (index < 0) {
    return QWidget::eventFilter(watched, event);
  }

  if (event->type() == QEvent::FocusIn) {
    Q_EMIT segmentFocusIn(index, box->text());
    d->_animateMarkIn(index);
    update();
    return false;
  }
  if (event->type() == QEvent::FocusOut) {
    Q_EMIT segmentFocusOut(index, box->text());
    if (!d->_pIsKeepMarkOnCompleted || box->text().isEmpty()) {
      d->_animateMarkOut(index);
    }
    update();
    return false;
  }
  if (event->type() == QEvent::Enter) {
    d->_hoveredIndex = index;
    update();
    return false;
  }
  if (event->type() == QEvent::Leave) {
    if (d->_hoveredIndex == index) {
      d->_hoveredIndex = -1;
      update();
    }
    return false;
  }
  if (event->type() == QEvent::KeyPress) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

    if (keyEvent->key() == Qt::Key_Tab || keyEvent->key() == Qt::Key_Backtab) {
      if (keyEvent->key() == Qt::Key_Tab && index < d->_segmentCount - 1) {
        d->_lineEdits[index + 1]->setFocus();
        return true;
      }
      if (keyEvent->key() == Qt::Key_Backtab && index > 0) {
        d->_lineEdits[index - 1]->setFocus();
        return true;
      }
      return false;
    }

    if (keyEvent->key() == Qt::Key_Left) {
      if (box->cursorPosition() == 0 && index > 0) {
        d->_lineEdits[index - 1]->setFocus();
        d->_lineEdits[index - 1]->setCursorPosition(
            d->_lineEdits[index - 1]->text().length());
        return true;
      }
      return false;
    }

    if (keyEvent->key() == Qt::Key_Right) {
      if (box->cursorPosition() == box->text().length() && index < d->_segmentCount - 1) {
        d->_lineEdits[index + 1]->setFocus();
        d->_lineEdits[index + 1]->setCursorPosition(0);
        return true;
      }
      return false;
    }

    if (keyEvent->key() == Qt::Key_Backspace) {
      if (box->text().isEmpty() && index > 0) {
        d->_lineEdits[index - 1]->setFocus();
        d->_lineEdits[index - 1]->setCursorPosition(
            d->_lineEdits[index - 1]->text().length());
        return true;
      }
      return false;
    }

    if (keyEvent->matches(QKeySequence::Paste)) {
      QString clipText = QApplication::clipboard()->text();
      if (!clipText.isEmpty()) {
        QStringList parts = clipText.split(d->_separatorText);
        if (parts.size() > 1) {
          for (int i = 0;
               i < parts.size() && (index + i) < d->_segmentCount; ++i) {
            d->_lineEdits[index + i]->blockSignals(true);
            d->_lineEdits[index + i]->setText(
                parts[i].left(d->_maxLengths[index + i]));
            d->_lineEdits[index + i]->blockSignals(false);
          }
          QString fullText;
          for (int i = 0; i < d->_segmentCount; ++i) {
            if (i > 0) fullText += d->_separatorText;
            fullText += d->_lineEdits[i]->text();
          }
          d->_updateCompletedState();
          Q_EMIT textChanged(fullText);
          update();

          int nextIndex = qMin(index + parts.size(), d->_segmentCount - 1);
          d->_lineEdits[nextIndex]->setFocus();
        } else {
          int cursorPos    = box->cursorPosition();
          QString existing = box->text();
          int remaining    = d->_maxLengths[index] - existing.length();
          if (remaining > 0) {
            QString merged = existing.left(cursorPos) + clipText.left(remaining) + existing.mid(cursorPos);
            box->setText(merged.left(d->_maxLengths[index]));
            box->setCursorPosition(cursorPos + qMin(clipText.length(), remaining));
          }
        }
      }
      return true;
    }
  }

  return QWidget::eventFilter(watched, event);
}
