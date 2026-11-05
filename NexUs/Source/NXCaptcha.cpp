#include "NXCaptcha.h"

#include <QApplication>
#include <QClipboard>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLineEdit>
#include <QRegularExpressionValidator>

#include "NXTheme.h"
#include "private/NXCaptchaPrivate.h"

Q_PROPERTY_CREATE_CPP(NXCaptcha, QPS_PAEMIT(int), BorderRadius)
Q_PROPERTY_CREATE_CPP(NXCaptcha, QPS_PAEMIT(int), BoxSpacing)

void NXCaptcha::setCodeLength(int CodeLength)
{
  Q_D(NXCaptcha);
  if (d->m_CodeLength == CodeLength) {
    return;
  }
  d->m_CodeLength = CodeLength;
  d->_buildBoxes();
  Q_EMIT signalCodeLengthChanged();
}

int NXCaptcha::getCodeLength() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  return d_ptr->m_CodeLength;
}

void NXCaptcha::setBoxSize(int BoxSize)
{
  Q_D(NXCaptcha);
  if (d->m_BoxSize == BoxSize) {
    return;
  }
  d->m_BoxSize = BoxSize;
  for (QLineEdit *box : d->m_Boxes) {
    box->setFixedSize(d->m_BoxSize, d->m_BoxSize);
  }
  setFixedHeight(d->m_BoxSize + 8);
  Q_EMIT signalBoxSizeChanged();
}

int NXCaptcha::getBoxSize() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  return d_ptr->m_BoxSize;
}

void NXCaptcha::setInputMode(InputMode mode)
{
  Q_D(NXCaptcha);
  d->m_InputMode = static_cast<int>(mode);
  d->_updateValidators();
}

NXCaptcha::InputMode NXCaptcha::getInputMode() const
{
  return static_cast<InputMode>(d_ptr->m_InputMode);
}

NXCaptcha::NXCaptcha(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXCaptchaPrivate())
{
  Q_D(NXCaptcha);
  d->q_ptr          = this;
  d->m_CodeLength   = 6;
  d->m_BorderRadius = 6;
  d->m_BoxSize      = 42;
  d->m_BoxSpacing   = 8;
  d->m_InputMode     = AlphaNumeric;
  setObjectName("NXCaptcha");
  setFixedHeight(d->m_BoxSize + 8);

  d->m_BoxLayout = new QHBoxLayout(this);
  d->m_BoxLayout->setContentsMargins(0, 0, 0, 0);
  d->m_BoxLayout->setSpacing(d->m_BoxSpacing);

  d->m_ThemeMode = nxTheme->getThemeMode();
  d->_buildBoxes();
  connect(nxTheme, &NXTheme::themeModeChanged, d, &NXCaptchaPrivate::onThemeChanged);
}

NXCaptcha::~NXCaptcha()
{
}

QString NXCaptcha::getCode() const
{
  Q_CONST_D(NXCaptcha);
  QString code;
  for (QLineEdit *box : d->m_Boxes) {
    code += box->text();
  }
  return code;
}

void NXCaptcha::clear()
{
  Q_D(NXCaptcha);
  for (QLineEdit *box : d->m_Boxes) {
    box->blockSignals(true);
    box->clear();
    box->blockSignals(false);
  }
  d->_applyThemeStyle();
  if (!d->m_Boxes.isEmpty()) {
    d->m_Boxes.first()->setFocus();
  }
  Q_EMIT codeChanged(QString());
}

