#include "NXPagination.h"

#include <QIntValidator>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

#include "NXLineEdit.h"
#include "NXTheme.h"
#include "private/NXPaginationPrivate.h"

Q_PROPERTY_CREATE_CPP(NXPagination, QPS_PAEMIT(int), CurrentPage)
Q_PROPERTY_CREATE_CPP(NXPagination, QPS_PAEMIT(int), TotalPages)
Q_PROPERTY_CREATE_CPP(NXPagination, QPS_PAEMIT(int), ButtonSize)
Q_PROPERTY_CREATE_CPP(NXPagination, QPS_PAEMIT(int), PagerCount)
Q_PROPERTY_CREATE_CPP(NXPagination, QPS_PAEMIT(bool), JumperVisible)

NXPaginationPrivate::NXPaginationPrivate(QObject *parent)
    : QObject(parent)
{
}

NXPaginationPrivate::~NXPaginationPrivate()
{
}

void NXPaginationPrivate::_updateJumperPosition()
{
  Q_Q(NXPagination);
  if (!m_JumperEdit) {
    return;
  }
  m_JumperEdit->setVisible(m_JumperVisible);
  if (!m_JumperVisible) {
    return;
  }
  QList<QPair<QRect, int>> buttonRects = _getButtonRects();
  int jumperX                          = 0;
  if (!buttonRects.isEmpty()) {
    QRect lastRect = buttonRects.last().first;
    jumperX        = lastRect.x() + lastRect.width() + 12;
  }
  int editWidth  = 55;
  int editHeight = m_ButtonSize;
  int y          = (q->height() - editHeight) / 2;
  m_JumperEdit->setGeometry(jumperX, y, editWidth, editHeight);
}

QList<int> NXPaginationPrivate::_getVisiblePages() const
{
  QList<int> pages;
  int total      = m_TotalPages;
  int current    = m_CurrentPage;
  int pagerCount = m_PagerCount;

  if (total <= 0) {
    return pages;
  }

  if (total <= pagerCount) {
    for (int i = 1; i <= total; i++) {
      pages.append(i);
    }
    return pages;
  }

  int halfPager          = (pagerCount - 2) / 2;
  bool showLeftEllipsis  = current > halfPager + 2;
  bool showRightEllipsis = current < total - halfPager - 1;

  if (!showLeftEllipsis && showRightEllipsis) {
    for (int i = 1; i <= pagerCount - 2; i++) {
      pages.append(i);
    }
    pages.append(-1);
    pages.append(total);
  } else if (showLeftEllipsis && !showRightEllipsis) {
    pages.append(1);
    pages.append(-3);
    for (int i = total - (pagerCount - 3); i <= total; i++) {
      pages.append(i);
    }
  } else {
    pages.append(1);
    pages.append(-3);
    int sideCount = (pagerCount - 4) / 2;
    for (int i = current - sideCount; i <= current + sideCount; i++) {
      pages.append(i);
    }
    pages.append(-1);
    pages.append(total);
  }

  return pages;
}

QList<QPair<QRect, int>> NXPaginationPrivate::_getButtonRects() const
{
  QList<QPair<QRect, int>> rects;
  int size    = m_ButtonSize;
  int spacing = 4;
  int x       = 0;
  int y       = 4;

  rects.append(qMakePair(QRect(x, y, size, size), 0));
  x += size + spacing;

  QList<int> pages = _getVisiblePages();
  for (int page : pages) {
    if (page == -1) {
      rects.append(qMakePair(QRect(x, y, size, size), -1));
    } else if (page == -3) {
      rects.append(qMakePair(QRect(x, y, size, size), -3));
    } else {
      rects.append(qMakePair(QRect(x, y, size, size), page));
    }
    x += size + spacing;
  }

  rects.append(qMakePair(QRect(x, y, size, size), -2));

  return rects;
}

NXPagination::NXPagination(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXPaginationPrivate())
{
  Q_D(NXPagination);
  d->q_ptr           = this;
  d->m_CurrentPage   = 1;
  d->m_TotalPages    = 1;
  d->m_ButtonSize    = 28;
  d->m_PagerCount    = 11;
  d->m_JumperVisible = false;
  setObjectName("NXPagination");
  setMouseTracking(true);
  setFixedHeight(d->m_ButtonSize + 8);
  d->m_ThemeMode = nxTheme->getThemeMode();

  d->m_JumperEdit = new NXLineEdit(this);
  d->m_JumperEdit->setFixedHeight(d->m_ButtonSize);
  d->m_JumperEdit->setPlaceholderText(QStringLiteral("页码"));
  d->m_JumperEdit->setAlignment(Qt::AlignCenter);
  d->m_JumperEdit->setStyleSheet(
      QStringLiteral("#NXLineEdit{background-color:transparent;padding-left:0px;padding-right:0px;}"));
  d->m_JumperEdit->setValidator(new QIntValidator(1, 9999, d->m_JumperEdit));
  d->m_JumperEdit->setVisible(false);
  connect(d->m_JumperEdit, &NXLineEdit::returnPressed, this, [=]() {
    QString text = d->m_JumperEdit->text().trimmed();
    bool ok      = false;
    int page     = text.toInt(&ok);
    if (ok && page >= 1 && page <= d->m_TotalPages && page != d->m_CurrentPage) {
      d->m_CurrentPage = page;
      Q_EMIT signalCurrentPageChanged();
      Q_EMIT currentPageChanged(page);
      update();
    }
    d->m_JumperEdit->clear();
  });

  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
    update();
  });
}

