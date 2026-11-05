#ifndef NXEMOJIPICKERPRIVATE_H
#define NXEMOJIPICKERPRIVATE_H

#include <QObject>
#include <QStringList>
#include <QVector>

#include "NXDef.h"

class NXEmojiPicker;
class QGridLayout;
class NXLineEdit;

class NXEmojiPickerPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXEmojiPicker)
  Q_PROPERTY_CREATE_D(int, EmojiSize)
  Q_PROPERTY_CREATE_D(int, Columns)

public:
  struct EmojiCategory
  {
    QString name;
    QString icon;
    QStringList emojis;
  };

  int m_CurrentCategory { 0 };
  NXThemeType::ThemeMode m_ThemeMode;
  QVector<EmojiCategory> m_Categories;
  QStringList m_RecentEmojis;

  QGridLayout *m_GridLayout { nullptr };
  NXLineEdit *m_SearchEdit { nullptr };
  QWidget *m_GridContainer { nullptr };
  explicit NXEmojiPickerPrivate(QObject *parent = nullptr);
  ~NXEmojiPickerPrivate();


  void initEmojis();
  void buildGrid(int categoryIndex);
  void filterEmojis(QString const &text);
};

#endif // NXEMOJIPICKERPRIVATE_H
