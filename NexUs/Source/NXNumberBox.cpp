#include "NXNumberBox.h"

#include <QKeyEvent>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QWheelEvent>

#include "NXTheme.h"
#include "private/NXNumberBoxPrivate.h"

Q_PROPERTY_CREATE_CPP(NXNumberBox, double, Minimum)
Q_PROPERTY_CREATE_CPP(NXNumberBox, double, Maximum)
Q_PROPERTY_CREATE_CPP(NXNumberBox, double, Step)
Q_PROPERTY_CREATE_CPP(NXNumberBox, int, BorderRadius)
Q_PROPERTY_CREATE_CPP(NXNumberBox, int, Decimals)
Q_PROPERTY_CREATE_CPP(NXNumberBox, bool, IsWrapping)

// Custom setter for Value to clamp and emit
void NXNumberBox::setValue(double value)
{
  Q_D(NXNumberBox);
  d->_pValue = value;
  d->_clampValue();
  Q_EMIT pValueChanged();
  Q_EMIT valueChanged(d->_pValue);
  update();
}

double NXNumberBox::getValue() const
{
  return d_ptr->_pValue;
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
  Q_Q(const NXNumberBox);
  return QRect(0, 0, _buttonWidth, q->height());
}

QRect NXNumberBoxPrivate::_plusRect() const
{
  Q_Q(const NXNumberBox);
  return QRect(q->width() - _buttonWidth, 0, _buttonWidth, q->height());
}

QRect NXNumberBoxPrivate::_valueRect() const
{
  Q_Q(const NXNumberBox);
  return QRect(_buttonWidth, 0, q->width() - _buttonWidth * 2, q->height());
}

QString NXNumberBoxPrivate::_formatValue() const
{
  return QString::number(_pValue, 'f', _pDecimals);
}

void NXNumberBoxPrivate::_clampValue()
{
  if (_pValue < _pMinimum) {
    _pValue = _pIsWrapping ? _pMaximum : _pMinimum;
  }
  if (_pValue > _pMaximum) {
    _pValue = _pIsWrapping ? _pMinimum : _pMaximum;
  }
}

void NXNumberBoxPrivate::_startEditing()
{
  Q_Q(NXNumberBox);
  _isEditing = true;
  if (!_lineEdit) {
    _lineEdit = new QLineEdit(q);
    _lineEdit->setAlignment(Qt::AlignCenter);
    _lineEdit->setFrame(false);
    QFont f = q->font();
    f.setPixelSize(14);
    _lineEdit->setFont(f);
    _lineEdit->setStyleSheet(QStringLiteral("QLineEdit { background: transparent; border: none; }"));
    _lineEdit->installEventFilter(q);
  }
  _lineEdit->setGeometry(_valueRect());
  _lineEdit->setText(_formatValue());
  _lineEdit->selectAll();
  _lineEdit->show();
  _lineEdit->setFocus();
}

void NXNumberBoxPrivate::_finishEditing()
{
  Q_Q(NXNumberBox);
  if (!_isEditing) {
    return;
  }
  _isEditing = false;
  if (_lineEdit) {
    bool ok;
    double newVal = _lineEdit->text().toDouble(&ok);
    if (ok) {
      _pValue = newVal;
      _clampValue();
      Q_EMIT q->pValueChanged();
      Q_EMIT q->valueChanged(_pValue);
    }
    _lineEdit->hide();
  }
  q->update();
}

NXNumberBox::NXNumberBox(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXNumberBoxPrivate())
{
  Q_D(NXNumberBox);
  d->q_ptr          = this;
  d->_pBorderRadius = 6;
  d->_pValue        = 0.0;
  d->_pMinimum      = 0.0;
  d->_pMaximum      = 100.0;
  d->_pStep         = 1.0;
  d->_pDecimals     = 0;
  d->_pIsWrapping   = false;
  setObjectName("NXNumberBox");
  setMouseTracking(true);
  setFocusPolicy(Qt::StrongFocus);
  setFixedSize(160, 36);

  d->_themeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
    update();
  });
}

NXNumberBox::~NXNumberBox()
{
}

void NXNumberBox::stepUp()
{
  Q_D(NXNumberBox);
  setValue(d->_pValue + d->_pStep);
}

void NXNumberBox::stepDown()
{
  Q_D(NXNumberBox);
  setValue(d->_pValue - d->_pStep);
}

