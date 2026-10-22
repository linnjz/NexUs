#include "NXCalendarPrivate.h"

#include <QApplication>
#include <QPropertyAnimation>
#include <QScrollBar>

#include "DeveloperComponents/NXBaseListView.h"
#include "DeveloperComponents/NXCalendarDelegate.h"
#include "DeveloperComponents/NXCalendarModel.h"
#include "NXCalendar.h"
#include "NXToolButton.h"

NXCalendarPrivate::NXCalendarPrivate(QObject *parent)
    : QObject { parent }
{
  _pZoomRatio  = 1;
  _pPixOpacity = 1;
}

NXCalendarPrivate::~NXCalendarPrivate()
{
}

void NXCalendarPrivate::onSwitchButtonClicked()
{
  if (!_isSwitchAnimationFinished) {
    return;
  }
  Q_Q(NXCalendar);
  NXCalendarType displayMode = _calendarModel->getDisplayMode();
  if (displayMode == NXCalendarType::DayMode) {
    _oldCalendarViewPix = q->grab(
        QRect(_borderWidth, _borderWidth + 45, q->width() - 2 * _borderWidth, q->height() - 2 * _borderWidth - 45));
    _calendarModel->setDisplayMode(NXCalendarType::MonthMode);
    _calendarTitleView->setVisible(false);
    QApplication::processEvents();
    _calendarView->setSpacing(15);
    _scrollToDate(_pSelectedDate.isValid() ? _pSelectedDate : QDate::currentDate());
    _newCalendarViewPix = q->grab(
        QRect(_borderWidth, _borderWidth + 45, q->width() - 2 * _borderWidth, q->height() - 2 * _borderWidth - 45));
    _doSwitchAnimation(false);
  } else if (displayMode == NXCalendarType::MonthMode) {
    _oldCalendarViewPix = q->grab(
        QRect(_borderWidth, _borderWidth + 45, q->width() - 2 * _borderWidth, q->height() - 2 * _borderWidth - 45));
    _calendarModel->setDisplayMode(NXCalendarType::YearMode);
    _scrollToDate(_pSelectedDate.isValid() ? _pSelectedDate : QDate::currentDate());
    _newCalendarViewPix = q->grab(
        QRect(_borderWidth, _borderWidth + 45, q->width() - 2 * _borderWidth, q->height() - 2 * _borderWidth - 45));
    _doSwitchAnimation(false);
  }
}

void NXCalendarPrivate::onCalendarViewClicked(const QModelIndex &index)
{
  Q_Q(NXCalendar);
  if (!_isSwitchAnimationFinished) {
    return;
  }
  switch (_calendarModel->getDisplayMode()) {
  case YearMode : {
    _oldCalendarViewPix = q->grab(
        QRect(_borderWidth, _borderWidth + 45, q->width() - 2 * _borderWidth, q->height() - 2 * _borderWidth - 45));
    _lastSelectedYear = _calendarModel->getMinimumDate().year() + index.row();
    _calendarModel->setDisplayMode(NXCalendarType::MonthMode);
    _scrollToDate(QDate(_lastSelectedYear, _lastSelectedMonth, 15));
    _newCalendarViewPix = q->grab(
        QRect(_borderWidth, _borderWidth + 45, q->width() - 2 * _borderWidth, q->height() - 2 * _borderWidth - 45));
    _doSwitchAnimation(true);
    break;
  }
  case MonthMode : {
    _oldCalendarViewPix = q->grab(
        QRect(_borderWidth, _borderWidth + 45, q->width() - 2 * _borderWidth, q->height() - 2 * _borderWidth - 45));
    _lastSelectedMonth = index.row() % 12 + 1;
    _calendarModel->setDisplayMode(NXCalendarType::DayMode);
    _calendarTitleView->setVisible(true);
    _calendarView->setSpacing(0);
    _scrollToDate(QDate(_lastSelectedYear, _lastSelectedMonth, 15));
    _calendarView->selectionModel()->setCurrentIndex(_calendarModel->getIndexFromDate(_pSelectedDate), QItemSelectionModel::Select);
    _newCalendarViewPix = q->grab(
        QRect(_borderWidth, _borderWidth + 45, q->width() - 2 * _borderWidth, q->height() - 2 * _borderWidth - 45));
    _calendarTitleView->setVisible(false);
    _doSwitchAnimation(true);
    break;
  }
  case DayMode : {
    QDate date = _calendarModel->getDateFromIndex(index);
    if (date.isValid()) {
      _calendarView->clearSelection();
      q->setSelectedDate(date);
    }
    Q_EMIT q->clicked(_pSelectedDate);
    break;
  }
  }
}

