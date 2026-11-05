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

NXSplitInput::NXSplitInput(int segmentCount, QString const &separator, QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXSplitInputPrivate())
{
  Q_D(NXSplitInput);
  d->q_ptr                   = this;
  d->m_SegmentCount           = qMax(1, segmentCount);
  d->m_SegmentSpacing        = 4;
  d->m_BorderRadius          = 6;
  d->m_IsClearButtonEnable   = true;
  d->m_IsKeepMarkOnCompleted = true;
  d->m_SeparatorText          = separator;
  setObjectName(QStringLiteral("NXSplitInput"));
  setStyleSheet(QStringLiteral("#NXSplitInput{background-color:transparent;}"));
  setFocusPolicy(Qt::StrongFocus);
  setMouseTracking(true);
  setFixedHeight(33);
  d->m_BoxLayout = new QHBoxLayout(this);
  d->m_BoxLayout->setContentsMargins(0, 0, 0, 0);
  d->m_BoxLayout->setSpacing(d->m_SegmentSpacing);

  d->m_ThemeMode = nxTheme->getThemeMode();
  d->_buildWidgets();

  connect(nxTheme, &NXTheme::themeModeChanged, d, &NXSplitInputPrivate::onThemeChanged);
}

NXSplitInput::~NXSplitInput()
{
}

int NXSplitInput::getSegmentCount() const
{
  Q_CONST_D(NXSplitInput);
  return d->m_SegmentCount;
}

QString NXSplitInput::getSegmentText(int index) const
{
  Q_CONST_D(NXSplitInput);
  if (index < 0 || index >= d->m_SegmentCount) {
    return {};
  }
  return d->m_LineEdits[index]->text();
}

void NXSplitInput::setSegmentText(int index, QString const &text)
{
  Q_D(NXSplitInput);
  if (index < 0 || index >= d->m_SegmentCount) {
    return;
  }
  d->m_LineEdits[index]->setText(text.left(d->m_MaxLengths[index]));
}

void NXSplitInput::setSegmentMaxLength(int index, int maxLength)
{
  Q_D(NXSplitInput);
  if (index < 0 || index >= d->m_SegmentCount) {
    return;
  }
  d->m_MaxLengths[index] = qMax(1, maxLength);
  d->m_LineEdits[index]->setMaxLength(d->m_MaxLengths[index]);
}

int NXSplitInput::getSegmentMaxLength(int index) const
{
  Q_CONST_D(NXSplitInput);
  if (index < 0 || index >= d->m_SegmentCount) {
    return 0;
  }
  return d->m_MaxLengths[index];
}

void NXSplitInput::setMaxLength(int maxLength)
{
  Q_D(NXSplitInput);
  d->m_DefaultMaxLength = qMax(1, maxLength);
  for (int i = 0; i < d->m_SegmentCount; ++i) {
    d->m_MaxLengths[i] = d->m_DefaultMaxLength;
    d->m_LineEdits[i]->setMaxLength(d->m_DefaultMaxLength);
  }
}

void NXSplitInput::setSegmentPlaceholderText(int index, QString const &text)
{
  Q_D(NXSplitInput);
  if (index < 0 || index >= d->m_SegmentCount) {
    return;
  }
  d->m_LineEdits[index]->setPlaceholderText(text);
}

QString NXSplitInput::getSegmentPlaceholderText(int index) const
{
  Q_CONST_D(NXSplitInput);
  if (index < 0 || index >= d->m_SegmentCount) {
    return {};
  }
  return d->m_LineEdits[index]->placeholderText();
}

void NXSplitInput::setSegmentValidator(int index, QValidator *validator)
{
  Q_D(NXSplitInput);
  if (index < 0 || index >= d->m_SegmentCount) {
    return;
  }
  d->m_LineEdits[index]->setValidator(validator);
}

QValidator const *NXSplitInput::getSegmentValidator(int index) const
{
  Q_CONST_D(NXSplitInput);
  if (index < 0 || index >= d->m_SegmentCount) {
    return nullptr;
  }
  return d->m_LineEdits[index]->validator();
}

void NXSplitInput::setText(QString const &Text)
{
  Q_D(NXSplitInput);
  QStringList parts = Text.split(d->m_SeparatorText);
  if (parts.size() != d->m_SegmentCount) {
    return;
  }
  for (int i = 0; i < d->m_SegmentCount; ++i) {
    d->m_LineEdits[i]->blockSignals(true);
    d->m_LineEdits[i]->setText(parts[i].left(d->m_MaxLengths[i]));
    d->m_LineEdits[i]->blockSignals(false);
  }
  d->_updateCompletedState();
  QString fullText;
  for (int i = 0; i < d->m_SegmentCount; ++i) {
    if (i > 0) {
      fullText += d->m_SeparatorText;
    }
    fullText += d->m_LineEdits[i]->text();
  }
  Q_EMIT signalTextChanged();
  Q_EMIT textChanged(fullText);
  update();
}

