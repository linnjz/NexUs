#ifndef NXCOLORDIALOG_H
#define NXCOLORDIALOG_H

#include <QDialog>

#include "NXAppBar.h"
#include "NXProperty.h"
class NXColorDialogPrivate;

class NX_EXPORT NXColorDialog : public QDialog
{
  Q_OBJECT
  Q_Q_CREATE(NXColorDialog)
  Q_PROPERTY_CREATE_H(QPS_SCREF(QColor), CurrentColor)
  Q_PROPERTY_CREATE_H(NXColorSchemeType::ColorSchemeType, ColorSchemeType)
  Q_TAKEOVER_NATIVEEVENT_H

public:
  explicit NXColorDialog(QWidget *parent = nullptr);
  explicit NXColorDialog(QColor const &currentColor, QWidget *parent = nullptr);
  ~NXColorDialog();

  QList<QColor> getCustomColorList() const;
  QColor getCustomColor(int index) const;
  QString getCurrent4ChannelColor() const;
  Q_SIGNAL void colorSelected(QColor const &color);

protected:
  void paintEvent(QPaintEvent *event) override;
};

#endif // NXCOLORDIALOG_H
