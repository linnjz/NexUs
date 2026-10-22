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
  d->_toolBarStyle = new NXToolBarStyle(style());
  setStyle(d->_toolBarStyle);
  layout()->setSpacing(10);
  layout()->setContentsMargins(3, 3, 3, 3);

  d->_themeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
    if (this->isFloating()) {
      update();
    }
  });
  setAttribute(Qt::WA_TranslucentBackground);

  connect(this, &NXToolBar::topLevelChanged, this, [=](bool topLevel) {
    if (topLevel) {
      layout()->setContentsMargins(d->_shadowBorderWidth + 3, d->_shadowBorderWidth + 3, d->_shadowBorderWidth + 3, d->_shadowBorderWidth + 3);
    } else {
      layout()->setContentsMargins(3, 3, 3, 3);
    }
  });
}

NXToolBar::NXToolBar(const QString &title, QWidget *parent)
    : NXToolBar(parent)
{
  setWindowTitle(title);
}

NXToolBar::~NXToolBar()
{
  Q_D(NXToolBar);
  QStyle *proxy    = d->_toolBarStyle;
  d->_toolBarStyle = nullptr;
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
  d->_toolBarStyle->setToolButtonSize(size);
}

QSize NXToolBar::getToolButtonSize() const
{
  Q_D(const NXToolBar);
  return d->_toolBarStyle->getToolButtonSize();
}

QAction * NXToolBar::addNXIconAction(NXIconType::IconName icon, const QString &text)
{
  QAction *action = new QAction(text, this);
  action->setProperty("NXIconType", QChar((unsigned short) icon));
  action->setIcon(NXIcon::getInstance()->getNXIcon(NXIconType::Broom, 1));
  addAction(action);
  return action;
}

QAction * NXToolBar::addNXIconAction(NXIconType::IconName icon, const QString &text, const QKeySequence &shortcut)
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
    nxTheme->drawEffectShadow(&painter, rect(), d->_shadowBorderWidth, 6);
    //背景
    painter.setPen(NXThemeColor(d->_themeMode, PopupBorder));
    painter.setBrush(NXThemeColor(d->_themeMode, DialogBase));
    QRect foregroundRect(d->_shadowBorderWidth, d->_shadowBorderWidth, width() - 2 * d->_shadowBorderWidth, height() - 2 * d->_shadowBorderWidth);
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
