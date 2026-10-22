#include "NXColorDialogPrivate.h"

#include <QPainter>
#include <QSlider>
#include <QtMath>

#include "DeveloperComponents/NXBaseListView.h"
#include "DeveloperComponents/NXColorDisplayModel.h"
#include "DeveloperComponents/NXColorPreview.h"
#include "DeveloperComponents/NXColorValueSliderStyle.h"
#include "DeveloperComponents/NXIntValidator.h"
#include "NXColorDialog.h"
#include "NXComboBox.h"
#include "NXLineEdit.h"
#include "NXText.h"

static constexpr float INV_255 = 1.0f / 255.0f;

NXColorDialogPrivate::NXColorDialogPrivate(QObject *parent)
    : QObject { parent }
{
}

NXColorDialogPrivate::~NXColorDialogPrivate()
{
}

void NXColorDialogPrivate::onColorPickerColorChanged(const QColor &selectedColor)
{
  Q_Q(NXColorDialog);
  // float <h, s, v, a> range from 0.0 to 1.0
  // int <h> range from 0 to 359, <s, v, a> range from 0 to 255
  _pCurrentColor = selectedColor;
  _pCurrentColor.setHsvF(selectedColor.hueF(), selectedColor.saturationF(), _colorValueSlider->value() * INV_255, _transparencyValueSlider->value() * INV_255);
  _updateHtmlEditValue();
  _updateEditValue();
  _updateColorPreview();
  _updateColorValueSlider();
  _updateTransparencyValueSlider();
  Q_EMIT q->pCurrentColorChanged();
}

void NXColorDialogPrivate::onColorValueSliderChanged(int value)
{
  Q_Q(NXColorDialog);
  QColor baseColor = _pCurrentColor;
  baseColor.setHsvF(baseColor.hueF(), baseColor.saturationF(), value * INV_255, _transparencyValueSlider->value() * INV_255);
  q->setCurrentColor(baseColor);
}

void NXColorDialogPrivate::onTransparencyValueSliderChanged(int value)
{
  Q_Q(NXColorDialog);
  QColor baseColor = _pCurrentColor;
  baseColor.setHsvF(baseColor.hueF(), baseColor.saturationF(), baseColor.value() * INV_255, value * INV_255);
  q->setCurrentColor(baseColor);
}

void NXColorDialogPrivate::onColorModeChanged(int index)
{
  if (index == 0) {
    _firstText->setText(QStringLiteral("红色"));
    _secondText->setText(QStringLiteral("绿色"));
    _thridText->setText(QStringLiteral("蓝色"));
    _fourthText->setText(QStringLiteral("透明度"));
    _firstEdit->setValidator(new NXIntValidator(0, 255, this));
  } else if (index == 1) {
    _firstText->setText(QStringLiteral("色调"));
    _secondText->setText(QStringLiteral("饱和度"));
    _thridText->setText(QStringLiteral("值"));
    _fourthText->setText(QStringLiteral("透明度"));
    _firstEdit->setValidator(new NXIntValidator(0, 359, this));
  }
  _updateEditValue();
}

void NXColorDialogPrivate::onHtmlEditFocusOut(const QString &text)
{
  // 自动补全
  if (text == QStringLiteral("#")) {
    Q_Q(NXColorDialog);
    //_htmlEdit->setText(QStringLiteral("#FF000000"));
    // #FF000000 is argb, qcolor(0,0,0,0) is rgba
    q->setCurrentColor(QColor(0xFF, 0xFF, 0xFF, 0xFF));
  } else if (text.length() > 1 && text.length() < 9) {
    _htmlEdit->setText(_completeColorText(text.toUpper()));
  }
}

void NXColorDialogPrivate::onHtmlEditChanged(const QString &text)
{
  Q_Q(NXColorDialog);
  if (text.length() > 1) {
    if (_pColorSchemeType == NXColorSchemeType::Rgba) {
      QString fullHex = _completeColorText(text.toUpper());
      q->setCurrentColor(QColor(QStringLiteral("#") + fullHex.right(2) + fullHex.mid(1, 6)));
    } else if (_pColorSchemeType == NXColorSchemeType::Argb) {
      q->setCurrentColor(QColor(_completeColorText(text.toUpper())));
    }
  }
}