bool NXCaptcha::eventFilter(QObject *watched, QEvent *event)
{
  Q_D(NXCaptcha);
  QLineEdit *box = qobject_cast<QLineEdit *>(watched);
  if (!box) {
    return QWidget::eventFilter(watched, event);
  }
  int index = d->m_Boxes.indexOf(box);
  if (index < 0) {
    return QWidget::eventFilter(watched, event);
  }

  if (event->type() == QEvent::FocusIn) {
    d->_applyThemeStyle();
    return false;
  }
  if (event->type() == QEvent::FocusOut) {
    d->_applyThemeStyle();
    return false;
  }
  if (event->type() == QEvent::KeyPress) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

    if (keyEvent->matches(QKeySequence::Paste)) {
      QString clipText = QApplication::clipboard()->text().trimmed().toUpper();
      if (d->m_InputMode == NXCaptcha::DigitOnly) {
        clipText.remove(QRegularExpression(QStringLiteral("[^0-9]")));
      } else {
        clipText.remove(QRegularExpression(QStringLiteral("[^A-Z0-9]")));
      }
      if (!clipText.isEmpty()) {
        for (int i = 0; i < clipText.length() && (index + i) < d->m_Boxes.count(); ++i) {
          d->m_Boxes[index + i]->blockSignals(true);
          d->m_Boxes[index + i]->setText(clipText.mid(i, 1));
          d->m_Boxes[index + i]->blockSignals(false);
        }
        d->_applyThemeStyle();
        int nextIndex = qMin(index + clipText.length(), d->m_Boxes.count() - 1);
        d->m_Boxes[nextIndex]->setFocus();
        QString code = getCode();
        Q_EMIT codeChanged(code);
        if (code.length() == d->m_CodeLength) {
          Q_EMIT codeCompleted(code);
        }
      }
      return true;
    }

    if (keyEvent->key() == Qt::Key_Left) {
      if (index > 0) {
        d->m_Boxes[index - 1]->setFocus();
      }
      return true;
    }
    if (keyEvent->key() == Qt::Key_Right) {
      if (index < d->m_Boxes.count() - 1) {
        d->m_Boxes[index + 1]->setFocus();
      }
      return true;
    }

    if (keyEvent->key() == Qt::Key_Backspace) {
      if (box->text().isEmpty()) {
        if (index > 0) {
          d->m_Boxes[index - 1]->clear();
          d->m_Boxes[index - 1]->setFocus();
          d->_applyThemeStyle();
          Q_EMIT codeChanged(getCode());
        }
        return true;
      }
      box->clear();
      d->_applyThemeStyle();
      Q_EMIT codeChanged(getCode());
      return true;
    }

    QString text = keyEvent->text();
    if (!text.isEmpty()) {
      QChar ch   = text.at(0);
      bool valid = false;
      if (d->m_InputMode == NXCaptcha::DigitOnly) {
        valid = ch.isDigit();
      } else {
        valid = ch.isLetterOrNumber();
      }
      if (!valid) {
        return true;
      }
      text = text.toUpper();
      box->blockSignals(true);
      box->setText(text);
      box->blockSignals(false);
      d->_applyThemeStyle();
      if (index < d->m_Boxes.count() - 1) {
        d->m_Boxes[index + 1]->setFocus();
      }
      QString code = getCode();
      Q_EMIT codeChanged(code);
      if (code.length() == d->m_CodeLength) {
        Q_EMIT codeCompleted(code);
      }
      return true;
    }
  }
  return QWidget::eventFilter(watched, event);
}

NXCaptchaPrivate::NXCaptchaPrivate(QObject *parent)
    : QObject { parent }
{
}

NXCaptchaPrivate::~NXCaptchaPrivate()
{
}

void NXCaptchaPrivate::onThemeChanged(NXThemeType::ThemeMode themeMode)
{
  m_ThemeMode = themeMode;
  _applyThemeStyle();
}

void NXCaptchaPrivate::_buildBoxes()
{
  Q_Q(NXCaptcha);
  for (QLineEdit *box : m_Boxes) {
    m_BoxLayout->removeWidget(box);
    box->removeEventFilter(q);
    box->deleteLater();
  }
  m_Boxes.clear();

  m_BoxLayout->setSpacing(m_BoxSpacing);

  for (int i = 0; i < m_CodeLength; ++i) {
    QLineEdit *box = new QLineEdit(q);
    box->setMaxLength(1);
    box->setAlignment(Qt::AlignCenter);
    box->setFixedSize(m_BoxSize, m_BoxSize);
    box->setFocusPolicy(Qt::StrongFocus);
    box->setContextMenuPolicy(Qt::NoContextMenu);
    QFont font = box->font();
    font.setPixelSize(20);
    font.setBold(true);
    box->setFont(font);
    box->installEventFilter(q);
    m_BoxLayout->addWidget(box);
    m_Boxes.append(box);
  }

  _applyThemeStyle();

  if (!m_Boxes.isEmpty()) {
    m_Boxes.first()->setFocus();
  }
}

void NXCaptchaPrivate::_updateValidators()
{
  Q_Q(NXCaptcha);
  QString pattern                        = (m_InputMode == NXCaptcha::DigitOnly) ? QStringLiteral("[0-9]") : QStringLiteral("[A-Za-z0-9]");
  QRegularExpressionValidator *validator = new QRegularExpressionValidator(QRegularExpression(pattern), q);
  for (QLineEdit *box : m_Boxes) {
    box->setValidator(validator);
  }
}

void NXCaptchaPrivate::_applyThemeStyle()
{
  for (int i = 0; i < m_Boxes.count(); ++i) {
    QLineEdit *box = m_Boxes[i];
    bool focused   = box->hasFocus();
    _applyBoxStyle(box, focused);
  }
}

void NXCaptchaPrivate::_applyBoxStyle(QLineEdit *box, bool focused)
{
  QColor bgColor   = NXThemeColor(m_ThemeMode, BasicBase);
  QColor textColor = NXThemeColor(m_ThemeMode, BasicText);
  QColor borderColor;
  if (focused || !box->text().isEmpty()) {
    borderColor = NXThemeColor(m_ThemeMode, PrimaryNormal);
  } else {
    borderColor = NXThemeColor(m_ThemeMode, BasicBorder);
  }
  box->setStyleSheet(QString(QStringLiteral("QLineEdit { background: %1; border: 1px solid %2; border-radius: %3px; "
                                            "font-size: 20px; font-weight: bold; color: %4; }"
                                            "QLineEdit:focus { border: 2px solid %5; }"))
                         .arg(bgColor.name(QColor::HexArgb))
                         .arg(borderColor.name(QColor::HexArgb))
                         .arg(m_BorderRadius)
                         .arg(textColor.name(QColor::HexArgb))
                         .arg(NXThemeColor(m_ThemeMode, PrimaryNormal).name(QColor::HexArgb)));
}