void NXNumberBox::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);
  Q_D(NXNumberBox);
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  QRect fullRect(0, 0, width(), height());
  QColor borderColor = NXThemeColor(d->_themeMode, BasicBorder);
  QColor baseColor   = NXThemeColor(d->_themeMode, BasicBase);
  QColor hoverColor  = NXThemeColor(d->_themeMode, BasicHover);
  QColor pressColor  = NXThemeColor(d->_themeMode, BasicPress);
  QColor textColor   = NXThemeColor(d->_themeMode, BasicText);

  // Background
  QPainterPath clipPath;
  clipPath.addRoundedRect(fullRect, d->_pBorderRadius, d->_pBorderRadius);
  painter.setClipPath(clipPath);

  // Minus button
  QRect minusR   = d->_minusRect();
  QColor minusBg = baseColor;
  if (d->_isMinusPressed)
    minusBg = pressColor;
  else if (d->_isMinusHovered)
    minusBg = hoverColor;
  painter.setPen(Qt::NoPen);
  painter.setBrush(minusBg);
  painter.drawRect(minusR);

  // Plus button
  QRect plusR   = d->_plusRect();
  QColor plusBg = baseColor;
  if (d->_isPlusPressed)
    plusBg = pressColor;
  else if (d->_isPlusHovered)
    plusBg = hoverColor;
  painter.setBrush(plusBg);
  painter.drawRect(plusR);

  // Value area
  QRect valR = d->_valueRect();
  painter.setBrush(NXThemeColor(d->_themeMode, BasicBase));
  painter.drawRect(valR);

  // Reset clip, draw border
  painter.setClipPath(clipPath);
  painter.setPen(QPen(borderColor, 1));
  painter.setBrush(Qt::NoBrush);
  painter.drawRoundedRect(QRectF(0.5, 0.5, width() - 1, height() - 1), d->_pBorderRadius, d->_pBorderRadius);

  // Separator lines
  painter.setPen(QPen(borderColor, 1));
  painter.drawLine(d->_buttonWidth, 4, d->_buttonWidth, height() - 4);
  painter.drawLine(width() - d->_buttonWidth, 4, width() - d->_buttonWidth, height() - 4);

  // Minus icon
  QFont btnFont = font();
  btnFont.setPixelSize(18);
  btnFont.setBold(true);
  painter.setFont(btnFont);
  bool atMin = d->_pValue <= d->_pMinimum && !d->_pIsWrapping;
  painter.setPen(atMin ? NXThemeColor(d->_themeMode, BasicTextDisable) : textColor);
  painter.drawText(minusR, Qt::AlignCenter, QStringLiteral("-"));

  // Plus icon
  bool atMax = d->_pValue >= d->_pMaximum && !d->_pIsWrapping;
  painter.setPen(atMax ? NXThemeColor(d->_themeMode, BasicTextDisable) : textColor);
  painter.drawText(plusR, Qt::AlignCenter, QStringLiteral("+"));

  // Value text (only when not editing)
  if (!d->_isEditing) {
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
      d->_isMinusPressed = true;
      update();
    } else if (d->_plusRect().contains(event->pos())) {
      d->_isPlusPressed = true;
      update();
    }
  }
  QWidget::mousePressEvent(event);
}

void NXNumberBox::mouseReleaseEvent(QMouseEvent *event)
{
  Q_D(NXNumberBox);
  if (event->button() == Qt::LeftButton) {
    if (d->_isMinusPressed && d->_minusRect().contains(event->pos())) {
      stepDown();
    }
    if (d->_isPlusPressed && d->_plusRect().contains(event->pos())) {
      stepUp();
    }
    d->_isMinusPressed = false;
    d->_isPlusPressed  = false;
    update();
  }
  QWidget::mouseReleaseEvent(event);
}

void NXNumberBox::mouseMoveEvent(QMouseEvent *event)
{
  Q_D(NXNumberBox);
  bool minusHover = d->_minusRect().contains(event->pos());
  bool plusHover  = d->_plusRect().contains(event->pos());
  if (d->_isMinusHovered != minusHover || d->_isPlusHovered != plusHover) {
    d->_isMinusHovered = minusHover;
    d->_isPlusHovered  = plusHover;
    update();
  }
  QWidget::mouseMoveEvent(event);
}

void NXNumberBox::leaveEvent(QEvent *event)
{
  Q_D(NXNumberBox);
  d->_isMinusHovered = false;
  d->_isPlusHovered  = false;
  d->_isMinusPressed = false;
  d->_isPlusPressed  = false;
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
    if (d->_isEditing) {
      d->_finishEditing();
    } else {
      d->_startEditing();
    }
    return;
  case Qt::Key_Escape :
    if (d->_isEditing) {
      d->_isEditing = false;
      if (d->_lineEdit) {
        d->_lineEdit->hide();
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
  if (watched == d->_lineEdit) {
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
        d->_isEditing = false;
        d->_lineEdit->hide();
        setFocus();
        update();
        return true;
      }
    }
  }
  return QWidget::eventFilter(watched, event);
}
