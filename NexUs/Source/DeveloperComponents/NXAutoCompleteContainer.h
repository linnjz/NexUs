#ifndef NXAUTOCOMPLETECONTAINER_H
#define NXAUTOCOMPLETECONTAINER_H

#include <QWidget>

#include "NXDef.h"

class NXLineEdit;

class NXAutoCompleteContainer : public QWidget
{
  Q_OBJECT

public:
  explicit NXAutoCompleteContainer(QWidget *parent = nullptr);
  ~NXAutoCompleteContainer();

  void setForwardTarget(NXLineEdit *target);
  Q_SIGNAL void popupClosed();

protected:
  void paintEvent(QPaintEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;
  void hideEvent(QHideEvent *event) override;

private:
  NXThemeType::ThemeMode m_ThemeMode;
  NXLineEdit *m_ForwardTarget { nullptr };
};

#endif // NXAUTOCOMPLETECONTAINER_H
