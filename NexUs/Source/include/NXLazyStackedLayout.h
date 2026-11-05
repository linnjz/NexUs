#ifndef NXLAZYSTACKEDLAYOUT_H
#define NXLAZYSTACKEDLAYOUT_H

#include <functional>

#include <QLayout>

#include "NXProperty.h"

class NXLazyStackedLayoutPrivate;

class NX_EXPORT NXLazyStackedLayout : public QLayout
{
  Q_OBJECT
  Q_Q_CREATE(NXLazyStackedLayout)
  Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentChanged)
  Q_PROPERTY(StackingMode stackingMode READ stackingMode WRITE setStackingMode)
  Q_PROPERTY(int count READ count)

public:
  enum StackingMode
  {
    StackOne,
    StackAll
  };
  Q_ENUM(StackingMode)

  explicit NXLazyStackedLayout();
  explicit NXLazyStackedLayout(QWidget *parent);
  ~NXLazyStackedLayout() override;

  int addWidget(QWidget *widget);
  int insertWidget(int index, QWidget *widget);

  int addLazyWidget(std::function<QWidget *()> &&lazyFactory);
  int insertLazyWidget(int index, std::function<QWidget *()> &&lazyFactory);

  QWidget *ensureWidget(int index);
  bool isWidgetCreated(int index) const;

  QWidget *currentWidget() const;
  int currentIndex() const;
  using QLayout::widget;
  QWidget *widget(int index) const;
  int count() const override;
  int indexOf(QWidget const *widget) const override;

  StackingMode stackingMode() const;
  void setStackingMode(StackingMode stackingMode);

  void addItem(QLayoutItem *item) override;
  QSize sizeHint() const override;
  QSize minimumSize() const override;
  QLayoutItem *itemAt(int index) const override;
  QLayoutItem *takeAt(int index) override;
  void setGeometry(QRect const &rect) override;
  bool hasHeightForWidth() const override;
  int heightForWidth(int width) const override;

  Q_SIGNAL void widgetRemoved(int index);
  Q_SIGNAL void currentChanged(int index);
  Q_SIGNAL void widgetCreated(int index, QWidget *widget);

public Q_SLOTS:
  void setCurrentIndex(int index);
  void setCurrentWidget(QWidget *widget);
};

#endif // NXLAZYSTACKEDLAYOUT_H
