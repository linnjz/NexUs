#include "NXAcrylicUrlCard.h"

#include <QDesktopServices>
#include <QPainter>
#include <QPainterPath>
#include <QUrl>

#include "NXApplication.h"
#include "NXTheme.h"
#include "private/NXAcrylicUrlCardPrivate.h"
Q_PROPERTY_CREATE_CPP(NXAcrylicUrlCard, QPS_SCREF_PAEMIT(QString), Title)
Q_PROPERTY_CREATE_CPP(NXAcrylicUrlCard, QPS_SCREF_PAEMIT(QString), SubTitle)
Q_PROPERTY_CREATE_CPP(NXAcrylicUrlCard, QPS_SCREF_PAEMIT(QString), Url)
Q_PROPERTY_CREATE_CPP(NXAcrylicUrlCard, QPS_SCREF_PAEMIT(QPixmap), CardPixmap)
Q_PROPERTY_CREATE_CPP(NXAcrylicUrlCard, QPS_PAEMIT(qreal), MainOpacity)
Q_PROPERTY_CREATE_CPP(NXAcrylicUrlCard, QPS_PAEMIT(qreal), NoiseOpacity)
Q_PROPERTY_CREATE_CPP(NXAcrylicUrlCard, QPS_PAEMIT(QSize), CardPixmapSize)
Q_PROPERTY_CREATE_CPP(NXAcrylicUrlCard, QPS_PAEMIT(int), BorderRadius)
Q_PROPERTY_CREATE_CPP(NXAcrylicUrlCard, QPS_PAEMIT(int), BrushAlpha)
Q_PROPERTY_CREATE_CPP(NXAcrylicUrlCard, QPS_PAEMIT(int), TitlePixelSize)
Q_PROPERTY_CREATE_CPP(NXAcrylicUrlCard, QPS_PAEMIT(int), SubTitlePixelSize)
Q_PROPERTY_CREATE_CPP(NXAcrylicUrlCard, QPS_PAEMIT(int), TitleSpacing)
Q_PROPERTY_CREATE_CPP(NXAcrylicUrlCard, QPS_PAEMIT(int), SubTitleSpacing)
Q_PROPERTY_CREATE_CPP(NXAcrylicUrlCard, QPS_PAEMIT(int), CardPixmapBorderRadius)
Q_PROPERTY_CREATE_CPP(NXAcrylicUrlCard, QPS_PAEMIT(NXCardPixType::PixMode), CardPixMode)

NXAcrylicUrlCard::NXAcrylicUrlCard(QWidget *parent)
    : QPushButton(parent)
    , d_ptr(new NXAcrylicUrlCardPrivate())
{
  Q_D(NXAcrylicUrlCard);
  d->q_ptr               = this;
  d->m_NoisePix           = QPixmap(QStringLiteral(":/Resource/Image/noise.png"));
  d->m_BorderRadius      = 5;
  d->m_MainOpacity       = 0.95;
  d->m_NoiseOpacity      = 0.06;
  d->m_BrushAlpha        = 245;
  d->m_TitlePixelSize    = 15;
  d->m_SubTitlePixelSize = 12;
  setFixedSize(180, 200);
  d->m_CardPixmapSize         = QSize(54, 54);
  d->m_TitleSpacing           = 10;
  d->m_SubTitleSpacing        = 10;
  d->m_CardPixmapBorderRadius = 6;
  d->m_CardPixMode            = NXCardPixType::PixMode::Ellipse;
  d->m_ThemeMode               = nxTheme->getThemeMode();
  connect(this, &NXAcrylicUrlCard::clicked, this, [=]() {
    QDesktopServices::openUrl(QUrl(d->m_Url));
  });
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
  });
}

NXAcrylicUrlCard::~NXAcrylicUrlCard()
{
}

void NXAcrylicUrlCard::setCardPixmapSize(int width, int height)
{
  Q_D(NXAcrylicUrlCard);
  d->m_CardPixmapSize = QSize(width, height);
  Q_EMIT signalCardPixmapSizeChanged();
}

