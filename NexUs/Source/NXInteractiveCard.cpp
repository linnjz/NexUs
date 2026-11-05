#include "NXInteractiveCard.h"

#include <QPainter>
#include <QPainterPath>

#include "NXTheme.h"
#include "private/NXInteractiveCardPrivate.h"
Q_PROPERTY_CREATE_CPP(NXInteractiveCard, QPS_SCREF_PAEMIT(QString), Title)
Q_PROPERTY_CREATE_CPP(NXInteractiveCard, QPS_SCREF_PAEMIT(QString), SubTitle)
Q_PROPERTY_CREATE_CPP(NXInteractiveCard, QPS_SCREF_PAEMIT(QPixmap), CardPixmap)
Q_PROPERTY_CREATE_CPP(NXInteractiveCard, QPS_PAEMIT(QSize), CardPixmapSize)
Q_PROPERTY_CREATE_CPP(NXInteractiveCard, QPS_PAEMIT(int), BorderRadius)
Q_PROPERTY_CREATE_CPP(NXInteractiveCard, QPS_PAEMIT(int), TitlePixelSize)
Q_PROPERTY_CREATE_CPP(NXInteractiveCard, QPS_PAEMIT(int), SubTitlePixelSize)
Q_PROPERTY_CREATE_CPP(NXInteractiveCard, QPS_PAEMIT(int), TitleSpacing)
Q_PROPERTY_CREATE_CPP(NXInteractiveCard, QPS_PAEMIT(int), CardPixmapBorderRadius)
Q_PROPERTY_CREATE_CPP(NXInteractiveCard, QPS_PAEMIT(NXCardPixType::PixMode), CardPixMode)

NXInteractiveCard::NXInteractiveCard(QWidget *parent)
    : QPushButton(parent)
    , d_ptr(new NXInteractiveCardPrivate())
{
  Q_D(NXInteractiveCard);
  d->q_ptr               = this;
  d->m_BorderRadius      = 6;
  d->m_TitlePixelSize    = 15;
  d->m_SubTitlePixelSize = 12;
  setMinimumSize(270, 80);
  d->m_CardPixmapSize         = QSize(64, 64);
  d->m_TitleSpacing           = 2;
  d->m_CardPixmapBorderRadius = 6;
  d->m_CardPixMode            = NXCardPixType::PixMode::Ellipse;
  d->m_ThemeMode               = nxTheme->getThemeMode();
  setMouseTracking(true);
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
  });
}

NXInteractiveCard::~NXInteractiveCard()
{
}

void NXInteractiveCard::setCardPixmapSize(int width, int height)
{
  Q_D(NXInteractiveCard);
  d->m_CardPixmapSize = QSize(width, height);
  Q_EMIT signalCardPixmapSizeChanged();
}

void NXInteractiveCard::paintEvent(QPaintEvent *event)
{
  Q_D(NXInteractiveCard);
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing | QPainter::TextAntialiasing);
  painter.setPen(Qt::NoPen);
  painter.setBrush(underMouse() ? NXThemeColor(d->m_ThemeMode, BasicHoverAlpha) : Qt::transparent);
  painter.drawRoundedRect(rect(), d->m_BorderRadius, d->m_BorderRadius);
  // 图片绘制
  if (!d->m_CardPixmap.isNull()) {
    painter.save();
    QPainterPath path;
    if (d->m_CardPixMode == NXCardPixType::PixMode::Ellipse) {
      path.addEllipse(QPointF(d->m_CardPixmapSize.width() / 2 + 10, height() / 2), d->m_CardPixmapSize.width() / 2, d->m_CardPixmapSize.height() / 2);
      painter.setClipPath(path);
      painter.drawPixmap(QRect(10, (height() - d->m_CardPixmapSize.height()) / 2, d->m_CardPixmapSize.width(), d->m_CardPixmapSize.height()),
                         d->m_CardPixmap); // rect为绘制区域，image为要绘制的图片
    } else if (d->m_CardPixMode == NXCardPixType::PixMode::Default) {
      painter.drawPixmap(10, (height() - d->m_CardPixmapSize.height()) / 2, d->m_CardPixmapSize.width(), d->m_CardPixmapSize.height(), d->m_CardPixmap);
    } else if (d->m_CardPixMode == NXCardPixType::PixMode::RoundedRect) {
      path.addRoundedRect(QRectF(10, (height() - d->m_CardPixmapSize.height()) / 2, d->m_CardPixmapSize.width(), d->m_CardPixmapSize.height()), d->m_CardPixmapBorderRadius, d->m_CardPixmapBorderRadius);
      painter.setClipPath(path);
      painter.drawPixmap(10, (height() - d->m_CardPixmapSize.height()) / 2, d->m_CardPixmapSize.width(), d->m_CardPixmapSize.height(), d->m_CardPixmap);
    }
    painter.restore();
  }
  // 文字绘制
  painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
  QFont font = this->font();
  font.setWeight(QFont::Bold);
  font.setPixelSize(d->m_TitlePixelSize);
  painter.setFont(font);
  int textStartX = d->m_CardPixmapSize.width() + 26;
  int textWidth  = width() - textStartX;
  painter.drawText(QRect(textStartX, rect().y(), textWidth, height() / 2 - d->m_TitleSpacing), Qt::TextWordWrap | Qt::AlignBottom | Qt::AlignLeft, d->m_Title);
  font.setWeight(QFont::Normal);
  font.setPixelSize(d->m_SubTitlePixelSize);
  painter.setFont(font);
  painter.drawText(QRect(textStartX, height() / 2 + d->m_TitleSpacing, textWidth, height() / 2 - d->m_TitleSpacing), Qt::TextWordWrap | Qt::AlignTop | Qt::AlignLeft, d->m_SubTitle);
  painter.restore();
}
