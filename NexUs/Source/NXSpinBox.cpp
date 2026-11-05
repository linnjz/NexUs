#include "NXSpinBox.h"

#include <QContextMenuEvent>
#include <QLineEdit>
#include <QLocale>
#include <QPainter>
#include <QPropertyAnimation>

#include "DeveloperComponents/NXSpinBoxStyle.h"
#include "NXMenu.h"
#include "NXTheme.h"
#include "private/NXSpinBoxPrivate.h"

NXSpinBox::NXSpinBox(QWidget *parent)
    : QSpinBox(parent)
    , d_ptr(new NXSpinBoxPrivate())
{
  Q_D(NXSpinBox);
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
  connect(nxTheme, &NXTheme::themeModeChanged, d, &NXSpinBoxPrivate::onThemeChanged);
}

NXSpinBox::~NXSpinBox()
{
  Q_D(NXSpinBox);
  delete d->m_Style;
}

void NXSpinBox::setButtonMode(NXSpinBoxType::ButtonMode buttonMode)
{
  Q_D(NXSpinBox);
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
  setFrame(hasFrame());
  d->onThemeChanged(nxTheme->getThemeMode());
  Q_EMIT signalButtonModeChanged();
}

NXSpinBoxType::ButtonMode NXSpinBox::getButtonMode() const noexcept(std::is_nothrow_copy_constructible_v<NXSpinBoxType::ButtonMode>)
{
  Q_CONST_D(NXSpinBox);
  return d->m_Style->getButtonMode();
}

void NXSpinBox::focusInEvent(QFocusEvent *event)
{
  Q_D(NXSpinBox);
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
  QSpinBox::focusInEvent(event);
}

void NXSpinBox::focusOutEvent(QFocusEvent *event)
{
  Q_D(NXSpinBox);
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
  QSpinBox::focusOutEvent(event);
}

void NXSpinBox::paintEvent(QPaintEvent *event)
{
  Q_D(NXSpinBox);
  if (palette().color(QPalette::Text) != NXThemeColor(d->m_ThemeMode, BasicText)) {
    d->onThemeChanged(d->m_ThemeMode);
  }
  QSpinBox::paintEvent(event);
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  painter.setPen(Qt::NoPen);
  painter.setBrush(NXThemeColor(d->m_ThemeMode, PrimaryNormal));
  painter.drawRoundedRect(QRectF(width() / 2 - d->m_ExpandMarkWidth, height() - 2.5, d->m_ExpandMarkWidth * 2, 2.5), 2, 2);
  painter.restore();
}

void NXSpinBox::contextMenuEvent(QContextMenuEvent *event)
{
  Q_D(NXSpinBox);
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
