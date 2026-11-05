#ifndef NXRIBBONGROUP_H
#define NXRIBBONGROUP_H

#include <QWidget>

#include "NXDef.h"
#include "NXProperty.h"

class QHBoxLayout;
class QAction;
class NXMenu;
class NXToolButton;
class NXRibbonGroupPrivate;

class NX_EXPORT NXRibbonGroup : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXRibbonGroup)

public:
  enum ButtonSize
  {
    Large = 0x00'01,
    Small = 0x00'02,
  };

  explicit NXRibbonGroup(QString const &title, QWidget *parent = nullptr);
  ~NXRibbonGroup();

  void setTitle(QString const &title);
  QString getTitle() const;

  NXToolButton *addToolButton(NXIconType::IconName icon, QString const &text, ButtonSize size = Large);
  void addWidget(QWidget *widget);

  void removeButton(NXToolButton *button);
  QList<NXToolButton *> getButtons() const;

  Q_SIGNAL void ribbonButtonTriggered(NXToolButton *button);

protected:
  void paintEvent(QPaintEvent *event) override;
  QSize sizeHint() const override;
};

#endif // NXRIBBONGROUP_H
