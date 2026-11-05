#include "NXPasswordBox.h"

#include <QAction>
#include <QClipboard>
#include <QContextMenuEvent>
#include <QGuiApplication>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>

#include "DeveloperComponents/NXLineEditStyle.h"
#include "NXApplication.h"
#include "NXDef.h"
#include "NXEventBus.h"
#include "NXIcon.h"
#include "NXMenu.h"
#include "NXTheme.h"
#include "private/NXPasswordBoxPrivate.h"
Q_PROPERTY_CREATE_CPP(NXPasswordBox, QPS_PAEMIT(int), BorderRadius)

NXPasswordBox::NXPasswordBox(QWidget *parent)
    : QLineEdit(parent)
    , d_ptr(new NXPasswordBoxPrivate())
{
  Q_D(NXPasswordBox);
  d->q_ptr = this;
  setObjectName("NXPasswordBox");
  setFixedHeight(35);
  d->m_ThemeMode          = nxTheme->getThemeMode();
  d->m_BorderRadius      = 6;
  d->m_ExpandMarkWidth   = 0;
  d->m_IsPasswordVisible = false;
  setEchoMode(QLineEdit::Password);
  setFocusPolicy(Qt::StrongFocus);
  // 事件总线
  d->m_FocusEvent = new NXEvent(QStringLiteral("WMWindowClicked"), QStringLiteral("onWMWindowClickedEvent"), d);
  d->m_FocusEvent->registerAndInit();
  setMouseTracking(true);
  QFont textFont = font();
  textFont.setLetterSpacing(QFont::AbsoluteSpacing, d->m_TextSpacing);
  setFont(textFont);
  setStyle(new NXLineEditStyle(style()));
  setStyleSheet(QStringLiteral("#NXPasswordBox{background-color:transparent;padding-left: 10px;padding-right: 36px;}"));
  // Toggle action for password visibility
  d->m_ToggleAction = new QAction(this);
  d->m_ToggleAction->setIcon(
      NXIcon::getInstance()->getNXIcon(NXIconType::Eye, 22, 32, 32, NXThemeColor(d->m_ThemeMode, BasicText)));
  addAction(d->m_ToggleAction, QLineEdit::TrailingPosition);
  connect(d->m_ToggleAction, &QAction::triggered, this, [=]() {
    setIsPasswordVisible(!getIsPasswordVisible());
  });
  d->onThemeChanged(nxTheme->getThemeMode());
  connect(nxTheme, &NXTheme::themeModeChanged, d, &NXPasswordBoxPrivate::onThemeChanged);
  setVisible(true);
}

NXPasswordBox::~NXPasswordBox()
{
  delete this->style();
}

void NXPasswordBox::setIsPasswordVisible(bool isPasswordVisible)
{
  Q_D(NXPasswordBox);
  d->m_IsPasswordVisible = isPasswordVisible;
  if (isPasswordVisible) {
    setEchoMode(QLineEdit::Normal);
    d->m_ToggleAction->setIcon(
        NXIcon::getInstance()->getNXIcon(NXIconType::EyeSlash, 22, 32, 32, NXThemeColor(d->m_ThemeMode, BasicText)));
  } else {
    setEchoMode(QLineEdit::Password);
    d->m_ToggleAction->setIcon(
        NXIcon::getInstance()->getNXIcon(NXIconType::Eye, 22, 32, 32, NXThemeColor(d->m_ThemeMode, BasicText)));
  }
  Q_EMIT signalIsPasswordVisibleChanged();
}

bool NXPasswordBox::getIsPasswordVisible() const noexcept(std::is_nothrow_copy_constructible_v<bool>)
{
  Q_CONST_D(NXPasswordBox);
  return d->m_IsPasswordVisible;
}

void NXPasswordBox::focusInEvent(QFocusEvent *event)
{
  Q_D(NXPasswordBox);
  Q_EMIT focusIn(this->text());
  if (event->reason() == Qt::MouseFocusReason) {
    QPropertyAnimation *markAnimation = new QPropertyAnimation(d, "ExpandMarkWidth");
    connect(markAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
      update();
    });
    markAnimation->setDuration(300);
    markAnimation->setEasingCurve(QEasingCurve::InOutSine);
    markAnimation->setStartValue(d->m_ExpandMarkWidth);
    markAnimation->setEndValue(width() / 2 - d->m_BorderRadius / 2);
    markAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  }
  QLineEdit::focusInEvent(event);
}

void NXPasswordBox::focusOutEvent(QFocusEvent *event)
{
  Q_D(NXPasswordBox);
  Q_EMIT focusOut(this->text());
  if (event->reason() != Qt::PopupFocusReason) {
    QPropertyAnimation *markAnimation = new QPropertyAnimation(d, "ExpandMarkWidth");
    connect(markAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
      update();
    });
    markAnimation->setDuration(300);
    markAnimation->setEasingCurve(QEasingCurve::InOutSine);
    markAnimation->setStartValue(d->m_ExpandMarkWidth);
    markAnimation->setEndValue(0);
    markAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    Q_EMIT wmFocusOut(text());
  }
  QLineEdit::focusOutEvent(event);
}

