#include "NXCustomWidget.h"

#include <QPainter>
#include <QVBoxLayout>

#include "NXApplication.h"
#include "NXTheme.h"
Q_TAKEOVER_NATIVEEVENT_CPP(NXCustomWidget, m_AppBar);

NXCustomWidget::NXCustomWidget(QWidget *parent)
    : QDialog { parent }
    , m_MainLayout { new QVBoxLayout(this) }
    , m_ThemeMode { nxTheme->getThemeMode() }
    , m_WindowDisplayMode { nxApp->getWindowDisplayMode() }
{
  resize(500, 500); // 默认宽高
  setObjectName("NXCustomWidget");
  setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint | Qt::WindowFullscreenButtonHint);
#if (QT_VERSION < QT_VERSION_CHECK(6, 5, 3) || QT_VERSION > QT_VERSION_CHECK(6, 6, 1))
  setStyleSheet(QStringLiteral("#NXCustomWidget{background-color:transparent;}"));
#endif
  // 自定义AppBar
  m_AppBar = new NXAppBar(this);
  m_AppBar->setWindowButtonFlags(NXAppBarType::MinimizeButtonHint | NXAppBarType::MaximizeButtonHint | NXAppBarType::CloseButtonHint);
  m_MainLayout->setContentsMargins(0, 0, 0, 0);

  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    m_ThemeMode = themeMode;
    update();
  });

  connect(nxApp, &NXApplication::signalWindowDisplayModeChanged, this, [=]() {
    m_WindowDisplayMode = nxApp->getWindowDisplayMode();
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
  m_CentralWidget = widget;
  m_MainLayout->addWidget(widget);
  widget->setVisible(true);
}

void NXCustomWidget::paintEvent(QPaintEvent *event)
{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 5, 3) && QT_VERSION <= QT_VERSION_CHECK(6, 6, 1))
  if (m_WindowDisplayMode != NXApplicationType::WindowDisplayMode::NXMica)
#else
  if (m_WindowDisplayMode == NXApplicationType::WindowDisplayMode::Normal)
#endif
  {
    QPainter painter(this);
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(NXThemeColor(m_ThemeMode, WindowBase));
    painter.drawRect(rect());
    painter.restore();
  }
}
