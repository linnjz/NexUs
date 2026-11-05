#include "NXThemeAnimationWidget.h"

#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>

NXThemeAnimationWidget::NXThemeAnimationWidget(QWidget *parent)
    : QWidget { parent }
    , m_Radius {}
    , m_EndRadius { 0.01 }
    , m_Center {}
{
}

NXThemeAnimationWidget::~NXThemeAnimationWidget()
{
}

void NXThemeAnimationWidget::startAnimation(int msec)
{
  QPropertyAnimation *themeChangeAnimation = new QPropertyAnimation(this, "Radius");
  themeChangeAnimation->setDuration(msec);
  themeChangeAnimation->setEasingCurve(QEasingCurve::InOutSine);
  connect(themeChangeAnimation, &QPropertyAnimation::finished, this, [=]() {
    Q_EMIT animationFinished();
    this->deleteLater();
  });
  connect(themeChangeAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
    update();
  });
  themeChangeAnimation->setStartValue(0);
  themeChangeAnimation->setEndValue(m_EndRadius);
  themeChangeAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  show();
}

void NXThemeAnimationWidget::paintEvent(QPaintEvent *event)
{
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
  painter.setPen(Qt::NoPen);

  // 合成图片
  QImage animationImage(m_OldWindowBackground.size(), QImage::Format_ARGB32);
  animationImage.fill(Qt::transparent);
  QPainter animationImagePainter(&animationImage);
  animationImagePainter.setRenderHints(QPainter::Antialiasing);
  animationImagePainter.drawImage(m_OldWindowBackground.rect(), m_OldWindowBackground);
  animationImagePainter.setCompositionMode(QPainter::CompositionMode::CompositionMode_SourceOut);
  qreal devicePixelRatioF = m_OldWindowBackground.devicePixelRatioF();
  QPainterPath clipPath;
  clipPath.moveTo(m_Center.x() * devicePixelRatioF, m_Center.y() * devicePixelRatioF);
  clipPath.addEllipse(QPointF(m_Center.x() * devicePixelRatioF, m_Center.y() * devicePixelRatioF), m_Radius * devicePixelRatioF, m_Radius * devicePixelRatioF);
  animationImagePainter.setClipPath(clipPath);
  animationImagePainter.drawImage(animationImage.rect(), animationImage);
  animationImagePainter.end();

  painter.drawImage(rect(), animationImage);
  painter.restore();
}
