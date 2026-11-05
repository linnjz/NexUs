#ifndef NXCOLORDISPLAYMODEL_H
#define NXCOLORDISPLAYMODEL_H

#include <QAbstractListModel>
#include <QColor>

class NXColorDisplayModel : public QAbstractListModel
{
  Q_OBJECT

public:
  explicit NXColorDisplayModel(QObject *parent = nullptr);
  ~NXColorDisplayModel();
  void appendDisplayColor(QList<QColor> const &colorList);
  void appendDisplayColor(QColor const &color);
  void removeDisplayColor(int index);
  void replaceDisplayColor(QColor const &color, int index);

  QList<QColor> getDisplayColorList() const;
  QColor getDisplayColor(int index) const;

protected:
  int rowCount(QModelIndex const &parent = QModelIndex()) const override;
  QVariant data(QModelIndex const &index, int role) const override;

private:
  QList<QColor> m_DisplayColorList;
};

#endif // NXCOLORDISPLAYMODEL_H
