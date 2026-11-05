#include "NXLineEdit.h"

#include <QClipboard>
#include <QContextMenuEvent>
#include <QGuiApplication>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>

#include "DeveloperComponents/NXLineEditStyle.h"
#include "NXEventBus.h"
#include "NXMenu.h"
#include "NXTheme.h"
#include "private/NXLineEditPrivate.h"

NXLineEdit::NXLineEdit(QWidget *parent)
    : QLineEdit(parent)
    , d_ptr(new NXLineEditPrivate())
{
  Q_D(NXLineEdit);
  d->q_ptr = this;
  setObjectName("NXLineEdit");
  setFixedHeight(35);
  d->m_ThemeMode            = nxTheme->getThemeMode();
  d->m_ExpandMarkWidth     = 0;
  d->m_IsClearButtonEnable = true;
  setFocusPolicy(Qt::StrongFocus);
  // 事件总线
  d->m_FocusEvent = new NXEvent(QStringLiteral("WMWindowClicked"), QStringLiteral("onWMWindowClickedEvent"), d);
  d->m_FocusEvent->registerAndInit();
  setMouseTracking(true);
  QFont textFont = font();
  textFont.setLetterSpacing(QFont::AbsoluteSpacing, d->m_TextSpacing);
  setFont(textFont);
  d->m_LineEditStyle = new NXLineEditStyle(style());
  setStyle(d->m_LineEditStyle);
  setStyleSheet(QStringLiteral("#NXLineEdit{background-color:transparent;}"));
  d->onThemeChanged(nxTheme->getThemeMode());
  connect(nxTheme, &NXTheme::themeModeChanged, d, &NXLineEditPrivate::onThemeChanged);
  QWidget::setVisible(true);
}

NXLineEdit::NXLineEdit(QString const &text, QWidget *parent)
    : NXLineEdit(parent)
{
  Q_D(NXLineEdit);
  setText(text);
}

NXLineEdit::~NXLineEdit()
{
  delete this->style();
}

void NXLineEdit::setContentsMargins(QMargins margins)
{
  Q_D(NXLineEdit);
  d->m_LineEditStyle->setContentsMargins(margins);
  update();
  Q_EMIT signalContentsMarginsChanged();
}

QMargins NXLineEdit::getContentsMargins() const noexcept(std::is_nothrow_copy_constructible_v<QMargins>)
{
  Q_CONST_D(NXLineEdit);
  return d->m_LineEditStyle->getContentsMargins();
}

void NXLineEdit::setIsClearButtonEnable(bool isClearButtonEnable)
{
  Q_D(NXLineEdit);
  d->m_IsClearButtonEnable = isClearButtonEnable;
  setClearButtonEnabled(isClearButtonEnable);
  Q_EMIT signalIsClearButtonEnableChanged();
}

bool NXLineEdit::getIsClearButtonEnable() const noexcept(std::is_nothrow_copy_constructible_v<bool>)
{
  Q_CONST_D(NXLineEdit);
  return d->m_IsClearButtonEnable;
}

void NXLineEdit::setBorderRadius(int borderRadius)
{
  Q_CONST_D(NXLineEdit);
  d->m_LineEditStyle->setBorderRadius(borderRadius);
}

int NXLineEdit::getBorderRadius() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXLineEdit);
  return d->m_LineEditStyle->getBorderRadius();
}

void NXLineEdit::setIconMargin(int margin)
{
  Q_D(NXLineEdit);
  d->m_LineEditStyle->setIconMargin(margin);
}

int NXLineEdit::getIconMargin() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXLineEdit);
  return d->m_LineEditStyle->getIconMargin();
}

void NXLineEdit::focusInEvent(QFocusEvent *event)
{
  Q_D(NXLineEdit);
  Q_EMIT focusIn(this->text());
  if (event->reason() != Qt::PopupFocusReason) {
    if (d->m_IsClearButtonEnable) {
      setClearButtonEnabled(true);
    }
    QPropertyAnimation *markAnimation = new QPropertyAnimation(d, "ExpandMarkWidth");
    connect(markAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
      update();
    });
    markAnimation->setDuration(300);
    markAnimation->setEasingCurve(QEasingCurve::InOutSine);
    markAnimation->setStartValue(d->m_ExpandMarkWidth);
    markAnimation->setEndValue(width() / 2 - d->m_LineEditStyle->getBorderRadius() / 2);
    markAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  }
  QLineEdit::focusInEvent(event);
}

void NXLineEdit::focusOutEvent(QFocusEvent *event)
{
  Q_D(NXLineEdit);
  Q_EMIT focusOut(this->text());
  if (event->reason() != Qt::PopupFocusReason) {
    if (d->m_IsClearButtonEnable) {
      setClearButtonEnabled(false);
    }
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

void NXLineEdit::paintEvent(QPaintEvent *event)
{
  Q_D(NXLineEdit);
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

void NXLineEdit::contextMenuEvent(QContextMenuEvent *event)
{
  NXMenu *menu = new NXMenu(this);
  menu->setMenuItemHeight(27);
  menu->setAttribute(Qt::WA_DeleteOnClose);
  QAction *action = nullptr;
  if (!isReadOnly()) {
    action = menu->addNXIconAction(NXIconType::ArrowRotateLeft, tr("撤销"), QKeySequence::Undo);
    action->setEnabled(isUndoAvailable());
    connect(action, &QAction::triggered, this, &NXLineEdit::undo);

    action = menu->addNXIconAction(NXIconType::ArrowRotateRight, tr("恢复"), QKeySequence::Redo);
    action->setEnabled(isRedoAvailable());
    connect(action, &QAction::triggered, this, &NXLineEdit::redo);
    menu->addSeparator();
  }
#ifndef QT_NO_CLIPBOARD
  if (!isReadOnly()) {
    action = menu->addNXIconAction(NXIconType::KnifeKitchen, tr("剪切"), QKeySequence::Cut);
    action->setEnabled(!isReadOnly() && hasSelectedText() && echoMode() == QLineEdit::Normal);
    connect(action, &QAction::triggered, this, &NXLineEdit::cut);
  }

  action = menu->addNXIconAction(NXIconType::Copy, tr("复制"), QKeySequence::Copy);
  action->setEnabled(hasSelectedText() && echoMode() == QLineEdit::Normal);
  connect(action, &QAction::triggered, this, &NXLineEdit::copy);

  if (!isReadOnly()) {
    action = menu->addNXIconAction(NXIconType::Paste, tr("粘贴"), QKeySequence::Paste);
    action->setEnabled(!isReadOnly() && !QGuiApplication::clipboard()->text().isEmpty());
    connect(action, &QAction::triggered, this, &NXLineEdit::paste);
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
  connect(action, &QAction::triggered, this, &NXLineEdit::selectAll);
  menu->popup(event->globalPos());
}
