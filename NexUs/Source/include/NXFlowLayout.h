#ifndef NXFLOWLAYOUT_H
#define NXFLOWLAYOUT_H

#include <QLayout>
#include <QMap>
#include <QStyle>

#include "NXProperty.h"
class NXFlowLayoutPrivate;

class NX_EXPORT NXFlowLayout : public QLayout
{
  Q_OBJECT
  Q_Q_CREATE(NXFlowLayout)

public:
  explicit NXFlowLayout(QWidget *parent, int margin = -1, int hSpacing = -1, int vSpacing = -1);
  explicit NXFlowLayout(int margin = -1, int hSpacing = -1, int vSpacing = -1);
  ~NXFlowLayout();

  void addItem(QLayoutItem *item) override;
  int horizontalSpacing() const;
  int verticalSpacing() const;
  Qt::Orientations expandingDirections() const override;
  bool hasHeightForWidth() const override;
  int heightForWidth(int) const override;
  int count() const override;
  QLayoutItem *itemAt(int index) const override;
  QSize minimumSize() const override;
  void setGeometry(QRect const &rect) override;
  QSize sizeHint() const override;
  QLayoutItem *takeAt(int index) override;

  void setIsAnimation(bool isAnimation);
};

#endif // NXFLOWLAYOUT_H
