#include "NXToolTip.h"

#include <QPainter>
#include <QVBoxLayout>

#include "NXText.h"
#include "NXTheme.h"
#include "private/NXToolTipPrivate.h"
Q_PROPERTY_CREATE_CPP(NXToolTip, int, BorderRadius)
Q_PROPERTY_CREATE_CPP(NXToolTip, int, DisplayMsec)
Q_PROPERTY_CREATE_CPP(NXToolTip, int, ShowDelayMsec)
Q_PROPERTY_CREATE_CPP(NXToolTip, int, HideDelayMsec)
Q_PROPERTY_CREATE_CPP(NXToolTip, int, OffSetX)
Q_PROPERTY_CREATE_CPP(NXToolTip, int, OffSetY)
Q_PROPERTY_CREATE_CPP(NXToolTip, bool, IsMoveEnable)

NXToolTip::NXToolTip(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXToolTipPrivate())
{
  Q_D(NXToolTip);
  d->q_ptr           = this;
  d->_pIsMoveEnable  = false;
  d->_pOffSetX       = 10;
  d->_pOffSetY       = 0;
  d->_pBorderRadius  = 5;
  d->_pDisplayMsec   = -1;
  d->_pShowDelayMsec = 0;
  d->_pHideDelayMsec = 0;
  d->_pCustomWidget  = nullptr;
  setObjectName("NXToolTip");
  if (parent) {
    parent->installEventFilter(d);
  }
  setAttribute(Qt::WA_TransparentForMouseEvents);
  setAttribute(Qt::WA_TranslucentBackground);
  setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);

  d->_toolTipText = new NXText(this);
  d->_toolTipText->setWordWrap(false);
  d->_toolTipText->setTextPixelSize(17);
  d->_mainLayout = new QVBoxLayout(this);
#if defined(Q_OS_WIN) && QT_VERSION == QT_VERSION_CHECK(6, 11, 0)
  d->_mainLayout->setContentsMargins(d->_shadowBorderWidth, d->_shadowBorderWidth, d->_shadowBorderWidth, d->_shadowBorderWidth);
#else
  d->_mainLayout->setContentsMargins(d->_shadowBorderWidth * 2, d->_shadowBorderWidth * 2, d->_shadowBorderWidth * 2, d->_shadowBorderWidth * 2);
#endif
  d->_mainLayout->addWidget(d->_toolTipText);

  d->_themeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
    update();
  });
  hide();
}

NXToolTip::~NXToolTip()
{
}

void NXToolTip::setToolTip(const QString &toolTip)
{
  Q_D(NXToolTip);
  d->_toolTipText->setText(toolTip);
  d->_toolTipText->adjustSize();
  d->_toolTipText->updateGeometry();
  d->_mainLayout->invalidate();
  d->_mainLayout->activate();
  setFixedSize(sizeHint());
  Q_EMIT pToolTipChanged();
}

QString NXToolTip::getToolTip() const
{
  Q_D(const NXToolTip);
  return d->_toolTipText->text();
}

void NXToolTip::setCustomWidget(QWidget *customWidget)
{
  Q_D(NXToolTip);
  if (!customWidget || customWidget == this) {
    return;
  }
  if (d->_pCustomWidget) {
    d->_mainLayout->removeWidget(d->_pCustomWidget);
    d->_pCustomWidget->deleteLater();
  }
  d->_toolTipText->hide();
  d->_mainLayout->addWidget(customWidget);
  d->_pCustomWidget = customWidget;
  Q_EMIT pCustomWidgetChanged();
}

QWidget * NXToolTip::getCustomWidget() const
{
  Q_D(const NXToolTip);
  return d->_pCustomWidget;
}

void NXToolTip::updatePos()
{
  Q_D(NXToolTip);
  d->_updatePos();
}

void NXToolTip::paintEvent(QPaintEvent *event)
{
  Q_D(NXToolTip);
  QPainter painter(this);
  painter.save();
  painter.setRenderHint(QPainter::Antialiasing);
#if defined(Q_OS_WIN) && QT_VERSION == QT_VERSION_CHECK(6, 11, 0)
  QRect foregroundRect = rect();
#else
  //阴影
  nxTheme->drawEffectShadow(&painter, rect(), d->_shadowBorderWidth, d->_pBorderRadius);
  QRect foregroundRect = rect();
  foregroundRect.adjust(d->_shadowBorderWidth, d->_shadowBorderWidth, -d->_shadowBorderWidth, -d->_shadowBorderWidth);
#endif
  painter.setPen(NXThemeColor(d->_themeMode, PopupBorder));
  painter.setBrush(NXThemeColor(d->_themeMode, PopupBase));
  painter.drawRoundedRect(foregroundRect, d->_pBorderRadius, d->_pBorderRadius);
  painter.restore();
}
