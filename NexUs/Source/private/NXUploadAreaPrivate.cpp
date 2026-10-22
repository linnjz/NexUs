#include "NXUploadAreaPrivate.h"

#include <QFileDialog>
#include <QFileInfo>

#include "NXTheme.h"
#include "NXUploadArea.h"

NXUploadAreaPrivate::NXUploadAreaPrivate(QObject *parent)
    : QObject { parent }
{
}

NXUploadAreaPrivate::~NXUploadAreaPrivate()
{
}

void NXUploadAreaPrivate::onThemeChanged(NXThemeType::ThemeMode themeMode)
{
  Q_Q(NXUploadArea);
  _themeMode = themeMode;
  q->update();
}

void NXUploadAreaPrivate::_openFileDialog()
{
  Q_Q(NXUploadArea);
  QString dialogTitle = _pDialogTitle.isEmpty() ? QStringLiteral("选择文件") : _pDialogTitle;
  QString filter;
  if (!_pAcceptedSuffixes.isEmpty()) {
    QStringList patterns;
    for (const QString &suffix : _pAcceptedSuffixes) {
      patterns.append(QStringLiteral("*.") + suffix);
    }
    filter = QString(QStringLiteral("允许的文件")) + QStringLiteral(" (") + patterns.join(QStringLiteral(" ")) +
             QStringLiteral(")");
  } else if (!_mimeFilter.isEmpty()) {
    filter = _mimeFilter;
  }

  QStringList files;
  if (_pIsMultiple) {
    files = QFileDialog::getOpenFileNames(q, dialogTitle, QString(), filter);
  } else {
    QString file = QFileDialog::getOpenFileName(q, dialogTitle, QString(), filter);
    if (!file.isEmpty()) {
      files.append(file);
    }
  }
  if (!files.isEmpty()) {
    _addFiles(files);
  }
}

bool NXUploadAreaPrivate::_validateFile(const QString &filePath, QString &reason) const
{
  QFileInfo fileInfo(filePath);
  if (!fileInfo.exists()) {
    reason = QStringLiteral("文件不存在");
    return false;
  }
  if (!_pAcceptedSuffixes.isEmpty()) {
    QString suffix = fileInfo.suffix().toLower();
    bool accepted  = false;
    for (const QString &s : _pAcceptedSuffixes) {
      if (s.toLower() == suffix) {
        accepted = true;
        break;
      }
    }
    if (!accepted) {
      reason = QStringLiteral("不支持的文件类型: ") + suffix;
      return false;
    }
  }
  if (_pMaxFileSize > 0 && fileInfo.size() > _pMaxFileSize) {
    reason = QStringLiteral("文件过大");
    return false;
  }
  if (_pMaxFileCount > 0 && _filePaths.size() >= _pMaxFileCount) {
    reason = QStringLiteral("已达到最大文件数量");
    return false;
  }
  if (_filePaths.contains(filePath)) {
    reason = QStringLiteral("文件已存在");
    return false;
  }
  return true;
}

void NXUploadAreaPrivate::_addFiles(const QStringList &filePaths)
{
  Q_Q(NXUploadArea);
  QStringList addedFiles;
  for (const QString &filePath : filePaths) {
    QString reason;
    if (_validateFile(filePath, reason)) {
      _filePaths.append(filePath);
      addedFiles.append(filePath);
      Q_EMIT q->fileAdded(filePath);
    } else {
      Q_EMIT q->fileRejected(filePath, reason);
    }
  }
  if (!addedFiles.isEmpty()) {
    Q_EMIT q->filesSelected(_filePaths);
    q->update();
  }
}

void NXUploadAreaPrivate::_removeFile(int index)
{
  Q_Q(NXUploadArea);
  if (index >= 0 && index < _filePaths.size()) {
    QString removedPath = _filePaths.takeAt(index);
    Q_EMIT q->fileRemoved(removedPath);
    Q_EMIT q->filesSelected(_filePaths);
    q->update();
  }
}
