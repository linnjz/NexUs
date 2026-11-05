#include "NXNavigationStyle.h"

#include <QDebug>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QStyleOption>

#include "NXApplication.h"
#include "NXNavigationModel.h"
#include "NXNavigationNode.h"
#include "NXNavigationView.h"
#include "NXTheme.h"

NXNavigationStyle::NXNavigationStyle(QStyle *style)
{
  m_Opacity              = 1;
  m_ItemHeight           = 38;
  m_LastSelectMarkTop    = 10.5;
  m_LastSelectMarkBottom = 10.5;
  m_SelectMarkTop        = 10.5;
  m_SelectMarkBottom     = 10.5;

  // Mark向上
  m_LastSelectMarkTopAnimation = new QPropertyAnimation(this, "LastSelectMarkTop");
  connect(m_LastSelectMarkTopAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
    m_NavigationView->viewport()->update();
  });
  m_LastSelectMarkTopAnimation->setDuration(300);
  m_LastSelectMarkTopAnimation->setEasingCurve(QEasingCurve::InOutSine);

  m_SelectMarkBottomAnimation = new QPropertyAnimation(this, "SelectMarkBottom");
  connect(m_SelectMarkBottomAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
    m_NavigationView->viewport()->update();
  });
  m_SelectMarkBottomAnimation->setDuration(300);
  m_SelectMarkBottomAnimation->setEasingCurve(QEasingCurve::InOutSine);
  connect(m_LastSelectMarkTopAnimation, &QPropertyAnimation::finished, this, [=]() {
    m_IsSelectMarkDisplay = true;
    m_LastSelectedNode    = nullptr;
    m_SelectMarkBottomAnimation->setStartValue(0);
    m_SelectMarkBottomAnimation->setEndValue(10.5);
    m_SelectMarkBottomAnimation->start();
  });

  // Mark向下
  m_LastSelectMarkBottomAnimation = new QPropertyAnimation(this, "LastSelectMarkBottom");
  connect(m_LastSelectMarkBottomAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
    m_NavigationView->viewport()->update();
  });
  m_LastSelectMarkBottomAnimation->setDuration(300);
  m_LastSelectMarkBottomAnimation->setEasingCurve(QEasingCurve::InOutSine);

  m_SelectMarkTopAnimation = new QPropertyAnimation(this, "SelectMarkTop");
  connect(m_SelectMarkTopAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
    m_NavigationView->viewport()->update();
  });
  m_SelectMarkTopAnimation->setDuration(300);
  m_SelectMarkTopAnimation->setEasingCurve(QEasingCurve::InOutSine);
  connect(m_LastSelectMarkBottomAnimation, &QPropertyAnimation::finished, this, [=]() {
    m_IsSelectMarkDisplay = true;
    m_LastSelectedNode    = nullptr;
    m_SelectMarkTopAnimation->setStartValue(0.0);
    m_SelectMarkTopAnimation->setEndValue(10.5);
    m_SelectMarkTopAnimation->start();
  });
  m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    m_ThemeMode = themeMode;
  });
}

NXNavigationStyle::~NXNavigationStyle()
{
}