void NXColorDialogPrivate::onColorEditChanged(const QString &text)
{
  Q_Q(NXColorDialog);
  q->setCurrentColor(_getColorFromEdit());
}

void NXColorDialogPrivate::onBasicColorViewClicked(const QModelIndex &index)
{
  Q_Q(NXColorDialog);
  QColor basicColor = index.data(Qt::UserRole).value<QColor>();
  q->setCurrentColor(basicColor);
}

void NXColorDialogPrivate::onCustomColorViewClicked(const QModelIndex &index)
{
  Q_Q(NXColorDialog);
  QColor color = index.data(Qt::UserRole).value<QColor>();
  if (color.isValid()) {
    q->setCurrentColor(color);
  }
}

void NXColorDialogPrivate::onAddCustomColorButtonClicked()
{
  QModelIndexList selectedIndexs = _customColorView->selectionModel()->selectedIndexes();
  if (selectedIndexs.count() > 0) {
    _customColorModel->replaceDisplayColor(_pCurrentColor, selectedIndexs[0].row());
  }
}

void NXColorDialogPrivate::onRemoveCustomColorButtonClicked()
{
  QModelIndexList selectedIndexs = _customColorView->selectionModel()->selectedIndexes();
  if (selectedIndexs.count() > 0) {
    _customColorModel->replaceDisplayColor(QColor(), selectedIndexs[0].row());
  }
}

void NXColorDialogPrivate::_initBasicColor()
{
  QList<QColor> basicColorList;
  basicColorList << QColor(0xF0, 0x87, 0x84) << QColor(0xEB, 0x33, 0x24) << QColor(0x77, 0x43, 0x42)
                 << QColor(0x8E, 0x40, 0x3A) << QColor(0x3A, 0x06, 0x03) << QColor(0x9F, 0xFC, 0xFD)
                 << QColor(0x73, 0xFB, 0xFD) << QColor(0x32, 0x82, 0xF6) << QColor(0x00, 0x23, 0xF5)
                 << QColor(0x00, 0x12, 0x9A) << QColor(0x16, 0x41, 0x7C) << QColor(0x00, 0x0C, 0x7B)
                 << QColor(0xFF, 0xFE, 0x91) << QColor(0xFF, 0xFD, 0x55) << QColor(0xF0, 0x9B, 0x59)
                 << QColor(0xF0, 0x86, 0x50) << QColor(0x78, 0x43, 0x15) << QColor(0x81, 0x7F, 0x26)
                 << QColor(0x7E, 0x84, 0xF7) << QColor(0x73, 0x2B, 0xF5) << QColor(0x35, 0x80, 0xBB)
                 << QColor(0x00, 0x02, 0x3D) << QColor(0x58, 0x13, 0x5E) << QColor(0x3A, 0x08, 0x3E)
                 << QColor(0xA1, 0xFB, 0x8E) << QColor(0xA1, 0xFA, 0x4F) << QColor(0x75, 0xF9, 0x4D)
                 << QColor(0x75, 0xFA, 0x61) << QColor(0x75, 0xFA, 0x8D) << QColor(0x81, 0x80, 0x49)
                 << QColor(0xEF, 0x88, 0xBE) << QColor(0xEE, 0x8A, 0xF8) << QColor(0xEA, 0x3F, 0xF7)
                 << QColor(0xEA, 0x36, 0x80) << QColor(0x7F, 0x82, 0xBB) << QColor(0x75, 0x16, 0x3F)
                 << QColor(0x37, 0x7D, 0x22) << QColor(0x37, 0x7E, 0x47) << QColor(0x36, 0x7E, 0x7F)
                 << QColor(0x50, 0x7F, 0x80) << QColor(0x18, 0x3E, 0x0C) << QColor(0x17, 0x3F, 0x3F)
                 << QColor(0x74, 0x1B, 0x7C) << QColor(0x39, 0x10, 0x7B) << QColor(0x00, 0x00, 0x00)
                 << QColor(0x80, 0x80, 0x80) << QColor(0xC0, 0xC0, 0xC0) << QColor(0xFF, 0xFF, 0xFF);
  _basicColorModel->appendDisplayColor(basicColorList);
}

