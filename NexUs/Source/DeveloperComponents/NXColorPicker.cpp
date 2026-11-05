#include "NXColorPicker.h"

#include <utility>

#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QtMath>

#include "NXTheme.h"

NXColorPicker::NXColorPicker(QWidget *parent)
    : QWidget { parent }
    , m_ThemeMode { nxTheme->getThemeMode() }
{
  setFixedSize(260, 260);
  // 初始化色相图
  QPixmap colorPickerPix(QSize(360, 360));
  QImage colorPickerImage = colorPickerPix.toImage();
  for (int y = 0; y < colorPickerImage.height(); y++) {
    for (int x = 0; x < colorPickerImage.width(); x++) {
      QColor pixColor;
      qreal h = qBound(0.0, (qreal) x / colorPickerPix.width(), 1.0);
      qreal s = qBound(0.0, 1 - (qreal) y / (colorPickerPix.height() - 1), 1.0);
      pixColor.setHsvF(h, s, 1);
      pixColor = pixColor.toRgb();
      colorPickerImage.setPixelColor(x, y, pixColor);
    }
  }
  m_ColorPickerImage = colorPickerImage;
}

NXColorPicker::~NXColorPicker()
{
}

void NXColorPicker::setSelectedColor(QColor const &color)
{
  m_SelectedColor = std::move(color);
  m_SelectedPoint = QPoint(qRound(3 + 254 * m_SelectedColor.hueF() + 0.5f), qRound(257 - 254 * m_SelectedColor.saturationF() + 0.5f));
  update();
}

QColor NXColorPicker::getSelectedColor() const
{
  return m_SelectedColor;
}

void NXColorPicker::mousePressEvent(QMouseEvent *event)
{
  m_SelectedPoint = _adjustPointLimit(event->pos());
  QPointF colorPoint((m_SelectedPoint.x() - 3) / 254.0 * 359, (m_SelectedPoint.y() - 3) / 254.0 * 359);
  QColor pixColor;
  qreal h = qBound(0.0, colorPoint.x() / m_ColorPickerImage.width(), 1.0);
  qreal s = qBound(0.0, 1 - colorPoint.y() / (m_ColorPickerImage.height() - 1), 1.0);
  pixColor.setHsvF(h, s, 1);
  m_SelectedColor = pixColor.toRgb();
  Q_EMIT selectedColorChanged(m_SelectedColor);
  QWidget::mousePressEvent(event);
  update();
}

void NXColorPicker::mouseReleaseEvent(QMouseEvent *event)
{
  m_SelectedPoint = _adjustPointLimit(event->pos());
  QPointF colorPoint((m_SelectedPoint.x() - 3) / 254.0 * 359, (m_SelectedPoint.y() - 3) / 254.0 * 359);
  QColor pixColor;
  qreal h = qBound(0.0, colorPoint.x() / m_ColorPickerImage.width(), 1.0);
  qreal s = qBound(0.0, 1 - colorPoint.y() / (m_ColorPickerImage.height() - 1), 1.0);
  pixColor.setHsvF(h, s, 1);
  m_SelectedColor = pixColor.toRgb();
  Q_EMIT selectedColorChanged(m_SelectedColor);
  QWidget::mouseReleaseEvent(event);
  update();
}

void NXColorPicker::mouseMoveEvent(QMouseEvent *event)
{
  m_SelectedPoint = _adjustPointLimit(event->pos());
  QPointF colorPoint((m_SelectedPoint.x() - 3) / 254.0 * 359, (m_SelectedPoint.y() - 3) / 254.0 * 359);
  QColor pixColor;
  qreal h = qBound(0.0, colorPoint.x() / m_ColorPickerImage.width(), 1.0);
  qreal s = qBound(0.0, 1 - colorPoint.y() / (m_ColorPickerImage.height() - 1), 1.0);
  pixColor.setHsvF(h, s, 1);
  m_SelectedColor = pixColor.toRgb();
  Q_EMIT selectedColorChanged(m_SelectedColor);
  QWidget::mouseMoveEvent(event);
  update();
}

void NXColorPicker::paintEvent(QPaintEvent *event)
{
  int penWidth     = 3;
  int borderRadius = 5;
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
  // 色相图绘制
  QRect pickerRect = rect();
  pickerRect.adjust(penWidth, penWidth, -penWidth, -penWidth);
  painter.setPen(QPen(NXThemeColor(m_ThemeMode, BasicBorder), penWidth));
  painter.drawRoundedRect(pickerRect, borderRadius, borderRadius);
  QPainterPath path;
  path.addRoundedRect(pickerRect, borderRadius, borderRadius);
  painter.setClipPath(path);
  painter.drawImage(pickerRect, m_ColorPickerImage);

  // 取色点绘制
  if (!m_SelectedPoint.isNull()) {
    painter.setPen(QPen(Qt::black, 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(m_SelectedPoint, 8, 8);
  }
  painter.restore();
}

QPoint NXColorPicker::_adjustPointLimit(QPoint point)
{
  QRect pickerRect = rect();
  if (point.x() < 3) {
    point.setX(3);
  } else if (point.x() > pickerRect.width() - 3) {
    point.setX(pickerRect.width() - 3);
  }
  if (point.y() < 3) {
    point.setY(3);
  } else if (point.y() > pickerRect.height() - 3) {
    point.setY(pickerRect.height() - 3);
  }
  return point;
}
