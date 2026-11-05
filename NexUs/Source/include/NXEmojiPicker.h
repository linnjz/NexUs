#ifndef NXEMOJIPICKER_H
#define NXEMOJIPICKER_H

#include <QWidget>

#include "NXProperty.h"

class NXEmojiPickerPrivate;

class NX_EXPORT NXEmojiPicker : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXEmojiPicker)
  Q_PROPERTY_CREATE_H(int, EmojiSize)
  Q_PROPERTY_CREATE_H(int, Columns)

public:
  explicit NXEmojiPicker(QWidget *parent = nullptr);
  ~NXEmojiPicker();

  void popup(QWidget *anchor);
  void popup(QPoint pos);
  Q_SIGNAL void emojiSelected(QString const &emoji);

protected:
  void paintEvent(QPaintEvent *event) override;
};

#endif // NXEMOJIPICKER_H
