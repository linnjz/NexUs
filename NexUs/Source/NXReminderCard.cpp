#include "NXReminderCard.h"

#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QPainterPath>

#include "NXTheme.h"
#include "private/NXReminderCardPrivate.h"
Q_PROPERTY_CREATE_CPP(NXReminderCard, QS_SET_CREF(QString), Title)
Q_PROPERTY_CREATE_CPP(NXReminderCard, QS_SET_CREF(QString), SubTitle)
Q_PROPERTY_CREATE_CPP(NXReminderCard, QS_SET_CREF(QPixmap), CardPixmap)
Q_PROPERTY_CREATE_CPP(NXReminderCard, QSize, CardPixmapSize)
Q_PROPERTY_CREATE_CPP(NXReminderCard, int, BorderRadius)
Q_PROPERTY_CREATE_CPP(NXReminderCard, int, TitlePixelSize)
Q_PROPERTY_CREATE_CPP(NXReminderCard, int, SubTitlePixelSize)
Q_PROPERTY_CREATE_CPP(NXReminderCard, int, TitleSpacing)
Q_PROPERTY_CREATE_CPP(NXReminderCard, int, CardPixmapBorderRadius)
Q_PROPERTY_CREATE_CPP(NXReminderCard, NXCardPixType::PixMode, CardPixMode)

NXReminderCard::NXReminderCard(QWidget *parent)
    : QPushButton { parent }
    , d_ptr(new NXReminderCardPrivate())
{
  Q_D(NXReminderCard);
  d->q_ptr               = this;
  d->_pBorderRadius      = 6;
  d->_pTitlePixelSize    = 15;
  d->_pSubTitlePixelSize = 12;
  setFixedSize(330, 105);
  d->_pCardPixmapSize         = QSize(79, 79);
  d->_pTitleSpacing           = 3;
  d->_pCardPixmapBorderRadius = 6;
  d->_pCardPixMode            = NXCardPixType::PixMode::Default;
  d->_themeMode               = nxTheme->getThemeMode();
  setMouseTracking(true);
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
  });
}

NXReminderCard::~NXReminderCard()
{
}

void NXReminderCard::setCardPixmapSize(int width, int height)
{
  Q_D(NXReminderCard);
  d->_pCardPixmapSize = QSize(width, height);
  Q_EMIT pCardPixmapSizeChanged();
}

void NXReminderCard::paintEvent(QPaintEvent *event)
{
  Q_D(NXReminderCard);
  QPainter painter(this);
  painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing | QPainter::TextAntialiasing);
  // 高性能阴影
  nxTheme->drawEffectShadow(&painter, rect(), d->_shadowBorderWidth, d->_pBorderRadius);

  // 背景绘制
  painter.save();
  painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing | QPainter::TextAntialiasing);
  painter.setPen(NXThemeColor(d->_themeMode, BasicBorder));
  painter.setBrush(underMouse() ? NXThemeColor(d->_themeMode, BasicHoverAlpha) : NXThemeColor(d->_themeMode, BasicBaseAlpha));
  QRect foregroundRect(d->_shadowBorderWidth, d->_shadowBorderWidth, width() - 2 * d->_shadowBorderWidth, height() - 2 * d->_shadowBorderWidth);
  int width = foregroundRect.width();
  painter.drawRoundedRect(foregroundRect, d->_pBorderRadius, d->_pBorderRadius);

  // 图片绘制
  int pixLeftMargin = width / 28;
  if (!d->_pCardPixmap.isNull()) {
    painter.save();
    QPainterPath path;
    if (d->_pCardPixMode == NXCardPixType::PixMode::Ellipse) {
      path.addEllipse(QPointF(d->_pCardPixmapSize.width() / 2 + pixLeftMargin, height() / 2), d->_pCardPixmapSize.width() / 2, d->_pCardPixmapSize.height() / 2);
      painter.setClipPath(path);
      painter.drawPixmap(QRect(pixLeftMargin + d->_shadowBorderWidth, (height() - d->_pCardPixmapSize.height()) / 2, d->_pCardPixmapSize.width(), d->_pCardPixmapSize.height()),
                         d->_pCardPixmap); // rect为绘制区域，image为要绘制的图片
    } else if (d->_pCardPixMode == NXCardPixType::PixMode::Default) {
      painter.drawPixmap(pixLeftMargin + d->_shadowBorderWidth, (height() - d->_pCardPixmapSize.height()) / 2, d->_pCardPixmapSize.width(), d->_pCardPixmapSize.height(), d->_pCardPixmap);
    } else if (d->_pCardPixMode == NXCardPixType::PixMode::RoundedRect) {
      path.addRoundedRect(QRectF(pixLeftMargin, (height() - d->_pCardPixmapSize.height()) / 2, d->_pCardPixmapSize.width(), d->_pCardPixmapSize.height()), d->_pCardPixmapBorderRadius, d->_pCardPixmapBorderRadius);
      painter.setClipPath(path);
      painter.drawPixmap(pixLeftMargin + d->_shadowBorderWidth, (height() - d->_pCardPixmapSize.height()) / 2, d->_pCardPixmapSize.width(), d->_pCardPixmapSize.height(), d->_pCardPixmap);
    }
    painter.restore();
  }

  // 焦点圆型绘制
  painter.save();
  painter.setBrush(NXThemeColor(d->_themeMode, PrimaryNormal));
  painter.drawEllipse(QPointF(width * 0.95 + d->_shadowBorderWidth, height() * 0.25), height() / 17, height() / 17);
  painter.restore();

  // 文字绘制
  QFont font = this->font();
  font.setWeight(QFont::Bold);
  font.setPixelSize(d->_pTitlePixelSize);
  painter.setPen(NXThemeColor(d->_themeMode, BasicText));
  painter.setFont(font);
  // 主标题
  int textLeftMargin = width / 11;
  int textStartX     = d->_pCardPixmapSize.width() + textLeftMargin + d->_shadowBorderWidth;
  int textWidth      = foregroundRect.width() - textStartX - d->_shadowBorderWidth - 5;
  painter.drawText(QRect(textStartX, foregroundRect.y(), textWidth, this->height() / 2 - d->_pTitleSpacing), Qt::AlignLeft | Qt::AlignBottom | Qt::TextWordWrap | Qt::TextWrapAnywhere, d->_pTitle);
  // 副标题
  font.setWeight(QFont::Normal);
  font.setPixelSize(d->_pSubTitlePixelSize);
  painter.setFont(font);
  painter.setPen(NXThemeColor(d->_themeMode, BasicDetailsText));
  painter.drawText(QRect(textStartX, height() / 2 + d->_pTitleSpacing, textWidth, height() / 2 - d->_pTitleSpacing), Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap | Qt::TextWrapAnywhere, d->_pSubTitle);
  painter.restore();
}
