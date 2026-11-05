#ifndef NXCOLORDIALOGPRIVATE_H
#define NXCOLORDIALOGPRIVATE_H

#include <QModelIndex>
#include <QObject>
#include <QPixmap>

#include "NXDef.h"
class NXAppBar;
class NXColorDialog;
class NXColorPicker;
class NXBaseListView;
class NXColorDisplayModel;
class NXColorDisplayDelegate;
class NXColorPreview;
class NXLineEdit;
class NXComboBox;
class NXText;
class NXPushButton;
class NXToolButton;
class QSlider;
class NXColorValueSliderStyle;

class NXColorDialogPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXColorDialog)
  Q_PROPERTY_CREATE_D(QColor, CurrentColor)
  Q_PROPERTY_CREATE_D(NXColorSchemeType::ColorSchemeType, ColorSchemeType)

public:
  explicit NXColorDialogPrivate(QObject *parent = nullptr);
  ~NXColorDialogPrivate();

  Q_SLOT void onColorPickerColorChanged(QColor const &selectedColor);
  Q_SLOT void onColorValueSliderChanged(int value);
  Q_SLOT void onTransparencyValueSliderChanged(int value);

  Q_SLOT void onColorModeChanged(int index);
  Q_SLOT void onHtmlEditFocusOut(QString const &text);
  Q_SLOT void onHtmlEditChanged(QString const &text);
  Q_SLOT void onColorEditChanged(QString const &text);

  Q_SLOT void onBasicColorViewClicked(QModelIndex const &index);
  Q_SLOT void onCustomColorViewClicked(QModelIndex const &index);

  Q_SLOT void onAddCustomColorButtonClicked();
  Q_SLOT void onRemoveCustomColorButtonClicked();

private:
  NXThemeType::ThemeMode m_ThemeMode;
  NXAppBar *m_AppBar { nullptr };
  NXColorPicker *m_ColorPicker { nullptr };
  NXColorPreview *m_ColorPreview { nullptr };
  QSlider *m_ColorValueSlider { nullptr };
  QSlider *m_TransparencyValueSlider { nullptr };
  NXColorValueSliderStyle *m_ColorValueSliderStyle { nullptr };
  NXColorValueSliderStyle *m_TransparencyValueSliderStyle { nullptr };
  NXLineEdit *m_HtmlEdit { nullptr };
  NXComboBox *m_ModeComboBox { nullptr };
  NXLineEdit *m_FirstEdit { nullptr };
  NXLineEdit *m_SecondEdit { nullptr };
  NXLineEdit *m_ThridEdit { nullptr };
  NXText *m_FirstText { nullptr };
  NXText *m_SecondText { nullptr };
  NXText *m_ThridText { nullptr };
  NXText *m_FourthText { nullptr };
  NXLineEdit *m_FourthEdit { nullptr };
  NXBaseListView *m_BasicColorView { nullptr };
  NXColorDisplayModel *m_BasicColorModel { nullptr };
  NXColorDisplayDelegate *m_BasicColorDelegate { nullptr };
  NXToolButton *m_AddCustomColorButton { nullptr };
  NXToolButton *m_RemoveCustomColorButton { nullptr };
  NXBaseListView *m_CustomColorView { nullptr };
  NXColorDisplayModel *m_CustomColorModel { nullptr };
  NXColorDisplayDelegate *m_CustomColorDelegate { nullptr };
  NXPushButton *m_OverButton { nullptr };
  NXPushButton *m_CancelButton { nullptr };


  void _initBasicColor();
  void _initCustomColor();
  void _updateHtmlEditValue();
  void _updateEditValue();
  void _updateColorPreview();
  void _updateColorValueSlider();
  void _updateTransparencyValueSlider();
  QString _completeColorText(QString const &text) const;
  QString _getHex4ChannelValue() const;
  QColor _getColorFromEdit() const;
};

#endif // NXCOLORDIALOGPRIVATE_H
