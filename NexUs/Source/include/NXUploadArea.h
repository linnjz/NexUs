#ifndef NXUPLOADAREA_H
#define NXUPLOADAREA_H

#include <QWidget>

#include "NXDef.h"
#include "NXProperty.h"

class NXUploadAreaPrivate;

class NX_EXPORT NXUploadArea : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXUploadArea)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), Title)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), SubTitle)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), DialogTitle)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QStringList), AcceptedSuffixes)
  Q_PROPERTY_CREATE_H(qint64, MaxFileSize)
  Q_PROPERTY_CREATE_H(int, BorderRadius)
  Q_PROPERTY_CREATE_H(int, MaxFileCount)
  Q_PROPERTY_CREATE_H(bool, IsMultiple)

public:
  explicit NXUploadArea(QWidget *parent = nullptr);
  ~NXUploadArea();

  QStringList getSelectedFiles() const;
  void clearFiles();

  void setAcceptedMimeFilter(QString const &filter);
  QString getAcceptedMimeFilter() const;
  Q_SIGNAL void filesSelected(QStringList const &filePaths);
  Q_SIGNAL void fileAdded(QString const &filePath);
  Q_SIGNAL void fileRemoved(QString const &filePath);
  Q_SIGNAL void fileRejected(QString const &filePath, QString const &reason);

protected:
  void paintEvent(QPaintEvent *event) override;
  void dragEnterEvent(QDragEnterEvent *event) override;
  void dragMoveEvent(QDragMoveEvent *event) override;
  void dragLeaveEvent(QDragLeaveEvent *event) override;
  void dropEvent(QDropEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
  void enterEvent(QEnterEvent *event) override;
#else
  void enterEvent(QEvent *event) override;
#endif
  void leaveEvent(QEvent *event) override;
};

#endif // NXUPLOADAREA_H
