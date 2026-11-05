#include "NXToolBar.h"

#include <QLayout>
#include <QPainter>
#include <QStyleOption>

#include "DeveloperComponents/NXToolBarStyle.h"
#include "NXIcon.h"
#include "NXTheme.h"
#include "private/NXToolBarPrivate.h"

NXToolBar::NXToolBar(QWidget *parent)
    : QToolBar(parent)
    , d_ptr(new NXToolBarPrivate())
{
  Q_D(NXToolBar);
  d->q_ptr = this;
#ifdef Q_OS_MACOS
  setAttribute(Qt::WA_Hover);
#endif
  setObjectName("NXToolBar");
  d->m_ToolBarStyle = new NXToolBarStyle(style());
  setStyle(d->m_ToolBarStyle);
  layout()->setSpacing(10);
  layout()->setContentsMargins(3, 3, 3, 3);

  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
    if (this->isFloating()) {
      update();
    }
  });
  setAttribute(Qt::WA_TranslucentBackground);

  connect(this, &NXToolBar::topLevelChanged, this, [=](bool topLevel) {
    if (topLevel) {
      layout()->setContentsMargins(d->m_ShadowBorderWidth + 3, d->m_ShadowBorderWidth + 3, d->m_ShadowBorderWidth + 3, d->m_ShadowBorderWidth + 3);
    } else {
      layout()->setContentsMargins(3, 3, 3, 3);
    }
  });
}

NXToolBar::NXToolBar(QString const &title, QWidget *parent)
    : NXToolBar(parent)
{
  setWindowTitle(title);
}

NXToolBar::~NXToolBar()
{
  Q_D(NXToolBar);
  QStyle *proxy    = d->m_ToolBarStyle;
  d->m_ToolBarStyle = nullptr;
  setStyle(nullptr);
  delete proxy;
}

void NXToolBar::setToolBarSpacing(int spacing)
{
  layout()->setSpacing(spacing);
}

int NXToolBar::getToolBarSpacing() const
{
  return layout()->spacing();
}

void NXToolBar::setToolButtonSize(QSize size)
{
  Q_D(NXToolBar);
  d->m_ToolBarStyle->setToolButtonSize(size);
}

QSize NXToolBar::getToolButtonSize() const
{
  Q_CONST_D(NXToolBar);
  return d->m_ToolBarStyle->getToolButtonSize();
}

QAction *NXToolBar::addNXIconAction(NXIconType::IconName icon, QString const &text)
{
  QAction *action = new QAction(text, this);
  action->setProperty("NXIconType", QChar((unsigned short) icon));
  action->setIcon(NXIcon::getInstance()->getNXIcon(NXIconType::Broom, 1));
  addAction(action);
  return action;
}

QAction *NXToolBar::addNXIconAction(NXIconType::IconName icon, QString const &text, QKeySequence const &shortcut)
{
  QAction *action = new QAction(text, this);
  action->setShortcut(shortcut);
  action->setProperty("NXIconType", QChar((unsigned short) icon));
  action->setIcon(NXIcon::getInstance()->getNXIcon(NXIconType::Broom, 1));
  addAction(action);
  return action;
}

void NXToolBar::paintEvent(QPaintEvent *event)
{
  Q_D(NXToolBar);
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::Antialiasing);
  if (isFloating()) {
    // 高性能阴影
    nxTheme->drawEffectShadow(&painter, rect(), d->m_ShadowBorderWidth, 6);
    //背景
    painter.setPen(NXThemeColor(d->m_ThemeMode, PopupBorder));
    painter.setBrush(NXThemeColor(d->m_ThemeMode, DialogBase));
    QRect foregroundRect(d->m_ShadowBorderWidth, d->m_ShadowBorderWidth, width() - 2 * d->m_ShadowBorderWidth, height() - 2 * d->m_ShadowBorderWidth);
    painter.drawRoundedRect(foregroundRect, 5, 5);
    QStyle *style = this->style();
    QStyleOptionToolBar opt;
    initStyleOption(&opt);
    opt.rect = style->subElementRect(QStyle::SE_ToolBarHandle, &opt, this);
    if (opt.rect.isValid()) {
      style->drawPrimitive(QStyle::PE_IndicatorToolBarHandle, &opt, &painter, this);
    }
  } else {
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::transparent);
    painter.drawRoundedRect(rect(), 5, 5);
    QToolBar::paintEvent(event);
  }
  painter.restore();
}
