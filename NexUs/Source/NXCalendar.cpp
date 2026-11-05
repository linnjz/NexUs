#include "NXCalendar.h"

#include <QEvent>
#include <QHBoxLayout>
#include <QPainter>
#include <QVBoxLayout>

#include "DeveloperComponents/NXBaseListView.h"
#include "DeveloperComponents/NXCalendarDelegate.h"
#include "DeveloperComponents/NXCalendarModel.h"
#include "DeveloperComponents/NXCalendarTitleDelegate.h"
#include "DeveloperComponents/NXCalendarTitleModel.h"
#include "NXScrollBar.h"
#include "NXTheme.h"
#include "NXToolButton.h"
#include "private/NXCalendarPrivate.h"
Q_PROPERTY_CREATE_CPP(NXCalendar, QPS_PAEMIT(int), BorderRaiuds)

NXCalendar::NXCalendar(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXCalendarPrivate())
{
  Q_D(NXCalendar);
  setFixedSize(305, 340);
  setObjectName("NXCalendar");
  d->q_ptr          = this;
  d->m_BorderRaiuds = 5;

  // 日历标题
  d->m_CalendarTitleView = new NXBaseListView(this);
  d->m_CalendarTitleView->setFlow(QListView::LeftToRight);
  d->m_CalendarTitleView->setViewMode(QListView::IconMode);
  d->m_CalendarTitleView->setResizeMode(QListView::Adjust);
  d->m_CalendarTitleView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  d->m_CalendarTitleView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  d->m_CalendarTitleView->setModel(new NXCalendarTitleModel(this));
  d->m_CalendarTitleView->setItemDelegate(new NXCalendarTitleDelegate(this));
  d->m_CalendarTitleView->setFixedHeight(30);

  // 日历内容
  d->m_CalendarView = new NXBaseListView(this);
  d->m_CalendarView->setFlow(QListView::LeftToRight);
  d->m_CalendarView->setViewMode(QListView::IconMode);
  d->m_CalendarView->setResizeMode(QListView::Adjust);
  NXScrollBar *vScrollBar = dynamic_cast<NXScrollBar *>(d->m_CalendarView->verticalScrollBar());
  vScrollBar->setSpeedLimit(6);
  d->m_CalendarView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  d->m_CalendarView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  d->m_CalendarView->setSelectionMode(QAbstractItemView::NoSelection);
  d->m_CalendarModel = new NXCalendarModel(this);
  d->m_CalendarView->setModel(d->m_CalendarModel);
  d->m_CalendarDelegate = new NXCalendarDelegate(d->m_CalendarModel, this);
  d->m_CalendarView->setItemDelegate(d->m_CalendarDelegate);
  connect(d->m_CalendarView, &NXBaseListView::clicked, d, &NXCalendarPrivate::onCalendarViewClicked);

  // 模式切换按钮
  d->m_ModeSwitchButton = new NXToolButton(this);
  d->m_ModeSwitchButton->setText(QStringLiteral("1924年1月"));
  QFont switchButtonFont = d->m_ModeSwitchButton->font();
  switchButtonFont.setWeight(QFont::Bold);
  d->m_ModeSwitchButton->setFont(switchButtonFont);
  d->m_ModeSwitchButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
  d->m_ModeSwitchButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  connect(d->m_ModeSwitchButton, &NXToolButton::clicked, d, &NXCalendarPrivate::onSwitchButtonClicked);
  connect(d->m_CalendarView->verticalScrollBar(), &QScrollBar::valueChanged, d, [=]() {
    d->_updateSwitchButtonText();
  });

  // 翻页按钮
  d->m_UpButton = new NXToolButton(this);
  d->m_UpButton->setFixedSize(36, 36);
  d->m_UpButton->setNXIcon(NXIconType::CaretUp);
  connect(d->m_UpButton, &NXToolButton::clicked, d, &NXCalendarPrivate::onUpButtonClicked);

  d->m_DownButton = new NXToolButton(this);
  d->m_DownButton->setFixedSize(36, 36);
  d->m_DownButton->setNXIcon(NXIconType::CaretDown);
  connect(d->m_DownButton, &NXToolButton::clicked, d, &NXCalendarPrivate::onDownButtonClicked);

  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->setContentsMargins(5, 5, 10, 0);
  buttonLayout->addWidget(d->m_ModeSwitchButton);
  buttonLayout->addWidget(d->m_UpButton);
  buttonLayout->addWidget(d->m_DownButton);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(5, 0, 0, 0);
  mainLayout->addLayout(buttonLayout);
  mainLayout->addWidget(d->m_CalendarTitleView);
  mainLayout->addWidget(d->m_CalendarView);

  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
  });
  setVisible(true);
  QDate currentDate     = QDate::currentDate();
  d->m_LastSelectedYear  = currentDate.year();
  d->m_LastSelectedMonth = currentDate.month();
  d->_scrollToDate(currentDate);
}

