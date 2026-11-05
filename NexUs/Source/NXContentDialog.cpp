#include "NXContentDialog.h"

#include <NXPushButton.h>

#include <QApplication>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QPainter>
#include <QScreen>
#include <QVBoxLayout>

#include "DeveloperComponents/NXMaskWidget.h"
#include "DeveloperComponents/NXWinShadowHelper.h"
#include "NXTheme.h"
#include "private/NXContentDialogPrivate.h"
Q_TAKEOVER_NATIVEEVENT_CPP(NXContentDialog, d_func()->m_AppBar);

NXContentDialog::NXContentDialog(QWidget *parent)
    : QDialog { parent }
    , d_ptr(new NXContentDialogPrivate())
{
  Q_D(NXContentDialog);
  d->q_ptr = this;

  d->m_MaskWidget = new NXMaskWidget(parent);
  d->m_MaskWidget->move(0, 0);
  d->m_MaskWidget->setFixedSize(parent->size());
  d->m_MaskWidget->setVisible(false);

  resize(400, height());
  setWindowModality(Qt::ApplicationModal);

  d->m_AppBar = new NXAppBar(this);
  d->m_AppBar->setWindowButtonFlags(NXAppBarType::NoneButtonHint);
  d->m_AppBar->setIsFixedSize(true);
  d->m_AppBar->setAppBarHeight(0);
#ifdef Q_OS_WIN
  createWinId();
#endif

  d->m_MainLayout = new QVBoxLayout(this);
  d->m_MainLayout->setContentsMargins(0, 0, 0, 0);
  d->m_CentralWidget = new QWidget(this);
  d->m_MainLayout->addWidget(d->m_CentralWidget);
  d->m_ButtonWidget = new QWidget(this);
  d->m_ButtonWidget->setFixedHeight(60);
  d->m_ButtonLayout = new QHBoxLayout(d->m_ButtonWidget);
  d->m_MainLayout->addWidget(d->m_ButtonWidget);

  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
  });
}

NXContentDialog::~NXContentDialog()
{
  Q_D(NXContentDialog);
  d->m_MaskWidget->deleteLater();
}

void NXContentDialog::setCentralWidget(QWidget *widget)
{
  Q_D(NXContentDialog);
  int idx = d->m_MainLayout->indexOf(d->m_CentralWidget);
  if (idx >= 0) {
    d->m_MainLayout->takeAt(idx);
  }
  delete d->m_CentralWidget;
  d->m_CentralWidget = widget;
  d->m_MainLayout->insertWidget(idx < 0 ? 0 : idx, widget);
}

NXPushButton *NXContentDialog::addButton(QString const &text)
{
  Q_D(NXContentDialog);
  auto *btn = new NXPushButton(text, this);
  btn->setMinimumSize(0, 0);
  btn->setMaximumSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX));
  btn->setFixedHeight(38);
  btn->setBorderRadius(6);

  connect(btn, &NXPushButton::clicked, this, [=]() {
    d->startCloseAnimation(btn);
  });

  d->m_Buttons.append(btn);
  d->m_ButtonLayout->addWidget(btn);
  return btn;
}

void NXContentDialog::removeButton(NXPushButton *button)
{
  Q_D(NXContentDialog);
  if (d->m_Buttons.removeOne(button)) {
    d->m_ButtonLayout->removeWidget(button);
    button->deleteLater();
  }
}

QList<NXPushButton *> NXContentDialog::buttons() const
{
  Q_CONST_D(NXContentDialog);
  return d->m_Buttons;
}

void NXContentDialog::close()
{
  Q_D(NXContentDialog);
  d->startCloseAnimation(nullptr);
}

NXAppBar *NXContentDialog::appBar() const
{
  Q_CONST_D(NXContentDialog);
  return d->m_AppBar;
}

void NXContentDialog::showEvent(QShowEvent *event)
{
  Q_D(NXContentDialog);
  d->m_MaskWidget->setVisible(true);
  d->m_MaskWidget->raise();
  d->m_MaskWidget->setFixedSize(parentWidget()->size());
  d->m_MaskWidget->doMaskAnimation(90);
  d->_moveToCenter();
  QDialog::showEvent(event);
}

void NXContentDialog::paintEvent(QPaintEvent *event)
{
  Q_D(NXContentDialog);
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  painter.setPen(Qt::NoPen);
  // 背景绘制
  painter.setBrush(NXThemeColor(d->m_ThemeMode, DialogBase));
  painter.drawRect(rect());
  // 按钮栏背景绘制
  painter.setBrush(NXThemeColor(d->m_ThemeMode, DialogLayoutArea));
  painter.drawRoundedRect(QRectF(0, height() - 60, width(), 60), 8, 8);
  painter.restore();
}

void NXContentDialog::keyPressEvent(QKeyEvent *event)
{
  Q_D(NXContentDialog);
  switch (event->key()) {
  case Qt::Key_Escape : {
    d->startCloseAnimation(nullptr);
    break;
  }
  default : {
    break;
  }
  }
  event->accept();
}