void NXColorDialogPrivate::_initCustomColor()
{
  QList<QColor> customColorList;
  for (int i = 0; i < 24; i++) {
    customColorList.append(QColor());
  }
  _customColorModel->appendDisplayColor(customColorList);
}

void NXColorDialogPrivate::_updateHtmlEditValue()
{
  if (!_htmlEdit->hasFocus()) {
    // 非编辑模式下 进行自动补全
    _htmlEdit->setText(_getHex4ChannelValue());
  }
}

void NXColorDialogPrivate::_updateEditValue()
{
  if (_modeComboBox->currentIndex() == 0) {
    _firstEdit->setText(QString::number(_pCurrentColor.red()));
    _secondEdit->setText(QString::number(_pCurrentColor.green()));
    _thridEdit->setText(QString::number(_pCurrentColor.blue()));
    _fourthEdit->setText(QString::number(_pCurrentColor.alpha()));
  } else {
    QColor hsvColor = _pCurrentColor.toHsv();
    int hue         = hsvColor.hue();
    if (hue < 0) {
      hue = 0;
    }
    _firstEdit->setText(QString::number(hue));
    _secondEdit->setText(QString::number(hsvColor.saturation()));
    _thridEdit->setText(QString::number(hsvColor.value()));
    _fourthEdit->setText(QString::number(hsvColor.alpha()));
  }
}

void NXColorDialogPrivate::_updateColorPreview()
{
  _colorPreview->setBaseColor(_pCurrentColor);
  _colorPreview->update();
}

void NXColorDialogPrivate::_updateColorValueSlider()
{
  _colorValueSlider->blockSignals(true);
  _colorValueSlider->setValue(_pCurrentColor.value());
  _colorValueSlider->blockSignals(false);
  _colorValueSliderStyle->setBaseColor(_pCurrentColor);
  _colorValueSlider->update();
}

void NXColorDialogPrivate::_updateTransparencyValueSlider()
{
  _transparencyValueSlider->blockSignals(true);
  _transparencyValueSlider->setValue(_pCurrentColor.alpha());
  _transparencyValueSlider->blockSignals(false);
  _transparencyValueSliderStyle->setBaseColor(_pCurrentColor);
  _transparencyValueSlider->update();
}

QString NXColorDialogPrivate::_completeColorText(const QString &text) const
{
  QString result = text;

  if (int len = result.size(), pos = _pColorSchemeType == NXColorSchemeType::Argb ? 1 : len; len == 6) {
    result.insert(pos, QStringLiteral("FF"));
  } else {
    result.insert(pos, QString(8 - len, '0'));
  }
  return result;
}

QString NXColorDialogPrivate::_getHex4ChannelValue() const
{
  QString colorHex;
  QString red = QString::number(_pCurrentColor.red(), 16);
  if (red.length() < 2) {
    red.prepend(QStringLiteral("0"));
  }
  QString green = QString::number(_pCurrentColor.green(), 16);
  if (green.length() < 2) {
    green.prepend(QStringLiteral("0"));
  }
  QString blue = QString::number(_pCurrentColor.blue(), 16);
  if (blue.length() < 2) {
    blue.prepend(QStringLiteral("0"));
  }
  QString alpha = QString::number(_pCurrentColor.alpha(), 16);
  if (alpha.length() < 2) {
    alpha.prepend(QStringLiteral("0"));
  }
  colorHex = _pColorSchemeType == NXColorSchemeType::Argb ? alpha + red + green + blue : red + green + blue + alpha;
  colorHex = colorHex.toUpper();
  colorHex.prepend(QStringLiteral("#"));
  return colorHex;
}

QColor NXColorDialogPrivate::_getColorFromEdit() const
{
  QColor editColor;
  if (_modeComboBox->currentIndex() == 0) {
    // RGB
    editColor.setRgba(qRgba(_firstEdit->text().toUInt(), _secondEdit->text().toUInt(), _thridEdit->text().toUInt(), _fourthEdit->text().toUInt()));
  } else {
    // HSV
    editColor.setHsv(_firstEdit->text().toUInt(), _secondEdit->text().toUInt(), _thridEdit->text().toUInt(), _fourthEdit->text().toUInt());
  }
  return editColor;
}
