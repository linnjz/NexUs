#include "NXReminderCard.h"

#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QPainterPath>

#include "NXTheme.h"
#include "private/NXReminderCardPrivate.h"
Q_PROPERTY_CREATE_CPP(NXReminderCard, QPS_SCREF_PAEMIT(QString), Title)
Q_PROPERTY_CREATE_CPP(NXReminderCard, QPS_SCREF_PAEMIT(QString), SubTitle)
Q_PROPERTY_CREATE_CPP(NXReminderCard, QPS_SCREF_PAEMIT(QPixmap), CardPixmap)
Q_PROPERTY_CREATE_CPP(NXReminderCard, QPS_PAEMIT(QSize), CardPixmapSize)
Q_PROPERTY_CREATE_CPP(NXReminderCard, QPS_PAEMIT(int), BorderRadius)
Q_PROPERTY_CREATE_CPP(NXReminderCard, QPS_PAEMIT(int), TitlePixelSize)
Q_PROPERTY_CREATE_CPP(NXReminderCard, QPS_PAEMIT(int), SubTitlePixelSize)
Q_PROPERTY_CREATE_CPP(NXReminderCard, QPS_PAEMIT(int), TitleSpacing)
Q_PROPERTY_CREATE_CPP(NXReminderCard, QPS_PAEMIT(int), CardPixmapBorderRadius)
Q_PROPERTY_CREATE_CPP(NXReminderCard, QPS_PAEMIT(NXCardPixType::PixMode), CardPixMode)

NXReminderCard::NXReminderCard(QWidget *parent)
    : QPushButton { parent }
    , d_ptr(new NXReminderCardPrivate())
{
  Q_D(NXReminderCard);
  d->q_ptr               = this;
  d->m_BorderRadius      = 6;
  d->m_TitlePixelSize    = 15;
  d->m_SubTitlePixelSize = 12;
  setFixedSize(330, 105);
  d->m_CardPixmapSize         = QSize(79, 79);
  d->m_TitleSpacing           = 3;
  d->m_CardPixmapBorderRadius = 6;
  d->m_CardPixMode            = NXCardPixType::PixMode::Default;
  d->m_ThemeMode               = nxTheme->getThemeMode();
  setMouseTracking(true);
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
  });
}

NXReminderCard::~NXReminderCard()
{
}

void NXReminderCard::setCardPixmapSize(int width, int height)
{
  Q_D(NXReminderCard);
  d->m_CardPixmapSize = QSize(width, height);
  Q_EMIT signalCardPixmapSizeChanged();
}

void NXReminderCard::paintEvent(QPaintEvent *event)
{
  Q_D(NXReminderCard);
  QPainter painter(this);
  painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing | QPainter::TextAntialiasing);
  // 高性能阴影
  nxTheme->drawEffectShadow(&painter, rect(), d->m_ShadowBorderWidth, d->m_BorderRadius);

  // 背景绘制
  painter.save();
  painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing | QPainter::TextAntialiasing);
  painter.setPen(NXThemeColor(d->m_ThemeMode, BasicBorder));
  painter.setBrush(underMouse() ? NXThemeColor(d->m_ThemeMode, BasicHoverAlpha) : NXThemeColor(d->m_ThemeMode, BasicBaseAlpha));
  QRect foregroundRect(d->m_ShadowBorderWidth, d->m_ShadowBorderWidth, width() - 2 * d->m_ShadowBorderWidth, height() - 2 * d->m_ShadowBorderWidth);
  int width = foregroundRect.width();
  painter.drawRoundedRect(foregroundRect, d->m_BorderRadius, d->m_BorderRadius);

  // 图片绘制
  int pixLeftMargin = width / 28;
  if (!d->m_CardPixmap.isNull()) {
    painter.save();
    QPainterPath path;
    if (d->m_CardPixMode == NXCardPixType::PixMode::Ellipse) {
      path.addEllipse(QPointF(d->m_CardPixmapSize.width() / 2 + pixLeftMargin, height() / 2), d->m_CardPixmapSize.width() / 2, d->m_CardPixmapSize.height() / 2);
      painter.setClipPath(path);
      painter.drawPixmap(QRect(pixLeftMargin + d->m_ShadowBorderWidth, (height() - d->m_CardPixmapSize.height()) / 2, d->m_CardPixmapSize.width(), d->m_CardPixmapSize.height()),
                         d->m_CardPixmap); // rect为绘制区域，image为要绘制的图片
    } else if (d->m_CardPixMode == NXCardPixType::PixMode::Default) {
      painter.drawPixmap(pixLeftMargin + d->m_ShadowBorderWidth, (height() - d->m_CardPixmapSize.height()) / 2, d->m_CardPixmapSize.width(), d->m_CardPixmapSize.height(), d->m_CardPixmap);
    } else if (d->m_CardPixMode == NXCardPixType::PixMode::RoundedRect) {
      path.addRoundedRect(QRectF(pixLeftMargin, (height() - d->m_CardPixmapSize.height()) / 2, d->m_CardPixmapSize.width(), d->m_CardPixmapSize.height()), d->m_CardPixmapBorderRadius, d->m_CardPixmapBorderRadius);
      painter.setClipPath(path);
      painter.drawPixmap(pixLeftMargin + d->m_ShadowBorderWidth, (height() - d->m_CardPixmapSize.height()) / 2, d->m_CardPixmapSize.width(), d->m_CardPixmapSize.height(), d->m_CardPixmap);
    }
    painter.restore();
  }

  // 焦点圆型绘制
  painter.save();
  painter.setBrush(NXThemeColor(d->m_ThemeMode, PrimaryNormal));
  painter.drawEllipse(QPointF(width * 0.95 + d->m_ShadowBorderWidth, height() * 0.25), height() / 17, height() / 17);
  painter.restore();

  // 文字绘制
  QFont font = this->font();
  font.setWeight(QFont::Bold);
  font.setPixelSize(d->m_TitlePixelSize);
  painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
  painter.setFont(font);
  // 主标题
  int textLeftMargin = width / 11;
  int textStartX     = d->m_CardPixmapSize.width() + textLeftMargin + d->m_ShadowBorderWidth;
  int textWidth      = foregroundRect.width() - textStartX - d->m_ShadowBorderWidth - 5;
  painter.drawText(QRect(textStartX, foregroundRect.y(), textWidth, this->height() / 2 - d->m_TitleSpacing), Qt::AlignLeft | Qt::AlignBottom | Qt::TextWordWrap | Qt::TextWrapAnywhere, d->m_Title);
  // 副标题
  font.setWeight(QFont::Normal);
  font.setPixelSize(d->m_SubTitlePixelSize);
  painter.setFont(font);
  painter.setPen(NXThemeColor(d->m_ThemeMode, BasicDetailsText));
  painter.drawText(QRect(textStartX, height() / 2 + d->m_TitleSpacing, textWidth, height() / 2 - d->m_TitleSpacing), Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap | Qt::TextWrapAnywhere, d->m_SubTitle);
  painter.restore();
}
