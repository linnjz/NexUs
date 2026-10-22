#include "NXRibbonGroup.h"

#include <QFontMetrics>
#include <QHBoxLayout>
#include <QPainter>
#include <QVBoxLayout>

#include "NXApplication.h"
#include "NXIcon.h"
#include "NXTheme.h"
#include "NXToolButton.h"
#include "private/NXRibbonGroupPrivate.h"

NXRibbonGroup::NXRibbonGroup(const QString &title, QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXRibbonGroupPrivate())
{
  Q_D(NXRibbonGroup);
  d->q_ptr  = this;
  d->_title = title;

  setObjectName("NXRibbonGroup");
  setAttribute(Qt::WA_StyledBackground, false);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(4, 2, 10, 2);
  mainLayout->setSpacing(0);

  QWidget *contentWidget = new QWidget(this);
  contentWidget->setAttribute(Qt::WA_TranslucentBackground);
  d->_contentLayout = new QHBoxLayout(contentWidget);
  d->_contentLayout->setContentsMargins(0, 0, 0, 0);
  d->_contentLayout->setSpacing(1);
  d->_contentLayout->addStretch();
  mainLayout->addWidget(contentWidget, 1);
  mainLayout->addSpacing(16);

  d->_themeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
    update();
  });
}

NXRibbonGroup::~NXRibbonGroup()
{
}

void NXRibbonGroup::setTitle(const QString &title)
{
  Q_D(NXRibbonGroup);
  d->_title = title;
  update();
}

QString NXRibbonGroup::getTitle() const
{
  Q_D(const NXRibbonGroup);
  return d->_title;
}

NXToolButton * NXRibbonGroup::addToolButton(NXIconType::IconName icon, const QString &text, ButtonSize size)
{
  Q_D(NXRibbonGroup);
  NXToolButton *button = new NXToolButton(this);
  button->setNXIcon(icon);
  button->setText(text);
  if (const int fs = nxApp->getFontPixelSize(); size == Large) {
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    button->setIconSize(QSize(fs * 2 - 2, fs * 2 - 2));
    button->setFixedSize(fs * 4, fs * 4);
    QFont f = button->font();
    f.setPixelSize(fs - 2);
    button->setFont(f);
  } else {
    button->setToolButtonStyle(Qt::ToolButtonIconOnly);
    button->setIconSize(QSize(fs + 5, fs + 5));
    button->setFixedSize(fs * 2 + 4, fs * 2 + 4);
  }
  d->_contentLayout->insertWidget(d->_contentLayout->count() - 1, button);
  connect(button, &NXToolButton::clicked, this, [this, button] {
    Q_EMIT ribbonButtonTriggered(button);
  });
  return button;
}

void NXRibbonGroup::addWidget(QWidget *widget)
{
  Q_D(NXRibbonGroup);
  d->_contentLayout->insertWidget(d->_contentLayout->count() - 1, widget);
}

void NXRibbonGroup::removeButton(NXToolButton *button)
{
  Q_D(NXRibbonGroup);
  if (!button || button->parentWidget() != this) {
    return;
  }
  d->_contentLayout->removeWidget(button);
  button->deleteLater();
}

QList<NXToolButton *> NXRibbonGroup::getButtons() const
{
  Q_D(const NXRibbonGroup);
  QList<NXToolButton *> buttons;
  for (int i = 0; i < d->_contentLayout->count(); ++i) {
    if (auto *button = qobject_cast<NXToolButton *>(d->_contentLayout->itemAt(i)->widget())) {
      buttons.append(button);
    }
  }
  return buttons;
}

void NXRibbonGroup::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);
  Q_D(NXRibbonGroup);
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  if (!d->_title.isEmpty()) {
    const int fs    = nxApp->getFontPixelSize();
    QFont titleFont = font();
    titleFont.setPixelSize(fs - 2);
    painter.setFont(titleFont);
    QFontMetrics fm(titleFont);
    const int textW = fm.horizontalAdvance(d->_title);

    const int titleTop  = height() - 16;
    const int titleH    = 14;
    const int lineY     = titleTop + titleH / 2;
    const int leftStart = 4;
    const int rightEnd  = width() - 8;
    const int center    = (leftStart + rightEnd) / 2;
    const int textGap   = 6;
    const int textLeft  = center - textW / 2 - textGap;
    const int textRight = center + textW / 2 + textGap;

    painter.setPen(QPen(NXThemeColor(d->_themeMode, BasicBorder), 1));
    if (textLeft > leftStart) {
      painter.drawLine(leftStart, lineY, textLeft, lineY);
    }
    if (rightEnd > textRight) {
      painter.drawLine(textRight, lineY, rightEnd, lineY);
    }

    painter.setPen(NXThemeColor(d->_themeMode, BasicTextCategory));
    QRect titleRect(leftStart, titleTop, rightEnd - leftStart, titleH);
    painter.drawText(titleRect, Qt::AlignCenter, d->_title);
  }

  painter.setPen(QPen(NXThemeColor(d->_themeMode, BasicBorder), 1));
  int sepX      = width() - 5;
  int sepTop    = 4;
  int sepBottom = height() - 4;
  painter.drawLine(sepX, sepTop, sepX, sepBottom);
}

QSize NXRibbonGroup::sizeHint() const
{
  Q_D(const NXRibbonGroup);
  QSize hint   = QWidget::sizeHint();
  const int fs = nxApp->getFontPixelSize();
  QFont f      = font();
  f.setPixelSize(fs - 2);
  QFontMetrics fm(f);
  int titleW = fm.horizontalAdvance(d->_title) + fs + 7;
  if (hint.width() < titleW) {
    hint.setWidth(titleW);
  }
  if (hint.height() < fs * 6 + 2) {
    hint.setHeight(fs * 6 + 2);
  }
  return hint;
}

NXRibbonGroupPrivate::NXRibbonGroupPrivate(QObject *parent)
    : QObject { parent }
{
}

NXRibbonGroupPrivate::~NXRibbonGroupPrivate()
{
}
