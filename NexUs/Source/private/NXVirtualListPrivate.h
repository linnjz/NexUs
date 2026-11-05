#ifndef NXVIRTUALLISTPRIVATE_H
#define NXVIRTUALLISTPRIVATE_H

#include <QObject>

#include "NXDef.h"
class NXVirtualList;
class NXVirtualListStyle;

class NXVirtualListPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXVirtualList)
  Q_PROPERTY_CREATE_D(int, ItemHeight)
  Q_PROPERTY_CREATE_D(bool, IsTransparent)
  Q_PROPERTY_CREATE_D(bool, IsAlternatingRowColors)

public:
  explicit NXVirtualListPrivate(QObject *parent = nullptr);
  ~NXVirtualListPrivate();

private:
  NXThemeType::ThemeMode m_ThemeMode;
  int m_ItemCount { 0 };
  NXVirtualListStyle *m_ListViewStyle { nullptr };
};

#endif // NXVIRTUALLISTPRIVATE_H
