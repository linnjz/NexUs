#include "NXPopularCardFloater.h"

#include <QEvent>
#include <QGraphicsOpacityEffect>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>

#include "NXApplication.h"
#include "NXPopularCard.h"
#include "NXPushButton.h"
#include "NXTheme.h"
#include "private/NXPopularCardPrivate.h"

NXPopularCardFloater::NXPopularCardFloater(NXPopularCard *card, NXPopularCardPrivate *cardPrivate, QWidget *parent)
    : QWidget { parent }
    , m_HoverYOffset { 0.0 }
    , m_HoverOpacity { 0.0 }
    , m_ThemeMode { nxTheme->getThemeMode() }
    , m_OverButton { new NXPushButton(QStringLiteral("获取"), this) }
    , m_OpacityEffect { new QGraphicsOpacityEffect(m_OverButton) }
    , m_Card { card }
    , m_CardPrivate { cardPrivate }
{
  setObjectName("NXPopularCardFloater");
  setStyleSheet(QStringLiteral("#NXPopularCardFloater{background-color:transparent}"));
  setMouseTracking(true);

  m_OpacityEffect->setOpacity(1);
  m_OverButton->setGraphicsEffect(m_OpacityEffect);
  m_OverButton->move(0, 0);
  m_OverButton->setLightDefaultColor(NXThemeColor(NXThemeType::Light, PrimaryNormal));
  m_OverButton->setLightHoverColor(NXThemeColor(NXThemeType::Light, PrimaryHover));
  m_OverButton->setLightPressColor(NXThemeColor(NXThemeType::Light, PrimaryPress));
  m_OverButton->setLightTextColor(Qt::white);
  m_OverButton->setDarkDefaultColor(NXThemeColor(NXThemeType::Dark, PrimaryNormal));
  m_OverButton->setDarkHoverColor(NXThemeColor(NXThemeType::Dark, PrimaryHover));
  m_OverButton->setDarkPressColor(NXThemeColor(NXThemeType::Dark, PrimaryPress));
  m_OverButton->setDarkTextColor(Qt::white);
  m_OverButton->setMinimumSize(0, 0);
  m_OverButton->setMaximumSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX));
  connect(m_OverButton, &NXPushButton::clicked, m_Card, &NXPopularCard::popularCardButtonClicked);

  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    m_ThemeMode = themeMode;
  });
  setVisible(false);
}

NXPopularCardFloater::~NXPopularCardFloater()
{
}

void NXPopularCardFloater::showFloater()
{
  m_IsHideAnimationFinished = true;
  m_HoverYOffset           = 6;
  m_HoverOpacity           = 1;
  m_OpacityEffect->setOpacity(1);

  QPropertyAnimation *geometryAnimation = new QPropertyAnimation(this, "geometry");
  connect(geometryAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
    update();
  });
  geometryAnimation->setEasingCurve(QEasingCurve::OutQuad);
  geometryAnimation->setDuration(300);
  QRect cardGeometry  = QRect(m_Card->mapTo(m_CardPrivate->m_CardFloatArea, QPoint(0, 0)), m_Card->size());
  QRect endGeometry   = _calculateTargetGeometry(cardGeometry);
  QRect startGeometry = QRect(endGeometry.x() + m_FloatGeometryOffset, endGeometry.y() + m_FloatGeometryOffset, m_Card->width(), m_Card->height());
  geometryAnimation->setStartValue(startGeometry);
  geometryAnimation->setEndValue(endGeometry);
  geometryAnimation->start(QAbstractAnimation::DeleteWhenStopped);

  //Button动画
  QPropertyAnimation *buttonAnimation = new QPropertyAnimation(m_OverButton, "geometry");
  buttonAnimation->setEasingCurve(QEasingCurve::OutQuad);
  buttonAnimation->setDuration(300);
  buttonAnimation->setStartValue(m_CardPrivate->m_InteractiveTipsBaseRect);
  buttonAnimation->setEndValue(m_CardPrivate->m_ButtonTargetRect);
  buttonAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  setVisible(true);
}

