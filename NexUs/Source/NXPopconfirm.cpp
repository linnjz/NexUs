#include "NXPopconfirm.h"

#include <QApplication>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QPainter>
#include <QPropertyAnimation>
#include <QScreen>
#include <QVBoxLayout>

#include "NXIcon.h"
#include "NXPushButton.h"
#include "NXTheme.h"
#include "private/NXPopconfirmPrivate.h"
Q_PROPERTY_CREATE_CPP(NXPopconfirm, QPS_SCREF_PAEMIT(QString), Title)
Q_PROPERTY_CREATE_CPP(NXPopconfirm, QPS_SCREF_PAEMIT(QString), Content)
Q_PROPERTY_CREATE_CPP(NXPopconfirm, QPS_SCREF_PAEMIT(QString), ConfirmButtonText)
Q_PROPERTY_CREATE_CPP(NXPopconfirm, QPS_SCREF_PAEMIT(QString), CancelButtonText)
Q_PROPERTY_CREATE_CPP(NXPopconfirm, QPS_PAEMIT(int), BorderRadius)
Q_PROPERTY_CREATE_CPP(NXPopconfirm, QPS_PAEMIT(NXIconType::IconName), Icon)
Q_PROPERTY_CREATE_CPP(NXPopconfirm, QPS_PAEMIT(bool), IsLightDismiss)

NXPopconfirm::NXPopconfirm(QWidget *parent)
    : QWidget { nullptr }
    , d_ptr(new NXPopconfirmPrivate())
{
  Q_D(NXPopconfirm);
  d->q_ptr               = this;
  d->m_BorderRadius      = 8;
  d->m_ConfirmButtonText = QStringLiteral("确定");
  d->m_CancelButtonText  = QStringLiteral("取消");
  d->m_Icon              = NXIconType::CircleExclamation;
  d->m_IsLightDismiss    = true;
  d->m_ThemeMode          = nxTheme->getThemeMode();

  setObjectName("NXPopconfirm");
  setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
  setAttribute(Qt::WA_TranslucentBackground);

  d->m_OpacityEffect = new QGraphicsOpacityEffect(this);
  d->m_OpacityEffect->setOpacity(0);
  setGraphicsEffect(d->m_OpacityEffect);

  d->m_CancelButton = new NXPushButton(d->m_CancelButtonText, this);
  d->m_CancelButton->setFixedHeight(30);
  d->m_CancelButton->setBorderRadius(4);
  connect(d->m_CancelButton, &NXPushButton::clicked, this, [=]() {
    Q_EMIT cancelled();
    closePopconfirm();
  });

  d->m_ConfirmButton = new NXPushButton(d->m_ConfirmButtonText, this);
  d->m_ConfirmButton->setFixedHeight(30);
  d->m_ConfirmButton->setBorderRadius(4);
  d->m_ConfirmButton->setLightDefaultColor(NXThemeColor(NXThemeType::Light, PrimaryNormal));
  d->m_ConfirmButton->setLightHoverColor(NXThemeColor(NXThemeType::Light, PrimaryHover));
  d->m_ConfirmButton->setLightPressColor(NXThemeColor(NXThemeType::Light, PrimaryPress));
  d->m_ConfirmButton->setLightTextColor(Qt::white);
  d->m_ConfirmButton->setDarkDefaultColor(NXThemeColor(NXThemeType::Dark, PrimaryNormal));
  d->m_ConfirmButton->setDarkHoverColor(NXThemeColor(NXThemeType::Dark, PrimaryHover));
  d->m_ConfirmButton->setDarkPressColor(NXThemeColor(NXThemeType::Dark, PrimaryPress));
  d->m_ConfirmButton->setDarkTextColor(Qt::black);
  connect(d->m_ConfirmButton, &NXPushButton::clicked, this, [=]() {
    Q_EMIT confirmed();
    closePopconfirm();
  });

  d->m_MainLayout = new QVBoxLayout(this);
  int sw         = d->m_ShadowBorderWidth;
#if defined(Q_OS_WIN) && QT_VERSION == QT_VERSION_CHECK(6, 11, 0)
  d->m_MainLayout->setContentsMargins(sw, sw, sw, sw);
#else
  d->m_MainLayout->setContentsMargins(sw * 2, sw * 2, sw * 2, sw * 2);
#endif

  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->setSpacing(8);
  buttonLayout->addStretch();
  buttonLayout->addWidget(d->m_CancelButton);
  buttonLayout->addWidget(d->m_ConfirmButton);
  d->m_MainLayout->addStretch();
  d->m_MainLayout->addLayout(buttonLayout);

  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
    update();
  });
  hide();
}

NXPopconfirm::~NXPopconfirm()
{
}