void NXNavigationStyle::drawPrimitive(PrimitiveElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget) const
{
  switch (element) {
  case QStyle::PE_PanelItemViewItem : {
    // Item背景
    if (QStyleOptionViewItem const *vopt = qstyleoption_cast<QStyleOptionViewItem const *>(option)) {
      QModelIndex index      = vopt->index;
      NXNavigationNode *node = static_cast<NXNavigationNode *>(index.internalPointer());
      if (node->getIsCategoryNode()) {
        return;
      }
      painter->save();
      if (this->m_OpacityAnimationTargetNode && node->getParentNode() == this->m_OpacityAnimationTargetNode) {
        painter->setOpacity(m_Opacity);
      }
      painter->setRenderHint(QPainter::Antialiasing);
      QRect itemRect = vopt->rect;
      itemRect.setTop(itemRect.top() + 2);
      itemRect.setBottom(itemRect.bottom() - 2);
      QPainterPath path;
      path.addRoundedRect(itemRect, 5, 5);
      if (vopt->state & QStyle::State_Selected) {
        if (index == m_PressIndex) {
          // 选中时点击
          painter->fillPath(path, NXThemeColor(m_ThemeMode, BasicHoverAlpha));
        } else {
          if (vopt->state & QStyle::State_MouseOver) {
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
          if (vopt->state & QStyle::State_MouseOver) {
            // 覆盖时颜色
            painter->fillPath(path, NXThemeColor(m_ThemeMode, BasicHoverAlpha));
          }
        }
      }
      painter->restore();
    }
    return;
  }
  case QStyle::PE_PanelItemViewRow : {
    // 行背景
    return;
  }
  case QStyle::PE_IndicatorBranch : {
    //// Branch指示器
    //if (option->rect.isNull())
    //  return;

    //QRect itemRect = option->rect;
    //QPen pen;
    //pen.setColor(NXThemeColor(m_ThemeMode, PrimaryNormal));
    //pen.setWidth(2);
    //painter->setPen(pen);
    //painter->setRenderHint(QPainter::Antialiasing);
    //if (option->rect.height() == 0)
    //{
    //  painter->drawEllipse(QPoint(10, option->rect.top()), 4, 4);
    //  painter->drawLine(QPoint(10, option->rect.top()), QPoint(widget->width() - 10, option->rect.top()));
    //}
    //else
    //{
    //  itemRect.setLeft(5);
    //  itemRect.setRight(widget->width() - 5);
    //  painter->drawRect(itemRect);
    //}
    return;
  }
  default : {
    break;
  }
  }
  QProxyStyle::drawPrimitive(element, option, painter, widget);
}

void NXNavigationStyle::drawControl(ControlElement element, QStyleOption const *option, QPainter *painter, QWidget const *widget) const
{
  int fontPixelSize = nxApp->getFontPixelSize();
  switch (element) {
  case QStyle::CE_ShapedFrame : {
    // viewport视口外的其他区域背景
    return;
  }
  case QStyle::CE_ItemViewItem : {
    if (QStyleOptionViewItem const *vopt = qstyleoption_cast<QStyleOptionViewItem const *>(option)) {
      // 背景绘制
      this->drawPrimitive(QStyle::PE_PanelItemViewItem, option, painter, widget);

      // 内容绘制
      QRect itemRect = option->rect;
      painter->save();
      painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
      NXNavigationNode *node = static_cast<NXNavigationNode *>(vopt->index.internalPointer());
      NXNavigationModel *model =
          dynamic_cast<NXNavigationModel *>(const_cast<QAbstractItemModel *>(vopt->index.model()));

      if (this->m_OpacityAnimationTargetNode && node->getParentNode() == this->m_OpacityAnimationTargetNode) {
        painter->setOpacity(m_Opacity);
      }

      // 选中特效
      if (m_IsSelectMarkDisplay && (node == model->getSelectedNode() || node == model->getSelectedExpandedNode())) {
        painter->setPen(Qt::NoPen);
        painter->setBrush(NXThemeColor(m_ThemeMode, PrimaryNormal));
        painter->drawRoundedRect(QRectF(itemRect.x() + 3, itemRect.y() + m_SelectMarkTop, 3, itemRect.height() - m_SelectMarkTop - m_SelectMarkBottom), 1.5, 1.5);
      }
      if (node == m_LastSelectedNode) {
        painter->setPen(Qt::NoPen);
        painter->setBrush(NXThemeColor(m_ThemeMode, PrimaryNormal));
        painter->drawRoundedRect(QRectF(itemRect.x() + 3, itemRect.y() + m_LastSelectMarkTop, 3, itemRect.height() - m_LastSelectMarkTop - m_LastSelectMarkBottom), 1.5, 1.5);
      }

      // 图标绘制
      painter->setPen(vopt->index == m_PressIndex ? NXThemeColor(m_ThemeMode, BasicTextPress) : NXThemeColor(m_ThemeMode, BasicText));
      if (node->getAwesome() != NXIconType::None) {
        painter->save();
        QFont iconFont = QFont(QStringLiteral("NXAwesome"));
        iconFont.setPixelSize(fontPixelSize + 4);
        painter->setFont(iconFont);
        painter->drawText(QRect(itemRect.x(), itemRect.y(), m_IconAreaWidth, itemRect.height()), Qt::AlignCenter, QChar((unsigned short) node->getAwesome()));
        painter->restore();
      }

      int viewWidth = widget->width();
      // 文字绘制
      if (node->getIsCategoryNode()) {
        QFont categoryFont = painter->font();
        categoryFont.setBold(true);
        painter->setFont(categoryFont);
        painter->setPen(NXThemeColor(m_ThemeMode, BasicTextCategory));
      } else {
        painter->setPen((vopt->index == m_PressIndex) ? NXThemeColor(m_ThemeMode, BasicTextPress) : NXThemeColor(m_ThemeMode, BasicText));
      }
      QRect textRect;
      if (node->getAwesome() != NXIconType::None) {
        textRect =
            QRect(itemRect.x() + m_IconAreaWidth, itemRect.y(), itemRect.width() - m_TextRightSpacing - m_IndicatorIconAreaWidth - m_IconAreaWidth, itemRect.height());
      } else {
        textRect =
            QRect(itemRect.x() + m_LeftPadding, itemRect.y(), itemRect.width() - m_TextRightSpacing - m_IndicatorIconAreaWidth - m_LeftPadding, itemRect.height());
      }
      QString text = painter->fontMetrics().elidedText(node->getNodeTitle(), Qt::ElideRight, textRect.width());
      painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, text);

      if (viewWidth > m_NavigationView->getNavigationBarPrivate()->m_NavigationBarWidth - m_IndicatorIconAreaWidth) {
        // 展开图标 KeyPoints
        if (node->getIsExpanderNode()) {
          if (node->getIsHasChild()) {
            QRectF expandIconRect(itemRect.right() - m_IndicatorIconAreaWidth, itemRect.y(), 17, itemRect.height());
            painter->save();
            QFont iconFont = QFont(QStringLiteral("NXAwesome"));
            iconFont.setPixelSize(fontPixelSize + 4);
            painter->setFont(iconFont);
            painter->translate(expandIconRect.x() + (qreal) expandIconRect.width() / 2, expandIconRect.y() + (qreal) expandIconRect.height() / 2);
            if (node == m_ExpandAnimationTargetNode) {
              painter->rotate(m_Rotate);
            } else {
              if (node->getIsExpanded()) {
                //展开
                painter->rotate(-180);
              } else {
                // 未展开
                painter->rotate(0);
              }
            }
            painter->translate(-expandIconRect.x() - (qreal) expandIconRect.width() / 2 + 1, -expandIconRect.y() - (qreal) expandIconRect.height() / 2);
            painter->drawText(expandIconRect, Qt::AlignVCenter, QChar((unsigned short) NXIconType::AngleDown));
            painter->restore();
          }
          if (node->getIsChildHasKeyPoints()) {
            painter->save();
            painter->setPen(Qt::NoPen);
            painter->setBrush(NXThemeColor(m_ThemeMode, PrimaryNormal));
            painter->drawEllipse(QPoint(itemRect.right() - 17, itemRect.y() + 12), 3, 3);
            painter->restore();
          }
        } else {
          int keyPoints = node->getKeyPoints();
          if (keyPoints) {
            // KeyPoints
            painter->save();
            painter->setPen(Qt::NoPen);
            painter->setBrush(NXThemeColor(m_ThemeMode, PrimaryNormal));
            int keyPointRadius = 8;
            painter->drawEllipse(QPoint(itemRect.right() - 26, itemRect.y() + itemRect.height() / 2), keyPointRadius, keyPointRadius);
            painter->setPen(QPen(NXThemeColor(m_ThemeMode, BasicTextInvert), 2));
            QFont font = painter->font();
            font.setBold(true);
            if (keyPoints > 99) {
              keyPoints = 99;
            }
            if (keyPoints > 9) {
              font.setPixelSize(fontPixelSize - 3);
            } else {
              font.setPixelSize(fontPixelSize - 2);
            }
            painter->setFont(font);
            painter->drawText(QRect(QPoint(itemRect.right() - 26 - keyPointRadius, itemRect.y() + itemRect.height() / 2 - keyPointRadius), QSize(2 * keyPointRadius, 2 * keyPointRadius)), Qt::AlignCenter, QString::number(keyPoints));
            painter->restore();
          }
        }
      }
      painter->restore();
    }

    return;
  }
  default : {
    break;
  }
  }
  QProxyStyle::drawControl(element, option, painter, widget);
}

