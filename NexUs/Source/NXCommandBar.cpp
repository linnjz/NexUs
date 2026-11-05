#include "NXCommandBar.h"

#include <QFontMetrics>
#include <QMouseEvent>
#include <QPainter>

#include "NXMenu.h"
#include "NXTheme.h"
#include "private/NXCommandBarPrivate.h"

Q_PROPERTY_CREATE_CPP(NXCommandBar, QPS_PAEMIT(int), ButtonSize)

NXCommandBar::NXCommandBar(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXCommandBarPrivate())
{
  Q_D(NXCommandBar);
  d->q_ptr        = this;
  d->m_ButtonSize = 36;
  setFixedHeight(40);
  setObjectName("NXCommandBar");
  setMouseTracking(true);

  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
    update();
  });
}

NXCommandBar::~NXCommandBar()
{
}

void NXCommandBar::addItem(CommandItem const &item)
{
  Q_D(NXCommandBar);
  d->m_Items.append(item);
  update();
}

void NXCommandBar::addSeparator()
{
  CommandItem sep;
  sep.isSeparator = true;
  Q_D(NXCommandBar);
  d->m_Items.append(sep);
  update();
}

void NXCommandBar::clearItems()
{
  Q_D(NXCommandBar);
  d->m_Items.clear();
  d->m_HoveredIndex = -1;
  d->m_PressedIndex = -1;
  update();
}

void NXCommandBar::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);
  Q_D(NXCommandBar);
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  QList<QRect> rects = d->_getVisibleItemRects();
  bool hasOverflow   = (d->m_VisibleCount < d->m_Items.count());
  int totalVisible   = rects.count();
  if (hasOverflow) {
    totalVisible -= 1; // last rect is the overflow button
  }

  for (int i = 0; i < totalVisible; ++i) {
    NXCommandBar::CommandItem const &item = d->m_Items[i];
    QRect itemRect                        = rects[i];

    if (item.isSeparator) {
      painter.setPen(QPen(NXThemeColor(d->m_ThemeMode, BasicBorder), 1));
      int x  = itemRect.center().x();
      int y1 = itemRect.top() + 8;
      int y2 = itemRect.bottom() - 8;
      painter.drawLine(x, y1, x, y2);
    } else {
      // Hover / Press background
      if (d->m_PressedIndex == i) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(NXThemeColor(d->m_ThemeMode, BasicPress));
        painter.drawRoundedRect(itemRect, 4, 4);
      } else if (d->m_HoveredIndex == i) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(NXThemeColor(d->m_ThemeMode, BasicHover));
        painter.drawRoundedRect(itemRect, 4, 4);
      }

      int xOffset = itemRect.x();

      // Draw icon
      if (item.icon != NXIconType::None) {
        QFont iconFont = QFont(QStringLiteral("NXAwesome"));
        iconFont.setPixelSize(16);
        painter.setFont(iconFont);
        painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
        if (item.text.isEmpty()) {
          painter.drawText(itemRect, Qt::AlignCenter, QChar((unsigned short) item.icon));
        } else {
          QRect iconRect(xOffset + 6, itemRect.y(), 20, itemRect.height());
          painter.drawText(iconRect, Qt::AlignCenter, QChar((unsigned short) item.icon));
          xOffset += 26;
        }
      }

      // Draw text
      if (!item.text.isEmpty()) {
        QFont textFont = this->font();
        textFont.setPixelSize(12);
        painter.setFont(textFont);
        painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
        int textX = (item.icon != NXIconType::None) ? xOffset : itemRect.x() + 6;
        QRect textRect(textX, itemRect.y(), itemRect.right() - textX - 6, itemRect.height());
        painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, item.text);
      }
    }
  }

  // Draw overflow button
  if (hasOverflow && !rects.isEmpty()) {
    QRect overflowRect = rects.last();
    int overflowIndex  = -2; // special index for overflow
    if (d->m_PressedIndex == overflowIndex) {
      painter.setPen(Qt::NoPen);
      painter.setBrush(NXThemeColor(d->m_ThemeMode, BasicPress));
      painter.drawRoundedRect(overflowRect, 4, 4);
    } else if (d->m_HoveredIndex == overflowIndex) {
      painter.setPen(Qt::NoPen);
      painter.setBrush(NXThemeColor(d->m_ThemeMode, BasicHover));
      painter.drawRoundedRect(overflowRect, 4, 4);
    }
    QFont iconFont = QFont(QStringLiteral("NXAwesome"));
    iconFont.setPixelSize(16);
    painter.setFont(iconFont);
    painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
    painter.drawText(overflowRect, Qt::AlignCenter, QChar((unsigned short) NXIconType::Ellipsis));
  }

  painter.restore();
}

