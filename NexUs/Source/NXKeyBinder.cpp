#include <QMouseEvent>
#include "NXKeyBinder.h"

#include <NXPushButton.h>
#include <utility>

#include <QPainter>

#include "NXApplication.h"
#include "DeveloperComponents/NXKeyBinderContainer.h"
#include "NXContentDialog.h"
#include "NXTheme.h"
#include "private/NXKeyBinderPrivate.h"
Q_PROPERTY_CREATE_CPP(NXKeyBinder, int, BorderRadius)

NXKeyBinder::NXKeyBinder(QWidget *parent)
    : QLabel(parent)
    , d_ptr(new NXKeyBinderPrivate())
{
  Q_D(NXKeyBinder);
  d->q_ptr          = this;
  d->_pBorderRadius = 5;
  setFixedHeight(35);
  setMouseTracking(true);
  setStyleSheet(QStringLiteral("#NXKeyBinder{background-color:transparent;}"));
  QFont textFont = font();
  textFont.setLetterSpacing(QFont::AbsoluteSpacing, 0.5);
  textFont.setPixelSize(nxApp->getFontPixelSize() + 2);
  setFont(textFont);
  d->_binderContainer = new NXKeyBinderContainer(this);
  setText(QStringLiteral("  按键: 未绑定      "));
  d->_binderDialog = new NXContentDialog(window());
  d->_binderDialog->setCentralWidget(d->_binderContainer);
  auto *cancelBtn  = d->_binderDialog->addButton(QStringLiteral("取消"));
  auto *resetBtn   = d->_binderDialog->addButton(QStringLiteral("重置"));
  auto *confirmBtn = d->_binderDialog->addButton(QStringLiteral("确认"));

  confirmBtn->setLightDefaultColor(NXThemeColor(NXThemeType::Light, PrimaryNormal));
  confirmBtn->setLightHoverColor(NXThemeColor(NXThemeType::Light, PrimaryHover));
  confirmBtn->setLightPressColor(NXThemeColor(NXThemeType::Light, PrimaryPress));
  confirmBtn->setLightTextColor(Qt::white);
  confirmBtn->setDarkDefaultColor(NXThemeColor(NXThemeType::Dark, PrimaryNormal));
  confirmBtn->setDarkHoverColor(NXThemeColor(NXThemeType::Dark, PrimaryHover));
  confirmBtn->setDarkPressColor(NXThemeColor(NXThemeType::Dark, PrimaryPress));
  confirmBtn->setDarkTextColor(Qt::black);

  connect(d->_binderDialog, &NXContentDialog::buttonClicked, this, [=](NXPushButton *btn) {
    if (btn == resetBtn) {
      d->_binderContainer->logOrResetHistoryData(false);
    } else if (btn == confirmBtn) {
      d->_binderContainer->saveBinderChanged();
    }
  });
  d->onThemeChanged(nxTheme->getThemeMode());
  connect(nxTheme, &NXTheme::themeModeChanged, d, &NXKeyBinderPrivate::onThemeChanged);
}

NXKeyBinder::~NXKeyBinder()
{
}

void NXKeyBinder::setBinderKeyText(const QString &binderKeyText)
{
  Q_D(NXKeyBinder);
  d->_binderContainer->setBinderKeyText(binderKeyText);
  setText(QStringLiteral("  按键: ") + binderKeyText + QStringLiteral("      "));
}

QString NXKeyBinder::getBinderKeyText() const
{
  Q_D(const NXKeyBinder);
  return d->_binderContainer->getBinderKeyText();
}

void NXKeyBinder::setNativeVirtualBinderKey(quint32 binderKey)
{
  Q_D(NXKeyBinder);
  d->_binderContainer->setNativeVirtualBinderKey(binderKey);
}

quint32 NXKeyBinder::getNativeVirtualBinderKey() const
{
  Q_D(const NXKeyBinder);
  return d->_binderContainer->getNativeVirtualBinderKey();
}

bool NXKeyBinder::event(QEvent *event)
{
  switch (event->type()) {
  case QEvent::Enter :
  case QEvent::Leave : {
    update();
    break;
  }
  default : {
    break;
  }
  }
  return QLabel::event(event);
}

void NXKeyBinder::mouseReleaseEvent(QMouseEvent *event)
{
  Q_D(NXKeyBinder);
  if (event->button() == Qt::LeftButton) {
    d->_binderDialog->show();
    d->_binderContainer->setFocus();
    d->_binderContainer->logOrResetHistoryData(true);
  }
  QLabel::mouseReleaseEvent(event);
}

void NXKeyBinder::paintEvent(QPaintEvent *event)
{
  Q_D(NXKeyBinder);
  if (palette().color(QPalette::WindowText) != NXThemeColor(d->_themeMode, BasicText)) {
    d->onThemeChanged(d->_themeMode);
  }
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  painter.setPen(NXThemeColor(d->_themeMode, BasicBorder));
  painter.setBrush(underMouse() ? NXThemeColor(d->_themeMode, BasicHover) : NXThemeColor(d->_themeMode, BasicBase));
  QRect borderRect = rect();
  borderRect.adjust(1, 1, -1, -1);
  painter.drawRoundedRect(borderRect, d->_pBorderRadius, d->_pBorderRadius);
  // 图标绘制
  QFont iconFont = QFont(QStringLiteral("NXAwesome"));
  iconFont.setPixelSize(nxApp->getFontPixelSize() + 3);
  painter.setFont(iconFont);
  painter.setPen(NXThemeColor(d->_themeMode, BasicText));
  QRect iconRect = rect();
  iconRect.adjust(0, 0, -10, 0);
  painter.drawText(iconRect, Qt::AlignVCenter | Qt::AlignRight, QChar((unsigned short) NXIconType::Pencil));
  painter.restore();
  QLabel::paintEvent(event);
}
