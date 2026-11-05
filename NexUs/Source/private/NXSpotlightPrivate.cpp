#include "NXSpotlightPrivate.h"

#include <QPropertyAnimation>

#include "NXPushButton.h"
#include "NXSpotlight.h"
#include "NXText.h"
#include "NXTheme.h"

NXSpotlightPrivate::NXSpotlightPrivate(QObject *parent)
    : QObject { parent }
{
}

NXSpotlightPrivate::~NXSpotlightPrivate()
{
}

void NXSpotlightPrivate::_showStep(int index, bool animate)
{
  Q_Q(NXSpotlight);
  if (index < 0 || index >= m_Steps.size()) {
    return;
  }
  m_CurrentStep                           = index;
  NXSpotlight::SpotlightStep const &step = m_Steps[m_CurrentStep];
  m_CurrentTarget                         = step.target;
  m_IsCircle                             = step.isCircle;

  m_TipTitle->setText(step.title);
  m_TipContent->setText(step.content);
  m_TipTitle->setVisible(!step.title.isEmpty());
  m_TipContent->setVisible(!step.content.isEmpty());

  if (m_Steps.size() > 1) {
    m_PrevButton->setVisible(m_CurrentStep > 0);
    m_NextButton->setText(m_CurrentStep >= m_Steps.size() - 1 ? QStringLiteral("完成") : QStringLiteral("下一步"));
    m_StepIndicator->setVisible(true);
    m_StepIndicator->setText(QString(QStringLiteral("%1 / %2")).arg(m_CurrentStep + 1).arg(m_Steps.size()));
  }

  QRectF targetRect = _getTargetRect(m_CurrentTarget);

  if (animate && m_SpotlightRect.isValid()) {
    QPropertyAnimation *moveAnimation = new QPropertyAnimation(this, "SpotlightRect");
    connect(moveAnimation, &QPropertyAnimation::valueChanged, q, [=]() {
      q->update();
      _updateTipPosition();
    });
    moveAnimation->setDuration(350);
    moveAnimation->setEasingCurve(QEasingCurve::InOutCubic);
    moveAnimation->setStartValue(m_SpotlightRect);
    moveAnimation->setEndValue(targetRect);
    moveAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  } else {
    m_SpotlightRect = targetRect;
    q->update();
  }

  _updateTipPosition();
  m_TipWidget->setVisible(true);
  m_TipWidget->raise();

  Q_EMIT q->stepChanged(m_CurrentStep);
}

void NXSpotlightPrivate::_updateOverlay()
{
  Q_Q(NXSpotlight);
  if (m_CurrentTarget) {
    m_SpotlightRect = _getTargetRect(m_CurrentTarget);
    q->update();
  }
}

void NXSpotlightPrivate::_updateTipPosition()
{
  Q_Q(NXSpotlight);
  if (!m_CurrentTarget) {
    return;
  }
  m_TipWidget->adjustSize();
  int tipWidth = qMin(300, q->width() - 40);
  m_TipWidget->setFixedWidth(tipWidth);
  m_TipWidget->adjustSize();

  QRectF spotRect = m_SpotlightRect;
  int tipHeight   = m_TipWidget->sizeHint().height();
  int margin      = 12;

  int tipX = static_cast<int>(spotRect.center().x() - tipWidth / 2);
  int tipY = static_cast<int>(spotRect.bottom() + margin);

  if (tipY + tipHeight > q->height() - 10) {
    tipY = static_cast<int>(spotRect.top() - tipHeight - margin);
  }

  tipX = qMax(10, qMin(tipX, q->width() - tipWidth - 10));
  tipY = qMax(10, tipY);

  m_TipWidget->move(tipX, tipY);

  m_TipWidget->setStyleSheet(QString(QStringLiteral("background-color: %1; border-radius: 8px; border: 1px solid %2;"))
                                .arg(NXThemeColor(m_ThemeMode, DialogBase).name())
                                .arg(NXThemeColor(m_ThemeMode, PopupBorder).name()));
}

QRectF NXSpotlightPrivate::_getTargetRect(QWidget *target) const
{
  Q_Q(NXSpotlight const);
  if (!target || !q->parentWidget()) {
    return QRectF();
  }
  QPoint topLeft = target->mapTo(q->parentWidget(), QPoint(0, 0));
  QRectF rect(topLeft, target->size());
  rect.adjust(-m_Padding, -m_Padding, m_Padding, m_Padding);
  return rect;
}
