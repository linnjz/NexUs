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
    , m_ZoomRatio { 1.0 }
    , m_PixOpacity { 1.0 }
{
}

NXCalendarPrivate::~NXCalendarPrivate()
{
}

void NXCalendarPrivate::onSwitchButtonClicked()
{
  if (!m_IsSwitchAnimationFinished) {
    return;
  }
  Q_Q(NXCalendar);
  NXCalendarType displayMode = m_CalendarModel->getDisplayMode();
  if (displayMode == NXCalendarType::DayMode) {
    m_OldCalendarViewPix = q->grab(
        QRect(m_BorderWidth, m_BorderWidth + 45, q->width() - 2 * m_BorderWidth, q->height() - 2 * m_BorderWidth - 45));
    m_CalendarModel->setDisplayMode(NXCalendarType::MonthMode);
    m_CalendarTitleView->setVisible(false);
    QApplication::processEvents();
    m_CalendarView->setSpacing(15);
    _scrollToDate(m_SelectedDate.isValid() ? m_SelectedDate : QDate::currentDate());
    m_NewCalendarViewPix = q->grab(
        QRect(m_BorderWidth, m_BorderWidth + 45, q->width() - 2 * m_BorderWidth, q->height() - 2 * m_BorderWidth - 45));
    _doSwitchAnimation(false);
  } else if (displayMode == NXCalendarType::MonthMode) {
    m_OldCalendarViewPix = q->grab(
        QRect(m_BorderWidth, m_BorderWidth + 45, q->width() - 2 * m_BorderWidth, q->height() - 2 * m_BorderWidth - 45));
    m_CalendarModel->setDisplayMode(NXCalendarType::YearMode);
    _scrollToDate(m_SelectedDate.isValid() ? m_SelectedDate : QDate::currentDate());
    m_NewCalendarViewPix = q->grab(
        QRect(m_BorderWidth, m_BorderWidth + 45, q->width() - 2 * m_BorderWidth, q->height() - 2 * m_BorderWidth - 45));
    _doSwitchAnimation(false);
  }
}

void NXCalendarPrivate::onCalendarViewClicked(QModelIndex const &index)
{
  Q_Q(NXCalendar);
  if (!m_IsSwitchAnimationFinished) {
    return;
  }
  switch (m_CalendarModel->getDisplayMode()) {
  case YearMode : {
    m_OldCalendarViewPix = q->grab(
        QRect(m_BorderWidth, m_BorderWidth + 45, q->width() - 2 * m_BorderWidth, q->height() - 2 * m_BorderWidth - 45));
    m_LastSelectedYear = m_CalendarModel->getMinimumDate().year() + index.row();
    m_CalendarModel->setDisplayMode(NXCalendarType::MonthMode);
    _scrollToDate(QDate(m_LastSelectedYear, m_LastSelectedMonth, 15));
    m_NewCalendarViewPix = q->grab(
        QRect(m_BorderWidth, m_BorderWidth + 45, q->width() - 2 * m_BorderWidth, q->height() - 2 * m_BorderWidth - 45));
    _doSwitchAnimation(true);
    break;
  }
  case MonthMode : {
    m_OldCalendarViewPix = q->grab(
        QRect(m_BorderWidth, m_BorderWidth + 45, q->width() - 2 * m_BorderWidth, q->height() - 2 * m_BorderWidth - 45));
    m_LastSelectedMonth = index.row() % 12 + 1;
    m_CalendarModel->setDisplayMode(NXCalendarType::DayMode);
    m_CalendarTitleView->setVisible(true);
    m_CalendarView->setSpacing(0);
    _scrollToDate(QDate(m_LastSelectedYear, m_LastSelectedMonth, 15));
    m_CalendarView->selectionModel()->setCurrentIndex(m_CalendarModel->getIndexFromDate(m_SelectedDate), QItemSelectionModel::Select);
    m_NewCalendarViewPix = q->grab(
        QRect(m_BorderWidth, m_BorderWidth + 45, q->width() - 2 * m_BorderWidth, q->height() - 2 * m_BorderWidth - 45));
    m_CalendarTitleView->setVisible(false);
    _doSwitchAnimation(true);
    break;
  }
  case DayMode : {
    QDate date = m_CalendarModel->getDateFromIndex(index);
    if (date.isValid()) {
      m_CalendarView->clearSelection();
      q->setSelectedDate(date);
    }
    Q_EMIT q->clicked(m_SelectedDate);
    break;
  }
  }
}

