#include "NXRibbonTabBar.h"

#include <QFontMetrics>
#include <QMouseEvent>
#include <QPainter>

#include "NXApplication.h"
#include "NXTheme.h"
#include "private/NXRibbonTabBarPrivate.h"

NXRibbonTabBar::NXRibbonTabBar(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXRibbonTabBarPrivate())
{
  Q_D(NXRibbonTabBar);
  d->q_ptr          = this;
  d->m_CurrentIndex = 0;

  setObjectName("NXRibbonTabBar");
  setMouseTracking(true);
  setAttribute(Qt::WA_StyledBackground, false);
  // 高度随全局字号推导 基线fs=13时为32
  int const fontPixelSize = nxApp->getFontPixelSize();
  setFixedHeight(fontPixelSize * 2 + 6);

  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
    update();
  });
}

NXRibbonTabBar::~NXRibbonTabBar()
{
}

int NXRibbonTabBar::appendTab(QString const &title)
{
  Q_D(NXRibbonTabBar);
  d->m_TabTitles.append(title);
  d->m_TabEnableds.append(true);
  update();
  updateGeometry();
  return d->m_TabTitles.size() - 1;
}

void NXRibbonTabBar::removeTab(int index)
{
  Q_D(NXRibbonTabBar);
  if (index < 0 || index >= d->m_TabTitles.size()) {
    return;
  }
  d->m_TabTitles.removeAt(index);
  d->m_TabEnableds.removeAt(index);
  if (d->m_CurrentIndex >= d->m_TabTitles.size()) {
    d->m_CurrentIndex = qMax(0, d->m_TabTitles.size() - 1);
    Q_EMIT signalCurrentIndexChanged();
  }
  update();
  updateGeometry();
}

void NXRibbonTabBar::clear()
{
  Q_D(NXRibbonTabBar);
  d->m_TabTitles.clear();
  d->m_TabEnableds.clear();
  d->m_CurrentIndex = 0;
  d->m_HoveredIndex  = -1;
  update();
  updateGeometry();
}

int NXRibbonTabBar::tabCount() const
{
  Q_CONST_D(NXRibbonTabBar);
  return d->m_TabTitles.size();
}

QString NXRibbonTabBar::tabText(int index) const
{
  Q_CONST_D(NXRibbonTabBar);
  if (index < 0 || index >= d->m_TabTitles.size()) {
    return {};
  }
  return d->m_TabTitles.at(index);
}

void NXRibbonTabBar::setTabText(int index, QString const &title)
{
  Q_D(NXRibbonTabBar);
  if (index < 0 || index >= d->m_TabTitles.size()) {
    return;
  }
  d->m_TabTitles[index] = title;
  update();
  updateGeometry();
}

void NXRibbonTabBar::setTabEnabled(int index, bool isEnable)
{
  Q_D(NXRibbonTabBar);
  if (index < 0 || index >= d->m_TabEnableds.size()) {
    return;
  }
  d->m_TabEnableds[index] = isEnable;
  if (!isEnable && d->m_HoveredIndex == index) {
    d->m_HoveredIndex = -1;
  }
  update();
}

bool NXRibbonTabBar::isTabEnabled(int index) const
{
  Q_CONST_D(NXRibbonTabBar);
  if (index < 0 || index >= d->m_TabEnableds.size()) {
    return false;
  }
  return d->m_TabEnableds.at(index);
}

void NXRibbonTabBar::setCurrentIndex(int currentIndex)
{
  Q_D(NXRibbonTabBar);
  if (currentIndex < 0 || currentIndex >= d->m_TabTitles.size()) {
    return;
  }
  if (d->m_CurrentIndex == currentIndex) {
    return;
  }
  d->m_CurrentIndex = currentIndex;
  Q_EMIT signalCurrentIndexChanged();
  update();
}

int NXRibbonTabBar::getCurrentIndex() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXRibbonTabBar);
  return d->m_CurrentIndex;
}

