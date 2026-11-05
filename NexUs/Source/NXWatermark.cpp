#include "NXWatermark.h"

#include <QEvent>
#include <QPainter>
#include <QtMath>

#include "NXTheme.h"
#include "private/NXWatermarkPrivate.h"

Q_PROPERTY_CREATE_CPP(NXWatermark, QPS_SCREF_PAEMIT(QString), Text)
Q_PROPERTY_CREATE_CPP(NXWatermark, QPS_SCREF_PAEMIT(QImage), Image)
Q_PROPERTY_CREATE_CPP(NXWatermark, QPS_SCREF_PAEMIT(QColor), TextColor)
Q_PROPERTY_CREATE_CPP(NXWatermark, QPS_PAEMIT(qreal), Opacity)
Q_PROPERTY_CREATE_CPP(NXWatermark, QPS_PAEMIT(qreal), Rotation)
Q_PROPERTY_CREATE_CPP(NXWatermark, QPS_PAEMIT(int), FontPixelSize)
Q_PROPERTY_CREATE_CPP(NXWatermark, QPS_PAEMIT(int), GapX)
Q_PROPERTY_CREATE_CPP(NXWatermark, QPS_PAEMIT(int), GapY)
Q_PROPERTY_CREATE_CPP(NXWatermark, QPS_PAEMIT(int), ImageWidth)
Q_PROPERTY_CREATE_CPP(NXWatermark, QPS_PAEMIT(int), ImageHeight)

NXWatermarkPrivate::NXWatermarkPrivate(QObject *parent)
    : QObject(parent)
{
}

NXWatermarkPrivate::~NXWatermarkPrivate()
{
}

void NXWatermarkPrivate::rebuildPattern()
{
  Q_Q(NXWatermark);
  m_PatternDirty = false;

  bool hasText  = !m_Text.isEmpty();
  bool hasImage = !m_Image.isNull();
  if (!hasText && !hasImage) {
    m_PatternPixmap = QPixmap();
    return;
  }

  QFont font;
  font.setPixelSize(m_FontPixelSize);
  QFontMetrics fm(font);

  int contentWidth  = 0;
  int contentHeight = 0;
  if (hasText) {
    contentWidth  = fm.horizontalAdvance(m_Text);
    contentHeight = fm.height();
  }
  if (hasImage) {
    int imgW = m_ImageWidth > 0 ? m_ImageWidth : m_Image.width();
    int imgH = m_ImageHeight > 0 ? m_ImageHeight : m_Image.height();
    if (hasText) {
      contentWidth = qMax(contentWidth, imgW);
      contentHeight += 4 + imgH;
    } else {
      contentWidth  = imgW;
      contentHeight = imgH;
    }
  }

  if (contentWidth <= 0 || contentHeight <= 0) {
    m_PatternPixmap = QPixmap();
    return;
  }

  int cellWidth  = contentWidth + m_GapX;
  int cellHeight = contentHeight + m_GapY;

  qreal radians     = qDegreesToRadians(m_Rotation);
  int rotatedWidth  = qCeil(qAbs(cellWidth * qCos(radians)) + qAbs(cellHeight * qSin(radians)));
  int rotatedHeight = qCeil(qAbs(cellWidth * qSin(radians)) + qAbs(cellHeight * qCos(radians)));

  qreal dpr = q->devicePixelRatioF();
  QPixmap tile(QSize(rotatedWidth, rotatedHeight) * dpr);
  tile.setDevicePixelRatio(dpr);
  tile.fill(Qt::transparent);

  QPainter painter(&tile);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
  painter.translate(rotatedWidth / 2.0, rotatedHeight / 2.0);
  painter.rotate(m_Rotation);
  painter.translate(-cellWidth / 2.0, -cellHeight / 2.0);

  int yOffset = 0;
  if (hasImage) {
    int imgW = m_ImageWidth > 0 ? m_ImageWidth : m_Image.width();
    int imgH = m_ImageHeight > 0 ? m_ImageHeight : m_Image.height();
    int imgX = (contentWidth - imgW) / 2;
    painter.setOpacity(m_Opacity);
    painter.drawImage(QRect(imgX, 0, imgW, imgH), m_Image);
    yOffset = imgH + 4;
  }

  if (hasText) {
    QColor color = m_TextColor.isValid() ? m_TextColor : NXThemeColor(m_ThemeMode, BasicText);
    color.setAlphaF(color.alphaF() * m_Opacity);
    painter.setOpacity(1.0);
    painter.setFont(font);
    painter.setPen(color);
    painter.drawText(QRect(0, yOffset, contentWidth, fm.height()), Qt::AlignCenter, m_Text);
  }

  painter.end();
  m_PatternPixmap = tile;
}

NXWatermark::NXWatermark(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXWatermarkPrivate())
{
  Q_D(NXWatermark);
  d->q_ptr           = this;
  d->m_Text          = QStringLiteral("");
  d->m_FontPixelSize = 14;
  d->m_Opacity       = 0.12;
  d->m_Rotation      = -22;
  d->m_GapX          = 80;
  d->m_GapY          = 60;
  d->m_TextColor     = QColor();
  d->m_Image         = QImage();
  d->m_ImageWidth    = 0;
  d->m_ImageHeight   = 0;
  setObjectName("NXWatermark");

  setAttribute(Qt::WA_TransparentForMouseEvents);
  setAttribute(Qt::WA_TranslucentBackground);
  setFocusPolicy(Qt::NoFocus);

  if (parent) {
    parent->installEventFilter(this);
    resize(parent->size());
    raise();
  }

  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode    = themeMode;
    d->m_PatternDirty = true;
    update();
  });

  auto markDirty = [=]() {
    d->m_PatternDirty = true;
    update();
  };
  connect(this, &NXWatermark::signalTextChanged, this, markDirty);
  connect(this, &NXWatermark::signalFontPixelSizeChanged, this, markDirty);
  connect(this, &NXWatermark::signalOpacityChanged, this, markDirty);
  connect(this, &NXWatermark::signalRotationChanged, this, markDirty);
  connect(this, &NXWatermark::signalGapXChanged, this, markDirty);
  connect(this, &NXWatermark::signalGapYChanged, this, markDirty);
  connect(this, &NXWatermark::signalTextColorChanged, this, markDirty);
  connect(this, &NXWatermark::signalImageChanged, this, markDirty);
  connect(this, &NXWatermark::signalImageWidthChanged, this, markDirty);
  connect(this, &NXWatermark::signalImageHeightChanged, this, markDirty);
}

NXWatermark::NXWatermark(QString const &text, QWidget *parent)
    : NXWatermark(parent)
{
  Q_D(NXWatermark);
  d->m_Text        = text;
  d->m_PatternDirty = true;
}

NXWatermark::~NXWatermark()
{
}

bool NXWatermark::eventFilter(QObject *watched, QEvent *event)
{
  if (watched == parent() && event->type() == QEvent::Resize) {
    resize(static_cast<QWidget *>(parent())->size());
    raise();
  }
  return QWidget::eventFilter(watched, event);
}

void NXWatermark::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);
  Q_D(NXWatermark);

  if (d->m_PatternDirty) {
    d->rebuildPattern();
  }

  if (d->m_PatternPixmap.isNull()) {
    return;
  }

  QPainter painter(this);
  painter.drawTiledPixmap(rect(), d->m_PatternPixmap);
}
