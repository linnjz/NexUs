#include "NXColorDialog.h"

#include <utility>

#include <QHBoxLayout>
#include <QPainter>
#include <QVBoxLayout>

#include "DeveloperComponents/NXBaseListView.h"
#include "DeveloperComponents/NXColorDisplayDelegate.h"
#include "DeveloperComponents/NXColorDisplayModel.h"
#include "DeveloperComponents/NXColorPicker.h"
#include "DeveloperComponents/NXColorPreview.h"
#include "DeveloperComponents/NXColorValueSliderStyle.h"
#include "DeveloperComponents/NXIntValidator.h"
#include "NXComboBox.h"
#include "NXLineEdit.h"
#include "NXPushButton.h"
#include "NXText.h"
#include "NXTheme.h"
#include "NXToolButton.h"
#include "private/NXColorDialogPrivate.h"
Q_TAKEOVER_NATIVEEVENT_CPP(NXColorDialog, d_func()->m_AppBar);

NXColorDialog::NXColorDialog(QWidget *parent)
    : QDialog { parent }
    , d_ptr(new NXColorDialogPrivate())
{
  Q_D(NXColorDialog);
  d->q_ptr = this;
  setFixedSize(600, 600); // 默认宽高
  setObjectName("NXColorDialog");
  setWindowTitle(QStringLiteral("NXColorDialog"));
  setWindowModality(Qt::ApplicationModal);

  d->m_ColorSchemeType = NXColorSchemeType::Argb;
  // 自定义AppBar
  d->m_AppBar = new NXAppBar(this);
  d->m_AppBar->setAppBarHeight(30);
  d->m_AppBar->setIsFixedSize(true);
  d->m_AppBar->setIsStayTop(true);
  d->m_AppBar->setWindowButtonFlags(NXAppBarType::CloseButtonHint);

  // 颜色选择器
  NXText *colorPickerText = new NXText(QStringLiteral("编辑颜色"), this);
  colorPickerText->setTextPixelSize(17);
  QHBoxLayout *colorPickerTextLayout = new QHBoxLayout();
  colorPickerTextLayout->setContentsMargins(0, 0, 0, 0);
  colorPickerTextLayout->addSpacing(3);
  colorPickerTextLayout->addWidget(colorPickerText);

  d->m_ColorPicker = new NXColorPicker(this);
  connect(d->m_ColorPicker, &NXColorPicker::selectedColorChanged, d, &NXColorDialogPrivate::onColorPickerColorChanged);

  QVBoxLayout *colorPickerLayout = new QVBoxLayout();
  colorPickerLayout->setContentsMargins(0, 0, 0, 0);
  colorPickerLayout->addWidget(d->m_ColorPicker);
  colorPickerLayout->addStretch();

  // 颜色预览
  d->m_ColorPreview                = new NXColorPreview(this);
  QVBoxLayout *colorPreviewLayout = new QVBoxLayout();
  colorPreviewLayout->setContentsMargins(0, 0, 0, 0);
  colorPreviewLayout->addWidget(d->m_ColorPreview);
  colorPreviewLayout->addStretch();

  // 颜色value控制
  d->m_ColorValueSlider = new QSlider(this);
  d->m_ColorValueSlider->setFixedHeight(257);
  d->m_ColorValueSlider->setOrientation(Qt::Vertical);
  d->m_ColorValueSlider->setRange(0, 255);
  d->m_ColorValueSlider->setValue(255);
  d->m_ColorValueSliderStyle = new NXColorValueSliderStyle();
  d->m_ColorValueSlider->setStyle(d->m_ColorValueSliderStyle);
  connect(d->m_ColorValueSlider, &QSlider::valueChanged, d, &NXColorDialogPrivate::onColorValueSliderChanged);
  QVBoxLayout *colorValueSliderLayout = new QVBoxLayout();
  colorValueSliderLayout->setContentsMargins(0, 0, 0, 0);
  colorValueSliderLayout->addWidget(d->m_ColorValueSlider);
  colorValueSliderLayout->addStretch();

  d->m_TransparencyValueSlider = new QSlider(this);
  d->m_TransparencyValueSlider->setFixedHeight(257);
  d->m_TransparencyValueSlider->setOrientation(Qt::Vertical);
  d->m_TransparencyValueSlider->setRange(0, 255);
  d->m_TransparencyValueSlider->setValue(255);
  d->m_TransparencyValueSliderStyle = new NXColorValueSliderStyle(style());
  d->m_TransparencyValueSliderStyle->setIsUseAlpha(true);
  d->m_TransparencyValueSlider->setStyle(d->m_TransparencyValueSliderStyle);
  connect(d->m_TransparencyValueSlider, &QSlider::valueChanged, d, &NXColorDialogPrivate::onTransparencyValueSliderChanged);
  QVBoxLayout *transparencyValueSliderLayout = new QVBoxLayout();
  transparencyValueSliderLayout->setContentsMargins(0, 0, 0, 0);
  transparencyValueSliderLayout->addWidget(d->m_TransparencyValueSlider);
  transparencyValueSliderLayout->addStretch();

  // 颜色控制器
  d->m_HtmlEdit = new NXLineEdit(this);
  d->m_HtmlEdit->setText(QStringLiteral("#FFFFFFFF"));
  NXIntValidator *htmlValidator = new NXIntValidator(0x00'00'00'00, 0xFF'FF'FF'FF, this);
  htmlValidator->setIsHexMode(true);
  d->m_HtmlEdit->setValidator(htmlValidator);
  d->m_HtmlEdit->setFixedSize(120, 33);
  connect(d->m_HtmlEdit, &NXLineEdit::focusOut, d, &NXColorDialogPrivate::onHtmlEditFocusOut);
  connect(d->m_HtmlEdit, &NXLineEdit::textEdited, d, &NXColorDialogPrivate::onHtmlEditChanged);

  // 数值预览
  QHBoxLayout *htmlEditLayout = new QHBoxLayout();
  htmlEditLayout->setContentsMargins(0, 0, 0, 0);
  htmlEditLayout->addSpacing(6);
  htmlEditLayout->addWidget(d->m_HtmlEdit);
  htmlEditLayout->addStretch();
  d->m_ModeComboBox = new NXComboBox(this);
  d->m_ModeComboBox->setFixedSize(120, 33);
  d->m_ModeComboBox->addItem(QStringLiteral("ARGB"));
  d->m_ModeComboBox->addItem(QStringLiteral("AHSV"));
  d->m_ModeComboBox->setCurrentIndex(0);
  connect(d->m_ModeComboBox, QOverload<int>::of(&NXComboBox::currentIndexChanged), d, &NXColorDialogPrivate::onColorModeChanged);
  connect(this, &NXColorDialog::signalColorSchemeTypeChanged, d->m_ModeComboBox, [d]() {
    d->m_ModeComboBox->setItemText(0, QStringLiteral("RGBA"));
    d->m_ModeComboBox->setItemText(1, QStringLiteral("HSVA"));
  });
  QHBoxLayout *modeLayout = new QHBoxLayout();
  modeLayout->setContentsMargins(0, 0, 0, 0);
  modeLayout->addSpacing(6);
  modeLayout->addWidget(d->m_ModeComboBox);
  modeLayout->addStretch();

  // R或H
  d->m_FirstEdit = new NXLineEdit(this);
  d->m_FirstEdit->setText(QStringLiteral("0"));
  d->m_FirstEdit->setFixedSize(120, 33);
  d->m_FirstEdit->setValidator(new NXIntValidator(0, 255, this));
  d->m_FirstText = new NXText(QStringLiteral("红色"), this);
  d->m_FirstText->setTextPixelSize(13);
  connect(d->m_FirstEdit, &NXLineEdit::textEdited, d, &NXColorDialogPrivate::onColorEditChanged);

  QHBoxLayout *firstEditLayout = new QHBoxLayout();
  firstEditLayout->setContentsMargins(0, 0, 0, 0);
  firstEditLayout->addSpacing(6);
  firstEditLayout->addWidget(d->m_FirstEdit);
  firstEditLayout->addWidget(d->m_FirstText);
  firstEditLayout->addStretch();

  // G或S
  d->m_SecondEdit = new NXLineEdit(this);
  d->m_SecondEdit->setText(QStringLiteral("0"));
  d->m_SecondEdit->setFixedSize(120, 33);
  d->m_SecondEdit->setValidator(new NXIntValidator(0, 255, this));
  d->m_SecondText = new NXText(QStringLiteral("绿色"), this);
  d->m_SecondText->setTextPixelSize(13);
  connect(d->m_SecondEdit, &NXLineEdit::textEdited, d, &NXColorDialogPrivate::onColorEditChanged);
  QHBoxLayout *secondEditLayout = new QHBoxLayout();
  secondEditLayout->setContentsMargins(0, 0, 0, 0);
  secondEditLayout->addSpacing(6);
  secondEditLayout->addWidget(d->m_SecondEdit);
  secondEditLayout->addWidget(d->m_SecondText);
  secondEditLayout->addStretch();

  // B或V
  d->m_ThridEdit = new NXLineEdit(this);
  d->m_ThridEdit->setText(QStringLiteral("0"));
  d->m_ThridEdit->setFixedSize(120, 33);
  d->m_ThridEdit->setValidator(new NXIntValidator(0, 255, this));
  d->m_ThridText = new NXText(QStringLiteral("蓝色"), this);
  d->m_ThridText->setTextPixelSize(13);
  connect(d->m_ThridEdit, &NXLineEdit::textEdited, d, &NXColorDialogPrivate::onColorEditChanged);
  QHBoxLayout *thridEditLayout = new QHBoxLayout();
  thridEditLayout->setContentsMargins(0, 0, 0, 0);
  thridEditLayout->addSpacing(6);
  thridEditLayout->addWidget(d->m_ThridEdit);
  thridEditLayout->addWidget(d->m_ThridText);
  thridEditLayout->addStretch();

  // Alpha
  d->m_FourthEdit = new NXLineEdit(this);
  d->m_FourthEdit->setText(QStringLiteral("0"));
  d->m_FourthEdit->setFixedSize(120, 33);
  d->m_FourthEdit->setValidator(new NXIntValidator(0, 255, this));
  d->m_FourthText = new NXText(QStringLiteral("透明度"), this);
  d->m_FourthText->setTextPixelSize(13);
  connect(d->m_FourthEdit, &NXLineEdit::textEdited, d, &NXColorDialogPrivate::onColorEditChanged);
  QHBoxLayout *fourthEditLayout = new QHBoxLayout();
  fourthEditLayout->setContentsMargins(0, 0, 0, 0);
  fourthEditLayout->addSpacing(6);
  fourthEditLayout->addWidget(d->m_FourthEdit);
  fourthEditLayout->addWidget(d->m_FourthText);
  fourthEditLayout->addStretch();

  QVBoxLayout *paramControlLayout = new QVBoxLayout();
  paramControlLayout->setContentsMargins(0, 0, 0, 0);
  paramControlLayout->setSpacing(10);
  paramControlLayout->addLayout(htmlEditLayout);
  paramControlLayout->addLayout(modeLayout);
  paramControlLayout->addLayout(fourthEditLayout);
  paramControlLayout->addLayout(firstEditLayout);
  paramControlLayout->addLayout(secondEditLayout);
  paramControlLayout->addLayout(thridEditLayout);
  paramControlLayout->addStretch();

  QHBoxLayout *colorControlLayout = new QHBoxLayout();
  colorControlLayout->setSpacing(0);
  colorControlLayout->setContentsMargins(0, 0, 0, 0);
  colorControlLayout->addLayout(colorPickerLayout);
  colorControlLayout->addSpacing(5);
  colorControlLayout->addLayout(colorPreviewLayout);
  colorControlLayout->addSpacing(20);
  colorControlLayout->addLayout(colorValueSliderLayout);
  colorControlLayout->addSpacing(10);
  colorControlLayout->addLayout(transparencyValueSliderLayout);
  colorControlLayout->addSpacing(10);
  colorControlLayout->addLayout(paramControlLayout);
  colorControlLayout->addStretch();

  // 基本颜色
  d->m_BasicColorView = new NXBaseListView(this);
  d->m_BasicColorView->setFixedSize(380, 170);
  d->m_BasicColorView->setSelectionMode(QListView::NoSelection);
  d->m_BasicColorView->setFlow(QListView::LeftToRight);
  d->m_BasicColorView->setViewMode(QListView::IconMode);
  d->m_BasicColorView->setResizeMode(QListView::Adjust);
  d->m_BasicColorModel = new NXColorDisplayModel(this);
  d->m_BasicColorView->setModel(d->m_BasicColorModel);
  d->m_BasicColorDelegate = new NXColorDisplayDelegate(this);
  d->m_BasicColorView->setItemDelegate(d->m_BasicColorDelegate);
  d->_initBasicColor();
  connect(d->m_BasicColorView, &NXBaseListView::clicked, d, &NXColorDialogPrivate::onBasicColorViewClicked);

  NXText *basicColorViewText = new NXText(QStringLiteral("基本颜色"), this);
  basicColorViewText->setTextPixelSize(14);
  QVBoxLayout *basicColorLayout = new QVBoxLayout();
  basicColorLayout->setContentsMargins(3, 0, 0, 0);
  basicColorLayout->addWidget(basicColorViewText);
  basicColorLayout->addSpacing(15);
  basicColorLayout->addWidget(d->m_BasicColorView);
  basicColorLayout->addStretch();

  // 自定义颜色
  d->m_CustomColorView = new NXBaseListView(this);
  d->m_CustomColorView->setFixedSize(200, 170);
  d->m_CustomColorView->setSelectionMode(QListView::SingleSelection);
  d->m_CustomColorView->setFlow(QListView::LeftToRight);
  d->m_CustomColorView->setViewMode(QListView::IconMode);
  d->m_CustomColorView->setResizeMode(QListView::Adjust);
  d->m_CustomColorModel = new NXColorDisplayModel(this);
  d->m_CustomColorView->setModel(d->m_CustomColorModel);
  d->m_CustomColorDelegate = new NXColorDisplayDelegate(this);
  d->m_CustomColorView->setItemDelegate(d->m_CustomColorDelegate);
  d->_initCustomColor();
  connect(d->m_CustomColorView, &NXBaseListView::clicked, d, &NXColorDialogPrivate::onCustomColorViewClicked);

  // 增加移除按钮
  d->m_AddCustomColorButton = new NXToolButton(this);
  d->m_AddCustomColorButton->setIsTransparent(false);
  d->m_AddCustomColorButton->setNXIcon(NXIconType::Plus);
  connect(d->m_AddCustomColorButton, &NXToolButton::clicked, d, &NXColorDialogPrivate::onAddCustomColorButtonClicked);

  d->m_RemoveCustomColorButton = new NXToolButton(this);
  d->m_RemoveCustomColorButton->setIsTransparent(false);
  d->m_RemoveCustomColorButton->setNXIcon(NXIconType::Minus);
  connect(d->m_RemoveCustomColorButton, &NXToolButton::clicked, d, &NXColorDialogPrivate::onRemoveCustomColorButtonClicked);

  NXText *customColorViewText = new NXText(QStringLiteral("自定义颜色"), this);
  customColorViewText->setTextPixelSize(14);
  QHBoxLayout *customButtonLayout = new QHBoxLayout();
  customButtonLayout->setContentsMargins(0, 0, 0, 0);
  customButtonLayout->addWidget(customColorViewText);
  customButtonLayout->addSpacing(35);
  customButtonLayout->addWidget(d->m_AddCustomColorButton);
  customButtonLayout->addWidget(d->m_RemoveCustomColorButton);
  customButtonLayout->addStretch();

  QVBoxLayout *customColorLayout = new QVBoxLayout();
  customColorLayout->setContentsMargins(3, 0, 0, 0);
  customColorLayout->addLayout(customButtonLayout);
  customColorLayout->addSpacing(5);
  customColorLayout->addWidget(d->m_CustomColorView);
  customColorLayout->addStretch();

  QHBoxLayout *colorDisplayLayout = new QHBoxLayout();
  colorDisplayLayout->setContentsMargins(0, 0, 0, 0);
  colorDisplayLayout->addLayout(basicColorLayout);
  colorDisplayLayout->addLayout(customColorLayout);
  colorDisplayLayout->addStretch();

  // 确定取消按钮
  d->m_OverButton = new NXPushButton(QStringLiteral("确定"), this);
  d->m_OverButton->setBorderRadius(6);
  d->m_OverButton->setLightDefaultColor(NXThemeColor(NXThemeType::Light, PrimaryNormal));
  d->m_OverButton->setLightHoverColor(NXThemeColor(NXThemeType::Light, PrimaryHover));
  d->m_OverButton->setLightPressColor(NXThemeColor(NXThemeType::Light, PrimaryPress));
  d->m_OverButton->setLightTextColor(Qt::white);
  d->m_OverButton->setDarkDefaultColor(NXThemeColor(NXThemeType::Dark, PrimaryNormal));
  d->m_OverButton->setDarkHoverColor(NXThemeColor(NXThemeType::Dark, PrimaryHover));
  d->m_OverButton->setDarkPressColor(NXThemeColor(NXThemeType::Dark, PrimaryPress));
  d->m_OverButton->setDarkTextColor(Qt::white);
  connect(d->m_OverButton, &NXPushButton::clicked, this, [=]() {
    Q_EMIT colorSelected(d->m_CurrentColor);
    close();
  });
  d->m_CancelButton = new NXPushButton(QStringLiteral("取消"), this);
  d->m_CancelButton->setBorderRadius(6);
  connect(d->m_CancelButton, &NXPushButton::clicked, this, [=]() {
    close();
  });
  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->setContentsMargins(0, 0, 0, 0);
  buttonLayout->addWidget(d->m_OverButton);
  buttonLayout->addWidget(d->m_CancelButton);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(20, 10, 11, 20);
  mainLayout->addLayout(colorPickerTextLayout);
  mainLayout->addSpacing(3);
  mainLayout->addLayout(colorControlLayout);
  mainLayout->addSpacing(80);
  mainLayout->addLayout(colorDisplayLayout);
  mainLayout->addLayout(buttonLayout);
  mainLayout->addStretch();

  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
  });
  setCurrentColor(QColor(0x39, 0xC5, 0xBB));
}

