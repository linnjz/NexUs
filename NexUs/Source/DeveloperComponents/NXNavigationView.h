#ifndef NXNAVIGATIONVIEW_H
#define NXNAVIGATIONVIEW_H

#include <QObject>
#include <QTreeView>

#include "NXDef.h"
#include "private/NXNavigationBarPrivate.h"
class NXScrollBar;
class NXNavigationStyle;
class NXToolTip;

class NXNavigationView : public QTreeView
{
  Q_OBJECT
  Q_PRIVATE_CREATE(NXNavigationBarPrivate const *, NavigationBarPrivate)

public:
  explicit NXNavigationView(QWidget *parent = nullptr);
  ~NXNavigationView();
  NXToolTip *getCompactToolTip() const;
  void setIsDragDropEnable(bool enable);
  void navigationNodeStateChange(QVariantMap const &data);

  Q_SLOT void onCustomContextMenuRequested(QPoint pos);
  Q_SIGNAL void navigationClicked(QModelIndex const &index);
  Q_SIGNAL void navigationOpenNewWindow(QString const &nodeKey);
  Q_SIGNAL void navigationCloseCurrentWindow(QString const &nodeKey);

protected:
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  bool eventFilter(QObject *watched, QEvent *event) override;

private:
  NXNavigationStyle *m_NavigationStyle { nullptr };
  NXToolTip *m_CompactToolTip { nullptr };
  void _doCompactToolTip();
};

#endif // NXNAVIGATIONVIEW_H