NXCalendar::~NXCalendar()
{
}

void NXCalendar::setSelectedDate(QDate selectedDate)
{
  Q_D(NXCalendar);
  if (!selectedDate.isValid() || selectedDate.daysTo(d->m_CalendarModel->getMaximumDate()) < 0 ||
      selectedDate.daysTo(d->m_CalendarModel->getMinimumDate()) > 0) {
    return;
  }
  d->m_SelectedDate = selectedDate;
  d->m_CalendarView->selectionModel()->setCurrentIndex(d->m_CalendarModel->getIndexFromDate(selectedDate), QItemSelectionModel::Select);
  Q_EMIT signalSelectedDateChanged();
}

QDate NXCalendar::getSelectedDate() const noexcept(std::is_nothrow_copy_constructible_v<QDate>)
{
  Q_CONST_D(NXCalendar);
  return d->m_SelectedDate;
  //return d->m_CalendarModel->getDateFromIndex(d->m_CalendarView->selectionModel()->currentIndex());
}

void NXCalendar::setMinimumDate(QDate minimudate)
{
  Q_D(NXCalendar);
  if (!minimudate.isValid() || minimudate.daysTo(d->m_CalendarModel->getMaximumDate()) < 0) {
    return;
  }
  d->m_CalendarModel->setMaximumDate(minimudate);
  Q_EMIT signalMinimumDateChanged();
}

QDate NXCalendar::getMinimumDate() const noexcept(std::is_nothrow_copy_constructible_v<QDate>)
{
  Q_CONST_D(NXCalendar);
  return d->m_CalendarModel->getMinimumDate();
}

void NXCalendar::setMaximumDate(QDate maximumDate)
{
  Q_D(NXCalendar);
  if (!maximumDate.isValid() || maximumDate.daysTo(d->m_CalendarModel->getMinimumDate()) > 0) {
    return;
  }
  d->m_CalendarModel->setMaximumDate(maximumDate);
  Q_EMIT signalMaximumDateChanged();
}

QDate NXCalendar::getMaximumDate() const noexcept(std::is_nothrow_copy_constructible_v<QDate>)
{
  Q_CONST_D(NXCalendar);
  return d->m_CalendarModel->getMaximumDate();
}

void NXCalendar::paintEvent(QPaintEvent *event)
{
  Q_D(NXCalendar);
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

  QRect baseRect = rect();
  baseRect.adjust(d->m_BorderWidth, d->m_BorderWidth, -d->m_BorderWidth, -d->m_BorderWidth);
  painter.setPen(Qt::NoPen);
  painter.setBrush(NXThemeColor(d->m_ThemeMode, DialogBase));
  painter.drawRoundedRect(baseRect, d->m_BorderRaiuds, d->m_BorderRaiuds);
  // 缩放动画
  if (!d->m_IsSwitchAnimationFinished) {
    painter.save();
    QRect pixRect = QRect(baseRect.x(), d->m_BorderWidth + 45, baseRect.width(), baseRect.height() - 45);
    painter.setOpacity(d->m_PixOpacity);
    painter.translate(pixRect.center());
    painter.scale(d->m_ZoomRatio, d->m_ZoomRatio);
    painter.translate(-pixRect.center());
    painter.drawPixmap(pixRect, d->m_IsDrawNewPix ? d->m_NewCalendarViewPix : d->m_OldCalendarViewPix);
    painter.restore();
  }
  // 分割线
  painter.setPen(QPen(NXThemeColor(d->m_ThemeMode, BasicBorder), d->m_BorderWidth));
  painter.setBrush(Qt::NoBrush);
  painter.drawRoundedRect(baseRect, d->m_BorderRaiuds, d->m_BorderRaiuds);
  painter.drawLine(QPointF(baseRect.x(), 45), QPointF(baseRect.right(), 45));
  painter.restore();
}