QSize NXNavigationStyle::sizeFromContents(ContentsType type, QStyleOption const *option, QSize const &size, QWidget const *widget) const
{
  switch (type) {
  case QStyle::CT_ItemViewItem : {
    QSize itemSize = QProxyStyle::sizeFromContents(type, option, size, widget);
    itemSize.setHeight(m_ItemHeight);
    return itemSize;
  }
  default : {
    break;
  }
  }
  return QProxyStyle::sizeFromContents(type, option, size, widget);
}

void NXNavigationStyle::navigationNodeStateChange(QVariantMap const &data)
{
  if (data.contains(QStringLiteral("Expand"))) {
    NXNavigationNode *lastExpandNode         = m_ExpandAnimationTargetNode;
    m_OpacityAnimationTargetNode              = data.value(QStringLiteral("Expand")).value<NXNavigationNode *>();
    m_ExpandAnimationTargetNode               = m_OpacityAnimationTargetNode;
    QPropertyAnimation *nodeOpacityAnimation = new QPropertyAnimation(this, "Opacity");
    connect(nodeOpacityAnimation, &QPropertyAnimation::finished, this, [=]() {
      m_OpacityAnimationTargetNode = nullptr;
    });
    connect(nodeOpacityAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
      m_NavigationView->viewport()->update();
    });
    nodeOpacityAnimation->setDuration(480);
    nodeOpacityAnimation->setEasingCurve(QEasingCurve::InOutSine);
    nodeOpacityAnimation->setKeyValueAt(0.5, 0);
    nodeOpacityAnimation->setStartValue(0);
    nodeOpacityAnimation->setEndValue(1);
    nodeOpacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);

    QPropertyAnimation *rotateAnimation = new QPropertyAnimation(this, "Rotate");
    connect(rotateAnimation, &QPropertyAnimation::finished, this, [=]() {
      m_ExpandAnimationTargetNode = nullptr;
    });
    connect(rotateAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
      m_NavigationView->viewport()->update();
    });
    rotateAnimation->setDuration(300);
    rotateAnimation->setEasingCurve(QEasingCurve::InOutSine);
    if (lastExpandNode == m_ExpandAnimationTargetNode) {
      rotateAnimation->setStartValue(m_Rotate);
    } else {
      rotateAnimation->setStartValue(0);
    }
    rotateAnimation->setEndValue(-180);
    rotateAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  } else if (data.contains(QStringLiteral("Collapse"))) {
    NXNavigationNode *lastExpandNode = m_ExpandAnimationTargetNode;
    m_OpacityAnimationTargetNode      = data.value(QStringLiteral("Collapse")).value<NXNavigationNode *>();
    m_ExpandAnimationTargetNode       = m_OpacityAnimationTargetNode;
    m_Opacity                        = 0;

    QPropertyAnimation *rotateAnimation = new QPropertyAnimation(this, "Rotate");
    connect(rotateAnimation, &QPropertyAnimation::finished, this, [=]() {
      m_Opacity                  = 1;
      m_ExpandAnimationTargetNode = nullptr;
    });
    connect(rotateAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
      m_NavigationView->viewport()->update();
    });
    rotateAnimation->setDuration(300);
    rotateAnimation->setEasingCurve(QEasingCurve::InOutSine);
    if (lastExpandNode == m_ExpandAnimationTargetNode) {
      rotateAnimation->setStartValue(m_Rotate);
    } else {
      rotateAnimation->setStartValue(-180);
    }
    rotateAnimation->setEndValue(0);
    rotateAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  } else if (data.contains(QStringLiteral("SelectMarkChanged"))) {
    m_LastSelectedNode              = data.value(QStringLiteral("LastSelectedNode")).value<NXNavigationNode *>();
    NXNavigationNode *selectedNode = data.value(QStringLiteral("SelectedNode")).value<NXNavigationNode *>();
    bool direction                 = _compareItemY(selectedNode, m_LastSelectedNode);
    m_LastSelectMarkTop            = 10.5;
    m_LastSelectMarkBottom         = 10.5;
    m_SelectMarkTop                = 10.5;
    m_SelectMarkBottom             = 10.5;
    if (direction) {
      m_LastSelectMarkTopAnimation->setStartValue(10.5);
      m_LastSelectMarkTopAnimation->setEndValue(0.0);
      m_LastSelectMarkTopAnimation->start();
      m_LastSelectMarkBottomAnimation->stop();
      m_SelectMarkTopAnimation->stop();
      m_IsSelectMarkDisplay = false;
    } else {
      m_LastSelectMarkBottomAnimation->setStartValue(10.5);
      m_LastSelectMarkBottomAnimation->setEndValue(0.0);
      m_LastSelectMarkBottomAnimation->start();
      m_LastSelectMarkTopAnimation->stop();
      m_SelectMarkBottomAnimation->stop();
      m_IsSelectMarkDisplay = false;
    }
  }
}

