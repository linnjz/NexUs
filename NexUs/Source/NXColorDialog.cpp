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
Q_TAKEOVER_NATIVEEVENT_CPP(NXColorDialog, d_func()->_appBar);

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

  d->_pColorSchemeType = NXColorSchemeType::Argb;
  // 自定义AppBar
  d->_appBar = new NXAppBar(this);
  d->_appBar->setAppBarHeight(30);
  d->_appBar->setIsFixedSize(true);
  d->_appBar->setIsStayTop(true);
  d->_appBar->setWindowButtonFlags(NXAppBarType::CloseButtonHint);

  // 颜色选择器
  NXText *colorPickerText = new NXText(QStringLiteral("编辑颜色"), this);
  colorPickerText->setTextPixelSize(17);
  QHBoxLayout *colorPickerTextLayout = new QHBoxLayout();
  colorPickerTextLayout->setContentsMargins(0, 0, 0, 0);
  colorPickerTextLayout->addSpacing(3);
  colorPickerTextLayout->addWidget(colorPickerText);

  d->_colorPicker = new NXColorPicker(this);
  connect(d->_colorPicker, &NXColorPicker::selectedColorChanged, d, &NXColorDialogPrivate::onColorPickerColorChanged);

  QVBoxLayout *colorPickerLayout = new QVBoxLayout();
  colorPickerLayout->setContentsMargins(0, 0, 0, 0);
  colorPickerLayout->addWidget(d->_colorPicker);
  colorPickerLayout->addStretch();

  // 颜色预览
  d->_colorPreview                = new NXColorPreview(this);
  QVBoxLayout *colorPreviewLayout = new QVBoxLayout();
  colorPreviewLayout->setContentsMargins(0, 0, 0, 0);
  colorPreviewLayout->addWidget(d->_colorPreview);
  colorPreviewLayout->addStretch();

  // 颜色value控制
  d->_colorValueSlider = new QSlider(this);
  d->_colorValueSlider->setFixedHeight(257);
  d->_colorValueSlider->setOrientation(Qt::Vertical);
  d->_colorValueSlider->setRange(0, 255);
  d->_colorValueSlider->setValue(255);
  d->_colorValueSliderStyle = new NXColorValueSliderStyle();
  d->_colorValueSlider->setStyle(d->_colorValueSliderStyle);
  connect(d->_colorValueSlider, &QSlider::valueChanged, d, &NXColorDialogPrivate::onColorValueSliderChanged);
  QVBoxLayout *colorValueSliderLayout = new QVBoxLayout();
  colorValueSliderLayout->setContentsMargins(0, 0, 0, 0);
  colorValueSliderLayout->addWidget(d->_colorValueSlider);
  colorValueSliderLayout->addStretch();

  d->_transparencyValueSlider = new QSlider(this);
  d->_transparencyValueSlider->setFixedHeight(257);
  d->_transparencyValueSlider->setOrientation(Qt::Vertical);
  d->_transparencyValueSlider->setRange(0, 255);
  d->_transparencyValueSlider->setValue(255);
  d->_transparencyValueSliderStyle = new NXColorValueSliderStyle(style());
  d->_transparencyValueSliderStyle->setIsUseAlpha(true);
  d->_transparencyValueSlider->setStyle(d->_transparencyValueSliderStyle);
  connect(d->_transparencyValueSlider, &QSlider::valueChanged, d, &NXColorDialogPrivate::onTransparencyValueSliderChanged);
  QVBoxLayout *transparencyValueSliderLayout = new QVBoxLayout();
  transparencyValueSliderLayout->setContentsMargins(0, 0, 0, 0);
  transparencyValueSliderLayout->addWidget(d->_transparencyValueSlider);
  transparencyValueSliderLayout->addStretch();

  // 颜色控制器
  d->_htmlEdit = new NXLineEdit(this);
  d->_htmlEdit->setText(QStringLiteral("#FFFFFFFF"));
  NXIntValidator *htmlValidator = new NXIntValidator(0x00'00'00'00, 0xFF'FF'FF'FF, this);
  htmlValidator->setIsHexMode(true);
  d->_htmlEdit->setValidator(htmlValidator);
  d->_htmlEdit->setFixedSize(120, 33);
  connect(d->_htmlEdit, &NXLineEdit::focusOut, d, &NXColorDialogPrivate::onHtmlEditFocusOut);
  connect(d->_htmlEdit, &NXLineEdit::textEdited, d, &NXColorDialogPrivate::onHtmlEditChanged);

  // 数值预览
  QHBoxLayout *htmlEditLayout = new QHBoxLayout();
  htmlEditLayout->setContentsMargins(0, 0, 0, 0);
  htmlEditLayout->addSpacing(6);
  htmlEditLayout->addWidget(d->_htmlEdit);
  htmlEditLayout->addStretch();
  d->_modeComboBox = new NXComboBox(this);
  d->_modeComboBox->setFixedSize(120, 33);
  d->_modeComboBox->addItem(QStringLiteral("ARGB"));
  d->_modeComboBox->addItem(QStringLiteral("AHSV"));
  d->_modeComboBox->setCurrentIndex(0);
  connect(d->_modeComboBox, QOverload<int>::of(&NXComboBox::currentIndexChanged), d, &NXColorDialogPrivate::onColorModeChanged);
  connect(this, &NXColorDialog::pColorSchemeTypeChanged, d->_modeComboBox, [d]() {
    d->_modeComboBox->setItemText(0, QStringLiteral("RGBA"));
    d->_modeComboBox->setItemText(1, QStringLiteral("HSVA"));
  });
  QHBoxLayout *modeLayout = new QHBoxLayout();
  modeLayout->setContentsMargins(0, 0, 0, 0);
  modeLayout->addSpacing(6);
  modeLayout->addWidget(d->_modeComboBox);
  modeLayout->addStretch();

  // R或H
  d->_firstEdit = new NXLineEdit(this);
  d->_firstEdit->setText(QStringLiteral("0"));
  d->_firstEdit->setFixedSize(120, 33);
  d->_firstEdit->setValidator(new NXIntValidator(0, 255, this));
  d->_firstText = new NXText(QStringLiteral("红色"), this);
  d->_firstText->setTextPixelSize(13);
  connect(d->_firstEdit, &NXLineEdit::textEdited, d, &NXColorDialogPrivate::onColorEditChanged);

  QHBoxLayout *firstEditLayout = new QHBoxLayout();
  firstEditLayout->setContentsMargins(0, 0, 0, 0);
  firstEditLayout->addSpacing(6);
  firstEditLayout->addWidget(d->_firstEdit);
  firstEditLayout->addWidget(d->_firstText);
  firstEditLayout->addStretch();

  // G或S
  d->_secondEdit = new NXLineEdit(this);
  d->_secondEdit->setText(QStringLiteral("0"));
  d->_secondEdit->setFixedSize(120, 33);
  d->_secondEdit->setValidator(new NXIntValidator(0, 255, this));
  d->_secondText = new NXText(QStringLiteral("绿色"), this);
  d->_secondText->setTextPixelSize(13);
  connect(d->_secondEdit, &NXLineEdit::textEdited, d, &NXColorDialogPrivate::onColorEditChanged);
  QHBoxLayout *secondEditLayout = new QHBoxLayout();
  secondEditLayout->setContentsMargins(0, 0, 0, 0);
  secondEditLayout->addSpacing(6);
  secondEditLayout->addWidget(d->_secondEdit);
  secondEditLayout->addWidget(d->_secondText);
  secondEditLayout->addStretch();

  // B或V
  d->_thridEdit = new NXLineEdit(this);
  d->_thridEdit->setText(QStringLiteral("0"));
  d->_thridEdit->setFixedSize(120, 33);
  d->_thridEdit->setValidator(new NXIntValidator(0, 255, this));
  d->_thridText = new NXText(QStringLiteral("蓝色"), this);
  d->_thridText->setTextPixelSize(13);
  connect(d->_thridEdit, &NXLineEdit::textEdited, d, &NXColorDialogPrivate::onColorEditChanged);
  QHBoxLayout *thridEditLayout = new QHBoxLayout();
  thridEditLayout->setContentsMargins(0, 0, 0, 0);
  thridEditLayout->addSpacing(6);
  thridEditLayout->addWidget(d->_thridEdit);
  thridEditLayout->addWidget(d->_thridText);
  thridEditLayout->addStretch();

  // Alpha
  d->_fourthEdit = new NXLineEdit(this);
  d->_fourthEdit->setText(QStringLiteral("0"));
  d->_fourthEdit->setFixedSize(120, 33);
  d->_fourthEdit->setValidator(new NXIntValidator(0, 255, this));
  d->_fourthText = new NXText(QStringLiteral("透明度"), this);
  d->_fourthText->setTextPixelSize(13);
  connect(d->_fourthEdit, &NXLineEdit::textEdited, d, &NXColorDialogPrivate::onColorEditChanged);
  QHBoxLayout *fourthEditLayout = new QHBoxLayout();
  fourthEditLayout->setContentsMargins(0, 0, 0, 0);
  fourthEditLayout->addSpacing(6);
  fourthEditLayout->addWidget(d->_fourthEdit);
  fourthEditLayout->addWidget(d->_fourthText);
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
  d->_basicColorView = new NXBaseListView(this);
  d->_basicColorView->setFixedSize(380, 170);
  d->_basicColorView->setSelectionMode(QListView::NoSelection);
  d->_basicColorView->setFlow(QListView::LeftToRight);
  d->_basicColorView->setViewMode(QListView::IconMode);
  d->_basicColorView->setResizeMode(QListView::Adjust);
  d->_basicColorModel = new NXColorDisplayModel(this);
  d->_basicColorView->setModel(d->_basicColorModel);
  d->_basicColorDelegate = new NXColorDisplayDelegate(this);
  d->_basicColorView->setItemDelegate(d->_basicColorDelegate);
  d->_initBasicColor();
  connect(d->_basicColorView, &NXBaseListView::clicked, d, &NXColorDialogPrivate::onBasicColorViewClicked);

  NXText *basicColorViewText = new NXText(QStringLiteral("基本颜色"), this);
  basicColorViewText->setTextPixelSize(14);
  QVBoxLayout *basicColorLayout = new QVBoxLayout();
  basicColorLayout->setContentsMargins(3, 0, 0, 0);
  basicColorLayout->addWidget(basicColorViewText);
  basicColorLayout->addSpacing(15);
  basicColorLayout->addWidget(d->_basicColorView);
  basicColorLayout->addStretch();

  // 自定义颜色
  d->_customColorView = new NXBaseListView(this);
  d->_customColorView->setFixedSize(200, 170);
  d->_customColorView->setSelectionMode(QListView::SingleSelection);
  d->_customColorView->setFlow(QListView::LeftToRight);
  d->_customColorView->setViewMode(QListView::IconMode);
  d->_customColorView->setResizeMode(QListView::Adjust);
  d->_customColorModel = new NXColorDisplayModel(this);
  d->_customColorView->setModel(d->_customColorModel);
  d->_customColorDelegate = new NXColorDisplayDelegate(this);
  d->_customColorView->setItemDelegate(d->_customColorDelegate);
  d->_initCustomColor();
  connect(d->_customColorView, &NXBaseListView::clicked, d, &NXColorDialogPrivate::onCustomColorViewClicked);

  // 增加移除按钮
  d->_addCustomColorButton = new NXToolButton(this);
  d->_addCustomColorButton->setIsTransparent(false);
  d->_addCustomColorButton->setNXIcon(NXIconType::Plus);
  connect(d->_addCustomColorButton, &NXToolButton::clicked, d, &NXColorDialogPrivate::onAddCustomColorButtonClicked);

  d->_removeCustomColorButton = new NXToolButton(this);
  d->_removeCustomColorButton->setIsTransparent(false);
  d->_removeCustomColorButton->setNXIcon(NXIconType::Minus);
  connect(d->_removeCustomColorButton, &NXToolButton::clicked, d, &NXColorDialogPrivate::onRemoveCustomColorButtonClicked);

  NXText *customColorViewText = new NXText(QStringLiteral("自定义颜色"), this);
  customColorViewText->setTextPixelSize(14);
  QHBoxLayout *customButtonLayout = new QHBoxLayout();
  customButtonLayout->setContentsMargins(0, 0, 0, 0);
  customButtonLayout->addWidget(customColorViewText);
  customButtonLayout->addSpacing(35);
  customButtonLayout->addWidget(d->_addCustomColorButton);
  customButtonLayout->addWidget(d->_removeCustomColorButton);
  customButtonLayout->addStretch();

  QVBoxLayout *customColorLayout = new QVBoxLayout();
  customColorLayout->setContentsMargins(3, 0, 0, 0);
  customColorLayout->addLayout(customButtonLayout);
  customColorLayout->addSpacing(5);
  customColorLayout->addWidget(d->_customColorView);
  customColorLayout->addStretch();

  QHBoxLayout *colorDisplayLayout = new QHBoxLayout();
  colorDisplayLayout->setContentsMargins(0, 0, 0, 0);
  colorDisplayLayout->addLayout(basicColorLayout);
  colorDisplayLayout->addLayout(customColorLayout);
  colorDisplayLayout->addStretch();

  // 确定取消按钮
  d->_overButton = new NXPushButton(QStringLiteral("确定"), this);
  d->_overButton->setBorderRadius(6);
  d->_overButton->setLightDefaultColor(NXThemeColor(NXThemeType::Light, PrimaryNormal));
  d->_overButton->setLightHoverColor(NXThemeColor(NXThemeType::Light, PrimaryHover));
  d->_overButton->setLightPressColor(NXThemeColor(NXThemeType::Light, PrimaryPress));
  d->_overButton->setLightTextColor(Qt::white);
  d->_overButton->setDarkDefaultColor(NXThemeColor(NXThemeType::Dark, PrimaryNormal));
  d->_overButton->setDarkHoverColor(NXThemeColor(NXThemeType::Dark, PrimaryHover));
  d->_overButton->setDarkPressColor(NXThemeColor(NXThemeType::Dark, PrimaryPress));
  d->_overButton->setDarkTextColor(Qt::white);
  connect(d->_overButton, &NXPushButton::clicked, this, [=]() {
    Q_EMIT colorSelected(d->_pCurrentColor);
    close();
  });
  d->_cancelButton = new NXPushButton(QStringLiteral("取消"), this);
  d->_cancelButton->setBorderRadius(6);
  connect(d->_cancelButton, &NXPushButton::clicked, this, [=]() {
    close();
  });
  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->setContentsMargins(0, 0, 0, 0);
  buttonLayout->addWidget(d->_overButton);
  buttonLayout->addWidget(d->_cancelButton);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(20, 10, 11, 20);
  mainLayout->addLayout(colorPickerTextLayout);
  mainLayout->addSpacing(3);
  mainLayout->addLayout(colorControlLayout);
  mainLayout->addSpacing(80);
  mainLayout->addLayout(colorDisplayLayout);
  mainLayout->addLayout(buttonLayout);
  mainLayout->addStretch();

  d->_themeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
  });
  setCurrentColor(QColor(0x39, 0xC5, 0xBB));
}

