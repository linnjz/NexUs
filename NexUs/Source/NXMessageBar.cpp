#include "NXMessageBar.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QResizeEvent>

#include "NXApplication.h"
#include "NXIconButton.h"
#include "NXTheme.h"
#include "private/NXMessageBarPrivate.h"

NXMessageBar::NXMessageBar(NXMessageBarType::PositionPolicy policy, NXMessageBarType::MessageMode messageMode, const QString &title, const QString &text, int displayMsec, QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXMessageBarPrivate())
{
  Q_D(NXMessageBar);
  d->q_ptr         = this;
  d->_borderRadius = 6;
  d->_title        = title;
  d->_text         = text;
  d->_policy       = policy;
  d->_messageMode  = messageMode;
  d->_themeMode    = nxTheme->getThemeMode();
  setMouseTracking(true);
  d->_pOpacity = 1;
  setFont(qApp->font());
  parent->installEventFilter(this);
  int fontPixelSize = nxApp->getFontPixelSize();
  setFont(QFont("微软雅黑"));
  d->_closeButton = new NXIconButton(NXIconType::Xmark, fontPixelSize + 3, d->_closeButtonWidth, 30, this);
  switch (d->_messageMode) {
  case NXMessageBarType::Success : {
    d->_closeButton->setLightHoverColor(QColor(0xCA, 0xDE, 0xC8));
    d->_closeButton->setDarkHoverColor(QColor(0xCA, 0xDE, 0xC8));
    d->_closeButton->setDarkIconColor(Qt::black);
    d->_closeButton->setDarkHoverIconColor(Qt::black);
    break;
  }
  case NXMessageBarType::Warning : {
    d->_closeButton->setLightHoverColor(QColor(0xE8, 0xDE, 0xBB));
    d->_closeButton->setDarkHoverColor(QColor(0xE8, 0xDE, 0xBB));
    d->_closeButton->setDarkIconColor(Qt::black);
    d->_closeButton->setDarkHoverIconColor(Qt::black);
    break;
  }
  case NXMessageBarType::Information : {
    d->_closeButton->setLightHoverColor(QColor(0xDE, 0xDE, 0xDE));
    d->_closeButton->setDarkHoverColor(QColor(0xDE, 0xDE, 0xDE));
    d->_closeButton->setDarkIconColor(Qt::black);
    d->_closeButton->setDarkHoverIconColor(Qt::black);
    break;
  }
  case NXMessageBarType::Error : {
    d->_closeButton->setLightHoverColor(QColor(0xF2, 0xDD, 0xE0));
    d->_closeButton->setDarkHoverColor(QColor(0xF2, 0xDD, 0xE0));
    d->_closeButton->setDarkIconColor(Qt::black);
    d->_closeButton->setDarkHoverIconColor(Qt::black);
    break;
  }
  }
  d->_closeButton->setBorderRadius(5);
  connect(d->_closeButton, &NXIconButton::clicked, d, &NXMessageBarPrivate::messageBarEnd);
  QHBoxLayout *mainLayout = new QHBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 10, 0);
  mainLayout->addStretch();
  mainLayout->addWidget(d->_closeButton);
  setObjectName("NXMessageBar");
  setStyleSheet(QStringLiteral("#NXMessageBar{background-color:transparent;}"));
  connect(nxTheme, &NXTheme::themeModeChanged, d, &NXMessageBarPrivate::onThemeChanged);
  d->_messageBarCreate(displayMsec);
}

NXMessageBar::~NXMessageBar()
{
}

void NXMessageBar::success(NXMessageBarType::PositionPolicy policy, const QString &title, const QString &text, int displayMsec, QWidget *parent)
{
  // qDebug() << QApplication::topLevelWidgets();
  if (!parent) {
    QList<QWidget *> widgetList = QApplication::topLevelWidgets();
    for (auto widget : widgetList) {
      if (widget->property("NXBaseClassName").toString() == QStringLiteral("NXWindow")) {
        parent = widget;
      }
    }
    if (!parent) {
      return;
    }
  }

  NXMessageBar *bar = new NXMessageBar(policy, NXMessageBarType::Success, title, text, displayMsec, parent);
  Q_UNUSED(bar);
}

void NXMessageBar::warning(NXMessageBarType::PositionPolicy policy, const QString &title, const QString &text, int displayMsec, QWidget *parent)
{
  if (!parent) {
    QList<QWidget *> widgetList = QApplication::topLevelWidgets();
    for (auto widget : widgetList) {
      if (widget->property("NXBaseClassName").toString() == QStringLiteral("NXWindow")) {
        parent = widget;
      }
    }
    if (!parent) {
      return;
    }
  }
  NXMessageBar *bar = new NXMessageBar(policy, NXMessageBarType::Warning, title, text, displayMsec, parent);
  Q_UNUSED(bar);
}

