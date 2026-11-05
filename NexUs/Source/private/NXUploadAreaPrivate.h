#ifndef NXUPLOADAREAPRIVATE_H
#define NXUPLOADAREAPRIVATE_H

#include <QObject>

#include "NXDef.h"

class NXUploadArea;

class NXUploadAreaPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXUploadArea)
  Q_PROPERTY_CREATE_D(QString, Title)
  Q_PROPERTY_CREATE_D(QString, SubTitle)
  Q_PROPERTY_CREATE_D(QString, DialogTitle)
  Q_PROPERTY_CREATE_D(QStringList, AcceptedSuffixes)
  Q_PROPERTY_CREATE_D(qint64, MaxFileSize)
  Q_PROPERTY_CREATE_D(int, BorderRadius)
  Q_PROPERTY_CREATE_D(int, MaxFileCount)
  Q_PROPERTY_CREATE_D(bool, IsMultiple)

public:
  explicit NXUploadAreaPrivate(QObject *parent = nullptr);
  ~NXUploadAreaPrivate();

  Q_SLOT void onThemeChanged(NXThemeType::ThemeMode themeMode);

private:
  friend class NXUploadArea;
  bool m_IsDragOver { false };
  bool m_IsHover { false };
  bool m_IsPressed { false };
  NXThemeType::ThemeMode m_ThemeMode;
  QString m_MimeFilter;
  QStringList m_FilePaths;
  void _openFileDialog();
  bool _validateFile(QString const &filePath, QString &reason) const;
  void _addFiles(QStringList const &filePaths);
  void _removeFile(int index);
};

#endif // NXUPLOADAREAPRIVATE_H
