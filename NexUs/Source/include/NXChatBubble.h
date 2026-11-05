#ifndef NXCHATBUBBLE_H
#define NXCHATBUBBLE_H

#include <QWidget>

#include "NXDef.h"
#include "NXProperty.h"

class NXChatBubblePrivate;

class NX_EXPORT NXChatBubble : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXChatBubble)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), MessageText)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), SenderName)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), Timestamp)
  Q_PROPERTY_CREATE_H(int, BorderRadius)
  Q_PROPERTY_CREATE_H(int, AvatarSize)
  Q_PROPERTY_CREATE_H(int, MaxBubbleWidth)

public:
  enum BubbleDirection
  {
    Left = 0,
    Right
  };
  Q_ENUM(BubbleDirection)

  enum MessageStatus
  {
    None = 0,
    Sending,
    Sent,
    Read,
    Failed
  };
  Q_ENUM(MessageStatus)

  explicit NXChatBubble(QWidget *parent = nullptr);
  ~NXChatBubble();

  void setDirection(BubbleDirection direction);
  BubbleDirection getDirection() const;

  void setStatus(MessageStatus status);
  MessageStatus getStatus() const;

  void setAvatar(QPixmap const &avatar);
  QPixmap getAvatar() const;

  void setBubbleColor(QColor const &color);
  QColor getBubbleColor() const;

  void setMessageImage(QPixmap const &image);
  QPixmap getMessageImage() const;

  void setImageMaxWidth(int width);
  int getImageMaxWidth() const;

  QSize sizeHint() const override;
  Q_SIGNAL void imageDoubleClicked(QPixmap const &image);

protected:
  void paintEvent(QPaintEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;
};

#endif // NXCHATBUBBLE_H