NXColorDialog::NXColorDialog(QColor const &currentColor, QWidget *parent)
    : NXColorDialog { parent }
{
  setCurrentColor(currentColor);
}

NXColorDialog::~NXColorDialog()
{
  Q_D(NXColorDialog);
  delete d->m_ColorValueSlider->style();
  delete d->m_TransparencyValueSlider->style();
}

void NXColorDialog::setCurrentColor(QColor const &currentColor)
{
  Q_D(NXColorDialog);
  d->m_CurrentColor = currentColor;
  d->_updateHtmlEditValue();
  d->_updateEditValue();
  d->_updateColorPreview();
  d->_updateColorValueSlider();
  d->_updateTransparencyValueSlider();
  d->m_ColorPicker->setSelectedColor(d->m_CurrentColor);
  Q_EMIT signalCurrentColorChanged();
}

QColor NXColorDialog::getCurrentColor() const noexcept(std::is_nothrow_copy_constructible_v<QColor>)
{
  Q_CONST_D(NXColorDialog);
  return d->m_CurrentColor;
}

void NXColorDialog::setColorSchemeType(NXColorSchemeType::ColorSchemeType schemeType)
{
  Q_D(NXColorDialog);
  d->m_ColorSchemeType = schemeType;
  Q_EMIT signalColorSchemeTypeChanged();
}