void NXCalendarPrivate::onUpButtonClicked()
{
  QScrollBar *vscrollBar              = _calendarView->verticalScrollBar();
  QPropertyAnimation *scrollAnimation = new QPropertyAnimation(vscrollBar, "value");
  scrollAnimation->setEasingCurve(QEasingCurve::OutSine);
  scrollAnimation->setDuration(300);
  scrollAnimation->setStartValue(vscrollBar->value());
  scrollAnimation->setEndValue(vscrollBar->value() - 150);
  scrollAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void NXCalendarPrivate::onDownButtonClicked()
{
  QScrollBar *vscrollBar              = _calendarView->verticalScrollBar();
  QPropertyAnimation *scrollAnimation = new QPropertyAnimation(vscrollBar, "value");
  scrollAnimation->setEasingCurve(QEasingCurve::OutSine);
  scrollAnimation->setDuration(300);
  scrollAnimation->setStartValue(vscrollBar->value());
  scrollAnimation->setEndValue(vscrollBar->value() + 150);
  scrollAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void NXCalendarPrivate::_scrollToDate(QDate date)
{
  int index = _calendarModel->getIndexFromDate(date).row();
  switch (_calendarModel->getDisplayMode()) {
  case YearMode :
  case MonthMode : {
    _calendarView->scrollTo(_calendarModel->index(index - 4), QAbstractItemView::PositionAtTop);
    break;
  }
  case DayMode : {
    _calendarView->scrollTo(_calendarModel->index(index - 14), QAbstractItemView::PositionAtTop);
    break;
  }
  }
}

void NXCalendarPrivate::_doSwitchAnimation(bool isZoomIn)
{
  Q_Q(NXCalendar);
  if (!_isSwitchAnimationFinished) {
    return;
  }
  _isDrawNewPix              = false;
  _isSwitchAnimationFinished = false;
  _calendarDelegate->setIsTransparent(true);
  QPropertyAnimation *oldPixZoomAnimation = new QPropertyAnimation(this, "pZoomRatio");
  connect(oldPixZoomAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
    q->update();
  });
  connect(oldPixZoomAnimation, &QPropertyAnimation::finished, this, [=]() {
    _isDrawNewPix                           = true;
    QPropertyAnimation *newPixZoomAnimation = new QPropertyAnimation(this, "pZoomRatio");
    connect(newPixZoomAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
      q->update();
    });
    connect(newPixZoomAnimation, &QPropertyAnimation::finished, this, [=]() {
      if (_calendarModel->getDisplayMode() == NXCalendarType::DayMode) {
        _calendarTitleView->setVisible(true);
      }
      _isSwitchAnimationFinished = true;
      _calendarDelegate->setIsTransparent(false);
    });
    if (isZoomIn) {
      // 放大 年-月-日
      newPixZoomAnimation->setStartValue(0.85);
      newPixZoomAnimation->setEndValue(1);
    } else {
      newPixZoomAnimation->setStartValue(1.5);
      newPixZoomAnimation->setEndValue(1);
    }
    newPixZoomAnimation->setDuration(300);
    newPixZoomAnimation->setEasingCurve(QEasingCurve::OutCubic);
    newPixZoomAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  });
  if (isZoomIn) {
    // 放大 年-月-日
    oldPixZoomAnimation->setStartValue(1);
    oldPixZoomAnimation->setEndValue(1.15);
  } else {
    // 缩小 日-月-年
    oldPixZoomAnimation->setStartValue(1);
    oldPixZoomAnimation->setEndValue(0.85);
  }
  oldPixZoomAnimation->setDuration(150);
  oldPixZoomAnimation->start(QAbstractAnimation::DeleteWhenStopped);

  QPropertyAnimation *oldPixOpacityAnimation = new QPropertyAnimation(this, "pPixOpacity");
  connect(oldPixZoomAnimation, &QPropertyAnimation::finished, this, [=]() {
    QPropertyAnimation *newPixOpacityAnimation = new QPropertyAnimation(this, "pPixOpacity");
    newPixOpacityAnimation->setStartValue(0);
    newPixOpacityAnimation->setEndValue(1);
    newPixOpacityAnimation->setDuration(300);
    newPixOpacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  });
  oldPixOpacityAnimation->setStartValue(1);
  oldPixOpacityAnimation->setEndValue(0);
  oldPixOpacityAnimation->setDuration(150);
  oldPixOpacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void NXCalendarPrivate::_updateSwitchButtonText()
{
  QModelIndex modelIndex =
      _calendarView->indexAt(QPoint(_calendarView->rect().center().x() - 20, _calendarView->rect().center().y()));
  if (!modelIndex.isValid()) {
    return;
  }
  NXCalendarData data = _calendarModel->data(modelIndex, Qt::UserRole).value<NXCalendarData>();
  switch (_calendarModel->getDisplayMode()) {
  case YearMode : {
    _modeSwitchButton->setText(QString(QStringLiteral("%1-%2")).arg(data.year - 5).arg(data.year + 10));
    break;
  }
  case MonthMode : {
    _modeSwitchButton->setText(QString(QStringLiteral("%1年")).arg(data.year));
    break;
  }
  case DayMode : {
    _modeSwitchButton->setText(QString(QStringLiteral("%1年%2月")).arg(data.year).arg(data.month));
    break;
  }
  }
}
