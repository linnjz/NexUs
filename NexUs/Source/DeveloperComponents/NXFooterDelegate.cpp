#include "NXFooterDelegate.h"

#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>

#include "NXApplication.h"
#include "NXBaseListView.h"
#include "NXFooterModel.h"
#include "NXNavigationNode.h"
#include "NXTheme.h"

NXFooterDelegate::NXFooterDelegate(QObject *parent)
    : QStyledItemDelegate { parent }
    , m_NXListView { nullptr }
    , m_ThemeMode { nxTheme->getThemeMode() }
{
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    m_ThemeMode = themeMode;
  });
  setProperty("lastSelectMarkTop", 10.0);
  setProperty("lastSelectMarkBottom", 10.0);
  setProperty("selectMarkTop", 10.0);
  setProperty("selectMarkBottom", 10.0);
  // Mark向上
  m_LastSelectMarkTopAnimation = new QPropertyAnimation(this, "lastSelectMarkTop");
  connect(m_LastSelectMarkTopAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
    m_LastSelectMarkTop = value.toReal();
    m_NXListView->viewport()->update();
  });
  m_LastSelectMarkTopAnimation->setDuration(300);
  m_LastSelectMarkTopAnimation->setEasingCurve(QEasingCurve::InOutSine);

  m_SelectMarkBottomAnimation = new QPropertyAnimation(this, "selectMarkBottom");
  connect(m_SelectMarkBottomAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
    m_SelectMarkBottom = value.toReal();
    m_NXListView->viewport()->update();
  });
  m_SelectMarkBottomAnimation->setDuration(300);
  m_SelectMarkBottomAnimation->setEasingCurve(QEasingCurve::InOutSine);
  connect(m_LastSelectMarkTopAnimation, &QPropertyAnimation::finished, this, [=]() {
    m_IsSelectMarkDisplay = true;
    m_LastSelectedNode    = nullptr;
    m_SelectMarkBottomAnimation->setStartValue(0);
    m_SelectMarkBottomAnimation->setEndValue(10);
    m_SelectMarkBottomAnimation->start();
  });

  // Mark向下
  m_LastSelectMarkBottomAnimation = new QPropertyAnimation(this, "lastSelectMarkBottom");
  connect(m_LastSelectMarkBottomAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
    m_LastSelectMarkBottom = value.toReal();
    m_NXListView->viewport()->update();
  });
  m_LastSelectMarkBottomAnimation->setDuration(300);
  m_LastSelectMarkBottomAnimation->setEasingCurve(QEasingCurve::InOutSine);

  m_SelectMarkTopAnimation = new QPropertyAnimation(this, "selectMarkTop");
  connect(m_SelectMarkTopAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
    m_SelectMarkTop = value.toReal();
    m_NXListView->viewport()->update();
  });
  m_SelectMarkTopAnimation->setDuration(300);
  m_SelectMarkTopAnimation->setEasingCurve(QEasingCurve::InOutSine);
  connect(m_LastSelectMarkBottomAnimation, &QPropertyAnimation::finished, this, [=]() {
    m_IsSelectMarkDisplay = true;
    m_LastSelectedNode    = nullptr;
    m_SelectMarkTopAnimation->setStartValue(0);
    m_SelectMarkTopAnimation->setEndValue(10);
    m_SelectMarkTopAnimation->start();
  });
}

NXFooterDelegate::~NXFooterDelegate()
{
}

void NXFooterDelegate::navigationNodeStateChange(QVariantMap const &data)
{
  if (data.contains(QStringLiteral("SelectMarkChanged"))) {
    m_LastSelectedNode              = data.value(QStringLiteral("LastSelectedNode")).value<NXNavigationNode *>();
    NXNavigationNode *selectedNode = data.value(QStringLiteral("SelectedNode")).value<NXNavigationNode *>();
    bool direction                 = _compareItemY(selectedNode, m_LastSelectedNode);
    m_LastSelectMarkTop             = 10;
    m_LastSelectMarkBottom          = 10;
    m_SelectMarkTop                 = 10;
    m_SelectMarkBottom              = 10;
    if (direction) {
      m_LastSelectMarkTopAnimation->setStartValue(10);
      m_LastSelectMarkTopAnimation->setEndValue(0);
      m_LastSelectMarkTopAnimation->start();
      m_LastSelectMarkBottomAnimation->stop();
      m_SelectMarkTopAnimation->stop();
      m_IsSelectMarkDisplay = false;
    } else {
      m_LastSelectMarkBottomAnimation->setStartValue(10);
      m_LastSelectMarkBottomAnimation->setEndValue(0);
      m_LastSelectMarkBottomAnimation->start();
      m_LastSelectMarkTopAnimation->stop();
      m_SelectMarkBottomAnimation->stop();
      m_IsSelectMarkDisplay = false;
    }
  }
}

