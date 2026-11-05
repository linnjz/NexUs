#include "NXSpotlight.h"

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QVBoxLayout>

#include "NXPushButton.h"
#include "NXText.h"
#include "NXTheme.h"
#include "private/NXSpotlightPrivate.h"
Q_PROPERTY_CREATE_CPP(NXSpotlight, QPS_SCREF_PAEMIT(QString), Title)
Q_PROPERTY_CREATE_CPP(NXSpotlight, QPS_SCREF_PAEMIT(QString), Content)
Q_PROPERTY_CREATE_CPP(NXSpotlight, QPS_PAEMIT(int), BorderRadius)
Q_PROPERTY_CREATE_CPP(NXSpotlight, QPS_PAEMIT(int), Padding)
Q_PROPERTY_CREATE_CPP(NXSpotlight, QPS_PAEMIT(int), OverlayAlpha)
Q_PROPERTY_CREATE_CPP(NXSpotlight, QPS_PAEMIT(bool), IsCircle)

NXSpotlight::NXSpotlight(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new NXSpotlightPrivate())
{
  Q_D(NXSpotlight);
  d->q_ptr           = this;
  d->m_BorderRadius  = 8;
  d->m_Padding       = 8;
  d->m_OverlayAlpha  = 120;
  d->m_IsCircle      = false;
  d->m_ThemeMode      = nxTheme->getThemeMode();
  d->m_SpotlightRect = QRectF();

  setVisible(false);
  setAttribute(Qt::WA_TransparentForMouseEvents, false);

  d->m_TipWidget = new QWidget(this);
  d->m_TipWidget->setVisible(false);

  d->m_TipTitle = new NXText(d->m_TipWidget);
  d->m_TipTitle->setTextStyle(NXTextType::BodyStrong);
  d->m_TipContent = new NXText(d->m_TipWidget);
  d->m_TipContent->setTextStyle(NXTextType::Body);
  d->m_TipContent->setWordWrap(true);

  d->m_StepIndicator = new NXText(d->m_TipWidget);
  d->m_StepIndicator->setTextStyle(NXTextType::Caption);

  d->m_PrevButton = new NXPushButton(QStringLiteral("上一步"), d->m_TipWidget);
  d->m_PrevButton->setFixedSize(70, 30);
  d->m_PrevButton->setBorderRadius(4);
  connect(d->m_PrevButton, &NXPushButton::clicked, this, &NXSpotlight::previous);

  d->m_NextButton = new NXPushButton(QStringLiteral("下一步"), d->m_TipWidget);
  d->m_NextButton->setFixedSize(70, 30);
  d->m_NextButton->setBorderRadius(4);
  d->m_NextButton->setLightDefaultColor(NXThemeColor(NXThemeType::Light, PrimaryNormal));
  d->m_NextButton->setLightHoverColor(NXThemeColor(NXThemeType::Light, PrimaryHover));
  d->m_NextButton->setLightPressColor(NXThemeColor(NXThemeType::Light, PrimaryPress));
  d->m_NextButton->setLightTextColor(Qt::white);
  d->m_NextButton->setDarkDefaultColor(NXThemeColor(NXThemeType::Dark, PrimaryNormal));
  d->m_NextButton->setDarkHoverColor(NXThemeColor(NXThemeType::Dark, PrimaryHover));
  d->m_NextButton->setDarkPressColor(NXThemeColor(NXThemeType::Dark, PrimaryPress));
  d->m_NextButton->setDarkTextColor(Qt::black);
  connect(d->m_NextButton, &NXPushButton::clicked, this, [=]() {
    if (d->m_CurrentStep >= d->m_Steps.size() - 1) {
      finish();
    } else {
      next();
    }
  });

  QVBoxLayout *tipLayout = new QVBoxLayout(d->m_TipWidget);
  tipLayout->setContentsMargins(16, 12, 16, 12);
  tipLayout->setSpacing(6);
  tipLayout->addWidget(d->m_TipTitle);
  tipLayout->addWidget(d->m_TipContent);
  tipLayout->addSpacing(4);
  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->setSpacing(8);
  buttonLayout->addWidget(d->m_StepIndicator);
  buttonLayout->addStretch();
  buttonLayout->addWidget(d->m_PrevButton);
  buttonLayout->addWidget(d->m_NextButton);
  tipLayout->addLayout(buttonLayout);

  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
    update();
  });
}

NXSpotlight::~NXSpotlight()
{
}

void NXSpotlight::showSpotlight(QWidget *target, QString const &buttonText)
{
  Q_D(NXSpotlight);
  SpotlightStep step;
  step.target   = target;
  step.title    = d->m_Title;
  step.content  = d->m_Content;
  step.isCircle = d->m_IsCircle;
  d->m_Steps.clear();
  d->m_Steps.append(step);
  d->m_PrevButton->setVisible(false);
  d->m_NextButton->setText(buttonText);
  d->m_StepIndicator->setVisible(false);
  start();
}

void NXSpotlight::setSteps(QList<SpotlightStep> const &steps)
{
  Q_D(NXSpotlight);
  d->m_Steps = steps;
}