void NXCommandBar::mouseMoveEvent(QMouseEvent *event)
{
  Q_D(NXCommandBar);
  QList<QRect> rects = d->_getVisibleItemRects();
  bool hasOverflow   = (d->m_VisibleCount < d->m_Items.count());
  int totalVisible   = rects.count();

  int oldHovered   = d->m_HoveredIndex;
  d->m_HoveredIndex = -1;

  if (hasOverflow && !rects.isEmpty()) {
    QRect overflowRect = rects.last();
    if (overflowRect.contains(event->pos())) {
      d->m_HoveredIndex = -2;
    }
    totalVisible -= 1;
  }

  if (d->m_HoveredIndex == -1) {
    for (int i = 0; i < totalVisible; ++i) {
      if (!d->m_Items[i].isSeparator && rects[i].contains(event->pos())) {
        d->m_HoveredIndex = i;
        break;
      }
    }
  }

  if (oldHovered != d->m_HoveredIndex) {
    update();
  }
  QWidget::mouseMoveEvent(event);
}

void NXCommandBar::mousePressEvent(QMouseEvent *event)
{
  Q_D(NXCommandBar);
  if (event->button() == Qt::LeftButton) {
    d->m_PressedIndex = d->m_HoveredIndex;
    update();
  }
  QWidget::mousePressEvent(event);
}

void NXCommandBar::mouseReleaseEvent(QMouseEvent *event)
{
  Q_D(NXCommandBar);
  if (event->button() == Qt::LeftButton) {
    if (d->m_PressedIndex != -1 && d->m_PressedIndex == d->m_HoveredIndex) {
      if (d->m_PressedIndex == -2) {
        // Overflow menu
        if (!d->m_OverflowMenu) {
          d->m_OverflowMenu = new NXMenu(this);
        }
        d->m_OverflowMenu->clear();
        for (int i = d->m_VisibleCount; i < d->m_Items.count(); ++i) {
          CommandItem const &item = d->m_Items[i];
          if (item.isSeparator) {
            d->m_OverflowMenu->addSeparator();
          } else {
            QAction *action = nullptr;
            if (item.icon != NXIconType::None) {
              action = d->m_OverflowMenu->addNXIconAction(item.icon, item.text);
            } else {
              action = d->m_OverflowMenu->addAction(item.text);
            }
            int index = i;
            connect(action, &QAction::triggered, this, [=]() {
              Q_EMIT itemClicked(index);
            });
          }
        }
        QList<QRect> rects = d->_getVisibleItemRects();
        if (!rects.isEmpty()) {
          QRect overflowRect = rects.last();
          d->m_OverflowMenu->popup(mapToGlobal(overflowRect.bottomLeft()));
        }
      } else {
        Q_EMIT itemClicked(d->m_PressedIndex);
      }
    }
    d->m_PressedIndex = -1;
    update();
  }
  QWidget::mouseReleaseEvent(event);
}

void NXCommandBar::leaveEvent(QEvent *event)
{
  Q_D(NXCommandBar);
  d->m_HoveredIndex = -1;
  update();
  QWidget::leaveEvent(event);
}

QSize NXCommandBar::sizeHint() const
{
  Q_CONST_D(NXCommandBar);
  int totalWidth = 0;
  for (auto const &item : d->m_Items) {
    totalWidth += d->_getItemWidth(item);
  }
  return QSize(qMax(totalWidth, 50), 40);
}

NXCommandBarPrivate::NXCommandBarPrivate(QObject *parent)
    : QObject { parent }
{
}

NXCommandBarPrivate::~NXCommandBarPrivate()
{
}

int NXCommandBarPrivate::_getItemWidth(NXCommandBar::CommandItem const &item) const
{
  if (item.isSeparator) {
    return 8;
  }
  if (item.icon != NXIconType::None && item.text.isEmpty()) {
    return m_ButtonSize;
  }
  int w = 12; // padding
  if (item.icon != NXIconType::None) {
    w += 20; // icon space
  }
  QFont textFont = q_ptr->font();
  textFont.setPixelSize(12);
  QFontMetrics fm(textFont);
  w += fm.horizontalAdvance(item.text);
  if (w < m_ButtonSize) {
    w = m_ButtonSize;
  }
  return w;
}

QList<QRect> NXCommandBarPrivate::_getVisibleItemRects() const
{
  QList<QRect> rects;
  int availableWidth = q_ptr->width();
  int overflowWidth  = m_ButtonSize; // width for the "..." button
  int yOffset        = (q_ptr->height() - m_ButtonSize) / 2;
  int x              = 0;

  // First pass: check if all items fit
  int totalWidth = 0;
  for (auto const &item : m_Items) {
    totalWidth += _getItemWidth(item);
  }

  bool needsOverflow = (totalWidth > availableWidth);
  int maxWidth       = needsOverflow ? (availableWidth - overflowWidth) : availableWidth;

  int visibleCount = 0;
  for (int i = 0; i < m_Items.count(); ++i) {
    int itemWidth = _getItemWidth(m_Items[i]);
    if (x + itemWidth > maxWidth) {
      break;
    }
    int itemHeight = m_Items[i].isSeparator ? q_ptr->height() - 16 : m_ButtonSize;
    int itemY      = m_Items[i].isSeparator ? 8 : yOffset;
    rects.append(QRect(x, itemY, itemWidth, itemHeight));
    x += itemWidth;
    visibleCount++;
  }

  const_cast<NXCommandBarPrivate *>(this)->m_VisibleCount = visibleCount;

  if (needsOverflow) {
    rects.append(QRect(x, yOffset, overflowWidth, m_ButtonSize));
  }

  return rects;
}
