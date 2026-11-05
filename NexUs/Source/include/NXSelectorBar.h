#ifndef NXSELECTORBAR_H
#define NXSELECTORBAR_H

#include <QWidget>

#include "NXDef.h"
#include "NXProperty.h"

class NXSelectorBarPrivate;

class NX_EXPORT NXSelectorBar : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXSelectorBar)
  Q_PROPERTY_CREATE_H(int, CurrentIndex)
  Q_PROPERTY_CREATE_H(int, BorderRadius)

public:
  explicit NXSelectorBar(QWidget *parent = nullptr);
  ~NXSelectorBar();

  void addItem(QString const &text);
  void addItem(NXIconType::IconName icon, QString const &text);
  void clearItems();
  int getItemCount() const;

  QSize sizeHint() const override;
  Q_SIGNAL void currentIndexChanged(int index);

protected:
  void paintEvent(QPaintEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void leaveEvent(QEvent *event) override;
};

#endif // NXSELECTORBAR_H