QString NXSplitInput::getText() const noexcept(std::is_nothrow_copy_constructible_v<QString>)
{
  Q_CONST_D(NXSplitInput);
  QString result;
  for (int i = 0; i < d->m_SegmentCount; ++i) {
    if (i > 0) {
      result += d->m_SeparatorText;
    }
    result += d->m_LineEdits[i]->text();
  }
  return result;
}

void NXSplitInput::setSeparator(QString const &Separator)
{
  Q_D(NXSplitInput);
  if (d->m_SeparatorText == Separator) {
    return;
  }
  d->m_SeparatorText = Separator;
  for (QLabel *label : d->m_Separators) {
    label->setText(Separator);
    label->setFixedWidth(label->fontMetrics().horizontalAdvance(Separator) + 4);
  }
  Q_EMIT signalSeparatorChanged();
}

QString NXSplitInput::getSeparator() const noexcept(std::is_nothrow_copy_constructible_v<QString>)
{
  Q_CONST_D(NXSplitInput);
  return d->m_SeparatorText;
}

void NXSplitInput::setSegmentTexts(QStringList const &SegmentTexts)
{
  Q_D(NXSplitInput);
  if (SegmentTexts.size() != d->m_SegmentCount) {
    return;
  }
  for (int i = 0; i < d->m_SegmentCount; ++i) {
    d->m_LineEdits[i]->blockSignals(true);
    d->m_LineEdits[i]->setText(SegmentTexts[i].left(d->m_MaxLengths[i]));
    d->m_LineEdits[i]->blockSignals(false);
  }
  d->_updateCompletedState();
  QString fullText;
  for (int i = 0; i < d->m_SegmentCount; ++i) {
    if (i > 0) {
      fullText += d->m_SeparatorText;
    }
    fullText += d->m_LineEdits[i]->text();
  }
  Q_EMIT signalSegmentTextsChanged();
  Q_EMIT textChanged(fullText);
  update();
}

QStringList NXSplitInput::getSegmentTexts() const noexcept(std::is_nothrow_copy_constructible_v<QStringList>)
{
  Q_CONST_D(NXSplitInput);
  QStringList result;
  for (QLineEdit *box : d->m_LineEdits) {
    result.append(box->text());
  }
  return result;
}

void NXSplitInput::setSegmentSpacing(qreal SegmentSpacing)
{
  Q_D(NXSplitInput);
  d->m_SegmentSpacing = SegmentSpacing;
  d->m_BoxLayout->setSpacing(static_cast<int>(SegmentSpacing));
  update();
}

qreal NXSplitInput::getSegmentSpacing() const noexcept(std::is_nothrow_copy_constructible_v<qreal>)
{
  Q_CONST_D(NXSplitInput);
  return d->m_SegmentSpacing;
}

void NXSplitInput::setBorderRadius(int BorderRadius)
{
  Q_D(NXSplitInput);
  d->m_BorderRadius = BorderRadius;
  update();
}

int NXSplitInput::getBorderRadius() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXSplitInput);
  return d->m_BorderRadius;
}

void NXSplitInput::setIsClearButtonEnable(bool IsClearButtonEnable)
{
  Q_D(NXSplitInput);
  d->m_IsClearButtonEnable = IsClearButtonEnable;
  for (QLineEdit *box : d->m_LineEdits) {
    box->setClearButtonEnabled(IsClearButtonEnable);
  }
  Q_EMIT signalIsClearButtonEnableChanged();
}

bool NXSplitInput::getIsClearButtonEnable() const noexcept(std::is_nothrow_copy_constructible_v<bool>)
{
  Q_CONST_D(NXSplitInput);
  return d->m_IsClearButtonEnable;
}

void NXSplitInput::setIsKeepMarkOnCompleted(bool IsKeepMarkOnCompleted)
{
  Q_D(NXSplitInput);
  d->m_IsKeepMarkOnCompleted = IsKeepMarkOnCompleted;
  Q_EMIT signalIsKeepMarkOnCompletedChanged();
}

bool NXSplitInput::getIsKeepMarkOnCompleted() const noexcept(std::is_nothrow_copy_constructible_v<bool>)
{
  Q_CONST_D(NXSplitInput);
  return d->m_IsKeepMarkOnCompleted;
}