void NXSpotlight::start()
{
  Q_D(NXSpotlight);
  if (d->m_Steps.isEmpty()) {
    return;
  }
  QWidget *parent = parentWidget();
  if (!parent) {
    return;
  }
  setGeometry(0, 0, parent->width(), parent->height());
  parent->installEventFilter(this);
  setVisible(true);
  raise();

  d->m_SpotlightRect = QRectF();

  d->m_Opacity               = 0.0;
  QPropertyAnimation *fadeIn = new QPropertyAnimation(d, "Opacity");
  connect(fadeIn, &QPropertyAnimation::valueChanged, this, [=]() {
    update();
  });
  fadeIn->setDuration(300);
  fadeIn->setStartValue(0.0);
  fadeIn->setEndValue(1.0);
  fadeIn->setEasingCurve(QEasingCurve::OutCubic);
  fadeIn->start(QAbstractAnimation::DeleteWhenStopped);

  d->m_IsActive = true;
  d->_showStep(0);
}

void NXSpotlight::next()
{
  Q_D(NXSpotlight);
  if (d->m_CurrentStep < d->m_Steps.size() - 1) {
    d->_showStep(d->m_CurrentStep + 1);
  }
}

void NXSpotlight::previous()
{
  Q_D(NXSpotlight);
  if (d->m_CurrentStep > 0) {
    d->_showStep(d->m_CurrentStep - 1);
  }
}

void NXSpotlight::finish()
{
  Q_D(NXSpotlight);
  d->m_IsActive = false;

  QPropertyAnimation *fadeOut = new QPropertyAnimation(d, "Opacity");
  connect(fadeOut, &QPropertyAnimation::valueChanged, this, [=]() {
    d->m_TipWidget->setWindowOpacity(d->m_Opacity);
    update();
  });
  fadeOut->setDuration(250);
  fadeOut->setStartValue(d->m_Opacity);
  fadeOut->setEndValue(0.0);
  fadeOut->setEasingCurve(QEasingCurve::InCubic);
  connect(fadeOut, &QPropertyAnimation::finished, this, [=]() {
    setVisible(false);
    d->m_TipWidget->setVisible(false);
    d->m_TipWidget->setWindowOpacity(1.0);
    d->m_Opacity = 1.0;
    if (parentWidget()) {
      parentWidget()->removeEventFilter(this);
    }
    Q_EMIT finished();
  });
  fadeOut->start(QAbstractAnimation::DeleteWhenStopped);
}

int NXSpotlight::currentStep() const
{
  return d_ptr->m_CurrentStep;
}

int NXSpotlight::stepCount() const
{
  return d_ptr->m_Steps.size();
}

void NXSpotlight::paintEvent(QPaintEvent *event)
{
  Q_D(NXSpotlight);
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing);

  QPainterPath overlayPath;
  overlayPath.addRect(rect());

  QRectF spotRect = d->m_SpotlightRect;
  if (spotRect.isValid()) {
    QPainterPath holePath;
    if (d->m_IsCircle) {
      qreal radius = qMax(spotRect.width(), spotRect.height()) / 2.0;
      holePath.addEllipse(spotRect.center(), radius, radius);
    } else {
      holePath.addRoundedRect(spotRect, d->m_BorderRadius, d->m_BorderRadius);
    }
    overlayPath = overlayPath.subtracted(holePath);
  }

  painter.setPen(Qt::NoPen);
  painter.setBrush(QColor(0, 0, 0, static_cast<int>(d->m_OverlayAlpha * d->m_Opacity)));
  painter.drawPath(overlayPath);

  if (spotRect.isValid()) {
    QColor borderColor = NXThemeColor(d->m_ThemeMode, PrimaryNormal);
    borderColor.setAlphaF(d->m_Opacity);
    painter.setPen(QPen(borderColor, 2));
    painter.setBrush(Qt::NoBrush);
    if (d->m_IsCircle) {
      qreal radius = qMax(spotRect.width(), spotRect.height()) / 2.0;
      painter.drawEllipse(spotRect.center(), radius, radius);
    } else {
      painter.drawRoundedRect(spotRect, d->m_BorderRadius, d->m_BorderRadius);
    }
  }
}

void NXSpotlight::mousePressEvent(QMouseEvent *event)
{
  Q_D(NXSpotlight);
  QRectF spotRect = d->m_SpotlightRect;
  if (spotRect.isValid() && !spotRect.contains(event->pos())) {
    if (d->m_Steps.size() <= 1) {
      finish();
    }
  }
  event->accept();
}

bool NXSpotlight::eventFilter(QObject *watched, QEvent *event)
{
  Q_D(NXSpotlight);
  if (watched == parentWidget() && event->type() == QEvent::Resize) {
    setGeometry(0, 0, parentWidget()->width(), parentWidget()->height());
    if (d->m_IsActive && d->m_CurrentStep >= 0 && d->m_CurrentStep < d->m_Steps.size()) {
      d->_updateOverlay();
      d->_updateTipPosition();
    }
  }
  return QWidget::eventFilter(watched, event);
}
