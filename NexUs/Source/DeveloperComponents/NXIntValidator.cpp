#include "NXIntValidator.h"

NXIntValidator::NXIntValidator(QObject *parent)
    : QIntValidator { parent }
    , m_IsHexMode { false }
{
  
}

NXIntValidator::NXIntValidator(int bottom, int top, QObject *parent)
    : QIntValidator { bottom, top, parent }
    , m_IsHexMode { false }
{
}

NXIntValidator::~NXIntValidator()
{
}

QValidator::State NXIntValidator::validate(QString &input, int &pos) const
{
  QString inputCopy = input;
  if (m_IsHexMode) {
    inputCopy.remove(QStringLiteral("#"));
    if (!inputCopy.isEmpty()) {
      bool isInt = false;
      int value  = inputCopy.toInt(&isInt, 16);
      if (!isInt) {
        return QValidator::Invalid;
      }
      if (value < bottom() || value > top()) {
        return QValidator::Invalid;
      }
      int topLength = QString::number(top(), 16).length();
      if (inputCopy.length() > topLength) {
        return QValidator::Invalid;
      }
    }
    inputCopy.prepend(QStringLiteral("#"));
  } else {
    if (input.isEmpty()) {
      return QValidator::Intermediate;
    }
    bool isInt = false;
    int value  = inputCopy.toInt(&isInt);
    if (!isInt) {
      return QValidator::Invalid;
    }
    if (value < bottom() || value > top()) {
      return QValidator::Invalid;
    }
  }
  input = inputCopy;
  return QValidator::Acceptable;
}

void NXIntValidator::fixup(QString &input) const
{
  if (m_IsHexMode) {
    QString inputComplete = _completeInput(input, QString::number(top(), 16).length());
    input                 = QString(QStringLiteral("#")) + inputComplete;
  } else {
    input = QString::number(bottom());
  }
}

QString NXIntValidator::_completeInput(QString const &input, int length) const
{
  if (input.length() < length) {
    return QString(length - input.length(), '0') + input;
  }
  return input;
}