NXColorSchemeType::ColorSchemeType NXColorDialog::getColorSchemeType() const noexcept(std::is_nothrow_copy_constructible_v<NXColorSchemeType::ColorSchemeType>)
{
  Q_CONST_D(NXColorDialog);
  return d->m_ColorSchemeType;
}

QList<QColor> NXColorDialog::getCustomColorList() const
{
  Q_CONST_D(NXColorDialog);
  return d->m_CustomColorModel->getDisplayColorList();
}

QColor NXColorDialog::getCustomColor(int index) const
{
  Q_CONST_D(NXColorDialog);
  return d->m_CustomColorModel->getDisplayColor(index);
}

QString NXColorDialog::getCurrent4ChannelColor() const
{
  Q_CONST_D(NXColorDialog);
  return d->_getHex4ChannelValue();
}

void NXColorDialog::paintEvent(QPaintEvent *event)
{
  Q_D(NXColorDialog);
  QPainter painter(this);
  painter.save();
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setPen(Qt::NoPen);
  // 背景绘制
  painter.setBrush(NXThemeColor(d->m_ThemeMode, DialogBase));
  painter.drawRect(rect());

  // 按钮背景绘制
  painter.setBrush(NXThemeColor(d->m_ThemeMode, DialogLayoutArea));
  painter.drawRect(QRect(0, height() - 78, width(), 78));
  painter.restore();
  QDialog::paintEvent(event);
}