void NXPasswordBox::paintEvent(QPaintEvent *event)
{
  Q_D(NXPasswordBox);
  if (palette().color(QPalette::Text) != NXThemeColor(d->m_ThemeMode, BasicText)) {
    d->onThemeChanged(d->m_ThemeMode);
  }
  QLineEdit::paintEvent(event);
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  painter.setPen(Qt::NoPen);
  painter.setBrush(NXThemeColor(d->m_ThemeMode, PrimaryNormal));
  painter.drawRoundedRect(QRectF(width() / 2 - d->m_ExpandMarkWidth, height() - 2.5, d->m_ExpandMarkWidth * 2, 2.5), 2, 2);
  painter.restore();
}

void NXPasswordBox::contextMenuEvent(QContextMenuEvent *event)
{
  NXMenu *menu = new NXMenu(this);
  menu->setMenuItemHeight(27);
  menu->setAttribute(Qt::WA_DeleteOnClose);
  QAction *action = nullptr;
  if (!isReadOnly()) {
    action = menu->addNXIconAction(NXIconType::ArrowRotateLeft, tr("撤销"), QKeySequence::Undo);
    action->setEnabled(isUndoAvailable());
    connect(action, &QAction::triggered, this, &NXPasswordBox::undo);

    action = menu->addNXIconAction(NXIconType::ArrowRotateRight, tr("恢复"), QKeySequence::Redo);
    action->setEnabled(isRedoAvailable());
    connect(action, &QAction::triggered, this, &NXPasswordBox::redo);
    menu->addSeparator();
  }
#ifndef QT_NO_CLIPBOARD
  if (!isReadOnly()) {
    action = menu->addNXIconAction(NXIconType::Paste, tr("粘贴"), QKeySequence::Paste);
    action->setEnabled(!isReadOnly() && !QGuiApplication::clipboard()->text().isEmpty());
    connect(action, &QAction::triggered, this, &NXPasswordBox::paste);
  }
#endif
  if (!isReadOnly()) {
    action = menu->addNXIconAction(NXIconType::DeleteLeft, tr("删除"));
    action->setEnabled(!isReadOnly() && !text().isEmpty() && hasSelectedText());
    connect(action, &QAction::triggered, this, [=](bool checked) {
      if (hasSelectedText()) {
        int startIndex = selectionStart();
        int endIndex   = selectionEnd();
        setText(text().remove(startIndex, endIndex - startIndex));
      }
    });
  }
  if (!menu->isEmpty()) {
    menu->addSeparator();
  }
  action = menu->addAction(tr("全选"));
  action->setShortcut(QKeySequence::SelectAll);
  action->setEnabled(!text().isEmpty() && !(selectedText() == text()));
  connect(action, &QAction::triggered, this, &NXPasswordBox::selectAll);
  menu->popup(event->globalPos());
}

// NXPasswordBoxPrivate
NXPasswordBoxPrivate::NXPasswordBoxPrivate(QObject *parent)
    : QObject { parent }
{
}

NXPasswordBoxPrivate::~NXPasswordBoxPrivate()
{
}

void NXPasswordBoxPrivate::onWMWindowClickedEvent(QVariantMap const &data)
{
  Q_Q(NXPasswordBox);
  NXAppBarType::WMMouseActionType actionType =
      data.value(QStringLiteral("WMClickType")).value<NXAppBarType::WMMouseActionType>();
  if (actionType == NXAppBarType::WMLBUTTONDOWN) {
    if (q->hasSelectedText() && q->hasFocus()) {
      q->clearFocus();
    }
  } else if (actionType == NXAppBarType::WMLBUTTONUP || actionType == NXAppBarType::WMNCLBUTTONDOWN) {
    if (NXApplication::containsCursorToItem(q) || (actionType == NXAppBarType::WMLBUTTONUP && q->hasSelectedText())) {
      return;
    }
    if (q->hasFocus()) {
      q->clearFocus();
    }
  }
}

void NXPasswordBoxPrivate::onThemeChanged(NXThemeType::ThemeMode themeMode)
{
  Q_Q(NXPasswordBox);
  m_ThemeMode       = themeMode;
  QPalette palette = q->palette();
  palette.setColor(QPalette::Text, NXThemeColor(m_ThemeMode, BasicText));
  palette.setColor(QPalette::PlaceholderText, m_ThemeMode == NXThemeType::Light ? QColor(0x00, 0x00, 0x00, 128) : QColor(0xBA, 0xBA, 0xBA));
  q->setPalette(palette);
  // Update toggle icon color for new theme
  if (m_ToggleAction) {
    if (m_IsPasswordVisible) {
      m_ToggleAction->setIcon(
          NXIcon::getInstance()->getNXIcon(NXIconType::EyeSlash, 22, 32, 32, NXThemeColor(m_ThemeMode, BasicText)));
    } else {
      m_ToggleAction->setIcon(
          NXIcon::getInstance()->getNXIcon(NXIconType::Eye, 22, 32, 32, NXThemeColor(m_ThemeMode, BasicText)));
    }
  }
}
