#include "NXDockWidgetTitleBar.h"

#include <QHBoxLayout>
#include <QPainter>

#include "NXDockWidget.h"
#include "NXIconButton.h"
#include "NXText.h"
#include "NXTheme.h"

NXDockWidgetTitleBar::NXDockWidgetTitleBar(QWidget *parent)
    : QWidget { parent }
{
  _dockWidget = dynamic_cast<NXDockWidget *>(parent);
  _iconLabel  = new QLabel(this);
  _iconLabel->setPixmap(_dockWidget->windowIcon().pixmap(QSize(18, 18)));
  _titleLabel = new NXText(_dockWidget->windowTitle(), this);
  _titleLabel->setWordWrap(false);
  _titleLabel->setTextPixelSize(13);

  _floatButton = new NXIconButton(NXIconType::WindowRestore, 13, 32, 26, this);
  _floatButton->setLightHoverColor(NXThemeColor(NXThemeType::Light, BasicHoverAlpha));
  _floatButton->setDarkHoverColor(NXThemeColor(NXThemeType::Dark, BasicHoverAlpha));
  connect(_floatButton, &NXIconButton::clicked, this, &NXDockWidgetTitleBar::onFloatButtonClicked);
  _closeButton = new NXIconButton(NXIconType::Xmark, 17, 32, 26, this);
  _closeButton->setLightHoverColor(NXThemeColor(NXThemeType::Light, StatusDanger));
  _closeButton->setDarkHoverColor(NXThemeColor(NXThemeType::Dark, StatusDanger));
  connect(_closeButton, &NXIconButton::clicked, this, &NXDockWidgetTitleBar::onCloseButtonClicked);

  _setVisibleFromFeatures(_dockWidget->features());
  connect(_dockWidget, &QDockWidget::featuresChanged, this, [=](QDockWidget::DockWidgetFeatures features) {
    _setVisibleFromFeatures(features);
  });
  connect(_dockWidget, &QDockWidget::windowTitleChanged, this, [=](const QString &title) {
    _titleLabel->setText(title);
  });
  connect(_dockWidget, &QDockWidget::windowIconChanged, this, [=](const QIcon &icon) {
    _iconLabel->setPixmap(icon.pixmap(QSize(18, 18)));
  });

  QHBoxLayout *mainLayout = new QHBoxLayout(this);
  mainLayout->setSpacing(0);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->addSpacing(5);
  mainLayout->addWidget(_iconLabel);
  mainLayout->addSpacing(10);
  mainLayout->addWidget(_titleLabel);
  mainLayout->addSpacing(10);
  mainLayout->addStretch();
  mainLayout->addWidget(_floatButton);
  mainLayout->addWidget(_closeButton);

  //主题变更
  _themeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    _themeMode = themeMode;
  });
}

NXDockWidgetTitleBar::~NXDockWidgetTitleBar()
{
}

void NXDockWidgetTitleBar::onFloatButtonClicked()
{
  _dockWidget->setFloating(_dockWidget->isFloating() ? false : true);
}

void NXDockWidgetTitleBar::onCloseButtonClicked()
{
  _dockWidget->close();
}

void NXDockWidgetTitleBar::paintEvent(QPaintEvent *event)
{
  QPainter painter(this);
  painter.save();
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setPen(Qt::NoPen);
  painter.setBrush(_dockWidget->isFloating() ? Qt::transparent : NXThemeColor(_themeMode, BasicBaseAlpha));
  painter.drawRect(rect());
  painter.restore();
}

void NXDockWidgetTitleBar::_setVisibleFromFeatures(QDockWidget::DockWidgetFeatures features)
{
  _floatButton->setVisible(features.testFlag(QDockWidget::DockWidgetFloatable));
  _closeButton->setVisible(features.testFlag(QDockWidget::DockWidgetClosable));
}
