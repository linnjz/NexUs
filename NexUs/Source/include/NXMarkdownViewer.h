#ifndef NXMARKDOWNVIEWER_H
#define NXMARKDOWNVIEWER_H

#include <QWidget>

#include "NXProperty.h"

class NXMarkdownViewerPrivate;

class NX_EXPORT NXMarkdownViewer : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXMarkdownViewer)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), Markdown)
  Q_PROPERTY_CREATE_H(int, BorderRadius)

public:
  explicit NXMarkdownViewer(QWidget *parent = nullptr);
  ~NXMarkdownViewer();
};

#endif // NXMARKDOWNVIEWER_H