void NXRibbonTabBar::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);
  Q_D(NXRibbonTabBar);
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  int const fontPixelSize = nxApp->getFontPixelSize();
  QFont tabFont           = font();
  tabFont.setPixelSize(fontPixelSize + 1);
  painter.setFont(tabFont);

  QList<QRect> rects = d->tabRects();
  for (int i = 0; i < rects.size(); ++i) {
    QRect const &r       = rects.at(i);
    QString const &title = d->m_TabTitles.at(i);
    bool isSelected      = (i == d->m_CurrentIndex);
    bool isHovered       = (i == d->m_HoveredIndex) && isTabEnabled(i);
    bool isEnabled       = isTabEnabled(i);

    if (isSelected) {
      painter.setPen(Qt::NoPen);
      painter.setBrush(NXThemeColor(d->m_ThemeMode, PrimaryNormal));
      painter.drawRect(r.x() + 8, r.bottom() - 2, r.width() - 16, 2);
      painter.setPen(isEnabled ? NXThemeColor(d->m_ThemeMode, BasicText) : NXThemeColor(d->m_ThemeMode, BasicTextDisable));
    } else if (isHovered) {
      painter.setPen(Qt::NoPen);
      painter.setBrush(NXThemeColor(d->m_ThemeMode, BasicHover));
      painter.drawRoundedRect(r.adjusted(4, 4, -4, -4), 4, 4);
      painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
    } else {
      painter.setPen(isEnabled ? NXThemeColor(d->m_ThemeMode, BasicTextNoFocus) : NXThemeColor(d->m_ThemeMode, BasicTextDisable));
    }
    painter.drawText(r, Qt::AlignCenter, title);
  }
}

void NXRibbonTabBar::mouseMoveEvent(QMouseEvent *event)
{
  Q_D(NXRibbonTabBar);
  int idx = d->tabAt(event->pos());
  if (!isTabEnabled(idx)) {
    idx = -1;
  }
  if (idx != d->m_HoveredIndex) {
    d->m_HoveredIndex = idx;
    update();
  }
  QWidget::mouseMoveEvent(event);
}

void NXRibbonTabBar::mousePressEvent(QMouseEvent *event)
{
  Q_D(NXRibbonTabBar);
  if (event->button() == Qt::LeftButton) {
    int idx = d->tabAt(event->pos());
    if (idx >= 0) {
      if (!isTabEnabled(idx)) {
        return;
      }
      bool const isReclick = (idx == d->m_CurrentIndex);
      if (!isReclick) {
        d->m_CurrentIndex = idx;
        Q_EMIT signalCurrentIndexChanged();
        update();
      } else {
        Q_EMIT tabReclicked(idx);
      }
      Q_EMIT tabClicked(idx);
    }
  }
  QWidget::mousePressEvent(event);
}

void NXRibbonTabBar::leaveEvent(QEvent *event)
{
  Q_D(NXRibbonTabBar);
  if (d->m_HoveredIndex != -1) {
    d->m_HoveredIndex = -1;
    update();
  }
  QWidget::leaveEvent(event);
}

QSize NXRibbonTabBar::sizeHint() const
{
  Q_CONST_D(NXRibbonTabBar);
  int const fontPixelSize = nxApp->getFontPixelSize();
  QFont tabFont           = font();
  tabFont.setPixelSize(fontPixelSize + 1);
  QFontMetrics fm(tabFont);
  int total = fontPixelSize + 3;
  for (QString const &t : d->m_TabTitles) {
    total += fm.horizontalAdvance(t) + fontPixelSize * 2 + 2;
  }
  return QSize(qMax(total, 80), height());
}

NXRibbonTabBarPrivate::NXRibbonTabBarPrivate(QObject *parent)
    : QObject { parent }
{
}

NXRibbonTabBarPrivate::~NXRibbonTabBarPrivate()
{
}

QList<QRect> NXRibbonTabBarPrivate::tabRects() const
{
  QList<QRect> rects;
  if (m_TabTitles.isEmpty()) {
    return rects;
  }
  int const fontPixelSize = nxApp->getFontPixelSize();
  QFont tabFont           = q_ptr->font();
  tabFont.setPixelSize(fontPixelSize + 1);
  QFontMetrics fm(tabFont);
  int x = fontPixelSize + 3;
  for (QString const &t : m_TabTitles) {
    int w = fm.horizontalAdvance(t) + fontPixelSize * 2 + 2;
    rects.append(QRect(x, 0, w, q_ptr->height()));
    x += w;
  }
  return rects;
}

int NXRibbonTabBarPrivate::tabAt(QPoint pos) const
{
  QList<QRect> rects = tabRects();
  for (int i = 0; i < rects.size(); ++i) {
    if (rects.at(i).contains(pos)) {
      return i;
    }
  }
  return -1;
}
