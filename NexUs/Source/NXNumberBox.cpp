#include "NXNumberBox.h"

#include <QKeyEvent>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QWheelEvent>

#include "NXTheme.h"
#include "private/NXNumberBoxPrivate.h"

Q_PROPERTY_CREATE_CPP(NXNumberBox, QPS_PAEMIT(double), Minimum)
Q_PROPERTY_CREATE_CPP(NXNumberBox, QPS_PAEMIT(double), Maximum)
Q_PROPERTY_CREATE_CPP(NXNumberBox, QPS_PAEMIT(double), Step)
Q_PROPERTY_CREATE_CPP(NXNumberBox, QPS_PAEMIT(int), BorderRadius)
Q_PROPERTY_CREATE_CPP(NXNumberBox, QPS_PAEMIT(int), Decimals)
Q_PROPERTY_CREATE_CPP(NXNumberBox, QPS_PAEMIT(bool), IsWrapping)

// Custom setter for Value to clamp and emit
void NXNumberBox::setValue(double value)
{
  Q_D(NXNumberBox);
  d->m_Value = value;
  d->_clampValue();
  Q_EMIT signalValueChanged();
  Q_EMIT valueChanged(d->m_Value);
  update();
}

double NXNumberBox::getValue() const noexcept(std::is_nothrow_copy_constructible_v<double>)
{
  return d_ptr->m_Value;
}

NXNumberBoxPrivate::NXNumberBoxPrivate(QObject *parent)
    : QObject(parent)
{
}

NXNumberBoxPrivate::~NXNumberBoxPrivate()
{
}

QRect NXNumberBoxPrivate::_minusRect() const
{
  Q_Q(NXNumberBox const);
  return QRect(0, 0, m_ButtonWidth, q->height());
}

QRect NXNumberBoxPrivate::_plusRect() const
{
  Q_Q(NXNumberBox const);
  return QRect(q->width() - m_ButtonWidth, 0, m_ButtonWidth, q->height());
}

QRect NXNumberBoxPrivate::_valueRect() const
{
  Q_Q(NXNumberBox const);
  return QRect(m_ButtonWidth, 0, q->width() - m_ButtonWidth * 2, q->height());
}

QString NXNumberBoxPrivate::_formatValue() const
{
  return QString::number(m_Value, 'f', m_Decimals);
}

void NXNumberBoxPrivate::_clampValue()
{
  if (m_Value < m_Minimum) {
    m_Value = m_IsWrapping ? m_Maximum : m_Minimum;
  }
  if (m_Value > m_Maximum) {
    m_Value = m_IsWrapping ? m_Minimum : m_Maximum;
  }
}

void NXNumberBoxPrivate::_startEditing()
{
  Q_Q(NXNumberBox);
  m_IsEditing = true;
  if (!m_LineEdit) {
    m_LineEdit = new QLineEdit(q);
    m_LineEdit->setAlignment(Qt::AlignCenter);
    m_LineEdit->setFrame(false);
    QFont f = q->font();
    f.setPixelSize(14);
    m_LineEdit->setFont(f);
    m_LineEdit->setStyleSheet(QStringLiteral("QLineEdit { background: transparent; border: none; }"));
    m_LineEdit->installEventFilter(q);
  }
  m_LineEdit->setGeometry(_valueRect());
  m_LineEdit->setText(_formatValue());
  m_LineEdit->selectAll();
  m_LineEdit->show();
  m_LineEdit->setFocus();
}

void NXNumberBoxPrivate::_finishEditing()
{
  Q_Q(NXNumberBox);
  if (!m_IsEditing) {
    return;
  }
  m_IsEditing = false;
  if (m_LineEdit) {
    bool ok;
    double newVal = m_LineEdit->text().toDouble(&ok);
    if (ok) {
      m_Value = newVal;
      _clampValue();
      Q_EMIT q->signalValueChanged();
      Q_EMIT q->valueChanged(m_Value);
    }
    m_LineEdit->hide();
  }
  q->update();
}