void NXPopconfirm::showPopconfirm(QWidget *target)
{
  Q_D(NXPopconfirm);
  if (!target) {
    return;
  }
  d->m_CancelButton->setText(d->m_CancelButtonText);
  d->m_ConfirmButton->setText(d->m_ConfirmButtonText);

  int sw            = d->m_ShadowBorderWidth;
  int popupWidth    = 280;
  int contentHeight = sw * 2 + 16;
  int textWidth     = popupWidth - sw * 4 - 40;

  if (d->m_Icon != NXIconType::None) {
    contentHeight += 0;
  }
  if (!d->m_Title.isEmpty()) {
    QFont titleFont = font();
    titleFont.setPixelSize(14);
    titleFont.setBold(true);
    QFontMetrics tfm(titleFont);
    contentHeight += tfm.height() + 6;
  }
  if (!d->m_Content.isEmpty()) {
    QFont contentFont = font();
    contentFont.setPixelSize(13);
    QFontMetrics cfm(contentFont);
    QRect br = cfm.boundingRect(QRect(0, 0, textWidth, 0), Qt::TextWordWrap, d->m_Content);
    contentHeight += br.height() + 8;
  }
  contentHeight += 38;
  setFixedSize(popupWidth, contentHeight);

  QPoint targetGlobal = target->mapToGlobal(QPoint(0, target->height()));
  int x               = targetGlobal.x() + (target->width() - width()) / 2;
  int y               = targetGlobal.y() + 5;

  QScreen *screen = QApplication::screenAt(targetGlobal);
  if (screen) {
    QRect screenRect = screen->availableGeometry();
    if (x < screenRect.left()) {
      x = screenRect.left();
    }
    if (x + width() > screenRect.right()) {
      x = screenRect.right() - width();
    }
    if (y + height() > screenRect.bottom()) {
      y = targetGlobal.y() - target->height() - height() - 5;
    }
    if (y < screenRect.top()) {
      y = screenRect.top();
    }
  }
  move(x, y);
  show();
  raise();
  if (d->m_IsLightDismiss) {
    qApp->installEventFilter(d);
  }
  d->_doShowAnimation();
}

void NXPopconfirm::closePopconfirm()
{
  Q_D(NXPopconfirm);
  qApp->removeEventFilter(d);
  hide();
  Q_EMIT closed();
}

void NXPopconfirm::paintEvent(QPaintEvent *event)
{
  Q_D(NXPopconfirm);
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  int sw = d->m_ShadowBorderWidth;
#if defined(Q_OS_WIN) && QT_VERSION == QT_VERSION_CHECK(6, 11, 0)
  QRect fg = rect();
#else
  nxTheme->drawEffectShadow(&painter, rect(), sw, d->m_BorderRadius);
  QRect fg = rect().adjusted(sw, sw, -sw, -sw);
#endif
  painter.setPen(NXThemeColor(d->m_ThemeMode, PopupBorder));
  painter.setBrush(NXThemeColor(d->m_ThemeMode, PopupBase));
  painter.drawRoundedRect(fg, d->m_BorderRadius, d->m_BorderRadius);

  int textX      = fg.x() + 14;
  int textY      = fg.y() + 14;
  int textWidth  = fg.width() - 28;
  int iconOffset = 0;

  if (d->m_Icon != NXIconType::None) {
    QFont iconFont = QFont(QStringLiteral("NXAwesome"));
    iconFont.setPixelSize(18);
    painter.setFont(iconFont);
    painter.setPen(NXThemeColor(d->m_ThemeMode, PrimaryNormal));
    painter.drawText(QRect(textX, textY - 2, 22, 22), Qt::AlignCenter, QChar(static_cast<unsigned short>(d->m_Icon)));
    iconOffset = 26;
  }

  if (!d->m_Title.isEmpty()) {
    QFont titleFont = painter.font();
    titleFont.setFamily(font().family());
    titleFont.setPixelSize(14);
    titleFont.setBold(true);
    painter.setFont(titleFont);
    painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
    QRect titleRect(textX + iconOffset, textY, textWidth - iconOffset, painter.fontMetrics().height());
    painter.drawText(titleRect, Qt::AlignLeft | Qt::AlignVCenter, d->m_Title);
    textY += titleRect.height() + 6;
  }

  if (!d->m_Content.isEmpty()) {
    QFont contentFont = painter.font();
    contentFont.setPixelSize(13);
    contentFont.setBold(false);
    painter.setFont(contentFont);
    painter.setPen(NXThemeColor(d->m_ThemeMode, BasicDetailsText));
    QRect contentRect(textX + iconOffset, textY, textWidth - iconOffset, fg.bottom() - textY - 42);
    painter.drawText(contentRect, Qt::AlignLeft | Qt::TextWordWrap, d->m_Content);
  }

  painter.restore();
}

NXPopconfirmPrivate::NXPopconfirmPrivate(QObject *parent)
    : QObject(parent)
{
}

NXPopconfirmPrivate::~NXPopconfirmPrivate()
{
}

bool NXPopconfirmPrivate::eventFilter(QObject *watched, QEvent *event)
{
  Q_Q(NXPopconfirm);
  if (event->type() == QEvent::MouseButtonPress) {
    QWidget *clickedWidget = qobject_cast<QWidget *>(watched);
    if (clickedWidget && !q->isAncestorOf(clickedWidget) && clickedWidget != q) {
      Q_EMIT q->cancelled();
      q->closePopconfirm();
      return false;
    }
  }
  return QObject::eventFilter(watched, event);
}

void NXPopconfirmPrivate::_doShowAnimation()
{
  QPropertyAnimation *animation = new QPropertyAnimation(m_OpacityEffect, "opacity");
  animation->setDuration(250);
  animation->setStartValue(0.0);
  animation->setEndValue(1.0);
  animation->setEasingCurve(QEasingCurve::InOutCubic);
  animation->start(QAbstractAnimation::DeleteWhenStopped);
}
