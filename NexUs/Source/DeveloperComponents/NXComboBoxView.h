#ifndef NXCOMBOBOXVIEW_H
#define NXCOMBOBOXVIEW_H

#include <QListView>

class NXComboBoxView : public QListView
{
  Q_OBJECT

public:
  explicit NXComboBoxView(QWidget *parent = nullptr);
  ~NXComboBoxView();
  Q_SIGNAL void itemPressed(QModelIndex const &index);

protected:
  void mousePressEvent(QMouseEvent *event) override;
};

#endif // NXCOMBOBOXVIEW_H
