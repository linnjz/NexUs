#include "NXKeyBinderContainer.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QtMath>

#include "NXApplication.h"
#include "NXKeyBinderFnMonitor.h"
#include "NXKeyBinder.h"
#include "NXTheme.h"

NXKeyBinderContainer::NXKeyBinderContainer(QWidget *parent)
    : QWidget(parent)
    , m_ThemeMode { nxTheme->getThemeMode() }
    , m_KeyBinder { dynamic_cast<NXKeyBinder *>(parent) }
{
  setStyleSheet(QStringLiteral("#NXKeyBinderContainer{background-color:transparent;}"));
  setFocusPolicy(Qt::StrongFocus);
  setMouseTracking(true);
  setFixedHeight(140);
  QFont textFont = font();
  textFont.setLetterSpacing(QFont::AbsoluteSpacing, 0.5);
  textFont.setPixelSize(nxApp->getFontPixelSize() + 3);
  setFont(textFont);

  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    m_ThemeMode = themeMode;
    update();
  });
  startFnKeyMonitor([this]() {
    if (hasFocus()) {
      m_BinderKeyText          = QStringLiteral("Globe");
      m_NativeVirtualBinderKey = 0x3F;
      update();
    }
  });
}

NXKeyBinderContainer::~NXKeyBinderContainer()
{
  stopFnKeyMonitor();
}

void NXKeyBinderContainer::logOrResetHistoryData(bool isLog)
{
  if (isLog) {
    m_HistoryBinderKeyText          = m_BinderKeyText;
    m_HistoryNativeVirtualBinderKey = m_NativeVirtualBinderKey;
  } else {
    m_BinderKeyText          = m_HistoryBinderKeyText;
    m_NativeVirtualBinderKey = m_HistoryNativeVirtualBinderKey;
    update();
  }
}

void NXKeyBinderContainer::saveBinderChanged()
{
  Q_EMIT m_KeyBinder->binderKeyTextChanged(m_BinderKeyText);
  Q_EMIT m_KeyBinder->nativeVirtualBinderKeyChanged(m_NativeVirtualBinderKey);
  if (m_BinderKeyText.isEmpty()) {
    m_KeyBinder->setText(u8"  按键: " + QString(u8"未绑定") + QStringLiteral("      "));
  } else {
    m_KeyBinder->setText(u8"  按键: " + m_BinderKeyText + QStringLiteral("      "));
  }
}

bool NXKeyBinderContainer::event(QEvent *event)
{
  switch (event->type()) {
  case QEvent::KeyPress : {
    QKeyEvent *keyEvent = dynamic_cast<QKeyEvent *>(event);
    if (!keyEvent->isAutoRepeat()) {
      switch (keyEvent->key()) {
      case Qt::Key_Shift : {
        m_BinderKeyText = QStringLiteral("Shift");
        break;
      }
      case Qt::Key_Control : {
        m_BinderKeyText = QStringLiteral("Ctrl");
        break;
      }
      case Qt::Key_Alt : {
#ifdef Q_OS_MACOS
        m_BinderKeyText = QStringLiteral("Option");
#else
        m_BinderKeyText = QStringLiteral("Alt");
#endif
        break;
      }
      case Qt::Key_Meta : {
#ifdef Q_OS_MACOS
        m_BinderKeyText = QStringLiteral("Cmd");
#else
        m_BinderKeyText = QStringLiteral("Win");
#endif
        break;
      }
      default : {
        m_BinderKeyText = QKeySequence(keyEvent->key()).toString();
        break;
      }
      }
      m_NativeVirtualBinderKey = keyEvent->nativeVirtualKey();
      update();
    }
    event->accept();
    return true;
  }
  default : {
    break;
  }
  }
  return QWidget::event(event);
}

void NXKeyBinderContainer::mousePressEvent(QMouseEvent *event)
{
  switch (event->button()) {
  case Qt::LeftButton : {
    m_BinderKeyText          = QStringLiteral("Mouse1");
    m_NativeVirtualBinderKey = 0x01;
    break;
  }
  case Qt::RightButton : {
    m_BinderKeyText          = QStringLiteral("Mouse2");
    m_NativeVirtualBinderKey = 0x02;
    break;
  }
  case Qt::MiddleButton : {
    m_BinderKeyText          = QStringLiteral("Middle");
    m_NativeVirtualBinderKey = 0x04;
    break;
  }
  case Qt::BackButton : {
    m_BinderKeyText          = QStringLiteral("Back");
    m_NativeVirtualBinderKey = 0x05;
    break;
  }
  case Qt::ForwardButton : {
    m_BinderKeyText          = QStringLiteral("Forward");
    m_NativeVirtualBinderKey = 0x06;
    break;
  }
  default : {
    break;
  }
  }
  QWidget::mousePressEvent(event);
  update();
}

void NXKeyBinderContainer::focusOutEvent(QFocusEvent *event)
{
  QWidget::focusOutEvent(event);
  setFocus();
}

void NXKeyBinderContainer::paintEvent(QPaintEvent *event)
{
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  // 顶部提示绘制
  painter.setPen(NXThemeColor(m_ThemeMode, BasicText));
  painter.drawText(QRect(20, 20, width(), 50), Qt::AlignLeft | Qt::AlignTop, u8"按下任意按键以进行绑定");
  if (m_BinderKeyText.isEmpty()) {
    painter.restore();
    return;
  }
  // 中心文字背景绘制
  painter.setPen(NXThemeColor(m_ThemeMode, BasicBorder));
  painter.setBrush(NXThemeColor(m_ThemeMode, PrimaryNormal));
  qreal textWidth  = fontMetrics().horizontalAdvance(m_BinderKeyText) * 1.3;
  qreal textHeight = fontMetrics().height() * 1.2;
  textWidth        = qMax(textWidth, 35.0);
  textHeight       = qMax(textHeight, 35.0);
  painter.drawRoundedRect(QRectF((width() - textWidth) / 2.0, rect().center().y() + 10, textWidth, textHeight), 5, 5);
  // 文字绘制
  painter.setPen(NXThemeColor(m_ThemeMode, BasicTextInvert));
  painter.drawText(QRectF((width() - textWidth) / 2.0, rect().center().y() + 10, textWidth, textHeight), Qt::AlignCenter, m_BinderKeyText);
  painter.restore();
}