void NXSplitInput::paintEvent(QPaintEvent *event)
{
  Q_D(NXSplitInput);
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing);

  QRect r = rect();

  // Step 1: 整widget基底（覆盖所有区域，包括分隔符/间距）
  painter.setPen(Qt::NoPen);
  painter.setBrush(NXThemeColor(d->m_ThemeMode, BasicBaseAlpha));
  painter.drawRect(r);

  // Step 2: 各 segment 圆角背景覆盖层（三态 per segment）
  for (int i = 0; i < d->m_SegmentCount; ++i) {
    QRect segRect = d->m_LineEdits[i]->geometry();
    bool focused  = d->m_LineEdits[i]->hasFocus();
    bool hovered  = (i == d->m_HoveredIndex);

    if (focused) {
      painter.setBrush(NXThemeColor(d->m_ThemeMode, DialogBase));
      painter.drawRoundedRect(segRect, d->m_BorderRadius, d->m_BorderRadius);
    } else if (hovered) {
      painter.setBrush(NXThemeColor(d->m_ThemeMode, BasicHoverAlpha));
      painter.drawRoundedRect(segRect, d->m_BorderRadius, d->m_BorderRadius);
    }
  }

  // Step 3: 外框（后绘制，叠加在背景之上）
  painter.setPen(NXThemeColor(d->m_ThemeMode, BasicBorder));
  painter.setBrush(Qt::NoBrush);
  painter.drawRoundedRect(r, d->m_BorderRadius, d->m_BorderRadius);

  // 底边 hemline（参考 NXLineEditStyle）
  painter.setBrush(NXThemeColor(d->m_ThemeMode, BasicHemline));
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
  painter.setBrush(NXThemeColor(d->m_ThemeMode, PrimaryNormal));
  qreal markY = r.bottom() - 2.5;
  qreal markH = 2.5;

  if (d->m_AllCompleted) {
    QRect firstRect = d->m_LineEdits.first()->geometry();
    QRect lastRect  = d->m_LineEdits.last()->geometry();
    qreal fullLeft  = firstRect.left();
    qreal fullRight = lastRect.right();
    painter.drawRoundedRect(QRectF(fullLeft, markY, fullRight - fullLeft, markH), 2, 2);
  } else {
    for (int i = 0; i < d->m_SegmentCount; ++i) {
      qreal markW = d->m_SegmentMarks[i];
      if (markW <= 0) {
        continue;
      }
      QRect boxRect = d->m_LineEdits[i]->geometry();
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
  int index = d->m_LineEdits.indexOf(box);
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
    if (!d->m_IsKeepMarkOnCompleted || box->text().isEmpty()) {
      d->_animateMarkOut(index);
    }
    update();
    return false;
  }
  if (event->type() == QEvent::Enter) {
    d->m_HoveredIndex = index;
    update();
    return false;
  }
  if (event->type() == QEvent::Leave) {
    if (d->m_HoveredIndex == index) {
      d->m_HoveredIndex = -1;
      update();
    }
    return false;
  }
  if (event->type() == QEvent::KeyPress) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

    if (keyEvent->key() == Qt::Key_Tab || keyEvent->key() == Qt::Key_Backtab) {
      if (keyEvent->key() == Qt::Key_Tab && index < d->m_SegmentCount - 1) {
        d->m_LineEdits[index + 1]->setFocus();
        return true;
      }
      if (keyEvent->key() == Qt::Key_Backtab && index > 0) {
        d->m_LineEdits[index - 1]->setFocus();
        return true;
      }
      return false;
    }

    if (keyEvent->key() == Qt::Key_Left) {
      if (box->cursorPosition() == 0 && index > 0) {
        d->m_LineEdits[index - 1]->setFocus();
        d->m_LineEdits[index - 1]->setCursorPosition(
            d->m_LineEdits[index - 1]->text().length());
        return true;
      }
      return false;
    }

    if (keyEvent->key() == Qt::Key_Right) {
      if (box->cursorPosition() == box->text().length() && index < d->m_SegmentCount - 1) {
        d->m_LineEdits[index + 1]->setFocus();
        d->m_LineEdits[index + 1]->setCursorPosition(0);
        return true;
      }
      return false;
    }

    if (keyEvent->key() == Qt::Key_Backspace) {
      if (box->text().isEmpty() && index > 0) {
        d->m_LineEdits[index - 1]->setFocus();
        d->m_LineEdits[index - 1]->setCursorPosition(
            d->m_LineEdits[index - 1]->text().length());
        return true;
      }
      return false;
    }

    if (keyEvent->matches(QKeySequence::Paste)) {
      QString clipText = QApplication::clipboard()->text();
      if (!clipText.isEmpty()) {
        QStringList parts = clipText.split(d->m_SeparatorText);
        if (parts.size() > 1) {
          for (int i = 0;
               i < parts.size() && (index + i) < d->m_SegmentCount; ++i) {
            d->m_LineEdits[index + i]->blockSignals(true);
            d->m_LineEdits[index + i]->setText(
                parts[i].left(d->m_MaxLengths[index + i]));
            d->m_LineEdits[index + i]->blockSignals(false);
          }
          QString fullText;
          for (int i = 0; i < d->m_SegmentCount; ++i) {
            if (i > 0) fullText += d->m_SeparatorText;
            fullText += d->m_LineEdits[i]->text();
          }
          d->_updateCompletedState();
          Q_EMIT textChanged(fullText);
          update();

          int nextIndex = qMin(index + parts.size(), d->m_SegmentCount - 1);
          d->m_LineEdits[nextIndex]->setFocus();
        } else {
          int cursorPos    = box->cursorPosition();
          QString existing = box->text();
          int remaining    = d->m_MaxLengths[index] - existing.length();
          if (remaining > 0) {
            QString merged = existing.left(cursorPos) + clipText.left(remaining) + existing.mid(cursorPos);
            box->setText(merged.left(d->m_MaxLengths[index]));
            box->setCursorPosition(cursorPos + qMin(clipText.length(), remaining));
          }
        }
      }
      return true;
    }
  }

  return QWidget::eventFilter(watched, event);
}
