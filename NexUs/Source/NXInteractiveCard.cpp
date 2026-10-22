#include "NXInteractiveCard.h"

#include <QPainter>
#include <QPainterPath>

#include "NXTheme.h"
#include "private/NXInteractiveCardPrivate.h"
Q_PROPERTY_CREATE_CPP(NXInteractiveCard, QS_SET_CREF(QString), Title)
Q_PROPERTY_CREATE_CPP(NXInteractiveCard, QS_SET_CREF(QString), SubTitle)
Q_PROPERTY_CREATE_CPP(NXInteractiveCard, QS_SET_CREF(QPixmap), CardPixmap)
Q_PROPERTY_CREATE_CPP(NXInteractiveCard, QSize, CardPixmapSize)
Q_PROPERTY_CREATE_CPP(NXInteractiveCard, int, BorderRadius)
Q_PROPERTY_CREATE_CPP(NXInteractiveCard, int, TitlePixelSize)
Q_PROPERTY_CREATE_CPP(NXInteractiveCard, int, SubTitlePixelSize)
Q_PROPERTY_CREATE_CPP(NXInteractiveCard, int, TitleSpacing)
Q_PROPERTY_CREATE_CPP(NXInteractiveCard, int, CardPixmapBorderRadius)
Q_PROPERTY_CREATE_CPP(NXInteractiveCard, NXCardPixType::PixMode, CardPixMode)

NXInteractiveCard::NXInteractiveCard(QWidget *parent)
    : QPushButton(parent)
    , d_ptr(new NXInteractiveCardPrivate())
{
  Q_D(NXInteractiveCard);
  d->q_ptr               = this;
  d->_pBorderRadius      = 6;
  d->_pTitlePixelSize    = 15;
  d->_pSubTitlePixelSize = 12;
  setMinimumSize(270, 80);
  d->_pCardPixmapSize         = QSize(64, 64);
  d->_pTitleSpacing           = 2;
  d->_pCardPixmapBorderRadius = 6;
  d->_pCardPixMode            = NXCardPixType::PixMode::Ellipse;
  d->_themeMode               = nxTheme->getThemeMode();
  setMouseTracking(true);
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
  });
}

NXInteractiveCard::~NXInteractiveCard()
{
}

void NXInteractiveCard::setCardPixmapSize(int width, int height)
{
  Q_D(NXInteractiveCard);
  d->_pCardPixmapSize = QSize(width, height);
  Q_EMIT pCardPixmapSizeChanged();
}

void NXInteractiveCard::paintEvent(QPaintEvent *event)
{
  Q_D(NXInteractiveCard);
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing | QPainter::TextAntialiasing);
  painter.setPen(Qt::NoPen);
  painter.setBrush(underMouse() ? NXThemeColor(d->_themeMode, BasicHoverAlpha) : Qt::transparent);
  painter.drawRoundedRect(rect(), d->_pBorderRadius, d->_pBorderRadius);
  // 图片绘制
  if (!d->_pCardPixmap.isNull()) {
    painter.save();
    QPainterPath path;
    if (d->_pCardPixMode == NXCardPixType::PixMode::Ellipse) {
      path.addEllipse(QPointF(d->_pCardPixmapSize.width() / 2 + 10, height() / 2), d->_pCardPixmapSize.width() / 2, d->_pCardPixmapSize.height() / 2);
      painter.setClipPath(path);
      painter.drawPixmap(QRect(10, (height() - d->_pCardPixmapSize.height()) / 2, d->_pCardPixmapSize.width(), d->_pCardPixmapSize.height()),
                         d->_pCardPixmap); // rect为绘制区域，image为要绘制的图片
    } else if (d->_pCardPixMode == NXCardPixType::PixMode::Default) {
      painter.drawPixmap(10, (height() - d->_pCardPixmapSize.height()) / 2, d->_pCardPixmapSize.width(), d->_pCardPixmapSize.height(), d->_pCardPixmap);
    } else if (d->_pCardPixMode == NXCardPixType::PixMode::RoundedRect) {
      path.addRoundedRect(QRectF(10, (height() - d->_pCardPixmapSize.height()) / 2, d->_pCardPixmapSize.width(), d->_pCardPixmapSize.height()), d->_pCardPixmapBorderRadius, d->_pCardPixmapBorderRadius);
      painter.setClipPath(path);
      painter.drawPixmap(10, (height() - d->_pCardPixmapSize.height()) / 2, d->_pCardPixmapSize.width(), d->_pCardPixmapSize.height(), d->_pCardPixmap);
    }
    painter.restore();
  }
  // 文字绘制
  painter.setPen(NXThemeColor(d->_themeMode, BasicText));
  QFont font = this->font();
  font.setWeight(QFont::Bold);
  font.setPixelSize(d->_pTitlePixelSize);
  painter.setFont(font);
  int textStartX = d->_pCardPixmapSize.width() + 26;
  int textWidth  = width() - textStartX;
  painter.drawText(QRect(textStartX, rect().y(), textWidth, height() / 2 - d->_pTitleSpacing), Qt::TextWordWrap | Qt::AlignBottom | Qt::AlignLeft, d->_pTitle);
  font.setWeight(QFont::Normal);
  font.setPixelSize(d->_pSubTitlePixelSize);
  painter.setFont(font);
  painter.drawText(QRect(textStartX, height() / 2 + d->_pTitleSpacing, textWidth, height() / 2 - d->_pTitleSpacing), Qt::TextWordWrap | Qt::AlignTop | Qt::AlignLeft, d->_pSubTitle);
  painter.restore();
}
