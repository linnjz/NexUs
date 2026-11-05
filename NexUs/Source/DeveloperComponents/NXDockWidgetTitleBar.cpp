#include "NXDockWidgetTitleBar.h"

#include <QHBoxLayout>
#include <QPainter>

#include "NXDockWidget.h"
#include "NXIconButton.h"
#include "NXText.h"
#include "NXTheme.h"

NXDockWidgetTitleBar::NXDockWidgetTitleBar(QWidget *parent)
    : QWidget { parent }
    , m_ThemeMode { nxTheme->getThemeMode() }
    , m_DockWidget { dynamic_cast<NXDockWidget *>(parent) }
    , m_IconLabel { new QLabel(this) }
    , m_TitleLabel { new NXText(m_DockWidget->windowTitle(), this) }
    , m_FloatButton { new NXIconButton(NXIconType::WindowRestore, 13, 32, 26, this) }
    , m_CloseButton { new NXIconButton(NXIconType::Xmark, 17, 32, 26, this) }
{
  m_IconLabel->setPixmap(m_DockWidget->windowIcon().pixmap(QSize(18, 18)));
  m_TitleLabel->setWordWrap(false);
  m_TitleLabel->setTextPixelSize(13);

  m_FloatButton->setLightHoverColor(NXThemeColor(NXThemeType::Light, BasicHoverAlpha));
  m_FloatButton->setDarkHoverColor(NXThemeColor(NXThemeType::Dark, BasicHoverAlpha));
  connect(m_FloatButton, &NXIconButton::clicked, this, &NXDockWidgetTitleBar::onFloatButtonClicked);

  m_CloseButton->setLightHoverColor(NXThemeColor(NXThemeType::Light, StatusDanger));
  m_CloseButton->setDarkHoverColor(NXThemeColor(NXThemeType::Dark, StatusDanger));
  connect(m_CloseButton, &NXIconButton::clicked, this, &NXDockWidgetTitleBar::onCloseButtonClicked);

  _setVisibleFromFeatures(m_DockWidget->features());
  connect(m_DockWidget, &QDockWidget::featuresChanged, this, [=](QDockWidget::DockWidgetFeatures features) {
    _setVisibleFromFeatures(features);
  });
  connect(m_DockWidget, &QDockWidget::windowTitleChanged, this, [=](QString const &title) {
    m_TitleLabel->setText(title);
  });
  connect(m_DockWidget, &QDockWidget::windowIconChanged, this, [=](QIcon const &icon) {
    m_IconLabel->setPixmap(icon.pixmap(QSize(18, 18)));
  });

  QHBoxLayout *mainLayout = new QHBoxLayout(this);
  mainLayout->setSpacing(0);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->addSpacing(5);
  mainLayout->addWidget(m_IconLabel);
  mainLayout->addSpacing(10);
  mainLayout->addWidget(m_TitleLabel);
  mainLayout->addSpacing(10);
  mainLayout->addStretch();
  mainLayout->addWidget(m_FloatButton);
  mainLayout->addWidget(m_CloseButton);

  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    m_ThemeMode = themeMode;
  });
}

NXDockWidgetTitleBar::~NXDockWidgetTitleBar()
{
}

void NXDockWidgetTitleBar::onFloatButtonClicked()
{
  m_DockWidget->setFloating(m_DockWidget->isFloating() ? false : true);
}

void NXDockWidgetTitleBar::onCloseButtonClicked()
{
  m_DockWidget->close();
}

void NXDockWidgetTitleBar::paintEvent(QPaintEvent *event)
{
  QPainter painter(this);
  painter.save();
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setPen(Qt::NoPen);
  painter.setBrush(m_DockWidget->isFloating() ? Qt::transparent : NXThemeColor(m_ThemeMode, BasicBaseAlpha));
  painter.drawRect(rect());
  painter.restore();
}

void NXDockWidgetTitleBar::_setVisibleFromFeatures(QDockWidget::DockWidgetFeatures features)
{
  m_FloatButton->setVisible(features.testFlag(QDockWidget::DockWidgetFloatable));
  m_CloseButton->setVisible(features.testFlag(QDockWidget::DockWidgetClosable));
}