void NXPopularCardFloater::hideFloater()
{
  if (!m_IsHideAnimationFinished) {
    return;
  }
  m_IsHideAnimationFinished              = false;
  QPropertyAnimation *geometryAnimation = new QPropertyAnimation(this, "geometry");
  connect(geometryAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
    QRect endGeometry = QRect(m_Card->mapTo(m_CardPrivate->m_CardFloatArea, QPoint(0, 0)), m_Card->size());
    geometryAnimation->setEndValue(endGeometry);
  });
  connect(geometryAnimation, &QPropertyAnimation::finished, this, [=]() {
    m_CardPrivate->m_IsFloating = false;
    setVisible(false);
    m_Card->update();
  });
  geometryAnimation->setEasingCurve(QEasingCurve::InOutSine);
  geometryAnimation->setDuration(300);
  QRect endGeometry = QRect(m_Card->mapTo(m_CardPrivate->m_CardFloatArea, QPoint(0, 0)), m_Card->size());
  geometryAnimation->setStartValue(geometry());
  geometryAnimation->setEndValue(endGeometry);
  geometryAnimation->start(QAbstractAnimation::DeleteWhenStopped);

  QPropertyAnimation *opacityAnimation = new QPropertyAnimation(this, "HoverOpacity");
  opacityAnimation->setDuration(300);
  opacityAnimation->setStartValue(m_HoverOpacity);
  opacityAnimation->setEndValue(0);
  opacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);

  //内容调整动画
  QPropertyAnimation *hoverAnimation = new QPropertyAnimation(this, "HoverYOffset");
  connect(hoverAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
    update();
  });
  hoverAnimation->setDuration(300);
  hoverAnimation->setStartValue(m_HoverYOffset);
  hoverAnimation->setEndValue(0);
  hoverAnimation->start(QAbstractAnimation::DeleteWhenStopped);

  //Button动画
  QPropertyAnimation *buttonAnimation = new QPropertyAnimation(m_OverButton, "geometry");
  buttonAnimation->setEasingCurve(QEasingCurve::InOutSine);
  buttonAnimation->setDuration(300);
  buttonAnimation->setStartValue(m_OverButton->geometry());
  QRectF endRect = m_CardPrivate->m_InteractiveTipsBaseRect;
  endRect.adjust(0, 6, 0, 6);
  buttonAnimation->setEndValue(endRect);
  buttonAnimation->start(QAbstractAnimation::DeleteWhenStopped);

  QPropertyAnimation *buttonOpacityAnimation = new QPropertyAnimation(m_OverButton->graphicsEffect(), "opacity");
  buttonOpacityAnimation->setEasingCurve(QEasingCurve::InOutSine);
  buttonOpacityAnimation->setDuration(350);
  buttonOpacityAnimation->setStartValue(1);
  buttonOpacityAnimation->setEndValue(0);
  buttonOpacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

bool NXPopularCardFloater::event(QEvent *event)
{
  switch (event->type()) {
  case QEvent::Leave : {
    hideFloater();
    break;
  }
  case QEvent::MouseButtonRelease : {
    Q_EMIT m_Card->popularCardClicked();
    break;
  }
  default : {
    break;
  }
  }
  return QWidget::event(event);
}