NXColorDialog::NXColorDialog(const QColor &currentColor, QWidget *parent)
    : NXColorDialog { parent }
{
  setCurrentColor(currentColor);
}

NXColorDialog::~NXColorDialog()
{
  Q_D(NXColorDialog);
  delete d->_colorValueSlider->style();
  delete d->_transparencyValueSlider->style();
}

void NXColorDialog::setCurrentColor(const QColor &currentColor)
{
  Q_D(NXColorDialog);
  d->_pCurrentColor = currentColor;
  d->_updateHtmlEditValue();
  d->_updateEditValue();
  d->_updateColorPreview();
  d->_updateColorValueSlider();
  d->_updateTransparencyValueSlider();
  d->_colorPicker->setSelectedColor(d->_pCurrentColor);
  Q_EMIT pCurrentColorChanged();
}

QColor NXColorDialog::getCurrentColor() const
{
  Q_D(const NXColorDialog);
  return d->_pCurrentColor;
}

void NXColorDialog::setColorSchemeType(NXColorSchemeType::ColorSchemeType schemeType)
{
  Q_D(NXColorDialog);
  d->_pColorSchemeType = schemeType;
  Q_EMIT pColorSchemeTypeChanged();
}

NXColorSchemeType::ColorSchemeType NXColorDialog::getColorSchemeType() const
{
  Q_D(const NXColorDialog);
  return d->_pColorSchemeType;
}

QList<QColor> NXColorDialog::getCustomColorList() const
{
  Q_D(const NXColorDialog);
  return d->_customColorModel->getDisplayColorList();
}

QColor NXColorDialog::getCustomColor(int index) const
{
  Q_D(const NXColorDialog);
  return d->_customColorModel->getDisplayColor(index);
}

QString NXColorDialog::getCurrent4ChannelColor() const
{
  Q_D(const NXColorDialog);
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
  painter.setBrush(NXThemeColor(d->_themeMode, DialogBase));
  painter.drawRect(rect());

  // 按钮背景绘制
  painter.setBrush(NXThemeColor(d->_themeMode, DialogLayoutArea));
  painter.drawRect(QRect(0, height() - 78, width(), 78));
  painter.restore();
  QDialog::paintEvent(event);
}
