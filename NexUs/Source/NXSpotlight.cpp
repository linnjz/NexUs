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
Q_PROPERTY_CREATE_CPP(NXSpotlight, QS_SET_CREF(QString), Title)
Q_PROPERTY_CREATE_CPP(NXSpotlight, QS_SET_CREF(QString), Content)
Q_PROPERTY_CREATE_CPP(NXSpotlight, int, BorderRadius)
Q_PROPERTY_CREATE_CPP(NXSpotlight, int, Padding)
Q_PROPERTY_CREATE_CPP(NXSpotlight, int, OverlayAlpha)
Q_PROPERTY_CREATE_CPP(NXSpotlight, bool, IsCircle)

NXSpotlight::NXSpotlight(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new NXSpotlightPrivate())
{
  Q_D(NXSpotlight);
  d->q_ptr           = this;
  d->_pBorderRadius  = 8;
  d->_pPadding       = 8;
  d->_pOverlayAlpha  = 120;
  d->_pIsCircle      = false;
  d->_themeMode      = nxTheme->getThemeMode();
  d->_pSpotlightRect = QRectF();

  setVisible(false);
  setAttribute(Qt::WA_TransparentForMouseEvents, false);

  d->_tipWidget = new QWidget(this);
  d->_tipWidget->setVisible(false);

  d->_tipTitle = new NXText(d->_tipWidget);
  d->_tipTitle->setTextStyle(NXTextType::BodyStrong);
  d->_tipContent = new NXText(d->_tipWidget);
  d->_tipContent->setTextStyle(NXTextType::Body);
  d->_tipContent->setWordWrap(true);

  d->_stepIndicator = new NXText(d->_tipWidget);
  d->_stepIndicator->setTextStyle(NXTextType::Caption);

  d->_prevButton = new NXPushButton(QStringLiteral("上一步"), d->_tipWidget);
  d->_prevButton->setFixedSize(70, 30);
  d->_prevButton->setBorderRadius(4);
  connect(d->_prevButton, &NXPushButton::clicked, this, &NXSpotlight::previous);

  d->_nextButton = new NXPushButton(QStringLiteral("下一步"), d->_tipWidget);
  d->_nextButton->setFixedSize(70, 30);
  d->_nextButton->setBorderRadius(4);
  d->_nextButton->setLightDefaultColor(NXThemeColor(NXThemeType::Light, PrimaryNormal));
  d->_nextButton->setLightHoverColor(NXThemeColor(NXThemeType::Light, PrimaryHover));
  d->_nextButton->setLightPressColor(NXThemeColor(NXThemeType::Light, PrimaryPress));
  d->_nextButton->setLightTextColor(Qt::white);
  d->_nextButton->setDarkDefaultColor(NXThemeColor(NXThemeType::Dark, PrimaryNormal));
  d->_nextButton->setDarkHoverColor(NXThemeColor(NXThemeType::Dark, PrimaryHover));
  d->_nextButton->setDarkPressColor(NXThemeColor(NXThemeType::Dark, PrimaryPress));
  d->_nextButton->setDarkTextColor(Qt::black);
  connect(d->_nextButton, &NXPushButton::clicked, this, [=]() {
    if (d->_currentStep >= d->_steps.size() - 1) {
      finish();
    } else {
      next();
    }
  });

  QVBoxLayout *tipLayout = new QVBoxLayout(d->_tipWidget);
  tipLayout->setContentsMargins(16, 12, 16, 12);
  tipLayout->setSpacing(6);
  tipLayout->addWidget(d->_tipTitle);
  tipLayout->addWidget(d->_tipContent);
  tipLayout->addSpacing(4);
  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->setSpacing(8);
  buttonLayout->addWidget(d->_stepIndicator);
  buttonLayout->addStretch();
  buttonLayout->addWidget(d->_prevButton);
  buttonLayout->addWidget(d->_nextButton);
  tipLayout->addLayout(buttonLayout);

  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
    update();
  });
}

NXSpotlight::~NXSpotlight()
{
}

void NXSpotlight::showSpotlight(QWidget *target, const QString &buttonText)
{
  Q_D(NXSpotlight);
  SpotlightStep step;
  step.target   = target;
  step.title    = d->_pTitle;
  step.content  = d->_pContent;
  step.isCircle = d->_pIsCircle;
  d->_steps.clear();
  d->_steps.append(step);
  d->_prevButton->setVisible(false);
  d->_nextButton->setText(buttonText);
  d->_stepIndicator->setVisible(false);
  start();
}