NXNumberBox::NXNumberBox(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXNumberBoxPrivate())
{
  Q_D(NXNumberBox);
  d->q_ptr          = this;
  d->m_BorderRadius = 6;
  d->m_Value        = 0.0;
  d->m_Minimum      = 0.0;
  d->m_Maximum      = 100.0;
  d->m_Step         = 1.0;
  d->m_Decimals     = 0;
  d->m_IsWrapping   = false;
  setObjectName("NXNumberBox");
  setMouseTracking(true);
  setFocusPolicy(Qt::StrongFocus);
  setFixedSize(160, 36);

  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
    update();
  });
}

NXNumberBox::~NXNumberBox()
{
}

void NXNumberBox::stepUp()
{
  Q_D(NXNumberBox);
  setValue(d->m_Value + d->m_Step);
}

void NXNumberBox::stepDown()
{
  Q_D(NXNumberBox);
  setValue(d->m_Value - d->m_Step);
}

void NXNumberBox::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);
  Q_D(NXNumberBox);
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  QRect fullRect(0, 0, width(), height());
  QColor borderColor = NXThemeColor(d->m_ThemeMode, BasicBorder);
  QColor baseColor   = NXThemeColor(d->m_ThemeMode, BasicBase);
  QColor hoverColor  = NXThemeColor(d->m_ThemeMode, BasicHover);
  QColor pressColor  = NXThemeColor(d->m_ThemeMode, BasicPress);
  QColor textColor   = NXThemeColor(d->m_ThemeMode, BasicText);

  // Background
  QPainterPath clipPath;
  clipPath.addRoundedRect(fullRect, d->m_BorderRadius, d->m_BorderRadius);
  painter.setClipPath(clipPath);

  // Minus button
  QRect minusR   = d->_minusRect();
  QColor minusBg = baseColor;
  if (d->m_IsMinusPressed)
    minusBg = pressColor;
  else if (d->m_IsMinusHovered)
    minusBg = hoverColor;
  painter.setPen(Qt::NoPen);
  painter.setBrush(minusBg);
  painter.drawRect(minusR);

  // Plus button
  QRect plusR   = d->_plusRect();
  QColor plusBg = baseColor;
  if (d->m_IsPlusPressed)
    plusBg = pressColor;
  else if (d->m_IsPlusHovered)
    plusBg = hoverColor;
  painter.setBrush(plusBg);
  painter.drawRect(plusR);

  // Value area
  QRect valR = d->_valueRect();
  painter.setBrush(NXThemeColor(d->m_ThemeMode, BasicBase));
  painter.drawRect(valR);

  // Reset clip, draw border
  painter.setClipPath(clipPath);
  painter.setPen(QPen(borderColor, 1));
  painter.setBrush(Qt::NoBrush);
  painter.drawRoundedRect(QRectF(0.5, 0.5, width() - 1, height() - 1), d->m_BorderRadius, d->m_BorderRadius);

  // Separator lines
  painter.setPen(QPen(borderColor, 1));
  painter.drawLine(d->m_ButtonWidth, 4, d->m_ButtonWidth, height() - 4);
  painter.drawLine(width() - d->m_ButtonWidth, 4, width() - d->m_ButtonWidth, height() - 4);

  // Minus icon
  QFont btnFont = font();
  btnFont.setPixelSize(18);
  btnFont.setBold(true);
  painter.setFont(btnFont);
  bool atMin = d->m_Value <= d->m_Minimum && !d->m_IsWrapping;
  painter.setPen(atMin ? NXThemeColor(d->m_ThemeMode, BasicTextDisable) : textColor);
  painter.drawText(minusR, Qt::AlignCenter, QStringLiteral("-"));

  // Plus icon
  bool atMax = d->m_Value >= d->m_Maximum && !d->m_IsWrapping;
  painter.setPen(atMax ? NXThemeColor(d->m_ThemeMode, BasicTextDisable) : textColor);
  painter.drawText(plusR, Qt::AlignCenter, QStringLiteral("+"));

  // Value text (only when not editing)
  if (!d->m_IsEditing) {
    QFont valFont = font();
    valFont.setPixelSize(14);
    painter.setFont(valFont);
    painter.setPen(textColor);
    painter.drawText(valR, Qt::AlignCenter, d->_formatValue());
  }
}