void NXPopularCardFloater::paintEvent(QPaintEvent *event)
{
  int const fontPixelSize = nxApp->getFontPixelSize();
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing | QPainter::TextAntialiasing);
  // 阴影绘制
  painter.setOpacity(m_HoverOpacity);
  QRect shadowRect = rect();
  shadowRect.adjust(0, 0, 0, -m_HoverYOffset);
  nxTheme->drawEffectShadow(&painter, shadowRect, m_CardPrivate->m_ShadowBorderWidth, m_CardPrivate->m_BorderRadius);
  QRectF foregroundRect(m_CardPrivate->m_ShadowBorderWidth, m_CardPrivate->m_ShadowBorderWidth - m_HoverYOffset + 1, width() - 2 * m_CardPrivate->m_ShadowBorderWidth, height() - 2 * m_CardPrivate->m_ShadowBorderWidth);
  QRectF cardForegroundRect(
      m_CardPrivate->m_ShadowBorderWidth, m_CardPrivate->m_ShadowBorderWidth - m_CardPrivate->m_HoverYOffset + 1,
      m_Card->width() - 2 * m_CardPrivate->m_ShadowBorderWidth, m_Card->height() - 2 * m_CardPrivate->m_ShadowBorderWidth);

  //背景绘制
  painter.setOpacity(1);
  painter.setPen(NXThemeColor(m_ThemeMode, PopupBorderHover));
  painter.setBrush(NXThemeColor(m_ThemeMode, DialogBase));
  painter.drawRoundedRect(foregroundRect, m_CardPrivate->m_BorderRadius, m_CardPrivate->m_BorderRadius);
  painter.setClipRect(foregroundRect);
  //图片绘制
  painter.save();
  QRectF pixRect(foregroundRect.x() + cardForegroundRect.height() * 0.15, foregroundRect.y() + cardForegroundRect.height() * 0.15, cardForegroundRect.height() * 0.7, cardForegroundRect.height() * 0.7);

  QPainterPath pixPath;
  pixPath.addRoundedRect(pixRect, 4, 4);
  painter.setClipPath(pixPath);
  painter.drawPixmap(pixRect, m_CardPrivate->m_CardPixmap, m_CardPrivate->m_CardPixmap.rect());
  painter.restore();

  //文字绘制
  //Title
  painter.setPen(NXThemeColor(m_ThemeMode, BasicText));
  QFont font = painter.font();
  font.setWeight(QFont::Bold);
  font.setPixelSize(fontPixelSize + 2);
  painter.setFont(font);
  int titleHeight = painter.fontMetrics().height();
  QRectF titleRect(pixRect.right() + m_CardPrivate->m_TextHSpacing, pixRect.y(), m_FloatGeometryOffset * 2 + cardForegroundRect.width() - pixRect.width() - m_CardPrivate->m_TextHSpacing * 2 - cardForegroundRect.height() * 0.15 - m_CardPrivate->m_ButtonTargetRect.width(), titleHeight);
  QString titleText = painter.fontMetrics().elidedText(m_CardPrivate->m_Title, Qt::ElideRight, titleRect.width());
  painter.drawText(titleRect, Qt::AlignLeft | Qt::AlignTop | Qt::TextSingleLine, titleText);

  //SubTitle
  font.setWeight(QFont::DemiBold);
  font.setPixelSize(fontPixelSize);
  painter.setFont(font);
  int subTitleHeight = painter.fontMetrics().height();
  QRectF subTitleRect(pixRect.right() + m_CardPrivate->m_TextHSpacing, titleRect.bottom() + m_CardPrivate->m_TextVSpacing, m_FloatGeometryOffset * 2 + cardForegroundRect.width() - pixRect.width() - m_CardPrivate->m_TextHSpacing * 2 - cardForegroundRect.height() * 0.15 - m_CardPrivate->m_ButtonTargetRect.width(), subTitleHeight);
  QString subTitleText =
      painter.fontMetrics().elidedText(m_CardPrivate->m_SubTitle, Qt::ElideRight, subTitleRect.width());
  painter.drawText(subTitleRect, Qt::AlignLeft | Qt::AlignTop | Qt::TextSingleLine, subTitleText);

  //DetailedText
  painter.setPen(NXThemeColor(m_ThemeMode, BasicDetailsText));
  int detailedTextHeight = painter.fontMetrics().height() * 2 + 2;
  QRectF detailedTextRect(pixRect.x(), pixRect.bottom() + cardForegroundRect.height() * 0.15, cardForegroundRect.width() + 2 * m_FloatGeometryOffset - m_CardPrivate->m_TextHSpacing - cardForegroundRect.height() * 0.15, detailedTextHeight);
  QString detailedText =
      painter.fontMetrics().elidedText(m_CardPrivate->m_DetailedText, Qt::ElideRight, detailedTextRect.width() * 1.9);
  painter.drawText(detailedTextRect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, detailedText);

  //分割线绘制
  painter.setPen(NXThemeColor(m_ThemeMode, BasicBaseLine));
  painter.drawLine(foregroundRect.x(), detailedTextRect.bottom() + 5, foregroundRect.right(), detailedTextRect.bottom() + 5);

  //CardFloatPixmap
  painter.drawPixmap(QRect(pixRect.x(), detailedTextRect.bottom() + 15, cardForegroundRect.bottom() + m_FloatGeometryOffset + 90 - detailedTextRect.bottom() - 15 - m_CardPrivate->m_ShadowBorderWidth - 10, cardForegroundRect.bottom() + m_FloatGeometryOffset + 90 - detailedTextRect.bottom() - 15 - m_CardPrivate->m_ShadowBorderWidth - 10), m_CardPrivate->m_CardFloatPixmap);
  painter.restore();
}

QRect NXPopularCardFloater::_calculateTargetGeometry(QRect cardGeometry)
{
  QRect targetGeometry = cardGeometry;
  targetGeometry.adjust(-m_FloatGeometryOffset, -m_FloatGeometryOffset, m_FloatGeometryOffset, 90);
  QRect windowRect = m_CardPrivate->m_CardFloatArea->rect();
  if (targetGeometry.bottom() > windowRect.bottom()) {
    int offset = targetGeometry.bottom() - windowRect.bottom();
    targetGeometry.adjust(0, -offset, 0, -offset);
  }
  if (targetGeometry.top() < windowRect.top()) {
    int offset = targetGeometry.top() - windowRect.top();
    targetGeometry.adjust(0, -offset, 0, -offset);
  }
  return targetGeometry;
}