void NXSpotlight::setSteps(const QList<SpotlightStep> &steps)
{
  Q_D(NXSpotlight);
  d->_steps = steps;
}

void NXSpotlight::start()
{
  Q_D(NXSpotlight);
  if (d->_steps.isEmpty()) {
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

  d->_pSpotlightRect = QRectF();

  d->_pOpacity               = 0.0;
  QPropertyAnimation *fadeIn = new QPropertyAnimation(d, "pOpacity");
  connect(fadeIn, &QPropertyAnimation::valueChanged, this, [=]() {
    update();
  });
  fadeIn->setDuration(300);
  fadeIn->setStartValue(0.0);
  fadeIn->setEndValue(1.0);
  fadeIn->setEasingCurve(QEasingCurve::OutCubic);
  fadeIn->start(QAbstractAnimation::DeleteWhenStopped);

  d->_isActive = true;
  d->_showStep(0);
}

void NXSpotlight::next()
{
  Q_D(NXSpotlight);
  if (d->_currentStep < d->_steps.size() - 1) {
    d->_showStep(d->_currentStep + 1);
  }
}

void NXSpotlight::previous()
{
  Q_D(NXSpotlight);
  if (d->_currentStep > 0) {
    d->_showStep(d->_currentStep - 1);
  }
}

void NXSpotlight::finish()
{
  Q_D(NXSpotlight);
  d->_isActive = false;

  QPropertyAnimation *fadeOut = new QPropertyAnimation(d, "pOpacity");
  connect(fadeOut, &QPropertyAnimation::valueChanged, this, [=]() {
    d->_tipWidget->setWindowOpacity(d->_pOpacity);
    update();
  });
  fadeOut->setDuration(250);
  fadeOut->setStartValue(d->_pOpacity);
  fadeOut->setEndValue(0.0);
  fadeOut->setEasingCurve(QEasingCurve::InCubic);
  connect(fadeOut, &QPropertyAnimation::finished, this, [=]() {
    setVisible(false);
    d->_tipWidget->setVisible(false);
    d->_tipWidget->setWindowOpacity(1.0);
    d->_pOpacity = 1.0;
    if (parentWidget()) {
      parentWidget()->removeEventFilter(this);
    }
    Q_EMIT finished();
  });
  fadeOut->start(QAbstractAnimation::DeleteWhenStopped);
}

int NXSpotlight::currentStep() const
{
  return d_ptr->_currentStep;
}

int NXSpotlight::stepCount() const
{
  return d_ptr->_steps.size();
}

void NXSpotlight::paintEvent(QPaintEvent *event)
{
  Q_D(NXSpotlight);
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing);

  QPainterPath overlayPath;
  overlayPath.addRect(rect());

  QRectF spotRect = d->_pSpotlightRect;
  if (spotRect.isValid()) {
    QPainterPath holePath;
    if (d->_pIsCircle) {
      qreal radius = qMax(spotRect.width(), spotRect.height()) / 2.0;
      holePath.addEllipse(spotRect.center(), radius, radius);
    } else {
      holePath.addRoundedRect(spotRect, d->_pBorderRadius, d->_pBorderRadius);
    }
    overlayPath = overlayPath.subtracted(holePath);
  }

  painter.setPen(Qt::NoPen);
  painter.setBrush(QColor(0, 0, 0, static_cast<int>(d->_pOverlayAlpha * d->_pOpacity)));
  painter.drawPath(overlayPath);

  if (spotRect.isValid()) {
    QColor borderColor = NXThemeColor(d->_themeMode, PrimaryNormal);
    borderColor.setAlphaF(d->_pOpacity);
    painter.setPen(QPen(borderColor, 2));
    painter.setBrush(Qt::NoBrush);
    if (d->_pIsCircle) {
      qreal radius = qMax(spotRect.width(), spotRect.height()) / 2.0;
      painter.drawEllipse(spotRect.center(), radius, radius);
    } else {
      painter.drawRoundedRect(spotRect, d->_pBorderRadius, d->_pBorderRadius);
    }
  }
}

void NXSpotlight::mousePressEvent(QMouseEvent *event)
{
  Q_D(NXSpotlight);
  QRectF spotRect = d->_pSpotlightRect;
  if (spotRect.isValid() && !spotRect.contains(event->pos())) {
    if (d->_steps.size() <= 1) {
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
    if (d->_isActive && d->_currentStep >= 0 && d->_currentStep < d->_steps.size()) {
      d->_updateOverlay();
      d->_updateTipPosition();
    }
  }
  return QWidget::eventFilter(watched, event);
}
