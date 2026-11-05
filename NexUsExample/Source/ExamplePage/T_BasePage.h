#ifndef T_BASEPAGE_H
#define T_BASEPAGE_H

#include <NXScrollPage.h>

class QVBoxLayout;

class T_BasePage : public NXScrollPage
{
  Q_OBJECT

public:
  explicit T_BasePage(QWidget *parent = nullptr);
  ~T_BasePage();

protected:
  void createCustomWidget(QString const &desText);
};

#endif // T_BASEPAGE_H
