#include "NXCalendarDelegate.h"

#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>

#include "NXApplication.h"
#include "NXTheme.h"

NXCalendarDelegate::NXCalendarDelegate(NXCalendarModel *calendarModel, QObject *parent)
    : QStyledItemDelegate { parent }
    , m_ItemWidth { 42 }
    , m_ItemHeight { 42 }
    , m_IsTransparent { false }
    , m_ThemeMode { nxTheme->getThemeMode() }
    , m_NowDate { QDate::currentDate() }
    , m_CalendarModel { calendarModel }
{
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    m_ThemeMode = themeMode;
  });
  connect(m_CalendarModel, &NXCalendarModel::displayModeChanged, this, &NXCalendarDelegate::onCalendarModelDisplayModeChanged);
}

NXCalendarDelegate::~NXCalendarDelegate()
{
}

void NXCalendarDelegate::onCalendarModelDisplayModeChanged()
{
  switch (m_CalendarModel->getDisplayMode()) {
  case YearMode :
  case MonthMode : {
    m_ItemWidth  = 53;
    m_ItemHeight = 60;
    break;
  }
  case DayMode : {
    m_ItemWidth  = 42;
    m_ItemHeight = 42;
    break;
  }
  }
}

void NXCalendarDelegate::paint(QPainter *painter, QStyleOptionViewItem const &option, QModelIndex const &index) const
{
  if (m_IsTransparent) {
    return;
  }
  painter->save();
  painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  NXCalendarType displayModel = m_CalendarModel->getDisplayMode();
  switch (displayModel) {
  case YearMode :
  case MonthMode : {
    _drawYearOrMonth(painter, option, index);
    break;
  }
  case DayMode : {
    _drawDays(painter, option, index);
    break;
  }
  }
  painter->restore();
  QStyledItemDelegate::paint(painter, option, index);
}

QSize NXCalendarDelegate::sizeHint(QStyleOptionViewItem const &option, QModelIndex const &index) const
{
  return QSize(m_ItemWidth, m_ItemHeight);
}

void NXCalendarDelegate::_drawYearOrMonth(QPainter *painter, QStyleOptionViewItem const &option, QModelIndex const &index) const
{
  QRectF itemRect     = option.rect;
  bool isNow          = false;
  NXCalendarData data = index.data(Qt::UserRole).value<NXCalendarData>();
  qreal baseRadius    = m_ItemWidth * 0.5;
  // 当前日期绘制
  NXCalendarType displayMode = m_CalendarModel->getDisplayMode();
  if ((displayMode == NXCalendarType::YearMode && data.year == m_NowDate.year()) ||
      (displayMode == NXCalendarType::MonthMode && data.month == m_NowDate.month() && data.year == m_NowDate.year())) {
    isNow            = true;
    QColor drawColor = option.state.testFlag(QStyle::State_MouseOver) ? NXThemeColor(m_ThemeMode, PrimaryHover)
                                                                      : NXThemeColor(m_ThemeMode, PrimaryNormal);
    painter->setPen(Qt::NoPen);
    painter->setBrush(drawColor);
    painter->drawEllipse(itemRect.center(), baseRadius, baseRadius);
  } else {
    // 覆盖和选中效果绘制
    if (option.state.testFlag(QStyle::State_MouseOver)) {
      painter->setPen(Qt::NoPen);
      painter->setBrush(NXThemeColor(m_ThemeMode, BasicHover));
      painter->drawEllipse(itemRect.center(), baseRadius, baseRadius);
    }
  }

  // 文字绘制
  painter->setPen(isNow ? NXThemeColor(m_ThemeMode, BasicTextInvert) : NXThemeColor(m_ThemeMode, BasicText));
  painter->drawText(itemRect, Qt::AlignCenter, displayMode == NXCalendarType::YearMode ? QString::number(data.year) : QString::number(data.month));
  QString desText = data.desText;
  if (!desText.isEmpty()) {
    itemRect.adjust(0, 10, 0, 0);
    QFont font = painter->font();
    font.setPixelSize(nxApp->getFontPixelSize() - 4);
    painter->setFont(font);
    painter->drawText(itemRect, Qt::AlignTop | Qt::AlignHCenter, desText);
  }
}

void NXCalendarDelegate::_drawDays(QPainter *painter, QStyleOptionViewItem const &option, QModelIndex const &index) const
{
  qreal penWidth   = 1.5;
  qreal baseRadius = m_ItemWidth * 0.5 - penWidth;
  QVariant variant = index.data(Qt::UserRole);
  if (variant.isValid()) {
    QRectF itemRect     = option.rect;
    bool isNow          = false;
    NXCalendarData data = variant.value<NXCalendarData>();
    // 当前日期绘制
    if (data.year == m_NowDate.year() && data.month == m_NowDate.month() && data.day == m_NowDate.day()) {
      isNow            = true;
      QColor drawColor = option.state.testFlag(QStyle::State_MouseOver) ? NXThemeColor(m_ThemeMode, PrimaryHover)
                                                                        : NXThemeColor(m_ThemeMode, PrimaryNormal);
      if (option.state.testFlag(QStyle::State_Selected)) {
        painter->setPen(QPen(drawColor, penWidth));
        painter->drawEllipse(itemRect.center(), baseRadius, baseRadius);
        painter->setBrush(drawColor);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(itemRect.center(), baseRadius - 2 * penWidth, baseRadius - 2 * penWidth);
      } else {
        painter->setPen(Qt::NoPen);
        painter->setBrush(drawColor);
        painter->drawEllipse(itemRect.center(), baseRadius, baseRadius);
      }
    } else {
      // 覆盖和选中效果绘制
      if (option.state.testFlag(QStyle::State_Selected)) {
        painter->setPen(QPen(option.state.testFlag(QStyle::State_MouseOver) ? NXThemeColor(m_ThemeMode, PrimaryHover) : NXThemeColor(m_ThemeMode, PrimaryNormal), penWidth));
        painter->setBrush(Qt::NoBrush);
        painter->drawEllipse(itemRect.center(), baseRadius, baseRadius);
      }
      if (option.state.testFlag(QStyle::State_MouseOver)) {
        painter->setPen(Qt::NoPen);
        painter->setBrush(NXThemeColor(m_ThemeMode, BasicHover));
        if (option.state.testFlag(QStyle::State_Selected)) {
          painter->drawEllipse(itemRect.center(), baseRadius - penWidth, baseRadius - penWidth);
        } else {
          painter->drawEllipse(itemRect.center(), baseRadius, baseRadius);
        }
      }
    }

    // 文字绘制
    painter->setPen(isNow ? NXThemeColor(m_ThemeMode, BasicTextInvert) : NXThemeColor(m_ThemeMode, BasicText));
    painter->drawText(itemRect, Qt::AlignCenter, QString::number(data.day));
    QString desText = data.desText;
    if (!desText.isEmpty()) {
      itemRect.adjust(0, 2 * penWidth, 0, 0);
      QFont font = painter->font();
      font.setPixelSize(nxApp->getFontPixelSize() - 4);
      painter->setFont(font);
      painter->drawText(itemRect, Qt::AlignTop | Qt::AlignHCenter, desText);
    }
  }
}