void NXMessageBar::information(NXMessageBarType::PositionPolicy policy, const QString &title, const QString &text, int displayMsec, QWidget *parent)
{
  if (!parent) {
    QList<QWidget *> widgetList = QApplication::topLevelWidgets();
    for (auto widget : widgetList) {
      if (widget->property("NXBaseClassName").toString() == QStringLiteral("NXWindow")) {
        parent = widget;
      }
    }
    if (!parent) {
      return;
    }
  }
  NXMessageBar *bar = new NXMessageBar(policy, NXMessageBarType::Information, title, text, displayMsec, parent);
  Q_UNUSED(bar);
}

void NXMessageBar::error(NXMessageBarType::PositionPolicy policy, const QString &title, const QString &text, int displayMsec, QWidget *parent)
{
  if (!parent) {
    QList<QWidget *> widgetList = QApplication::topLevelWidgets();
    for (auto widget : widgetList) {
      if (widget->property("NXBaseClassName").toString() == QStringLiteral("NXWindow")) {
        parent = widget;
      }
    }
    if (!parent) {
      return;
    }
  }
  NXMessageBar *bar = new NXMessageBar(policy, NXMessageBarType::Error, title, text, displayMsec, parent);
  Q_UNUSED(bar);
}

void NXMessageBar::paintEvent(QPaintEvent *event)
{
  Q_D(NXMessageBar);
  const int fontPixelSize = nxApp->getFontPixelSize();
  QPainter painter(this);
  painter.setOpacity(d->_pOpacity);
  painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing | QPainter::TextAntialiasing);
  // 高性能阴影
  nxTheme->drawEffectShadow(&painter, rect(), d->_shadowBorderWidth, d->_borderRadius);

  // 背景和图标绘制
  painter.save();
  painter.setPen(d->_themeMode == NXThemeType::Light ? QColor(0xBE, 0xBA, 0xBE) : QColor(0x52, 0x50, 0x52));
  switch (d->_messageMode) {
  case NXMessageBarType::Success : {
    d->_drawSuccess(&painter);
    break;
  }
  case NXMessageBarType::Warning : {
    d->_drawWarning(&painter);
    break;
  }
  case NXMessageBarType::Information : {
    d->_drawInformation(&painter);
    break;
  }
  case NXMessageBarType::Error : {
    d->_drawError(&painter);
    break;
  }
  }
  // 文字绘制
  // 标题
  QFont font = this->font();
  font.setWeight(QFont::Bold);
  font.setPixelSize(fontPixelSize);
  painter.setFont(font);
  int titleTextWidth = painter.fontMetrics().horizontalAdvance(d->_title) + 1;
  if (titleTextWidth > 100) {
    titleTextWidth = 100;
  }
  int textFlags = Qt::AlignLeft | Qt::AlignVCenter | Qt::TextWordWrap | Qt::TextWrapAnywhere;
  painter.drawText(QRect(d->_leftPadding + d->_titleLeftSpacing, -1, titleTextWidth, height()), textFlags, d->_title);
  // 正文
  font.setWeight(QFont::Light);
  painter.setFont(font);
  painter.drawText(QRect(d->_leftPadding + d->_titleLeftSpacing + titleTextWidth + d->_textLeftSpacing, 0, width() - (d->_leftPadding + d->_titleLeftSpacing + titleTextWidth + d->_textLeftSpacing + d->_closeButtonWidth + d->_closeButtonLeftRightMargin / 2), height() - d->_timePercentHeight), textFlags, d->_text);
  painter.restore();
}

bool NXMessageBar::eventFilter(QObject *watched, QEvent *event)
{
  Q_D(NXMessageBar);
  if (watched == parentWidget()) {
    switch (event->type()) {
    case QEvent::Resize : {
      QResizeEvent *resizeEvent = dynamic_cast<QResizeEvent *>(event);
      QSize offsetSize          = parentWidget()->size() - resizeEvent->oldSize();
      if (d->_isNormalDisplay) {
        switch (d->_policy) {
        case NXMessageBarType::Top : {
          this->move(parentWidget()->width() / 2 - minimumWidth() / 2, this->y());
          break;
        }
        case NXMessageBarType::Bottom : {
          this->move(parentWidget()->width() / 2 - minimumWidth() / 2, this->pos().y() + offsetSize.height());
          break;
        }
        case NXMessageBarType::Left :
        case NXMessageBarType::TopLeft : {
          this->move(d->_messageBarHorizontalMargin, this->pos().y());
          break;
        }
        case NXMessageBarType::BottomLeft : {
          this->move(d->_messageBarHorizontalMargin, this->pos().y() + offsetSize.height());
          break;
        }
        case NXMessageBarType::Right :
        case NXMessageBarType::TopRight : {
          this->move(parentWidget()->width() - minimumWidth() - d->_messageBarHorizontalMargin, this->y());
          break;
        }
        case NXMessageBarType::BottomRight : {
          this->move(parentWidget()->width() - minimumWidth() - d->_messageBarHorizontalMargin, this->pos().y() + offsetSize.height());
          break;
        }
        }
      }
      break;
    }
    default : {
      break;
    }
    }
  }
  return QWidget::eventFilter(watched, event);
}