void NXNumberBox::mousePressEvent(QMouseEvent *event)
{
  Q_D(NXNumberBox);
  if (event->button() == Qt::LeftButton) {
    if (d->_minusRect().contains(event->pos())) {
      d->m_IsMinusPressed = true;
      update();
    } else if (d->_plusRect().contains(event->pos())) {
      d->m_IsPlusPressed = true;
      update();
    }
  }
  QWidget::mousePressEvent(event);
}

void NXNumberBox::mouseReleaseEvent(QMouseEvent *event)
{
  Q_D(NXNumberBox);
  if (event->button() == Qt::LeftButton) {
    if (d->m_IsMinusPressed && d->_minusRect().contains(event->pos())) {
      stepDown();
    }
    if (d->m_IsPlusPressed && d->_plusRect().contains(event->pos())) {
      stepUp();
    }
    d->m_IsMinusPressed = false;
    d->m_IsPlusPressed  = false;
    update();
  }
  QWidget::mouseReleaseEvent(event);
}

void NXNumberBox::mouseMoveEvent(QMouseEvent *event)
{
  Q_D(NXNumberBox);
  bool minusHover = d->_minusRect().contains(event->pos());
  bool plusHover  = d->_plusRect().contains(event->pos());
  if (d->m_IsMinusHovered != minusHover || d->m_IsPlusHovered != plusHover) {
    d->m_IsMinusHovered = minusHover;
    d->m_IsPlusHovered  = plusHover;
    update();
  }
  QWidget::mouseMoveEvent(event);
}

void NXNumberBox::leaveEvent(QEvent *event)
{
  Q_D(NXNumberBox);
  d->m_IsMinusHovered = false;
  d->m_IsPlusHovered  = false;
  d->m_IsMinusPressed = false;
  d->m_IsPlusPressed  = false;
  update();
  QWidget::leaveEvent(event);
}

void NXNumberBox::wheelEvent(QWheelEvent *event)
{
  if (event->angleDelta().y() > 0) {
    stepUp();
  } else if (event->angleDelta().y() < 0) {
    stepDown();
  }
  event->accept();
}

void NXNumberBox::keyPressEvent(QKeyEvent *event)
{
  Q_D(NXNumberBox);
  switch (event->key()) {
  case Qt::Key_Up   : stepUp(); return;
  case Qt::Key_Down : stepDown(); return;
  case Qt::Key_Return :
  case Qt::Key_Enter :
    if (d->m_IsEditing) {
      d->_finishEditing();
    } else {
      d->_startEditing();
    }
    return;
  case Qt::Key_Escape :
    if (d->m_IsEditing) {
      d->m_IsEditing = false;
      if (d->m_LineEdit) {
        d->m_LineEdit->hide();
      }
      setFocus();
      update();
    }
    return;
  default : break;
  }
  QWidget::keyPressEvent(event);
}

void NXNumberBox::mouseDoubleClickEvent(QMouseEvent *event)
{
  Q_D(NXNumberBox);
  if (d->_valueRect().contains(event->pos())) {
    d->_startEditing();
  }
  QWidget::mouseDoubleClickEvent(event);
}

bool NXNumberBox::eventFilter(QObject *watched, QEvent *event)
{
  Q_D(NXNumberBox);
  if (watched == d->m_LineEdit) {
    if (event->type() == QEvent::FocusOut) {
      d->_finishEditing();
    } else if (event->type() == QEvent::KeyPress) {
      QKeyEvent *ke = static_cast<QKeyEvent *>(event);
      if (ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Enter) {
        d->_finishEditing();
        setFocus();
        return true;
      }
      if (ke->key() == Qt::Key_Escape) {
        d->m_IsEditing = false;
        d->m_LineEdit->hide();
        setFocus();
        update();
        return true;
      }
    }
  }
  return QWidget::eventFilter(watched, event);
}