NXPagination::~NXPagination()
{
}

void NXPagination::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);
  Q_D(NXPagination);
  d->_updateJumperPosition();
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  QList<QPair<QRect, int>> buttonRects = d->_getButtonRects();
  QFont textFont                       = this->font();
  textFont.setPixelSize(14);

  QFont iconFont(QStringLiteral("NXAwesome"));
  iconFont.setPixelSize(16);

  for (int i = 0; i < buttonRects.size(); i++) {
    QRect rect    = buttonRects[i].first;
    int pageValue = buttonRects[i].second;

    bool isHovered     = (i == d->m_HoverIndex);
    bool isCurrentPage = (pageValue > 0 && pageValue == d->m_CurrentPage);
    bool isDisabled    = false;

    if (pageValue == 0 && d->m_CurrentPage <= 1) {
      isDisabled = true;
    }
    if (pageValue == -2 && d->m_CurrentPage >= d->m_TotalPages) {
      isDisabled = true;
    }

    painter.setPen(Qt::NoPen);
    if (isCurrentPage) {
      painter.setBrush(NXThemeColor(d->m_ThemeMode, PrimaryNormal));
    } else if (isDisabled) {
      painter.setBrush(NXThemeColor(d->m_ThemeMode, BasicDisable));
    } else if (isHovered) {
      painter.setBrush(NXThemeColor(d->m_ThemeMode, BasicHover));
    } else {
      painter.setBrush(NXThemeColor(d->m_ThemeMode, BasicBase));
    }

    QPainterPath path;
    path.addRoundedRect(rect, 6, 6);
    painter.drawPath(path);

    if (isCurrentPage) {
      painter.setPen(Qt::white);
    } else if (isDisabled) {
      painter.setPen(NXThemeColor(d->m_ThemeMode, BasicTextDisable));
    } else {
      painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
    }

    if (pageValue == 0) {
      painter.setFont(iconFont);
      painter.drawText(rect, Qt::AlignCenter, QChar((unsigned short) 0xea'84));
    } else if (pageValue == -2) {
      painter.setFont(iconFont);
      painter.drawText(rect, Qt::AlignCenter, QChar((unsigned short) 0xea'85));
    } else if (pageValue == -1 || pageValue == -3) {
      painter.setFont(iconFont);
      painter.drawText(rect, Qt::AlignCenter, QChar((unsigned short) 0xec'4d));
    } else {
      painter.setFont(textFont);
      painter.drawText(rect, Qt::AlignCenter, QString::number(pageValue));
    }
  }
}

void NXPagination::mouseMoveEvent(QMouseEvent *event)
{
  Q_D(NXPagination);
  QList<QPair<QRect, int>> buttonRects = d->_getButtonRects();
  int oldHover                         = d->m_HoverIndex;
  d->m_HoverIndex                       = -1;
  for (int i = 0; i < buttonRects.size(); i++) {
    if (buttonRects[i].first.contains(event->pos())) {
      d->m_HoverIndex = i;
      break;
    }
  }
  if (oldHover != d->m_HoverIndex) {
    update();
  }
  QWidget::mouseMoveEvent(event);
}

void NXPagination::mousePressEvent(QMouseEvent *event)
{
  Q_D(NXPagination);
  if (event->button() != Qt::LeftButton) {
    QWidget::mousePressEvent(event);
    return;
  }

  QList<QPair<QRect, int>> buttonRects = d->_getButtonRects();
  for (int i = 0; i < buttonRects.size(); i++) {
    if (buttonRects[i].first.contains(event->pos())) {
      int pageValue = buttonRects[i].second;
      int newPage   = d->m_CurrentPage;

      if (pageValue == 0) {
        if (d->m_CurrentPage > 1) {
          newPage = d->m_CurrentPage - 1;
        }
      } else if (pageValue == -2) {
        if (d->m_CurrentPage < d->m_TotalPages) {
          newPage = d->m_CurrentPage + 1;
        }
      } else if (pageValue == -1) {
        newPage = qMin(d->m_CurrentPage + d->m_PagerCount - 2, d->m_TotalPages);
      } else if (pageValue == -3) {
        newPage = qMax(d->m_CurrentPage - (d->m_PagerCount - 2), 1);
      } else if (pageValue > 0) {
        newPage = pageValue;
      }

      if (newPage != d->m_CurrentPage) {
        d->m_CurrentPage = newPage;
        Q_EMIT signalCurrentPageChanged();
        Q_EMIT currentPageChanged(newPage);
        update();
      }
      break;
    }
  }
  QWidget::mousePressEvent(event);
}

QSize NXPagination::sizeHint() const
{
  Q_CONST_D(NXPagination);
  QList<QPair<QRect, int>> buttonRects = d->_getButtonRects();
  int totalWidth                       = 0;
  if (!buttonRects.isEmpty()) {
    QRect lastRect = buttonRects.last().first;
    totalWidth     = lastRect.x() + lastRect.width();
  }
  if (d->m_JumperVisible) {
    totalWidth += 12 + 55;
  }
  return QSize(totalWidth, d->m_ButtonSize + 8);
}
