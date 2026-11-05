#include "T_Setting.h"

#include <QButtonGroup>
#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "NXApplication.h"
#include "NXComboBox.h"
#include "NXLog.h"
#include "NXRadioButton.h"
#include "NXScrollPageArea.h"
#include "NXText.h"
#include "NXTheme.h"
#include "NXToggleSwitch.h"
#include "NXWindow.h"

T_Setting::T_Setting(QWidget *parent)
    : T_BasePage(parent)
{
  // 预览窗口标题
  NXWindow *window = dynamic_cast<NXWindow *>(parent);
  setWindowTitle(QStringLiteral("Setting"));

  NXText *themeText = new NXText(QStringLiteral("主题设置"), this);
  themeText->setWordWrap(false);
  themeText->setTextPixelSize(18);

  _themeComboBox = new NXComboBox(this);
  _themeComboBox->addItem(QStringLiteral("跟随系统"));
  _themeComboBox->addItem(QStringLiteral("日间模式"));
  _themeComboBox->addItem(QStringLiteral("夜间模式"));
  NXScrollPageArea *themeSwitchArea = new NXScrollPageArea(this);
  QHBoxLayout *themeSwitchLayout    = new QHBoxLayout(themeSwitchArea);
  NXText *themeSwitchText           = new NXText(QStringLiteral("主题切换"), this);
  themeSwitchText->setWordWrap(false);
  themeSwitchText->setTextPixelSize(15);
  themeSwitchLayout->addWidget(themeSwitchText);
  themeSwitchLayout->addStretch();
  themeSwitchLayout->addWidget(_themeComboBox);
  connect(_themeComboBox, QOverload<int>::of(&NXComboBox::currentIndexChanged), this, [=](int index) {
    if (index == 0) {
      nxTheme->setIsFollowSystemTheme(true);
    } else if (index == 1) {
      nxTheme->setIsFollowSystemTheme(false);
      nxTheme->setThemeMode(NXThemeType::Light);
    } else {
      nxTheme->setIsFollowSystemTheme(false);
      nxTheme->setThemeMode(NXThemeType::Dark);
    }
  });
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    if (nxTheme->getIsFollowSystemTheme()) {
      return;
    }
    _themeComboBox->blockSignals(true);
    if (themeMode == NXThemeType::Light) {
      _themeComboBox->setCurrentIndex(1);
    } else {
      _themeComboBox->setCurrentIndex(2);
    }
    _themeComboBox->blockSignals(false);
  });

  nxTheme->setIsFollowSystemTheme(true);
  NXText *windowPaintText = new NXText(QStringLiteral("主窗口绘制设置"), this);
  windowPaintText->setWordWrap(false);
  windowPaintText->setTextPixelSize(15);

  _windowNormalButton = new NXRadioButton(QStringLiteral("Normal"), this);
  _windowNormalButton->setChecked(true);
  _windowPixmapButton = new NXRadioButton(QStringLiteral("Pixmap"), this);
  _windowMovieButton  = new NXRadioButton(QStringLiteral("Movie"), this);

  QButtonGroup *windowPaintButtonGroup = new QButtonGroup(this);
  windowPaintButtonGroup->addButton(_windowNormalButton, 0);
  windowPaintButtonGroup->addButton(_windowPixmapButton, 1);
  windowPaintButtonGroup->addButton(_windowMovieButton, 2);
  connect(windowPaintButtonGroup, QOverload<QAbstractButton *, bool>::of(&QButtonGroup::buttonToggled), this, [=](QAbstractButton *button, bool isToggled) {
    if (isToggled) {
      window->setWindowPaintMode((NXWindowType::PaintMode) windowPaintButtonGroup->id(button));
    }
  });
  connect(window, &NXWindow::signalWindowPaintModeChanged, this, [=]() {
    auto button                = windowPaintButtonGroup->button(window->getWindowPaintMode());
    NXRadioButton *radioButton = dynamic_cast<NXRadioButton *>(button);
    if (radioButton) {
      radioButton->setChecked(true);
    }
  });
  NXScrollPageArea *windowPaintModeArea = new NXScrollPageArea(this);
  QHBoxLayout *windowPaintModeLayout    = new QHBoxLayout(windowPaintModeArea);
  windowPaintModeLayout->addWidget(windowPaintText);
  windowPaintModeLayout->addStretch();
  windowPaintModeLayout->addWidget(_windowNormalButton);
  windowPaintModeLayout->addWidget(_windowPixmapButton);
  windowPaintModeLayout->addWidget(_windowMovieButton);

  NXText *helperText = new NXText(QStringLiteral("应用程序设置"), this);
  helperText->setWordWrap(false);
  helperText->setTextPixelSize(18);

  NXText *micaSwitchText = new NXText(QStringLiteral("窗口效果"), this);
  micaSwitchText->setWordWrap(false);
  micaSwitchText->setTextPixelSize(15);
  _normalButton = new NXRadioButton(QStringLiteral("Normal"), this);
  _micaButton   = new NXRadioButton(QStringLiteral("NXMica"), this);
