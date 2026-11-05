#ifndef NXINTVALIDATOR_H
#define NXINTVALIDATOR_H

#include <QIntValidator>

#include "NXProperty.h"

class NXIntValidator : public QIntValidator
{
  Q_OBJECT
  Q_PRIVATE_CREATE(bool, IsHexMode)

public:
  explicit NXIntValidator(QObject *parent = nullptr);
  explicit NXIntValidator(int bottom, int top, QObject *parent = nullptr);
  ~NXIntValidator();

  QValidator::State validate(QString &input, int &pos) const override;
  void fixup(QString &input) const override;

private:
  QString _completeInput(QString const &input, int length) const;
};

#endif // NXINTVALIDATOR_H
