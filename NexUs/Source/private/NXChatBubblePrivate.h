#ifndef NXCHATBUBBLEPRIVATE_H
#define NXCHATBUBBLEPRIVATE_H

#include <QObject>
#include <QPixmap>
#include <QRect>

#include "NXDef.h"
#include "NXProperty.h"

class NXChatBubble;

class NXChatBubblePrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXChatBubble)
  Q_PROPERTY_CREATE_D(QString, MessageText)
  Q_PROPERTY_CREATE_D(QString, SenderName)
  Q_PROPERTY_CREATE_D(QString, Timestamp)
  Q_PROPERTY_CREATE_D(int, BorderRadius)
  Q_PROPERTY_CREATE_D(int, AvatarSize)
  Q_PROPERTY_CREATE_D(int, MaxBubbleWidth)

public:
  explicit NXChatBubblePrivate(QObject *parent = nullptr);
  ~NXChatBubblePrivate();

private:
  NXThemeType::ThemeMode m_ThemeMode;
  NXChatBubble::BubbleDirection m_Direction { NXChatBubble::Left };
  NXChatBubble::MessageStatus m_Status { NXChatBubble::None };
  int m_ImageMaxWidth { 280 };
  QRect m_ImageRect;
  QColor m_BubbleColor;
  QPixmap m_Avatar;
  QPixmap m_MessageImage;
  QPixmap m_ScaledImage;
};

#endif // NXCHATBUBBLEPRIVATE_H
