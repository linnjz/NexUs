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
  d->_pExpandMarkWidth = 0;
  setLocale(QLocale::C);
  setFixedSize(115, 35);
#ifdef Q_OS_MACOS
  setAttribute(Qt::WA_Hover);
  setAttribute(Qt::WA_MacShowFocusRect, false);
#endif
  d->_style = new NXSpinBoxStyle(style());
  setStyle(d->_style);
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
  delete d->_style;
}

void NXSpinBox::setButtonMode(NXSpinBoxType::ButtonMode buttonMode)
{
  Q_D(NXSpinBox);
  if (minimumWidth() < 90) {
    setMinimumWidth(90);
  }
  d->_style->setButtonMode(buttonMode);
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
  Q_EMIT pButtonModeChanged();
}

NXSpinBoxType::ButtonMode NXSpinBox::getButtonMode() const
{
  Q_D(const NXSpinBox);
  return d->_style->getButtonMode();
}

void NXSpinBox::focusInEvent(QFocusEvent *event)
{
  Q_D(NXSpinBox);
  if (event->reason() == Qt::MouseFocusReason) {
    QPropertyAnimation *markAnimation = new QPropertyAnimation(d, "pExpandMarkWidth");
    connect(markAnimation, &QPropertyAnimation::valueChanged, this, [=](const QVariant &value) {
      update();
    });
    markAnimation->setDuration(300);
    markAnimation->setEasingCurve(QEasingCurve::InOutSine);
    markAnimation->setStartValue(d->_pExpandMarkWidth);
    markAnimation->setEndValue(width() / 2 - 3);
    markAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  }
  QSpinBox::focusInEvent(event);
}

void NXSpinBox::focusOutEvent(QFocusEvent *event)
{
  Q_D(NXSpinBox);
  if (event->reason() != Qt::PopupFocusReason) {
    QPropertyAnimation *markAnimation = new QPropertyAnimation(d, "pExpandMarkWidth");
    connect(markAnimation, &QPropertyAnimation::valueChanged, this, [=](const QVariant &value) {
      update();
    });
    markAnimation->setDuration(300);
    markAnimation->setEasingCurve(QEasingCurve::InOutSine);
    markAnimation->setStartValue(d->_pExpandMarkWidth);
    markAnimation->setEndValue(0);
    markAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  }
  QSpinBox::focusOutEvent(event);
}

void NXSpinBox::paintEvent(QPaintEvent *event)
{
  Q_D(NXSpinBox);
  if (palette().color(QPalette::Text) != NXThemeColor(d->_themeMode, BasicText)) {
    d->onThemeChanged(d->_themeMode);
  }
  QSpinBox::paintEvent(event);
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  painter.setPen(Qt::NoPen);
  painter.setBrush(NXThemeColor(d->_themeMode, PrimaryNormal));
  painter.drawRoundedRect(QRectF(width() / 2 - d->_pExpandMarkWidth, height() - 2.5, d->_pExpandMarkWidth * 2, 2.5), 2, 2);
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
  const uint se = stepEnabled();
  QAction *up   = menu->addNXIconAction(NXIconType::Plus, tr("增加"));
  up->setEnabled(se & StepUpEnabled);
  QAction *down = menu->addNXIconAction(NXIconType::Minus, tr("减少"));
  down->setEnabled(se & StepDownEnabled);
  menu->addSeparator();

  const QAbstractSpinBox *that = this;
  const QPoint pos             = (event->reason() == QContextMenuEvent::Mouse)
                                   ? event->globalPos()
                                   : mapToGlobal(QPoint(event->pos().x(), 0)) + QPoint(width() / 2, height() / 2);
  const QAction *action        = menu->exec(pos);
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
