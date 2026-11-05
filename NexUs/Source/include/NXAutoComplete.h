#ifndef NXAUTOCOMPLETE_H
#define NXAUTOCOMPLETE_H

#include <QWidget>

#include "NXDef.h"
#include "NXProperty.h"

class NXAutoCompletePrivate;

class NX_EXPORT NXAutoComplete : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXAutoComplete)
  Q_PROPERTY_CREATE_H(int, BorderRadius)
  Q_PROPERTY_CREATE_H(int, MaxVisibleItems)
  Q_PROPERTY_CREATE_H(Qt::CaseSensitivity, CaseSensitivity)

public:
  enum MatchMode
  {
    Contains = 0,
    StartsWith,
    EndsWith,
    RegExp,
  };
  Q_ENUM(MatchMode)

  explicit NXAutoComplete(QWidget *parent = nullptr);
  ~NXAutoComplete();

  void setPlaceholderText(QString const &placeholderText);
  QString placeholderText() const;

  void setText(QString const &text);
  QString text() const;

  void setCompletions(QStringList const &completions);
  QStringList completions() const;

  void setMatchMode(MatchMode mode);
  MatchMode matchMode() const;

  void setFixedHeight(int h);
  Q_SIGNAL void textChanged(QString const &text);
  Q_SIGNAL void textEdited(QString const &text);
  Q_SIGNAL void completionSelected(QString const &completion);
  Q_SIGNAL void returnPressed(QString const &text);
};

#endif // NXAUTOCOMPLETE_H