void NXAcrylicUrlCard::paintEvent(QPaintEvent *event)
{
  Q_D(NXAcrylicUrlCard);
  QPainter painter(this);
  painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing | QPainter::TextAntialiasing);
  // 亚克力绘制
  painter.save();
  painter.setPen(
      QPen(underMouse() ? NXThemeColor(d->m_ThemeMode, BasicBorderHover) : NXThemeColor(d->m_ThemeMode, BasicBorder), d->m_BorderWidth));
  QColor brushColor;
  brushColor = underMouse() ? NXThemeColor(d->m_ThemeMode, BasicHover) : NXThemeColor(d->m_ThemeMode, BasicBase);
  brushColor.setAlpha(d->m_BrushAlpha);
  painter.setBrush(brushColor);
  QRect foregroundRect = rect();
  foregroundRect.adjust(d->m_BorderWidth, d->m_BorderWidth, -d->m_BorderWidth, -d->m_BorderWidth);
  int width = foregroundRect.width();
  painter.setOpacity(d->m_NoiseOpacity);
  painter.drawPixmap(foregroundRect, d->m_NoisePix);
  painter.setOpacity(d->m_MainOpacity);
  painter.drawRoundedRect(foregroundRect, d->m_BorderRadius, d->m_BorderRadius);
  painter.restore();

  // 图片绘制
  QRectF pixRect(width / 8.5, height() / 4 - d->m_CardPixmapSize.height() / 2, d->m_CardPixmapSize.width(), d->m_CardPixmapSize.height());
  if (!d->m_CardPixmap.isNull()) {
    painter.save();
    QPainterPath path;
    if (d->m_CardPixMode == NXCardPixType::PixMode::Ellipse) {
      path.addEllipse(pixRect.center(), d->m_CardPixmapSize.width() / 2, d->m_CardPixmapSize.height() / 2);
      painter.setClipPath(path);
      painter.drawPixmap(pixRect, d->m_CardPixmap, d->m_CardPixmap.rect());
    } else if (d->m_CardPixMode == NXCardPixType::PixMode::Default) {
      painter.drawPixmap(pixRect, d->m_CardPixmap, d->m_CardPixmap.rect());
    } else if (d->m_CardPixMode == NXCardPixType::PixMode::RoundedRect) {
      path.addRoundedRect(pixRect, d->m_CardPixmapBorderRadius, d->m_CardPixmapBorderRadius);
      painter.setClipPath(path);
      painter.drawPixmap(pixRect, d->m_CardPixmap, d->m_CardPixmap.rect());
    }
    painter.restore();
  }

  // 文字绘制
  painter.save();
  QFont font = this->font();
  font.setWeight(QFont::Bold);
  font.setPixelSize(d->m_TitlePixelSize);
  painter.setFont(font);
  painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
  painter.drawText(QRect(pixRect.x(), pixRect.bottom() + d->m_TitleSpacing, width - width / 7, height() / 3), Qt::AlignLeft | Qt::AlignTop | Qt::TextSingleLine, d->m_Title);
  font.setWeight(QFont::Normal);
  font.setPixelSize(d->m_SubTitlePixelSize);
  painter.setFont(font);
  painter.setPen(NXThemeColor(d->m_ThemeMode, BasicDetailsText));
  int titleHeight = painter.fontMetrics().boundingRect(d->m_Title).height() * 1.1;
  painter.drawText(QRect(pixRect.x(), d->m_SubTitleSpacing + titleHeight + pixRect.bottom() + d->m_TitleSpacing, width - width / 7, height() / 3), Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, d->m_SubTitle);
  painter.restore();

  // 图标绘制
  painter.save();
  QFont iconFont = QFont(QStringLiteral("NXAwesome"));
  iconFont.setPixelSize(nxApp->getFontPixelSize());
  painter.setFont(iconFont);
  painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
  painter.drawText(width - 1.5 * iconFont.pixelSize(), height() - iconFont.pixelSize(), QChar((unsigned short) NXIconType::UpRightFromSquare));
  painter.restore();
}
