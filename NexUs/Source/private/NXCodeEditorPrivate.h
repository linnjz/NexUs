#ifndef NXCODEEDITORPRIVATE_H
#define NXCODEEDITORPRIVATE_H

#include <QObject>

#include "NXDef.h"
class NXCodePlainTextEdit;
class NXCodeEditor;

class NXCodeEditorPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXCodeEditor)
  Q_PROPERTY_CREATE_D(QString, Code)
  Q_PROPERTY_CREATE_D(int, TabSize)
  Q_PROPERTY_CREATE_D(bool, IsReadOnly)

public:
  explicit NXCodeEditorPrivate(QObject *parent = nullptr);
  ~NXCodeEditorPrivate();
  Q_SLOT void onThemeChanged(NXThemeType::ThemeMode themeMode);

private:
  friend class NXCodeEditorLineArea;
  friend class NXCodeHighlighter;
  NXThemeType::ThemeMode m_ThemeMode;
  int m_Language { 0 };
  NXCodePlainTextEdit *m_Editor { nullptr };
  QWidget *m_LineArea { nullptr };
  void *m_Highlighter { nullptr };
};

#endif // NXCODEEDITORPRIVATE_H
