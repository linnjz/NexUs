#include "NXCustomWidget.h"

#include <QPainter>
#include <QVBoxLayout>

#include "NXApplication.h"
#include "NXTheme.h"
Q_TAKEOVER_NATIVEEVENT_CPP(NXCustomWidget, _appBar);

NXCustomWidget::NXCustomWidget(QWidget *parent)
    : QDialog { parent }
{
  resize(500, 500); // 默认宽高
  setObjectName("NXCustomWidget");
  setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint | Qt::WindowFullscreenButtonHint);
#if (QT_VERSION < QT_VERSION_CHECK(6, 5, 3) || QT_VERSION > QT_VERSION_CHECK(6, 6, 1))
  setStyleSheet(QStringLiteral("#NXCustomWidget{background-color:transparent;}"));
#endif
  // 自定义AppBar
  _appBar = new NXAppBar(this);
  _appBar->setWindowButtonFlags(NXAppBarType::MinimizeButtonHint | NXAppBarType::MaximizeButtonHint | NXAppBarType::CloseButtonHint);
  _mainLayout = new QVBoxLayout(this);
  _mainLayout->setContentsMargins(0, 0, 0, 0);

  _themeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    _themeMode = themeMode;
    update();
  });

  _windowDisplayMode = nxApp->getWindowDisplayMode();
  connect(nxApp, &NXApplication::pWindowDisplayModeChanged, this, [=]() {
    _windowDisplayMode = nxApp->getWindowDisplayMode();
    update();
  });
  nxApp->syncWindowDisplayMode(this);
  setAttribute(Qt::WA_DeleteOnClose);
}

NXCustomWidget::~NXCustomWidget()
{
  nxApp->syncWindowDisplayMode(this, false);
}

void NXCustomWidget::setCentralWidget(QWidget *widget)
{
  if (!widget) {
    return;
  }
  _centralWidget = widget;
  _mainLayout->addWidget(widget);
  widget->setVisible(true);
}

void NXCustomWidget::paintEvent(QPaintEvent *event)
{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 5, 3) && QT_VERSION <= QT_VERSION_CHECK(6, 6, 1))
  if (_windowDisplayMode != NXApplicationType::WindowDisplayMode::NXMica)
#else
  if (_windowDisplayMode == NXApplicationType::WindowDisplayMode::Normal)
#endif
  {
    QPainter painter(this);
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(NXThemeColor(_themeMode, WindowBase));
    painter.drawRect(rect());
    painter.restore();
  }
}