#ifdef Q_OS_WIN
  _micaButton          = new NXRadioButton(QStringLiteral("Mica"), this);
  _micaAltButton       = new NXRadioButton(QStringLiteral("Mica-Alt"), this);
  _acrylicButton       = new NXRadioButton(QStringLiteral("Acrylic"), this);
  _dwmBlurnormalButton = new NXRadioButton(QStringLiteral("Dwm-Blur"), this);
#endif
  _normalButton->setChecked(true);
  QButtonGroup *displayButtonGroup = new QButtonGroup(this);
  displayButtonGroup->addButton(_normalButton, 0);
  displayButtonGroup->addButton(_micaButton, 1);
#ifdef Q_OS_WIN
  displayButtonGroup->addButton(_micaButton, 2);
  displayButtonGroup->addButton(_micaAltButton, 3);
  displayButtonGroup->addButton(_acrylicButton, 4);
  displayButtonGroup->addButton(_dwmBlurnormalButton, 5);
#endif
  connect(displayButtonGroup, QOverload<QAbstractButton *, bool>::of(&QButtonGroup::buttonToggled), this, [=](QAbstractButton *button, bool isToggled) {
    if (isToggled) {
      nxApp->setWindowDisplayMode((NXApplicationType::WindowDisplayMode) displayButtonGroup->id(button));
    }
  });
  connect(nxApp, &NXApplication::signalWindowDisplayModeChanged, this, [=]() {
    auto button                = displayButtonGroup->button(nxApp->getWindowDisplayMode());
    NXRadioButton *radioButton = dynamic_cast<NXRadioButton *>(button);
    if (radioButton) {
      radioButton->setChecked(true);
    }
  });
  NXScrollPageArea *micaSwitchArea = new NXScrollPageArea(this);
  QHBoxLayout *micaSwitchLayout    = new QHBoxLayout(micaSwitchArea);
  micaSwitchLayout->addWidget(micaSwitchText);
  micaSwitchLayout->addStretch();
  micaSwitchLayout->addWidget(_normalButton);
  micaSwitchLayout->addWidget(_micaButton);
#ifdef Q_OS_WIN
  micaSwitchLayout->addWidget(_micaButton);
  micaSwitchLayout->addWidget(_micaAltButton);
  micaSwitchLayout->addWidget(_acrylicButton);
  micaSwitchLayout->addWidget(_dwmBlurnormalButton);
