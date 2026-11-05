#include "NXToolTip.h"

#include <QPainter>
#include <QVBoxLayout>

#include "NXText.h"
#include "NXTheme.h"
#include "private/NXToolTipPrivate.h"
Q_PROPERTY_CREATE_CPP(NXToolTip, QPS_PAEMIT(int), BorderRadius)
Q_PROPERTY_CREATE_CPP(NXToolTip, QPS_PAEMIT(int), DisplayMsec)
Q_PROPERTY_CREATE_CPP(NXToolTip, QPS_PAEMIT(int), ShowDelayMsec)
Q_PROPERTY_CREATE_CPP(NXToolTip, QPS_PAEMIT(int), HideDelayMsec)
Q_PROPERTY_CREATE_CPP(NXToolTip, QPS_PAEMIT(int), OffSetX)
Q_PROPERTY_CREATE_CPP(NXToolTip, QPS_PAEMIT(int), OffSetY)
Q_PROPERTY_CREATE_CPP(NXToolTip, QPS_PAEMIT(bool), IsMoveEnable)

NXToolTip::NXToolTip(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXToolTipPrivate())
{
  Q_D(NXToolTip);
  d->q_ptr           = this;
  d->m_IsMoveEnable  = false;
  d->m_OffSetX       = 10;
  d->m_OffSetY       = 0;
  d->m_BorderRadius  = 5;
  d->m_DisplayMsec   = -1;
  d->m_ShowDelayMsec = 0;
  d->m_HideDelayMsec = 0;
  d->m_CustomWidget  = nullptr;
  setObjectName("NXToolTip");
  if (parent) {
    parent->installEventFilter(d);
  }
  setAttribute(Qt::WA_TransparentForMouseEvents);
  setAttribute(Qt::WA_TranslucentBackground);
  setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);

  d->m_ToolTipText = new NXText(this);
  d->m_ToolTipText->setWordWrap(false);
  d->m_ToolTipText->setTextPixelSize(17);
  d->m_MainLayout = new QVBoxLayout(this);
#if defined(Q_OS_WIN) && QT_VERSION == QT_VERSION_CHECK(6, 11, 0)
  d->m_MainLayout->setContentsMargins(d->m_ShadowBorderWidth, d->m_ShadowBorderWidth, d->m_ShadowBorderWidth, d->m_ShadowBorderWidth);
#else
  d->m_MainLayout->setContentsMargins(d->m_ShadowBorderWidth * 2, d->m_ShadowBorderWidth * 2, d->m_ShadowBorderWidth * 2, d->m_ShadowBorderWidth * 2);
#endif
  d->m_MainLayout->addWidget(d->m_ToolTipText);

  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
    update();
  });
  hide();
}

NXToolTip::~NXToolTip()
{
}

void NXToolTip::setToolTip(QString const &toolTip)
{
  Q_D(NXToolTip);
  d->m_ToolTipText->setText(toolTip);
  d->m_ToolTipText->adjustSize();
  d->m_ToolTipText->updateGeometry();
  d->m_MainLayout->invalidate();
  d->m_MainLayout->activate();
  setFixedSize(sizeHint());
  Q_EMIT signalToolTipChanged();
}

QString NXToolTip::getToolTip() const noexcept(std::is_nothrow_copy_constructible_v<QString>)
{
  Q_CONST_D(NXToolTip);
  return d->m_ToolTipText->text();
}

void NXToolTip::setCustomWidget(QWidget *customWidget)
{
  Q_D(NXToolTip);
  if (!customWidget || customWidget == this) {
    return;
  }
  if (d->m_CustomWidget) {
    d->m_MainLayout->removeWidget(d->m_CustomWidget);
    d->m_CustomWidget->deleteLater();
  }
  d->m_ToolTipText->hide();
  d->m_MainLayout->addWidget(customWidget);
  d->m_CustomWidget = customWidget;
  Q_EMIT signalCustomWidgetChanged();
}

QWidget *NXToolTip::getCustomWidget() const noexcept(std::is_nothrow_copy_constructible_v<QWidget *>)
{
  Q_CONST_D(NXToolTip);
  return d->m_CustomWidget;
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
  nxTheme->drawEffectShadow(&painter, rect(), d->m_ShadowBorderWidth, d->m_BorderRadius);
  QRect foregroundRect = rect();
  foregroundRect.adjust(d->m_ShadowBorderWidth, d->m_ShadowBorderWidth, -d->m_ShadowBorderWidth, -d->m_ShadowBorderWidth);
#endif
  painter.setPen(NXThemeColor(d->m_ThemeMode, PopupBorder));
  painter.setBrush(NXThemeColor(d->m_ThemeMode, PopupBase));
  painter.drawRoundedRect(foregroundRect, d->m_BorderRadius, d->m_BorderRadius);
  painter.restore();
}
