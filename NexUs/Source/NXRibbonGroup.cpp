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

NXRibbonGroup::NXRibbonGroup(QString const &title, QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXRibbonGroupPrivate())
{
  Q_D(NXRibbonGroup);
  d->q_ptr  = this;
  d->m_Title = title;

  setObjectName("NXRibbonGroup");
  setAttribute(Qt::WA_StyledBackground, false);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(4, 2, 10, 2);
  mainLayout->setSpacing(0);

  QWidget *contentWidget = new QWidget(this);
  contentWidget->setAttribute(Qt::WA_TranslucentBackground);
  d->m_ContentLayout = new QHBoxLayout(contentWidget);
  d->m_ContentLayout->setContentsMargins(0, 0, 0, 0);
  d->m_ContentLayout->setSpacing(1);
  d->m_ContentLayout->addStretch();
  mainLayout->addWidget(contentWidget, 1);
  mainLayout->addSpacing(16);

  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
    update();
  });
}

NXRibbonGroup::~NXRibbonGroup()
{
}

void NXRibbonGroup::setTitle(QString const &title)
{
  Q_D(NXRibbonGroup);
  d->m_Title = title;
  update();
}

QString NXRibbonGroup::getTitle() const
{
  Q_CONST_D(NXRibbonGroup);
  return d->m_Title;
}

NXToolButton *NXRibbonGroup::addToolButton(NXIconType::IconName icon, QString const &text, ButtonSize size)
{
  Q_D(NXRibbonGroup);
  NXToolButton *button = new NXToolButton(this);
  button->setNXIcon(icon);
  button->setText(text);
  if (int const fs = nxApp->getFontPixelSize(); size == Large) {
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
  d->m_ContentLayout->insertWidget(d->m_ContentLayout->count() - 1, button);
  connect(button, &NXToolButton::clicked, this, [this, button] {
    Q_EMIT ribbonButtonTriggered(button);
  });
  return button;
}

void NXRibbonGroup::addWidget(QWidget *widget)
{
  Q_D(NXRibbonGroup);
  d->m_ContentLayout->insertWidget(d->m_ContentLayout->count() - 1, widget);
}

void NXRibbonGroup::removeButton(NXToolButton *button)
{
  Q_D(NXRibbonGroup);
  if (!button || button->parentWidget() != this) {
    return;
  }
  d->m_ContentLayout->removeWidget(button);
  button->deleteLater();
}

QList<NXToolButton *> NXRibbonGroup::getButtons() const
{
  Q_CONST_D(NXRibbonGroup);
  QList<NXToolButton *> buttons;
  for (int i = 0; i < d->m_ContentLayout->count(); ++i) {
    if (auto *button = qobject_cast<NXToolButton *>(d->m_ContentLayout->itemAt(i)->widget())) {
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

  if (!d->m_Title.isEmpty()) {
    int const fs    = nxApp->getFontPixelSize();
    QFont titleFont = font();
    titleFont.setPixelSize(fs - 2);
    painter.setFont(titleFont);
    QFontMetrics fm(titleFont);
    int const textW = fm.horizontalAdvance(d->m_Title);

    int const titleTop  = height() - 16;
    int const titleH    = 14;
    int const lineY     = titleTop + titleH / 2;
    int const leftStart = 4;
    int const rightEnd  = width() - 8;
    int const center    = (leftStart + rightEnd) / 2;
    int const textGap   = 6;
    int const textLeft  = center - textW / 2 - textGap;
    int const textRight = center + textW / 2 + textGap;

    painter.setPen(QPen(NXThemeColor(d->m_ThemeMode, BasicBorder), 1));
    if (textLeft > leftStart) {
      painter.drawLine(leftStart, lineY, textLeft, lineY);
    }
    if (rightEnd > textRight) {
      painter.drawLine(textRight, lineY, rightEnd, lineY);
    }

    painter.setPen(NXThemeColor(d->m_ThemeMode, BasicTextCategory));
    QRect titleRect(leftStart, titleTop, rightEnd - leftStart, titleH);
    painter.drawText(titleRect, Qt::AlignCenter, d->m_Title);
  }

  painter.setPen(QPen(NXThemeColor(d->m_ThemeMode, BasicBorder), 1));
  int sepX      = width() - 5;
  int sepTop    = 4;
  int sepBottom = height() - 4;
  painter.drawLine(sepX, sepTop, sepX, sepBottom);
}

QSize NXRibbonGroup::sizeHint() const
{
  Q_CONST_D(NXRibbonGroup);
  QSize hint   = QWidget::sizeHint();
  int const fs = nxApp->getFontPixelSize();
  QFont f      = font();
  f.setPixelSize(fs - 2);
  QFontMetrics fm(f);
  int titleW = fm.horizontalAdvance(d->m_Title) + fs + 7;
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