void NXCalendarPrivate::onUpButtonClicked()
{
  QScrollBar *vscrollBar              = m_CalendarView->verticalScrollBar();
  QPropertyAnimation *scrollAnimation = new QPropertyAnimation(vscrollBar, "value");
  scrollAnimation->setEasingCurve(QEasingCurve::OutSine);
  scrollAnimation->setDuration(300);
  scrollAnimation->setStartValue(vscrollBar->value());
  scrollAnimation->setEndValue(vscrollBar->value() - 150);
  scrollAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void NXCalendarPrivate::onDownButtonClicked()
{
  QScrollBar *vscrollBar              = m_CalendarView->verticalScrollBar();
  QPropertyAnimation *scrollAnimation = new QPropertyAnimation(vscrollBar, "value");
  scrollAnimation->setEasingCurve(QEasingCurve::OutSine);
  scrollAnimation->setDuration(300);
  scrollAnimation->setStartValue(vscrollBar->value());
  scrollAnimation->setEndValue(vscrollBar->value() + 150);
  scrollAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void NXCalendarPrivate::_scrollToDate(QDate date)
{
  int index = m_CalendarModel->getIndexFromDate(date).row();
  switch (m_CalendarModel->getDisplayMode()) {
  case YearMode :
  case MonthMode : {
    m_CalendarView->scrollTo(m_CalendarModel->index(index - 4), QAbstractItemView::PositionAtTop);
    break;
  }
  case DayMode : {
    m_CalendarView->scrollTo(m_CalendarModel->index(index - 14), QAbstractItemView::PositionAtTop);
    break;
  }
  }
}

void NXCalendarPrivate::_doSwitchAnimation(bool isZoomIn)
{
  Q_Q(NXCalendar);
  if (!m_IsSwitchAnimationFinished) {
    return;
  }
  m_IsDrawNewPix              = false;
  m_IsSwitchAnimationFinished = false;
  m_CalendarDelegate->setIsTransparent(true);
  QPropertyAnimation *oldPixZoomAnimation = new QPropertyAnimation(this, "ZoomRatio");
  connect(oldPixZoomAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
    q->update();
  });
  connect(oldPixZoomAnimation, &QPropertyAnimation::finished, this, [=]() {
    m_IsDrawNewPix                           = true;
    QPropertyAnimation *newPixZoomAnimation = new QPropertyAnimation(this, "ZoomRatio");
    connect(newPixZoomAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
      q->update();
    });
    connect(newPixZoomAnimation, &QPropertyAnimation::finished, this, [=]() {
      if (m_CalendarModel->getDisplayMode() == NXCalendarType::DayMode) {
        m_CalendarTitleView->setVisible(true);
      }
      m_IsSwitchAnimationFinished = true;
      m_CalendarDelegate->setIsTransparent(false);
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

  QPropertyAnimation *oldPixOpacityAnimation = new QPropertyAnimation(this, "PixOpacity");
  connect(oldPixZoomAnimation, &QPropertyAnimation::finished, this, [=]() {
    QPropertyAnimation *newPixOpacityAnimation = new QPropertyAnimation(this, "PixOpacity");
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
      m_CalendarView->indexAt(QPoint(m_CalendarView->rect().center().x() - 20, m_CalendarView->rect().center().y()));
  if (!modelIndex.isValid()) {
    return;
  }
  NXCalendarData data = m_CalendarModel->data(modelIndex, Qt::UserRole).value<NXCalendarData>();
  switch (m_CalendarModel->getDisplayMode()) {
  case YearMode : {
    m_ModeSwitchButton->setText(QString(QStringLiteral("%1-%2")).arg(data.year - 5).arg(data.year + 10));
    break;
  }
  case MonthMode : {
    m_ModeSwitchButton->setText(QString(QStringLiteral("%1年")).arg(data.year));
    break;
  }
  case DayMode : {
    m_ModeSwitchButton->setText(QString(QStringLiteral("%1年%2月")).arg(data.year).arg(data.month));
    break;
  }
  }
}
