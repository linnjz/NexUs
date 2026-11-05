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

NXMessageBar::NXMessageBar(NXMessageBarType::PositionPolicy policy, NXMessageBarType::MessageMode messageMode, QString const &title, QString const &text, int displayMsec, QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXMessageBarPrivate())
{
  Q_D(NXMessageBar);
  d->q_ptr         = this;
  d->m_BorderRadius = 6;
  d->m_Title        = title;
  d->m_Text         = text;
  d->m_Policy       = policy;
  d->m_MessageMode  = messageMode;
  d->m_ThemeMode    = nxTheme->getThemeMode();
  setMouseTracking(true);
  d->m_Opacity = 1;
  setFont(qApp->font());
  parent->installEventFilter(this);
  int fontPixelSize = nxApp->getFontPixelSize();
  setFont(QFont("微软雅黑"));
  d->m_CloseButton = new NXIconButton(NXIconType::Xmark, fontPixelSize + 3, d->m_CloseButtonWidth, 30, this);
  switch (d->m_MessageMode) {
  case NXMessageBarType::Success : {
    d->m_CloseButton->setLightHoverColor(QColor(0xCA, 0xDE, 0xC8));
    d->m_CloseButton->setDarkHoverColor(QColor(0xCA, 0xDE, 0xC8));
    d->m_CloseButton->setDarkIconColor(Qt::black);
    d->m_CloseButton->setDarkHoverIconColor(Qt::black);
    break;
  }
  case NXMessageBarType::Warning : {
    d->m_CloseButton->setLightHoverColor(QColor(0xE8, 0xDE, 0xBB));
    d->m_CloseButton->setDarkHoverColor(QColor(0xE8, 0xDE, 0xBB));
    d->m_CloseButton->setDarkIconColor(Qt::black);
    d->m_CloseButton->setDarkHoverIconColor(Qt::black);
    break;
  }
  case NXMessageBarType::Information : {
    d->m_CloseButton->setLightHoverColor(QColor(0xDE, 0xDE, 0xDE));
    d->m_CloseButton->setDarkHoverColor(QColor(0xDE, 0xDE, 0xDE));
    d->m_CloseButton->setDarkIconColor(Qt::black);
    d->m_CloseButton->setDarkHoverIconColor(Qt::black);
    break;
  }
  case NXMessageBarType::Error : {
    d->m_CloseButton->setLightHoverColor(QColor(0xF2, 0xDD, 0xE0));
    d->m_CloseButton->setDarkHoverColor(QColor(0xF2, 0xDD, 0xE0));
    d->m_CloseButton->setDarkIconColor(Qt::black);
    d->m_CloseButton->setDarkHoverIconColor(Qt::black);
    break;
  }
  }
  d->m_CloseButton->setBorderRadius(5);
  connect(d->m_CloseButton, &NXIconButton::clicked, d, &NXMessageBarPrivate::messageBarEnd);
  QHBoxLayout *mainLayout = new QHBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 10, 0);
  mainLayout->addStretch();
  mainLayout->addWidget(d->m_CloseButton);
  setObjectName("NXMessageBar");
  setStyleSheet(QStringLiteral("#NXMessageBar{background-color:transparent;}"));
  connect(nxTheme, &NXTheme::themeModeChanged, d, &NXMessageBarPrivate::onThemeChanged);
  d->_messageBarCreate(displayMsec);
}

NXMessageBar::~NXMessageBar()
{
}

void NXMessageBar::success(NXMessageBarType::PositionPolicy policy, QString const &title, QString const &text, int displayMsec, QWidget *parent)
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

void NXMessageBar::warning(NXMessageBarType::PositionPolicy policy, QString const &title, QString const &text, int displayMsec, QWidget *parent)
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

void NXMessageBar::information(NXMessageBarType::PositionPolicy policy, QString const &title, QString const &text, int displayMsec, QWidget *parent)
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

void NXMessageBar::error(NXMessageBarType::PositionPolicy policy, QString const &title, QString const &text, int displayMsec, QWidget *parent)
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
  int const fontPixelSize = nxApp->getFontPixelSize();
  QPainter painter(this);
  painter.setOpacity(d->m_Opacity);
  painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing | QPainter::TextAntialiasing);
  // 高性能阴影
  nxTheme->drawEffectShadow(&painter, rect(), d->m_ShadowBorderWidth, d->m_BorderRadius);

  // 背景和图标绘制
  painter.save();
  painter.setPen(d->m_ThemeMode == NXThemeType::Light ? QColor(0xBE, 0xBA, 0xBE) : QColor(0x52, 0x50, 0x52));
  switch (d->m_MessageMode) {
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
  int titleTextWidth = painter.fontMetrics().horizontalAdvance(d->m_Title) + 1;
  if (titleTextWidth > 100) {
    titleTextWidth = 100;
  }
  int textFlags = Qt::AlignLeft | Qt::AlignVCenter | Qt::TextWordWrap | Qt::TextWrapAnywhere;
  painter.drawText(QRect(d->m_LeftPadding + d->m_TitleLeftSpacing, -1, titleTextWidth, height()), textFlags, d->m_Title);
  // 正文
  font.setWeight(QFont::Light);
  painter.setFont(font);
  painter.drawText(QRect(d->m_LeftPadding + d->m_TitleLeftSpacing + titleTextWidth + d->m_TextLeftSpacing, 0, width() - (d->m_LeftPadding + d->m_TitleLeftSpacing + titleTextWidth + d->m_TextLeftSpacing + d->m_CloseButtonWidth + d->m_CloseButtonLeftRightMargin / 2), height() - d->m_TimePercentHeight), textFlags, d->m_Text);
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
      if (d->m_IsNormalDisplay) {
        switch (d->m_Policy) {
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
          this->move(d->m_MessageBarHorizontalMargin, this->pos().y());
          break;
        }
        case NXMessageBarType::BottomLeft : {
          this->move(d->m_MessageBarHorizontalMargin, this->pos().y() + offsetSize.height());
          break;
        }
        case NXMessageBarType::Right :
        case NXMessageBarType::TopRight : {
          this->move(parentWidget()->width() - minimumWidth() - d->m_MessageBarHorizontalMargin, this->y());
          break;
        }
        case NXMessageBarType::BottomRight : {
          this->move(parentWidget()->width() - minimumWidth() - d->m_MessageBarHorizontalMargin, this->pos().y() + offsetSize.height());
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
