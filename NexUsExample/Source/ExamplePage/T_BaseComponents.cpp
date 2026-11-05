#include "T_BaseComponents.h"

#include <QButtonGroup>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "NXCheckBox.h"
#include "NXComboBox.h"
#include "NXGroupBox.h"
#include "NXMessageButton.h"
#include "NXMultiSelectComboBox.h"
#include "NXPlainTextEdit.h"
#include "NXProgressBar.h"
#include "NXProgressRing.h"
#include "NXRadioButton.h"
#include "NXScrollPageArea.h"
#include "NXSlider.h"
#include "NXSpinBox.h"
#include "NXText.h"
#include "NXToggleButton.h"
#include "NXToggleSwitch.h"

T_BaseComponents::T_BaseComponents(QWidget *parent)
    : T_BasePage(parent)
{
  // 预览窗口标题
  setWindowTitle(QStringLiteral("NXBaseComponents"));

  // 顶部元素
  createCustomWidget(QStringLiteral("一些常用的基础组件被放置于此，可在此界面体验其效果并按需添加进项目中"));

  _toggleSwitch                      = new NXToggleSwitch(this);
  NXScrollPageArea *toggleSwitchArea = new NXScrollPageArea(this);
  QHBoxLayout *toggleSwitchLayout    = new QHBoxLayout(toggleSwitchArea);
  NXText *toggleSwitchText           = new NXText(QStringLiteral("NXToggleSwitch"), this);
  toggleSwitchText->setTextPixelSize(15);
  toggleSwitchLayout->addWidget(toggleSwitchText);
  toggleSwitchLayout->addWidget(_toggleSwitch);
  toggleSwitchLayout->addStretch();
  NXToggleSwitch *toggleSwitchDisableSwitch = new NXToggleSwitch(this);
  NXText *toggleSwitchDisableText           = new NXText(QStringLiteral("禁用"), this);
  toggleSwitchDisableText->setTextPixelSize(15);
  connect(toggleSwitchDisableSwitch, &NXToggleSwitch::toggled, this, [=](bool checked) {
    _toggleSwitch->setDisabled(checked);
  });
  toggleSwitchLayout->addWidget(toggleSwitchDisableSwitch);
  toggleSwitchLayout->addWidget(toggleSwitchDisableText);
  toggleSwitchLayout->addSpacing(10);

  _toggleButton = new NXToggleButton(QStringLiteral("ToggleButton"), this);
  _toggleButton->setFixedWidth(120);
  NXScrollPageArea *toggleButtonArea = new NXScrollPageArea(this);
  QHBoxLayout *toggleButtonLayout    = new QHBoxLayout(toggleButtonArea);
  NXText *toggleButtonText           = new NXText(QStringLiteral("NXToggleButton"), this);
  toggleButtonText->setTextPixelSize(15);
  toggleButtonLayout->addWidget(toggleButtonText);
  toggleButtonLayout->addWidget(_toggleButton);
  toggleButtonLayout->addStretch();
  NXToggleSwitch *toggleButtonDisableSwitch = new NXToggleSwitch(this);
  NXText *toggleButtonDisableText           = new NXText(QStringLiteral("禁用"), this);
  toggleButtonDisableText->setTextPixelSize(15);
  connect(toggleButtonDisableSwitch, &NXToggleSwitch::toggled, this, [=](bool checked) {
    _toggleButton->setDisabled(checked);
  });
  toggleButtonLayout->addWidget(toggleButtonDisableSwitch);
  toggleButtonLayout->addWidget(toggleButtonDisableText);
  toggleButtonLayout->addSpacing(10);

  _comboBox = new NXComboBox(this);
  QStringList comboList { QStringLiteral("我愿投身前途未卜的群星"), QStringLiteral("潜行 步伐小心翼翼"),
                          QStringLiteral("不留游走痕迹"), QStringLiteral("如同一簇幽灵"),
                          QStringLiteral("所谓 道德加上伦理"), QStringLiteral("抱歉只能律己") };
  _comboBox->addItems(comboList);
  NXScrollPageArea *comboBoxArea = new NXScrollPageArea(this);
  QHBoxLayout *comboBoxLayout    = new QHBoxLayout(comboBoxArea);
  NXText *comboBoxText           = new NXText(QStringLiteral("NXComboBox"), this);
  comboBoxText->setTextPixelSize(15);
  comboBoxLayout->addWidget(comboBoxText);
  comboBoxLayout->addWidget(_comboBox);
  comboBoxLayout->addStretch();
  NXToggleSwitch *comboBoxDisableSwitch = new NXToggleSwitch(this);
  NXText *comboBoxDisableText           = new NXText(QStringLiteral("禁用"), this);
  comboBoxDisableText->setTextPixelSize(15);
  connect(comboBoxDisableSwitch, &NXToggleSwitch::toggled, this, [=](bool checked) {
    _comboBox->setDisabled(checked);
  });
  comboBoxLayout->addWidget(comboBoxDisableSwitch);
  comboBoxLayout->addWidget(comboBoxDisableText);
  comboBoxLayout->addSpacing(10);

  _multiSelectComboBox = new NXMultiSelectComboBox(this);
  QStringList multiComboList { QStringLiteral("执念的鱼"),
                               QStringLiteral("提着灯闯过远洋的甄选"),
                               QStringLiteral("继续下潜"),
                               QStringLiteral("无需誓言"),
                               QStringLiteral("我的心像自沉的旧母舰"),
                               QStringLiteral("没入深渊"),
                               QStringLiteral("我曾凝望曾是航向的日出") };
  QStringList multiSelectComboList { QStringLiteral("执念的鱼"), QStringLiteral("提着灯闯过远洋的甄选"),
                                     QStringLiteral("无需誓言"), QStringLiteral("我的心像自沉的旧母舰") };
  _multiSelectComboBox->addItems(multiComboList);
  _multiSelectComboBox->setCurrentSelection(multiSelectComboList);
  //_multiSelectComboBox->setShowCheckBox(true);
  NXScrollPageArea *multiSelectComboBoxArea = new NXScrollPageArea(this);
  QHBoxLayout *multiSelectComboBoxLayout    = new QHBoxLayout(multiSelectComboBoxArea);
  NXText *multiSelectComboBoxText           = new NXText(QStringLiteral("NXMutilSelectComboBox"), this);
  multiSelectComboBoxText->setTextPixelSize(15);
  multiSelectComboBoxLayout->addWidget(multiSelectComboBoxText);
  multiSelectComboBoxLayout->addWidget(_multiSelectComboBox);
  multiSelectComboBoxLayout->addStretch();
  NXToggleSwitch *multiSelectComboBoxDisableSwitch = new NXToggleSwitch(this);
  NXText *multiSelectComboBoxDisableText           = new NXText(QStringLiteral("禁用"), this);
  multiSelectComboBoxDisableText->setTextPixelSize(15);
  connect(multiSelectComboBoxDisableSwitch, &NXToggleSwitch::toggled, this, [=](bool checked) {
    _multiSelectComboBox->setDisabled(checked);
  });
  multiSelectComboBoxLayout->addWidget(multiSelectComboBoxDisableSwitch);
  multiSelectComboBoxLayout->addWidget(multiSelectComboBoxDisableText);
  multiSelectComboBoxLayout->addSpacing(10);

  _messageButton = new NXMessageButton(QStringLiteral("Success"), this);
  _messageButton->setBarTitle(QStringLiteral("Success"));
  _messageButton->setBarText(QStringLiteral("点燃星 亲手点燃黑暗森林的火星 蒙昧初醒 而我却 轻声告别这新生的黎明"));

  _infoMessageButton = new NXMessageButton(QStringLiteral("Info"), this);
  _infoMessageButton->setBarTitle(QStringLiteral("Information"));
  _infoMessageButton->setBarText(QStringLiteral("点燃星 亲手点燃黑暗森林的火星 蒙昧初醒 而我却 轻声告别这新生的黎明"));
  _infoMessageButton->setMessageMode(NXMessageBarType::Information);
  _infoMessageButton->setPositionPolicy(NXMessageBarType::TopLeft);

  _warningMessageButton = new NXMessageButton(QStringLiteral("Warning"), this);
  _warningMessageButton->setBarTitle(QStringLiteral("Warning"));
  _warningMessageButton->setBarText(
      QStringLiteral("点燃星 亲手点燃黑暗森林的火星 蒙昧初醒 而我却 轻声告别这新生的黎明"));
  _warningMessageButton->setMessageMode(NXMessageBarType::Warning);
  _warningMessageButton->setPositionPolicy(NXMessageBarType::BottomLeft);

  _errorMessageButton = new NXMessageButton(QStringLiteral("Error"), this);
  _errorMessageButton->setBarTitle(QStringLiteral("Error"));
  _errorMessageButton->setBarText(QStringLiteral("点燃星 亲手点燃黑暗森林的火星 蒙昧初醒 而我却 轻声告别这新生的黎明"));
  _errorMessageButton->setMessageMode(NXMessageBarType::Error);
  _errorMessageButton->setPositionPolicy(NXMessageBarType::BottomRight);

  NXScrollPageArea *messageButtonArea = new NXScrollPageArea(this);
  QHBoxLayout *messageButtonLayout    = new QHBoxLayout(messageButtonArea);
  NXText *messageButtonText           = new NXText(QStringLiteral("NXMessageButton"), this);
  messageButtonText->setTextPixelSize(15);
  messageButtonLayout->addWidget(messageButtonText);
  messageButtonLayout->addWidget(_messageButton);
  messageButtonLayout->addWidget(_infoMessageButton);
  messageButtonLayout->addWidget(_warningMessageButton);
  messageButtonLayout->addWidget(_errorMessageButton);
  messageButtonLayout->addStretch();
  NXToggleSwitch *messageButtonDisableSwitch = new NXToggleSwitch(this);
  NXText *messageButtonDisableText           = new NXText(QStringLiteral("禁用"), this);
  messageButtonDisableText->setTextPixelSize(15);
  connect(messageButtonDisableSwitch, &NXToggleSwitch::toggled, this, [=](bool checked) {
    _messageButton->setDisabled(checked);
    _infoMessageButton->setDisabled(checked);
    _warningMessageButton->setDisabled(checked);
    _errorMessageButton->setDisabled(checked);
  });
  messageButtonLayout->addWidget(messageButtonDisableSwitch);
  messageButtonLayout->addWidget(messageButtonDisableText);
  messageButtonLayout->addSpacing(10);

  _checkBox                      = new NXCheckBox(QStringLiteral("CheckBox"), this);
  NXScrollPageArea *checkBoxArea = new NXScrollPageArea(this);
  QHBoxLayout *checkBoxLayout    = new QHBoxLayout(checkBoxArea);
  NXText *checkBoxText           = new NXText(QStringLiteral("NXcheckBox"), this);
  checkBoxText->setTextPixelSize(15);
  checkBoxLayout->addWidget(checkBoxText);
  checkBoxLayout->addWidget(_checkBox);
  checkBoxLayout->addStretch();
  NXToggleSwitch *checkBoxDisableSwitch = new NXToggleSwitch(this);
  NXText *checkBoxDisableText           = new NXText(QStringLiteral("禁用"), this);
  checkBoxDisableText->setTextPixelSize(15);
  connect(checkBoxDisableSwitch, &NXToggleSwitch::toggled, this, [=](bool checked) {
    _checkBox->setDisabled(checked);
  });
  checkBoxLayout->addWidget(checkBoxDisableSwitch);
  checkBoxLayout->addWidget(checkBoxDisableText);
  checkBoxLayout->addSpacing(10);

  _spinBox                      = new NXSpinBox(this);
  NXScrollPageArea *spinBoxArea = new NXScrollPageArea(this);
  QHBoxLayout *spinBoxLayout    = new QHBoxLayout(spinBoxArea);
  NXText *spinBoxText           = new NXText(QStringLiteral("NXSpinBox"), this);
  spinBoxText->setTextPixelSize(15);
  spinBoxLayout->addWidget(spinBoxText);
  spinBoxLayout->addWidget(_spinBox);
  spinBoxLayout->addStretch();

  NXRadioButton *inlineButton  = new NXRadioButton(QStringLiteral("Inline"), this);
  NXRadioButton *compactButton = new NXRadioButton(QStringLiteral("Compact"), this);
  NXRadioButton *sideButton    = new NXRadioButton(QStringLiteral("Side"), this);
  NXRadioButton *pmSideButton  = new NXRadioButton(QStringLiteral("PMSide"), this);
  inlineButton->setChecked(true);
  QHBoxLayout *buttonModeLayout = new QHBoxLayout();
  NXText *buttonModeText        = new NXText(QStringLiteral("按钮模式切换"), this);
  buttonModeText->setWordWrap(false);
  buttonModeText->setTextPixelSize(15);
  buttonModeLayout->addWidget(buttonModeText);
  buttonModeLayout->addWidget(inlineButton);
  buttonModeLayout->addWidget(compactButton);
  buttonModeLayout->addWidget(sideButton);
  buttonModeLayout->addWidget(pmSideButton);

  spinBoxLayout->addLayout(buttonModeLayout);

  QButtonGroup *spinButtonGroup = new QButtonGroup(this);
  spinButtonGroup->addButton(inlineButton, 0);
  spinButtonGroup->addButton(compactButton, 1);
  spinButtonGroup->addButton(sideButton, 2);
  spinButtonGroup->addButton(pmSideButton, 3);
  connect(spinButtonGroup, QOverload<QAbstractButton *, bool>::of(&QButtonGroup::buttonToggled), this, [=](QAbstractButton *button, bool isToggled) {
    if (isToggled) {
      _spinBox->setButtonMode((NXSpinBoxType::ButtonMode) spinButtonGroup->id(button));
    }
  });

  _slider                      = new NXSlider(this);
  NXScrollPageArea *sliderArea = new NXScrollPageArea(this);
  QHBoxLayout *sliderLayout    = new QHBoxLayout(sliderArea);
  NXText *sliderText           = new NXText(QStringLiteral("NXSlider"), this);
  sliderText->setTextPixelSize(15);
  sliderLayout->addWidget(sliderText);
  sliderLayout->addWidget(_slider);
  sliderLayout->addStretch();

  _radioButton                      = new NXRadioButton(QStringLiteral("RadioButton"), this);
  NXScrollPageArea *radioButtonArea = new NXScrollPageArea(this);
  QHBoxLayout *radioButtonLayout    = new QHBoxLayout(radioButtonArea);
  NXText *radioButtonText           = new NXText(QStringLiteral("NXRadioButton"), this);
  radioButtonText->setTextPixelSize(15);
  radioButtonLayout->addWidget(radioButtonText);
  radioButtonLayout->addWidget(_radioButton);
  radioButtonLayout->addStretch();

  _progressBar = new NXProgressBar(this);
  _progressBar->setMinimum(0);
  _progressBar->setMaximum(0);
  NXScrollPageArea *progressBarArea = new NXScrollPageArea(this);
  QHBoxLayout *progressBarLayout    = new QHBoxLayout(progressBarArea);
  NXText *progressBarText           = new NXText(QStringLiteral("NXProgressBar"), this);
  progressBarText->setTextPixelSize(15);
  progressBarLayout->addWidget(progressBarText);
  progressBarLayout->addWidget(_progressBar);
  progressBarLayout->addStretch();

  _progressRing = new NXProgressRing(this);
  _progressRing->setValue(30);
  _progressPercentRing = new NXProgressRing(this);
  _progressPercentRing->setValue(50);
  _progressPercentRing->setValueDisplayMode(NXProgressRingType::ValueDisplayMode::Percent);
  _progressBusyRing = new NXProgressRing(this);
  _progressBusyRing->setIsBusying(true);
  _progressBusyTransparentRing = new NXProgressRing(this);
  _progressBusyTransparentRing->setIsBusying(true);
  _progressBusyTransparentRing->setIsTransparent(true);
  NXScrollPageArea *progressRingArea = new NXScrollPageArea(this);
  progressRingArea->setFixedHeight(90);
  QHBoxLayout *progressRingLayout = new QHBoxLayout(progressRingArea);
  NXText *progressRingText        = new NXText(QStringLiteral("NXProgressRing"), this);
  progressRingText->setTextPixelSize(15);
  progressRingLayout->addWidget(progressRingText);
  progressRingLayout->addWidget(_progressRing);
  progressRingLayout->addSpacing(10);
  progressRingLayout->addWidget(_progressPercentRing);
  progressRingLayout->addSpacing(10);
  progressRingLayout->addWidget(_progressBusyRing);
  progressRingLayout->addSpacing(10);
  progressRingLayout->addWidget(_progressBusyTransparentRing);
  progressRingLayout->addStretch();

  _groupBox = new NXGroupBox(QStringLiteral("NXGroupBox"), this);
  _groupBox->setFixedSize(350, 220);
  _groupBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  QVBoxLayout *groupBoxLayout = new QVBoxLayout(_groupBox);
  groupBoxLayout->setSpacing(8);
  groupBoxLayout->setContentsMargins(15, 25, 15, 15);

  NXText *label1        = new NXText(QStringLiteral("这是一个 NXGroupBox 示例"), _groupBox);
  NXText *label2        = new NXText(QStringLiteral("可以在其中放置各种控件"), _groupBox);
  NXCheckBox *checkbox  = new NXCheckBox(QStringLiteral("GroupBox 内的复选框"), _groupBox);
  NXRadioButton *radio1 = new NXRadioButton(QStringLiteral("选项 1"), _groupBox);
  NXRadioButton *radio2 = new NXRadioButton(QStringLiteral("选项 2"), _groupBox);
  NXRadioButton *radio3 = new NXRadioButton(QStringLiteral("选项 3"), _groupBox);
  checkbox->setChecked(true);
  radio1->setChecked(true);

  groupBoxLayout->addWidget(label1);
  groupBoxLayout->addWidget(label2);
  groupBoxLayout->addWidget(checkbox);
  groupBoxLayout->addWidget(radio1);
  groupBoxLayout->addWidget(radio2);
  groupBoxLayout->addWidget(radio3);
  groupBoxLayout->addStretch();

  NXScrollPageArea *groupBoxArea = new NXScrollPageArea(this);
  groupBoxArea->setFixedHeight(240);
  QHBoxLayout *groupBoxAreaLayout = new QHBoxLayout(groupBoxArea);
  NXText *groupBoxLabel           = new NXText(QStringLiteral("NXGroupBox"), this);
  groupBoxLabel->setTextPixelSize(12);
  groupBoxAreaLayout->addWidget(groupBoxLabel);
  groupBoxAreaLayout->addWidget(_groupBox);
  groupBoxAreaLayout->addStretch();
  NXToggleSwitch *groupBoxDisableSwitch = new NXToggleSwitch(this);
  NXText *groupBoxDisableText           = new NXText(QStringLiteral("禁用"), this);
  groupBoxDisableText->setTextPixelSize(15);
  connect(groupBoxDisableSwitch, &NXToggleSwitch::toggled, this, [=](bool checked) {
    _groupBox->setDisabled(checked);
  });
  groupBoxAreaLayout->addWidget(groupBoxDisableSwitch);
  groupBoxAreaLayout->addWidget(groupBoxDisableText);
  groupBoxAreaLayout->addSpacing(10);

  NXPlainTextEdit *edit = new NXPlainTextEdit(this);
  edit->setPlainText(QStringLiteral("这是一个NXPlainTextEdit  暂时放在这里"));

  QWidget *centralWidget = new QWidget(this);
  centralWidget->setWindowTitle(QStringLiteral("NXBaseComponents"));
  QVBoxLayout *centerLayout = new QVBoxLayout(centralWidget);
  centerLayout->addWidget(toggleSwitchArea);
  centerLayout->addWidget(toggleButtonArea);
  centerLayout->addWidget(comboBoxArea);
  centerLayout->addWidget(multiSelectComboBoxArea);
  centerLayout->addWidget(messageButtonArea);
  centerLayout->addWidget(checkBoxArea);
  centerLayout->addWidget(spinBoxArea);
  centerLayout->addWidget(sliderArea);
  centerLayout->addWidget(radioButtonArea);
  centerLayout->addWidget(progressBarArea);
  centerLayout->addWidget(progressRingArea);
  centerLayout->addWidget(groupBoxArea);
  centerLayout->addWidget(edit);
  centerLayout->addStretch();
  centerLayout->setContentsMargins(0, 0, 0, 0);
  addCentralWidget(centralWidget, true, true, 0);

  NXText *homeStack1 = new NXText(QStringLiteral("HomeStack1"), this);
  QFont font         = homeStack1->font();
  font.setPixelSize(32);
  homeStack1->setFont(font);
  homeStack1->setAlignment(Qt::AlignCenter);
  homeStack1->setWindowTitle(QStringLiteral("HomeStack1"));
  addCentralWidget(homeStack1);
  NXText *homeStack2 = new NXText(QStringLiteral("HomeStack2"), this);
  homeStack2->setFont(font);
  homeStack2->setAlignment(Qt::AlignCenter);
  homeStack2->setWindowTitle(QStringLiteral("HomeStack2"));
  addCentralWidget(homeStack2);
}

T_BaseComponents::~T_BaseComponents()
{
}

void T_BaseComponents::mouseReleaseEvent(QMouseEvent *event)
{
  switch (event->button()) {
  case Qt::LeftButton : {
    //NXMessageBar::success(NXMessageBarType::TopRight, "Success", "Never Close Your Eyes", 2500);
    //NXMessageBar::success(NXMessageBarType::TopRight, "Success", "Never Close Your Eyes", 1500);
    break;
  }
  case Qt::BackButton : {
    this->navigation(0);
    break;
  }
  case Qt::ForwardButton : {
    this->navigation(1);
    break;
  }
  case Qt::MiddleButton : {
    this->navigation(2);
    break;
  }
  default : {
    break;
  }
  }
  NXScrollPage::mouseReleaseEvent(event);
}
