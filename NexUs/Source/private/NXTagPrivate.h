#ifndef NXTAGPRIVATE_H
#define NXTAGPRIVATE_H

#include <QObject>

#include "NXDef.h"
#include "NXTag.h"

class NXTagPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXTag)
  Q_PROPERTY_CREATE_D(QString, TagText)
  Q_PROPERTY_CREATE_D(int, BorderRadius)
  Q_PROPERTY_CREATE_D(bool, IsClosable)
  Q_PROPERTY_CREATE_D(bool, IsCheckable)
  Q_PROPERTY_CREATE_D(bool, IsChecked)

public:
  explicit NXTagPrivate(QObject *parent = nullptr);
  ~NXTagPrivate();

private:
  NXTag::TagColor m_TagColor { NXTag::Default };
  NXThemeType::ThemeMode m_ThemeMode;
  int m_CloseButtonWidth { 16 };
  int m_CheckIconWidth { 16 };
  int m_Padding { 8 };
  QColor _getBackgroundColor() const;
  QColor _getForegroundColor() const;
};

#endif // NXTAGPRIVATE_H
