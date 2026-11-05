#include "T_BasePage.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "NXMenu.h"
#include "NXPushButton.h"
#include "NXText.h"
#include "NXTheme.h"
#include "NXToolButton.h"
#include "NXWindow.h"

T_BasePage::T_BasePage(QWidget *parent)
    : NXScrollPage(parent)
{
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=]() {
    if (!parent) {
      update();
    }
  });
  setContentsMargins(20, 5, 0, 0);
}

T_BasePage::~T_BasePage()
{
}

void T_BasePage::createCustomWidget(QString const &desText)
{
  // 顶部元素
  QWidget *customWidget = new QWidget(this);
  NXText *subTitleText  = new NXText(this);
  subTitleText->setText(QStringLiteral("https://github.com/RainbowCandyX/NexUs"));
  subTitleText->setTextInteractionFlags(Qt::TextSelectableByMouse);
  subTitleText->setTextPixelSize(11);

  NXToolButton *documentationButton = new NXToolButton(this);
  documentationButton->setFixedHeight(35);
  documentationButton->setIsTransparent(false);
  documentationButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  //_toolButton->setPopupMode(QToolButton::MenuButtonPopup);
  documentationButton->setText(QStringLiteral("Documentation"));
  documentationButton->setNXIcon(NXIconType::FileDoc);
  NXMenu *documentationMenu = new NXMenu(this);
  documentationMenu->addNXIconAction(NXIconType::CardsBlank, QStringLiteral("CardsBlank"));
  documentationMenu->addNXIconAction(NXIconType::EarthAmericas, QStringLiteral("EarthAmericas"));
  documentationButton->setMenu(documentationMenu);

  NXToolButton *sourceButton = new NXToolButton(this);
  sourceButton->setFixedHeight(35);
  sourceButton->setIsTransparent(false);
  sourceButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  sourceButton->setText(QStringLiteral("Source"));
  sourceButton->setNXIcon(NXIconType::NfcSymbol);
  NXMenu *sourceMenu = new NXMenu(this);
  sourceMenu->addNXIconAction(NXIconType::FireBurner, QStringLiteral("FireBurner"));
  sourceMenu->addNXIconAction(NXIconType::Galaxy, QStringLiteral("Galaxy~~~~"));
  sourceButton->setMenu(sourceMenu);

  NXToolButton *themeButton = new NXToolButton(this);
  themeButton->setFixedSize(35, 35);
  themeButton->setIsTransparent(false);
  themeButton->setNXIcon(NXIconType::MoonStars);
  connect(themeButton, &NXToolButton::clicked, this, [=]() {
    nxTheme->setThemeMode(nxTheme->getThemeMode() == NXThemeType::Light ? NXThemeType::Dark : NXThemeType::Light);
  });

  NXToolButton *backtrackButton = new NXToolButton(this);
  backtrackButton->setFixedSize(35, 35);
  backtrackButton->setIsTransparent(false);
  backtrackButton->setNXIcon(NXIconType::Timer);
  connect(backtrackButton, &NXToolButton::clicked, this, [=]() {
    NXWindow *window = dynamic_cast<NXWindow *>(this->window());
    if (window) {
      window->backtrackNavigationNode(property("NXPageKey").toString());
    }
  });

  NXPushButton *iconButton = new NXPushButton(QStringLiteral("带图标按钮"), this);
  iconButton->setNXIcon(NXIconType::Heart, 16);
  iconButton->setIsHoverEnable(false);
  iconButton->setFixedSize(130, 38);

  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->addWidget(documentationButton);
  buttonLayout->addSpacing(5);
  buttonLayout->addWidget(sourceButton);
  buttonLayout->addSpacing(5);
  buttonLayout->addWidget(iconButton);
  buttonLayout->addStretch();
  buttonLayout->addWidget(themeButton);
  buttonLayout->addSpacing(5);
  buttonLayout->addWidget(backtrackButton);
  buttonLayout->addSpacing(15);

  NXText *descText = new NXText(this);
  descText->setText(desText);
  descText->setTextPixelSize(13);

  QVBoxLayout *topLayout = new QVBoxLayout(customWidget);
  topLayout->setContentsMargins(0, 0, 0, 0);
  topLayout->addWidget(subTitleText);
  topLayout->addSpacing(5);
  topLayout->addLayout(buttonLayout);
  topLayout->addSpacing(5);
  topLayout->addWidget(descText);
  setTopCustomWidget(customWidget);
}
