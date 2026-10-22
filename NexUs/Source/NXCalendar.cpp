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
Q_PROPERTY_CREATE_CPP(NXCalendar, int, BorderRaiuds)

NXCalendar::NXCalendar(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXCalendarPrivate())
{
  Q_D(NXCalendar);
  setFixedSize(305, 340);
  setObjectName("NXCalendar");
  d->q_ptr          = this;
  d->_pBorderRaiuds = 5;

  // 日历标题
  d->_calendarTitleView = new NXBaseListView(this);
  d->_calendarTitleView->setFlow(QListView::LeftToRight);
  d->_calendarTitleView->setViewMode(QListView::IconMode);
  d->_calendarTitleView->setResizeMode(QListView::Adjust);
  d->_calendarTitleView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  d->_calendarTitleView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  d->_calendarTitleView->setModel(new NXCalendarTitleModel(this));
  d->_calendarTitleView->setItemDelegate(new NXCalendarTitleDelegate(this));
  d->_calendarTitleView->setFixedHeight(30);

  // 日历内容
  d->_calendarView = new NXBaseListView(this);
  d->_calendarView->setFlow(QListView::LeftToRight);
  d->_calendarView->setViewMode(QListView::IconMode);
  d->_calendarView->setResizeMode(QListView::Adjust);
  NXScrollBar *vScrollBar = dynamic_cast<NXScrollBar *>(d->_calendarView->verticalScrollBar());
  vScrollBar->setSpeedLimit(6);
  d->_calendarView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  d->_calendarView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  d->_calendarView->setSelectionMode(QAbstractItemView::NoSelection);
  d->_calendarModel = new NXCalendarModel(this);
  d->_calendarView->setModel(d->_calendarModel);
  d->_calendarDelegate = new NXCalendarDelegate(d->_calendarModel, this);
  d->_calendarView->setItemDelegate(d->_calendarDelegate);
  connect(d->_calendarView, &NXBaseListView::clicked, d, &NXCalendarPrivate::onCalendarViewClicked);

  // 模式切换按钮
  d->_modeSwitchButton = new NXToolButton(this);
  d->_modeSwitchButton->setText(QStringLiteral("1924年1月"));
  QFont switchButtonFont = d->_modeSwitchButton->font();
  switchButtonFont.setWeight(QFont::Bold);
  d->_modeSwitchButton->setFont(switchButtonFont);
  d->_modeSwitchButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
  d->_modeSwitchButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  connect(d->_modeSwitchButton, &NXToolButton::clicked, d, &NXCalendarPrivate::onSwitchButtonClicked);
  connect(d->_calendarView->verticalScrollBar(), &QScrollBar::valueChanged, d, [=]() {
    d->_updateSwitchButtonText();
  });

  // 翻页按钮
  d->_upButton = new NXToolButton(this);
  d->_upButton->setFixedSize(36, 36);
  d->_upButton->setNXIcon(NXIconType::CaretUp);
  connect(d->_upButton, &NXToolButton::clicked, d, &NXCalendarPrivate::onUpButtonClicked);

  d->_downButton = new NXToolButton(this);
  d->_downButton->setFixedSize(36, 36);
  d->_downButton->setNXIcon(NXIconType::CaretDown);
  connect(d->_downButton, &NXToolButton::clicked, d, &NXCalendarPrivate::onDownButtonClicked);

  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->setContentsMargins(5, 5, 10, 0);
  buttonLayout->addWidget(d->_modeSwitchButton);
  buttonLayout->addWidget(d->_upButton);
  buttonLayout->addWidget(d->_downButton);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(5, 0, 0, 0);
  mainLayout->addLayout(buttonLayout);
  mainLayout->addWidget(d->_calendarTitleView);
  mainLayout->addWidget(d->_calendarView);

  d->_themeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
  });
  setVisible(true);
  QDate currentDate     = QDate::currentDate();
  d->_lastSelectedYear  = currentDate.year();
  d->_lastSelectedMonth = currentDate.month();
  d->_scrollToDate(currentDate);
}

NXCalendar::~NXCalendar()
{
}

void NXCalendar::setSelectedDate(QDate selectedDate)
{
  Q_D(NXCalendar);
  if (!selectedDate.isValid() || selectedDate.daysTo(d->_calendarModel->getMaximumDate()) < 0 ||
      selectedDate.daysTo(d->_calendarModel->getMinimumDate()) > 0) {
    return;
  }
  d->_pSelectedDate = selectedDate;
  d->_calendarView->selectionModel()->setCurrentIndex(d->_calendarModel->getIndexFromDate(selectedDate), QItemSelectionModel::Select);
  Q_EMIT pSelectedDateChanged();
}

QDate NXCalendar::getSelectedDate() const
{
  Q_D(const NXCalendar);
  return d->_pSelectedDate;
  //return d->_calendarModel->getDateFromIndex(d->_calendarView->selectionModel()->currentIndex());
}

void NXCalendar::setMinimumDate(QDate minimudate)
{
  Q_D(NXCalendar);
  if (!minimudate.isValid() || minimudate.daysTo(d->_calendarModel->getMaximumDate()) < 0) {
    return;
  }
  d->_calendarModel->setMaximumDate(minimudate);
  Q_EMIT pMinimumDateChanged();
}

QDate NXCalendar::getMinimumDate() const
{
  Q_D(const NXCalendar);
  return d->_calendarModel->getMinimumDate();
}

void NXCalendar::setMaximumDate(QDate maximumDate)
{
  Q_D(NXCalendar);
  if (!maximumDate.isValid() || maximumDate.daysTo(d->_calendarModel->getMinimumDate()) > 0) {
    return;
  }
  d->_calendarModel->setMaximumDate(maximumDate);
  Q_EMIT pMaximumDateChanged();
}

QDate NXCalendar::getMaximumDate() const
{
  Q_D(const NXCalendar);
  return d->_calendarModel->getMaximumDate();
}

void NXCalendar::paintEvent(QPaintEvent *event)
{
  Q_D(NXCalendar);
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

  QRect baseRect = rect();
  baseRect.adjust(d->_borderWidth, d->_borderWidth, -d->_borderWidth, -d->_borderWidth);
  painter.setPen(Qt::NoPen);
  painter.setBrush(NXThemeColor(d->_themeMode, DialogBase));
  painter.drawRoundedRect(baseRect, d->_pBorderRaiuds, d->_pBorderRaiuds);
  // 缩放动画
  if (!d->_isSwitchAnimationFinished) {
    painter.save();
    QRect pixRect = QRect(baseRect.x(), d->_borderWidth + 45, baseRect.width(), baseRect.height() - 45);
    painter.setOpacity(d->_pPixOpacity);
    painter.translate(pixRect.center());
    painter.scale(d->_pZoomRatio, d->_pZoomRatio);
    painter.translate(-pixRect.center());
    painter.drawPixmap(pixRect, d->_isDrawNewPix ? d->_newCalendarViewPix : d->_oldCalendarViewPix);
    painter.restore();
  }
  // 分割线
  painter.setPen(QPen(NXThemeColor(d->_themeMode, BasicBorder), d->_borderWidth));
  painter.setBrush(Qt::NoBrush);
  painter.drawRoundedRect(baseRect, d->_pBorderRaiuds, d->_pBorderRaiuds);
  painter.drawLine(QPointF(baseRect.x(), 45), QPointF(baseRect.right(), 45));
  painter.restore();
}
