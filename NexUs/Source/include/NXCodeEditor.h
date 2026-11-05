#ifndef NXCODEEDITOR_H
#define NXCODEEDITOR_H

#include <QWidget>

#include "NXDef.h"
#include "NXProperty.h"
class NXCodeEditorPrivate;

class NX_EXPORT NXCodeEditor : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXCodeEditor)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), Code)
  Q_PROPERTY_CREATE_H(int, TabSize)
  Q_PROPERTY_CREATE_H(bool, IsReadOnly)

public:
  enum Language
  {
    CPP = 0,
    C,
    CSharp,
    Python,
    JavaScript,
    Lua,
    Rust,
    PHP
  };
  Q_ENUM(Language)

  explicit NXCodeEditor(QWidget *parent = nullptr);
  ~NXCodeEditor();

  void setLanguage(Language lang);
  Language getLanguage() const;
};

#endif // NXCODEEDITOR_H
