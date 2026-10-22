#ifndef NXSPLITINPUT_H
#define NXSPLITINPUT_H

#include <QWidget>

#include "NXProperty.h"

class NXSplitInputPrivate;
class QValidator;

class NX_EXPORT NXSplitInput : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXSplitInput)
  Q_PROPERTY_CREATE_H(QS_SET_CREF(QString), Text)
  Q_PROPERTY_CREATE_H(QS_SET_CREF(QString), Separator)
  Q_PROPERTY_CREATE_H(QS_SET_CREF(QStringList), SegmentTexts)
  Q_PROPERTY_CREATE_H(qreal, SegmentSpacing)
  Q_PROPERTY_CREATE_H(int, BorderRadius)
  Q_PROPERTY_CREATE_H(bool, IsClearButtonEnable)
  Q_PROPERTY_CREATE_H(bool, IsKeepMarkOnCompleted)

public:
  explicit NXSplitInput(QWidget *parent = nullptr);
  explicit NXSplitInput(int segmentCount, const QString &separator = QStringLiteral("."), QWidget *parent = nullptr);
  ~NXSplitInput();

  int getSegmentCount() const;
  QString getSegmentText(int index) const;
  void setSegmentText(int index, const QString &text);
  void setSegmentMaxLength(int index, int maxLength);
  int getSegmentMaxLength(int index) const;
  void setMaxLength(int maxLength);
  void setSegmentPlaceholderText(int index, const QString &text);
  QString getSegmentPlaceholderText(int index) const;
  void setSegmentValidator(int index, QValidator *validator);
  const QValidator *getSegmentValidator(int index) const;

  Q_SIGNAL void segmentTextChanged(int segmentIndex, const QString &text);
  Q_SIGNAL void textChanged(const QString &fullText);
  Q_SIGNAL void segmentFocusIn(int segmentIndex, const QString &text);
  Q_SIGNAL void segmentFocusOut(int segmentIndex, const QString &text);
  Q_SIGNAL void segmentEditingFinished(int segmentIndex);
  Q_SIGNAL void editingFinished();
  Q_SIGNAL void returnPressed(int segmentIndex);

protected:
  void paintEvent(QPaintEvent *event) override;
  bool eventFilter(QObject *watched, QEvent *event) override;
};

#endif // NXSPLITINPUT_H
