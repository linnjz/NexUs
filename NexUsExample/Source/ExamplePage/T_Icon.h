#ifndef T_ICON_H
#define T_ICON_H

#include <QMetaEnum>

#include "T_BasePage.h"
class NXLineEdit;
class NXListView;
class T_IconModel;
class T_IconDelegate;

class T_Icon : public T_BasePage
{
  Q_OBJECT

public:
  Q_INVOKABLE explicit T_Icon(QWidget *parent = nullptr);
  ~T_Icon();
  Q_SLOT void onSearchEditTextEdit(QString const &searchText);

private:
  QMetaEnum _metaEnum;
  NXLineEdit *_searchEdit { nullptr };
  NXListView *_iconView;
  T_IconModel *_iconModel;
  T_IconDelegate *_iconDelegate;
};

#endif // T_ICON_H