void NXFooterDelegate::paint(QPainter *painter, QStyleOptionViewItem const &option, QModelIndex const &index) const
{
  int const fontPixelSize = nxApp->getFontPixelSize();
  NXFooterModel *model    = dynamic_cast<NXFooterModel *>(const_cast<QAbstractItemModel *>(index.model()));
  NXNavigationNode *node  = index.data(Qt::UserRole).value<NXNavigationNode *>();
  // 背景绘制
  QRect itemRect = option.rect;
  painter->save();
  painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
  itemRect.setTop(itemRect.top() + 2);
  itemRect.setBottom(itemRect.bottom() - 2);
  QPainterPath path;
  path.addRoundedRect(itemRect, 8, 8);
  if (option.state & QStyle::State_Selected) {
    if (index == m_PressIndex) {
      // 选中时点击
      painter->fillPath(path, NXThemeColor(m_ThemeMode, BasicHoverAlpha));
    } else {
      if (option.state & QStyle::State_MouseOver) {
        // 选中时覆盖
        painter->fillPath(path, NXThemeColor(m_ThemeMode, BasicSelectedHoverAlpha));
      } else {
        // 选中
        painter->fillPath(path, NXThemeColor(m_ThemeMode, BasicSelectedAlpha));
      }
    }
  } else {
    if (index == m_PressIndex) {
      // 点击时颜色
      painter->fillPath(path, NXThemeColor(m_ThemeMode, BasicPressAlpha));
    } else {
      if (option.state & QStyle::State_MouseOver) {
        // 覆盖时颜色
        painter->fillPath(path, NXThemeColor(m_ThemeMode, BasicHoverAlpha));
      }
    }
  }
  painter->restore();

  painter->save();
  painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
  itemRect = option.rect;

  //顶边线绘制
  if (index.row() == 0) {
    painter->setPen(NXThemeColor(m_ThemeMode, BasicBaseLine));
    painter->drawLine(option.rect.x(), itemRect.y() + 1, option.rect.x() + option.rect.width(), itemRect.y() + 1);
  }
  // 图标绘制
  painter->setPen(index == m_PressIndex ? NXThemeColor(m_ThemeMode, BasicTextPress) : NXThemeColor(m_ThemeMode, BasicText));
  if (node->getAwesome() != NXIconType::None) {
    painter->save();
    QFont iconFont = QFont(QStringLiteral("NXAwesome"));
    iconFont.setPixelSize(fontPixelSize + 4);
    painter->setFont(iconFont);
    painter->drawText(QRect(itemRect.x(), itemRect.y(), m_IconAreaWidth, itemRect.height()), Qt::AlignCenter, QChar((unsigned short) node->getAwesome()));
    painter->restore();
  }

  if (int keyPoints = node->getKeyPoints()) {
    // KeyPoints
    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::white);
    painter->drawEllipse(QPoint(255, itemRect.y() + itemRect.height() / 2), 10, 10);
    painter->setBrush(NXThemeColor(m_ThemeMode, StatusDanger));
    painter->drawEllipse(QPoint(255, itemRect.y() + itemRect.height() / 2), 9, 9);
    painter->setPen(QPen(Qt::white, 2));
    QFont font = painter->font();
    font.setBold(true);
    if (keyPoints > 99) {
      keyPoints = 99;
    }
    if (keyPoints > 9) {
      font.setPixelSize(fontPixelSize - 2);
    } else {
      font.setPixelSize(fontPixelSize - 1);
    }
    painter->setFont(font);
    painter->drawText(keyPoints > 9 ? 248 : 251, itemRect.y() + itemRect.height() / 2 + 4, QString::number(keyPoints));
    painter->restore();
  }

  // 文字绘制
  painter->setPen(index == m_PressIndex ? NXThemeColor(m_ThemeMode, BasicTextPress) : NXThemeColor(m_ThemeMode, BasicText));
  QRect textRect;
  if (node->getAwesome() != NXIconType::None) {
    textRect =
        QRect(itemRect.x() + m_IconAreaWidth, itemRect.y(), itemRect.width() - m_TextRightSpacing - m_IndicatorIconAreaWidth - m_IconAreaWidth, itemRect.height());
  } else {
    textRect = QRect(itemRect.x() + m_LeftPadding, itemRect.y(), itemRect.width() - m_TextRightSpacing - m_IndicatorIconAreaWidth - m_LeftPadding, itemRect.height());
  }
  QString text = painter->fontMetrics().elidedText(node->getNodeTitle(), Qt::ElideRight, textRect.width());
  painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, text);
  // 选中特效
  if (m_IsSelectMarkDisplay && (node == model->getSelectedNode())) {
    painter->setPen(Qt::NoPen);
    painter->setBrush(NXThemeColor(m_ThemeMode, PrimaryNormal));
    painter->drawRoundedRect(QRectF(itemRect.x() + 3, itemRect.y() + m_SelectMarkTop, 3, itemRect.height() - m_SelectMarkTop - m_SelectMarkBottom), 1.5, 1.5);
  }
  if (node == m_LastSelectedNode) {
    painter->setPen(Qt::NoPen);
    painter->setBrush(NXThemeColor(m_ThemeMode, PrimaryNormal));
    painter->drawRoundedRect(QRectF(itemRect.x() + 3, itemRect.y() + m_LastSelectMarkTop, 3, itemRect.height() - m_LastSelectMarkTop - m_LastSelectMarkBottom), 3, 3);
  }
  painter->restore();
}

QSize NXFooterDelegate::sizeHint(QStyleOptionViewItem const &option, QModelIndex const &index) const
{
  QSize size = QStyledItemDelegate::sizeHint(option, index);
  size.setHeight(40);
  return size;
}

bool NXFooterDelegate::_compareItemY(NXNavigationNode *node1, NXNavigationNode *node2)
{
  if (!node1) {
    return true;
  }
  if (!node2) {
    return false;
  }
  if (node1->getModelIndex().row() < node2->getModelIndex().row()) {
    return true;
  }
  return false;
}
