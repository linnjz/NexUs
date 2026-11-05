#ifndef NXSUGGESTBOXSEARCHVIEWCONTAINER_H
#define NXSUGGESTBOXSEARCHVIEWCONTAINER_H

#include <QWidget>

#include "NXDef.h"

class NXSuggestBoxSearchViewContainer : public QWidget
{
  Q_OBJECT

public:
  explicit NXSuggestBoxSearchViewContainer(QWidget *parent = nullptr);
  ~NXSuggestBoxSearchViewContainer();

protected:
  void paintEvent(QPaintEvent *event) override;

private:
  NXThemeType::ThemeMode m_ThemeMode;
};

#endif // NXSUGGESTBOXSEARCHVIEWCONTAINER_H
