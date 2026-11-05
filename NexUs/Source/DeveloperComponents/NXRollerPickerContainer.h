#ifndef NXFRAMEWORK_NXROLLERPICKERCONTAINER_H
#define NXFRAMEWORK_NXROLLERPICKERCONTAINER_H

#include <QPropertyAnimation>
#include <QWidget>

#include "NXDef.h"
class NXRoller;

class NXRollerPickerContainer : public QWidget
{
  Q_OBJECT
  Q_PROPERTY_CREATE(QPS_PAEMIT(int), AnimationPixOffsetY)
  Q_PRIVATE_CREATE(int, ButtonAreaHeight)

public:
  explicit NXRollerPickerContainer(QWidget *parent = nullptr);
  ~NXRollerPickerContainer();

  void doPickerAnimation();
  Q_SIGNAL void overButtonClicked();
  Q_SIGNAL void cancelButtonClicked();

protected:
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void leaveEvent(QEvent *event) override;
  void hideEvent(QHideEvent *event) override;
  void paintEvent(QPaintEvent *event) override;

private:
  friend class NXRollerPicker;
  friend class NXRollerPickerPrivate;
  bool m_IsOverButtonHover { false };
  bool m_IsCancelButtonHover { false };
  bool m_IsOverButtonClicked { false };
  int m_ButtonMargin { 5 };
  int m_ButtonSpacing { 5 };
  NXThemeType::ThemeMode m_ThemeMode;
  QRect m_OverButtonRect;
  QRect m_CancelButtonRect;
  QPixmap m_AnimationPix;
  QList<NXRoller *> m_RollerList;
  QList<int> m_HistoryIndexList;
  void _handleSaveOrReset(bool isSave);
};

#endif //NXFRAMEWORK_NXROLLERPICKERCONTAINER_H
