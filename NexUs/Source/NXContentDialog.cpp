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
Q_TAKEOVER_NATIVEEVENT_CPP(NXContentDialog, d_func()->_appBar);

NXContentDialog::NXContentDialog(QWidget *parent)
    : QDialog { parent }
    , d_ptr(new NXContentDialogPrivate())
{
  Q_D(NXContentDialog);
  d->q_ptr = this;

  d->_maskWidget = new NXMaskWidget(parent);
  d->_maskWidget->move(0, 0);
  d->_maskWidget->setFixedSize(parent->size());
  d->_maskWidget->setVisible(false);

  resize(400, height());
  setWindowModality(Qt::ApplicationModal);

  d->_appBar = new NXAppBar(this);
  d->_appBar->setWindowButtonFlags(NXAppBarType::NoneButtonHint);
  d->_appBar->setIsFixedSize(true);
  d->_appBar->setAppBarHeight(0);
#ifdef Q_OS_WIN
  createWinId();
#endif

  d->_mainLayout = new QVBoxLayout(this);
  d->_mainLayout->setContentsMargins(0, 0, 0, 0);
  d->_centralWidget = new QWidget(this);
  d->_mainLayout->addWidget(d->_centralWidget);
  d->_buttonWidget = new QWidget(this);
  d->_buttonWidget->setFixedHeight(60);
  d->_buttonLayout = new QHBoxLayout(d->_buttonWidget);
  d->_mainLayout->addWidget(d->_buttonWidget);

  d->_themeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
  });
}

NXContentDialog::~NXContentDialog()
{
  Q_D(NXContentDialog);
  d->_maskWidget->deleteLater();
}

void NXContentDialog::setCentralWidget(QWidget *widget)
{
  Q_D(NXContentDialog);
  int idx = d->_mainLayout->indexOf(d->_centralWidget);
  if (idx >= 0) {
    d->_mainLayout->takeAt(idx);
  }
  delete d->_centralWidget;
  d->_centralWidget = widget;
  d->_mainLayout->insertWidget(idx < 0 ? 0 : idx, widget);
}

NXPushButton * NXContentDialog::addButton(const QString &text)
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

  d->_buttons.append(btn);
  d->_buttonLayout->addWidget(btn);
  return btn;
}

void NXContentDialog::removeButton(NXPushButton *button)
{
  Q_D(NXContentDialog);
  if (d->_buttons.removeOne(button)) {
    d->_buttonLayout->removeWidget(button);
    button->deleteLater();
  }
}

QList<NXPushButton *> NXContentDialog::buttons() const
{
  Q_D(const NXContentDialog);
  return d->_buttons;
}

void NXContentDialog::close()
{
  Q_D(NXContentDialog);
  d->startCloseAnimation(nullptr);
}

NXAppBar * NXContentDialog::appBar() const
{
  Q_D(const NXContentDialog);
  return d->_appBar;
}

void NXContentDialog::showEvent(QShowEvent *event)
{
  Q_D(NXContentDialog);
  d->_maskWidget->setVisible(true);
  d->_maskWidget->raise();
  d->_maskWidget->setFixedSize(parentWidget()->size());
  d->_maskWidget->doMaskAnimation(90);
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
  painter.setBrush(NXThemeColor(d->_themeMode, DialogBase));
  painter.drawRect(rect());
  // 按钮栏背景绘制
  painter.setBrush(NXThemeColor(d->_themeMode, DialogLayoutArea));
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
