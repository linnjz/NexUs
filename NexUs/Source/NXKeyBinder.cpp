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
Q_PROPERTY_CREATE_CPP(NXKeyBinder, QPS_PAEMIT(int), BorderRadius)

NXKeyBinder::NXKeyBinder(QWidget *parent)
    : QLabel(parent)
    , d_ptr(new NXKeyBinderPrivate())
{
  Q_D(NXKeyBinder);
  d->q_ptr          = this;
  d->m_BorderRadius = 5;
  setFixedHeight(35);
  setMouseTracking(true);
  setStyleSheet(QStringLiteral("#NXKeyBinder{background-color:transparent;}"));
  QFont textFont = font();
  textFont.setLetterSpacing(QFont::AbsoluteSpacing, 0.5);
  textFont.setPixelSize(nxApp->getFontPixelSize() + 2);
  setFont(textFont);
  d->m_BinderContainer = new NXKeyBinderContainer(this);
  setText(QStringLiteral("  按键: 未绑定      "));
  d->m_BinderDialog = new NXContentDialog(window());
  d->m_BinderDialog->setCentralWidget(d->m_BinderContainer);
  auto *cancelBtn  = d->m_BinderDialog->addButton(QStringLiteral("取消"));
  auto *resetBtn   = d->m_BinderDialog->addButton(QStringLiteral("重置"));
  auto *confirmBtn = d->m_BinderDialog->addButton(QStringLiteral("确认"));

  confirmBtn->setLightDefaultColor(NXThemeColor(NXThemeType::Light, PrimaryNormal));
  confirmBtn->setLightHoverColor(NXThemeColor(NXThemeType::Light, PrimaryHover));
  confirmBtn->setLightPressColor(NXThemeColor(NXThemeType::Light, PrimaryPress));
  confirmBtn->setLightTextColor(Qt::white);
  confirmBtn->setDarkDefaultColor(NXThemeColor(NXThemeType::Dark, PrimaryNormal));
  confirmBtn->setDarkHoverColor(NXThemeColor(NXThemeType::Dark, PrimaryHover));
  confirmBtn->setDarkPressColor(NXThemeColor(NXThemeType::Dark, PrimaryPress));
  confirmBtn->setDarkTextColor(Qt::black);

  connect(d->m_BinderDialog, &NXContentDialog::buttonClicked, this, [=](NXPushButton *btn) {
    if (btn == resetBtn) {
      d->m_BinderContainer->logOrResetHistoryData(false);
    } else if (btn == confirmBtn) {
      d->m_BinderContainer->saveBinderChanged();
    }
  });
  d->onThemeChanged(nxTheme->getThemeMode());
  connect(nxTheme, &NXTheme::themeModeChanged, d, &NXKeyBinderPrivate::onThemeChanged);
}

NXKeyBinder::~NXKeyBinder()
{
}

void NXKeyBinder::setBinderKeyText(QString const &binderKeyText)
{
  Q_D(NXKeyBinder);
  d->m_BinderContainer->setBinderKeyText(binderKeyText);
  setText(QStringLiteral("  按键: ") + binderKeyText + QStringLiteral("      "));
}

QString NXKeyBinder::getBinderKeyText() const noexcept(std::is_nothrow_copy_constructible_v<QString>)
{
  Q_CONST_D(NXKeyBinder);
  return d->m_BinderContainer->getBinderKeyText();
}

void NXKeyBinder::setNativeVirtualBinderKey(quint32 binderKey)
{
  Q_D(NXKeyBinder);
  d->m_BinderContainer->setNativeVirtualBinderKey(binderKey);
}

quint32 NXKeyBinder::getNativeVirtualBinderKey() const noexcept(std::is_nothrow_copy_constructible_v<quint32>)
{
  Q_CONST_D(NXKeyBinder);
  return d->m_BinderContainer->getNativeVirtualBinderKey();
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
    d->m_BinderDialog->show();
    d->m_BinderContainer->setFocus();
    d->m_BinderContainer->logOrResetHistoryData(true);
  }
  QLabel::mouseReleaseEvent(event);
}

void NXKeyBinder::paintEvent(QPaintEvent *event)
{
  Q_D(NXKeyBinder);
  if (palette().color(QPalette::WindowText) != NXThemeColor(d->m_ThemeMode, BasicText)) {
    d->onThemeChanged(d->m_ThemeMode);
  }
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  painter.setPen(NXThemeColor(d->m_ThemeMode, BasicBorder));
  painter.setBrush(underMouse() ? NXThemeColor(d->m_ThemeMode, BasicHover) : NXThemeColor(d->m_ThemeMode, BasicBase));
  QRect borderRect = rect();
  borderRect.adjust(1, 1, -1, -1);
  painter.drawRoundedRect(borderRect, d->m_BorderRadius, d->m_BorderRadius);
  // 图标绘制
  QFont iconFont = QFont(QStringLiteral("NXAwesome"));
  iconFont.setPixelSize(nxApp->getFontPixelSize() + 3);
  painter.setFont(iconFont);
  painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
  QRect iconRect = rect();
  iconRect.adjust(0, 0, -10, 0);
  painter.drawText(iconRect, Qt::AlignVCenter | Qt::AlignRight, QChar((unsigned short) NXIconType::Pencil));
  painter.restore();
  QLabel::paintEvent(event);
}
