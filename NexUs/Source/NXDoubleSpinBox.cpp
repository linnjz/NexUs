#include "NXDoubleSpinBox.h"

#include <QContextMenuEvent>
#include <QLineEdit>
#include <QLocale>
#include <QPainter>
#include <QPropertyAnimation>

#include "DeveloperComponents/NXSpinBoxStyle.h"
#include "NXMenu.h"
#include "NXTheme.h"
#include "private/NXDoubleSpinBoxPrivate.h"

NXDoubleSpinBox::NXDoubleSpinBox(QWidget *parent)
    : QDoubleSpinBox(parent)
    , d_ptr(new NXDoubleSpinBoxPrivate())
{
  Q_D(NXDoubleSpinBox);
  d->q_ptr             = this;
  d->m_ExpandMarkWidth = 0;
  setLocale(QLocale::C);
  setFixedSize(115, 35);
#ifdef Q_OS_MACOS
  setAttribute(Qt::WA_Hover);
  setAttribute(Qt::WA_MacShowFocusRect, false);
#endif
  d->m_Style = new NXSpinBoxStyle(style());
  setStyle(d->m_Style);
  lineEdit()->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
#ifdef Q_OS_MACOS
  lineEdit()->setStyleSheet(
      QStringLiteral("background-color:transparent;padding-left:10px;padding-bottom:3px;border:none;outline:none;"));
#else
  lineEdit()->setStyleSheet(QStringLiteral("background-color:transparent;padding-left:10px;padding-bottom:3px;"));
#endif
  d->onThemeChanged(nxTheme->getThemeMode());
  connect(nxTheme, &NXTheme::themeModeChanged, d, &NXDoubleSpinBoxPrivate::onThemeChanged);
}

NXDoubleSpinBox::~NXDoubleSpinBox()
{
  Q_D(NXDoubleSpinBox);
  delete d->m_Style;
}

void NXDoubleSpinBox::setButtonMode(NXSpinBoxType::ButtonMode buttonMode)
{
  Q_D(NXDoubleSpinBox);
  if (minimumWidth() < 90) {
    setMinimumWidth(90);
  }
  d->m_Style->setButtonMode(buttonMode);
  switch (buttonMode) {
  case NXSpinBoxType::Inline : {
    lineEdit()->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
#ifdef Q_OS_MACOS
    lineEdit()->setStyleSheet(
        QStringLiteral("background-color:transparent;padding-left:10px;padding-bottom:3px;border:none;outline:none;"));
#else
    lineEdit()->setStyleSheet(QStringLiteral("background-color:transparent;padding-left:10px;padding-bottom:3px;"));
#endif
    break;
  }
  case NXSpinBoxType::Compact :
  case NXSpinBoxType::Side :
  case NXSpinBoxType::PMSide  : {
    lineEdit()->setAlignment(Qt::AlignCenter);
#ifdef Q_OS_MACOS
    lineEdit()->setStyleSheet(
        QStringLiteral("background-color:transparent;padding-bottom:3px;border:none;outline:none;"));
#else
    lineEdit()->setStyleSheet(QStringLiteral("background-color:transparent;padding-bottom:3px;"));
#endif
    break;
  }
  }
  d->onThemeChanged(nxTheme->getThemeMode());
  setFrame(hasFrame());
  Q_EMIT signalButtonModeChanged();
}

NXSpinBoxType::ButtonMode NXDoubleSpinBox::getButtonMode() const noexcept(std::is_nothrow_copy_constructible_v<NXSpinBoxType::ButtonMode>)
{
  Q_CONST_D(NXDoubleSpinBox);
  return d->m_Style->getButtonMode();
}

void NXDoubleSpinBox::focusInEvent(QFocusEvent *event)
{
  Q_D(NXDoubleSpinBox);
  if (event->reason() == Qt::MouseFocusReason) {
    QPropertyAnimation *markAnimation = new QPropertyAnimation(d, "ExpandMarkWidth");
    connect(markAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
      update();
    });
    markAnimation->setDuration(300);
    markAnimation->setEasingCurve(QEasingCurve::InOutSine);
    markAnimation->setStartValue(d->m_ExpandMarkWidth);
    markAnimation->setEndValue(width() / 2 - 3);
    markAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  }
  QDoubleSpinBox::focusInEvent(event);
}

void NXDoubleSpinBox::focusOutEvent(QFocusEvent *event)
{
  Q_D(NXDoubleSpinBox);
  if (event->reason() != Qt::PopupFocusReason) {
    QPropertyAnimation *markAnimation = new QPropertyAnimation(d, "ExpandMarkWidth");
    connect(markAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
      update();
    });
    markAnimation->setDuration(300);
    markAnimation->setEasingCurve(QEasingCurve::InOutSine);
    markAnimation->setStartValue(d->m_ExpandMarkWidth);
    markAnimation->setEndValue(0);
    markAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  }
  QDoubleSpinBox::focusOutEvent(event);
}

void NXDoubleSpinBox::paintEvent(QPaintEvent *event)
{
  Q_D(NXDoubleSpinBox);
  QDoubleSpinBox::paintEvent(event);
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  painter.setPen(Qt::NoPen);
  painter.setBrush(NXThemeColor(d->m_ThemeMode, PrimaryNormal));
  painter.drawRoundedRect(QRectF(width() / 2 - d->m_ExpandMarkWidth, height() - 2.5, d->m_ExpandMarkWidth * 2, 2.5), 2, 2);
  painter.restore();
}

void NXDoubleSpinBox::contextMenuEvent(QContextMenuEvent *event)
{
  Q_D(NXDoubleSpinBox);
  NXMenu *menu = d->_createStandardContextMenu();
  if (!menu) {
    return;
  }
  menu->addSeparator();
  uint const se = stepEnabled();
  QAction *up   = menu->addNXIconAction(NXIconType::Plus, tr("增加"));
  up->setEnabled(se & StepUpEnabled);
  QAction *down = menu->addNXIconAction(NXIconType::Minus, tr("减少"));
  down->setEnabled(se & StepDownEnabled);
  menu->addSeparator();

  QAbstractSpinBox const *that = this;
  QPoint const pos             = (event->reason() == QContextMenuEvent::Mouse)
                                   ? event->globalPos()
                                   : mapToGlobal(QPoint(event->pos().x(), 0)) + QPoint(width() / 2, height() / 2);
  QAction const *action        = menu->exec(pos);
  delete menu;
  if (that && action) {
    if (action == up) {
      stepBy(1);
    } else if (action == down) {
      stepBy(-1);
    }
  }
  event->accept();
}
