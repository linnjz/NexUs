#ifndef NXTAG_H
#define NXTAG_H

#include <QWidget>

#include "NXDef.h"
#include "NXProperty.h"
class NXTagPrivate;

class NX_EXPORT NXTag : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXTag)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QString), TagText)
  Q_PROPERTY_CREATE_H(int, BorderRadius)
  Q_PROPERTY_CREATE_H(bool, IsClosable)
  Q_PROPERTY_CREATE_H(bool, IsCheckable)
  Q_PROPERTY_CREATE_H(bool, IsChecked)

public:
  enum TagColor
  {
    Default = 0,
    Primary,
    Success,
    Warning,
    Danger
  };
  Q_ENUM(TagColor)

  explicit NXTag(QWidget *parent = nullptr);
  explicit NXTag(QString const &text, QWidget *parent = nullptr);
  ~NXTag();

  void setTagColor(TagColor color);
  TagColor getTagColor() const;
  Q_SIGNAL void closed();
  Q_SIGNAL void clicked();
  Q_SIGNAL void checkedChanged(bool checked);

protected:
  void mousePressEvent(QMouseEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
  QSize sizeHint() const override;
};

#endif // NXTAG_H
