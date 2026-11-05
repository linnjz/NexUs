#ifndef NXTOOLBAR_H
#define NXTOOLBAR_H

#include <QToolBar>

#include "NXDef.h"
class NXToolBarPrivate;

class NX_EXPORT NXToolBar : public QToolBar
{
  Q_OBJECT
  Q_Q_CREATE(NXToolBar)

public:
  explicit NXToolBar(QWidget *parent = nullptr);
  explicit NXToolBar(QString const &title, QWidget *parent = nullptr);
  ~NXToolBar();

  void setToolBarSpacing(int spacing);
  int getToolBarSpacing() const;

  void setToolButtonSize(QSize size);
  QSize getToolButtonSize() const;

  QAction *addNXIconAction(NXIconType::IconName icon, QString const &text);
  QAction *addNXIconAction(NXIconType::IconName icon, QString const &text, QKeySequence const &shortcut);

protected:
  void paintEvent(QPaintEvent *event) override;
};

#endif // NXTOOLBAR_H
