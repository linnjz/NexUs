#include "NXAutoCompleteDelegate.h"

#include <QPainter>
#include <QPainterPath>
#include <QRegularExpression>

#include "NXTheme.h"

NXAutoCompleteDelegate::NXAutoCompleteDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
    , m_ThemeMode { nxTheme->getThemeMode() }
{
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    m_ThemeMode = themeMode;
  });
}

NXAutoCompleteDelegate::~NXAutoCompleteDelegate()
{
}

void NXAutoCompleteDelegate::setHighlightText(QString const &text)
{
  m_HighlightText = text;
}

void NXAutoCompleteDelegate::setMatchMode(NXAutoComplete::MatchMode mode)
{
  m_MatchMode = mode;
}

void NXAutoCompleteDelegate::paint(QPainter *painter, QStyleOptionViewItem const &option, QModelIndex const &index) const
{
  painter->save();
  painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  painter->setPen(Qt::NoPen);

  QRectF itemRect = option.rect;
  itemRect.adjust(4, 2, -4, -2);
  if (option.state & QStyle::State_MouseOver) {
    QPainterPath hoverPath;
    hoverPath.addRoundedRect(itemRect, 4, 4);
    painter->fillPath(hoverPath, NXThemeColor(m_ThemeMode, BasicHover));
  }

  QString displayText = index.data(Qt::DisplayRole).toString();
  QFont textFont      = option.font;
  textFont.setPixelSize(13);
  painter->setFont(textFont);

  QRectF textRect = itemRect.adjusted(10, 0, -4, 0);

  if (!m_HighlightText.isEmpty()) {
    QFontMetrics fm(textFont);
    QFont boldFont = textFont;
    boldFont.setBold(true);
    QFontMetrics boldFm(boldFont);

    int searchLen = m_HighlightText.length();
    QVector<QPair<int, int>> matches;
    if (m_MatchMode == NXAutoComplete::StartsWith) {
      if (displayText.startsWith(m_HighlightText, Qt::CaseInsensitive)) {
        matches.append({ 0, searchLen });
      }
    } else if (m_MatchMode == NXAutoComplete::EndsWith) {
      if (displayText.endsWith(m_HighlightText, Qt::CaseInsensitive)) {
        matches.append({ displayText.length() - searchLen, searchLen });
      }
    } else if (m_MatchMode == NXAutoComplete::RegExp) {
      QRegularExpression re(m_HighlightText, QRegularExpression::CaseInsensitiveOption);
      if (re.isValid()) {
        QRegularExpressionMatchIterator it = re.globalMatch(displayText);
        while (it.hasNext()) {
          QRegularExpressionMatch m = it.next();
          if (m.capturedLength() > 0) {
            matches.append({ static_cast<int>(m.capturedStart()), static_cast<int>(m.capturedLength()) });
          }
        }
      }
    } else {
      int searchFrom = 0;
      while (searchFrom < displayText.length()) {
        int idx = displayText.indexOf(m_HighlightText, searchFrom, Qt::CaseInsensitive);
        if (idx < 0) {
          break;
        }
        matches.append({ idx, searchLen });
        searchFrom = idx + searchLen;
      }
    }

    if (!matches.isEmpty()) {
      qreal x = textRect.left();
      qreal y = textRect.center().y() + fm.ascent() / 2.0 - 1;
      int pos = 0;

      for (auto const &m : matches) {
        if (m.first > pos) {
          QString segment = displayText.mid(pos, m.first - pos);
          painter->setFont(textFont);
          painter->setPen(NXThemeColor(m_ThemeMode, BasicText));
          painter->drawText(QPointF(x, y), segment);
          x += fm.horizontalAdvance(segment);
        }
        QString matchStr = displayText.mid(m.first, m.second);
        painter->setFont(boldFont);
        painter->setPen(NXThemeColor(m_ThemeMode, PrimaryNormal));
        painter->drawText(QPointF(x, y), matchStr);
        x += boldFm.horizontalAdvance(matchStr);
        pos = m.first + m.second;
      }
      if (pos < displayText.length()) {
        QString tail = displayText.mid(pos);
        painter->setFont(textFont);
        painter->setPen(NXThemeColor(m_ThemeMode, BasicText));
        painter->drawText(QPointF(x, y), tail);
      }

      painter->restore();
      return;
    }
  }

  painter->setPen(NXThemeColor(m_ThemeMode, BasicText));
  painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, displayText);

  painter->restore();
}

QSize NXAutoCompleteDelegate::sizeHint(QStyleOptionViewItem const &option, QModelIndex const &index) const
{
  Q_UNUSED(option)
  Q_UNUSED(index)
  return QSize(0, 36);
}