#endif

  _logSwitchButton                = new NXToggleSwitch(this);
  NXScrollPageArea *logSwitchArea = new NXScrollPageArea(this);
  QHBoxLayout *logSwitchLayout    = new QHBoxLayout(logSwitchArea);
  NXText *logSwitchText           = new NXText(QStringLiteral("启用日志功能"), this);
  logSwitchText->setWordWrap(false);
  logSwitchText->setTextPixelSize(15);
  logSwitchLayout->addWidget(logSwitchText);
  logSwitchLayout->addStretch();
  logSwitchLayout->addWidget(_logSwitchButton);
  connect(_logSwitchButton, &NXToggleSwitch::toggled, this, [=](bool checked) {
    NXLog::getInstance()->initMessageLog(checked);
    if (checked) {
      qDebug() << "日志已启用!";
    } else {
      qDebug() << "日志已关闭!";
    }
  });

  _userCardSwitchButton                = new NXToggleSwitch(this);
  NXScrollPageArea *userCardSwitchArea = new NXScrollPageArea(this);
  QHBoxLayout *userCardSwitchLayout    = new QHBoxLayout(userCardSwitchArea);
  NXText *userCardSwitchText           = new NXText(QStringLiteral("隐藏用户卡片"), this);
  userCardSwitchText->setWordWrap(false);
  userCardSwitchText->setTextPixelSize(15);
  userCardSwitchLayout->addWidget(userCardSwitchText);
  userCardSwitchLayout->addStretch();
  userCardSwitchLayout->addWidget(_userCardSwitchButton);
  connect(_userCardSwitchButton, &NXToggleSwitch::toggled, this, [=](bool checked) {
    window->setUserInfoCardVisible(!checked);
  });

  _minimumButton = new NXRadioButton(QStringLiteral("Minimum"), this);
  _compactButton = new NXRadioButton(QStringLiteral("Compact"), this);
  _maximumButton = new NXRadioButton(QStringLiteral("Maximum"), this);
  _autoButton    = new NXRadioButton(QStringLiteral("Auto"), this);
  _autoButton->setChecked(true);
  NXScrollPageArea *displayModeArea = new NXScrollPageArea(this);
  QHBoxLayout *displayModeLayout    = new QHBoxLayout(displayModeArea);
  NXText *displayModeText           = new NXText(QStringLiteral("导航栏模式选择"), this);
  displayModeText->setWordWrap(false);
  displayModeText->setTextPixelSize(15);
  displayModeLayout->addWidget(displayModeText);
  displayModeLayout->addStretch();
  displayModeLayout->addWidget(_minimumButton);
  displayModeLayout->addWidget(_compactButton);
  displayModeLayout->addWidget(_maximumButton);
  displayModeLayout->addWidget(_autoButton);

  QButtonGroup *navigationGroup = new QButtonGroup(this);
  navigationGroup->addButton(_autoButton, 0);
  navigationGroup->addButton(_minimumButton, 1);
  navigationGroup->addButton(_compactButton, 2);
  navigationGroup->addButton(_maximumButton, 3);
  connect(navigationGroup, QOverload<QAbstractButton *, bool>::of(&QButtonGroup::buttonToggled), this, [=](QAbstractButton *button, bool isToggled) {
    if (isToggled) {
      window->setNavigationBarDisplayMode((NXNavigationType::NavigationDisplayMode) navigationGroup->id(button));
    }
  });

  _noneButton  = new NXRadioButton(QStringLiteral("None"), this);
  _popupButton = new NXRadioButton(QStringLiteral("Popup"), this);
  _popupButton->setChecked(true);
  _scaleButton                          = new NXRadioButton(QStringLiteral("Scale"), this);
  _flipButton                           = new NXRadioButton(QStringLiteral("Flip"), this);
  _blurButton                           = new NXRadioButton(QStringLiteral("Blur"), this);
  NXScrollPageArea *stackSwitchModeArea = new NXScrollPageArea(this);
  QHBoxLayout *stackSwitchModeLayout    = new QHBoxLayout(stackSwitchModeArea);
  NXText *stackSwitchModeText           = new NXText(QStringLiteral("堆栈切换模式选择"), this);
  stackSwitchModeText->setWordWrap(false);
  stackSwitchModeText->setTextPixelSize(15);
  stackSwitchModeLayout->addWidget(stackSwitchModeText);
  stackSwitchModeLayout->addStretch();
  stackSwitchModeLayout->addWidget(_noneButton);
  stackSwitchModeLayout->addWidget(_popupButton);
  stackSwitchModeLayout->addWidget(_scaleButton);
  stackSwitchModeLayout->addWidget(_flipButton);
  stackSwitchModeLayout->addWidget(_blurButton);

  QButtonGroup *stackSwitchGroup = new QButtonGroup(this);
  stackSwitchGroup->addButton(_noneButton, 0);
  stackSwitchGroup->addButton(_popupButton, 1);
  stackSwitchGroup->addButton(_scaleButton, 2);
  stackSwitchGroup->addButton(_flipButton, 3);
  stackSwitchGroup->addButton(_blurButton, 4);
  connect(stackSwitchGroup, QOverload<QAbstractButton *, bool>::of(&QButtonGroup::buttonToggled), this, [=](QAbstractButton *button, bool isToggled) {
    if (isToggled) {
      window->setStackSwitchMode((NXWindowType::StackSwitchMode) stackSwitchGroup->id(button));
    }
  });
  connect(window, &NXWindow::signalStackSwitchModeChanged, this, [=]() {
    auto button                = stackSwitchGroup->button(window->getStackSwitchMode());
    NXRadioButton *radioButton = dynamic_cast<NXRadioButton *>(button);
    if (radioButton) {
      radioButton->setChecked(true);
    }
  });

  QWidget *centralWidget = new QWidget(this);
  centralWidget->setWindowTitle(QStringLiteral("Setting"));
  QVBoxLayout *centerLayout = new QVBoxLayout(centralWidget);
  centerLayout->addSpacing(30);
  centerLayout->addWidget(themeText);
  centerLayout->addSpacing(10);
  centerLayout->addWidget(themeSwitchArea);
  centerLayout->addSpacing(15);
  centerLayout->addWidget(helperText);
  centerLayout->addSpacing(10);
  centerLayout->addWidget(logSwitchArea);
  centerLayout->addWidget(userCardSwitchArea);
  centerLayout->addWidget(windowPaintModeArea);
  centerLayout->addWidget(micaSwitchArea);
  centerLayout->addWidget(displayModeArea);
  centerLayout->addWidget(stackSwitchModeArea);
  centerLayout->addStretch();
  centerLayout->setContentsMargins(0, 0, 0, 0);
  addCentralWidget(centralWidget, true, true, 0);
}

T_Setting::~T_Setting()
{
}