bool NXNavigationStyle::_compareItemY(NXNavigationNode *node1, NXNavigationNode *node2)
{
  // 返回true 即node1 高于 node2
  if (!node1) {
    return false;
  }
  if (!node2) {
    return true;
  }
  // 同一父节点
  if (node1->getParentNode() == node2->getParentNode()) {
    if (node1->getModelIndex().row() < node2->getModelIndex().row()) {
      return true;
    } else {
      return false;
    }
  } else {
    NXNavigationNode *node1OriginalNode = node1->getOriginalNode();
    NXNavigationNode *node2OriginalNode = node2->getOriginalNode();
    // 不同父节点  相同起源节点
    if (node1OriginalNode == node2OriginalNode) {
      int node1Depth = node1->getDepth();
      int node2Depth = node2->getDepth();
      // 相同深度
      if (node1Depth == node2Depth) {
        NXNavigationNode *node1ParentNode = node1->getParentNode();
        NXNavigationNode *node2ParentNode = node2->getParentNode();
        if (node1ParentNode->getModelIndex().row() < node2ParentNode->getModelIndex().row()) {
          return true;
        } else {
          return false;
        }
      } else {
        if (node1Depth < node2Depth) {
          NXNavigationNode *node2ParentNode = node2->getParentNode();
          while (node2ParentNode->getDepth() != node1Depth) {
            node2ParentNode = node2ParentNode->getParentNode();
          }
          // 父子节点关系
          if (node1 == node2ParentNode) {
            return true;
          }
          if (node1->getModelIndex().row() < node2ParentNode->getModelIndex().row()) {
            return true;
          } else {
            return false;
          }
        } else {
          NXNavigationNode *node1ParentNode = node1->getParentNode();
          while (node1ParentNode->getDepth() != node2Depth) {
            node1ParentNode = node1ParentNode->getParentNode();
          }
          if (node2 == node1ParentNode) {
            return false;
          }
          if (node1ParentNode->getModelIndex().row() < node2->getModelIndex().row()) {
            return true;
          } else {
            return false;
          }
        }
      }
    } else {
      if (node1OriginalNode->getModelIndex().row() < node2OriginalNode->getModelIndex().row()) {
        return true;
      } else {
        return false;
      }
    }
  }
}
