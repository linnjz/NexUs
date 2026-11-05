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
  m_ThemeMode = themeMode;
  q->update();
}

void NXUploadAreaPrivate::_openFileDialog()
{
  Q_Q(NXUploadArea);
  QString dialogTitle = m_DialogTitle.isEmpty() ? QStringLiteral("选择文件") : m_DialogTitle;
  QString filter;
  if (!m_AcceptedSuffixes.isEmpty()) {
    QStringList patterns;
    for (QString const &suffix : m_AcceptedSuffixes) {
      patterns.append(QStringLiteral("*.") + suffix);
    }
    filter = QString(QStringLiteral("允许的文件")) + QStringLiteral(" (") + patterns.join(QStringLiteral(" ")) +
             QStringLiteral(")");
  } else if (!m_MimeFilter.isEmpty()) {
    filter = m_MimeFilter;
  }

  QStringList files;
  if (m_IsMultiple) {
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

bool NXUploadAreaPrivate::_validateFile(QString const &filePath, QString &reason) const
{
  QFileInfo fileInfo(filePath);
  if (!fileInfo.exists()) {
    reason = QStringLiteral("文件不存在");
    return false;
  }
  if (!m_AcceptedSuffixes.isEmpty()) {
    QString suffix = fileInfo.suffix().toLower();
    bool accepted  = false;
    for (QString const &s : m_AcceptedSuffixes) {
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
  if (m_MaxFileSize > 0 && fileInfo.size() > m_MaxFileSize) {
    reason = QStringLiteral("文件过大");
    return false;
  }
  if (m_MaxFileCount > 0 && m_FilePaths.size() >= m_MaxFileCount) {
    reason = QStringLiteral("已达到最大文件数量");
    return false;
  }
  if (m_FilePaths.contains(filePath)) {
    reason = QStringLiteral("文件已存在");
    return false;
  }
  return true;
}

void NXUploadAreaPrivate::_addFiles(QStringList const &filePaths)
{
  Q_Q(NXUploadArea);
  QStringList addedFiles;
  for (QString const &filePath : filePaths) {
    QString reason;
    if (_validateFile(filePath, reason)) {
      m_FilePaths.append(filePath);
      addedFiles.append(filePath);
      Q_EMIT q->fileAdded(filePath);
    } else {
      Q_EMIT q->fileRejected(filePath, reason);
    }
  }
  if (!addedFiles.isEmpty()) {
    Q_EMIT q->filesSelected(m_FilePaths);
    q->update();
  }
}

void NXUploadAreaPrivate::_removeFile(int index)
{
  Q_Q(NXUploadArea);
  if (index >= 0 && index < m_FilePaths.size()) {
    QString removedPath = m_FilePaths.takeAt(index);
    Q_EMIT q->fileRemoved(removedPath);
    Q_EMIT q->filesSelected(m_FilePaths);
    q->update();
  }
}
