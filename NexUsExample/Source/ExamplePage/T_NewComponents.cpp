#include "T_NewComponents.h"

#include <QButtonGroup>
#include <QDateTime>
#include <QStackedWidget>
#include <QStandardItemModel>
#include <QTimer>
#include <QVBoxLayout>

#include "NXAutoComplete.h"
#include "NXCaptcha.h"
#include "NXCheckBox.h"
#include "NXComboBox.h"
#include "NXCommandBar.h"
#include "NXCopyButton.h"
#include "NXCountdown.h"
#include "NXDivider.h"
#include "NXDropDownButton.h"
#include "NXEmojiPicker.h"
#include "NXExpander.h"
#include "NXFloatButton.h"
#include "NXFlyout.h"
#include "NXInfoBadge.h"
#include "NXInfoBar.h"
#include "NXLineEdit.h"
#include "NXMarkdownViewer.h"
#include "NXMenu.h"
#include "NXNotificationCenter.h"
#include "NXNumberBox.h"
#include "NXPagination.h"
#include "NXPasswordBox.h"
#include "NXPersonPicture.h"
#include "NXPopconfirm.h"
#include "NXPushButton.h"
#include "NXQRCode.h"
#include "NXRadioButton.h"
#include "NXRatingControl.h"
#include "NXScrollPageArea.h"
#include "NXSelectorBar.h"
#include "NXSheetPanel.h"
#include "NXSkeleton.h"
#include "NXSlider.h"
#include "NXSnackbar.h"
#include "NXSplashScreen.h"
#include "NXSplitButton.h"
#include "NXSplitter.h"
#include "NXSpotlight.h"
#include "NXStatCard.h"
#include "NXSteps.h"
#include "NXTag.h"
#include "NXTeachingTip.h"
#include "NXText.h"
#include "NXTimeline.h"
#include "NXToast.h"
#include "NXToggleSwitch.h"
#include "NXTransfer.h"
#include "NXTreeSelect.h"
#include "NXUploadArea.h"
#include "NXVirtualList.h"
#include "NXWatermark.h"
#include "NXWizard.h"

T_NewComponents::T_NewComponents(QWidget *parent)
    : T_BasePage(parent)
{
  setWindowTitle(QStringLiteral("NXNewComponents"));
  createCustomWidget(QStringLiteral("新增组件的演示页面"));

  QWidget *centralWidget = new QWidget(this);
  centralWidget->setWindowTitle(QStringLiteral("NXNewComponents"));

  // ========== NXInfoBadge 示例 ==========
  NXPushButton *dotTargetButton = new NXPushButton(QStringLiteral("邮件"), this);
  dotTargetButton->setFixedSize(80, 36);
  _dotBadge = new NXInfoBadge(this);
  _dotBadge->setBadgeMode(NXInfoBadge::Dot);
  _dotBadge->setSeverity(NXInfoBadge::Attention);
  _dotBadge->attachTo(dotTargetButton);

  NXPushButton *valueTargetButton = new NXPushButton(QStringLiteral("通知"), this);
  valueTargetButton->setFixedSize(80, 36);
  _valueBadge = new NXInfoBadge(5, this);
  _valueBadge->setSeverity(NXInfoBadge::Informational);
  _valueBadge->attachTo(valueTargetButton);

  NXPushButton *iconTargetButton = new NXPushButton(QStringLiteral("设置"), this);
  iconTargetButton->setFixedSize(80, 36);
  _iconBadge = new NXInfoBadge(NXIconType::CircleExclamation, this);
  _iconBadge->setSeverity(NXInfoBadge::Caution);
  _iconBadge->attachTo(iconTargetButton);

  NXScrollPageArea *badgeArea = new NXScrollPageArea(this);
  QHBoxLayout *badgeLayout    = new QHBoxLayout(badgeArea);
  NXText *badgeText           = new NXText(QStringLiteral("NXInfoBadge"), this);
  badgeText->setTextPixelSize(15);
  badgeLayout->addWidget(badgeText);
  badgeLayout->addSpacing(20);
  badgeLayout->addWidget(dotTargetButton);
  badgeLayout->addSpacing(20);
  badgeLayout->addWidget(valueTargetButton);
  badgeLayout->addSpacing(20);
  badgeLayout->addWidget(iconTargetButton);
  badgeLayout->addStretch();

  // InfoBadge 值控制
  NXScrollPageArea *badgeControlArea = new NXScrollPageArea(this);
  QHBoxLayout *badgeControlLayout    = new QHBoxLayout(badgeControlArea);
  badgeControlLayout->addWidget(new NXText(QStringLiteral("InfoBadge 值控制"), 15, this));
  NXSlider *badgeSlider = new NXSlider(this);
  badgeSlider->setRange(0, 150);
  badgeSlider->setValue(5);
  badgeSlider->setFixedWidth(200);
  NXText *badgeValueText = new NXText(QStringLiteral("5"), this);
  badgeValueText->setTextPixelSize(13);
  connect(badgeSlider, &NXSlider::valueChanged, this, [=](int v) {
    _valueBadge->setValue(v);
    badgeValueText->setText(QString::number(v));
  });
  badgeControlLayout->addWidget(badgeSlider);
  badgeControlLayout->addWidget(badgeValueText);
  badgeControlLayout->addStretch();

  // InfoBadge 严重等级
  NXScrollPageArea *severityArea = new NXScrollPageArea(this);
  QHBoxLayout *severityLayout    = new QHBoxLayout(severityArea);
  severityLayout->addWidget(new NXText(QStringLiteral("InfoBadge 严重等级"), 15, this));
  QButtonGroup *severityGroup   = new QButtonGroup(this);
  NXRadioButton *attentionRadio = new NXRadioButton(QStringLiteral("Attention"), this);
  NXRadioButton *infoRadio      = new NXRadioButton(QStringLiteral("Informational"), this);
  NXRadioButton *successRadio   = new NXRadioButton(QStringLiteral("Success"), this);
  NXRadioButton *cautionRadio   = new NXRadioButton(QStringLiteral("Caution"), this);
  NXRadioButton *criticalRadio  = new NXRadioButton(QStringLiteral("Critical"), this);
  severityGroup->addButton(attentionRadio, 0);
  severityGroup->addButton(infoRadio, 1);
  severityGroup->addButton(successRadio, 2);
  severityGroup->addButton(cautionRadio, 3);
  severityGroup->addButton(criticalRadio, 4);
  infoRadio->setChecked(true);
  connect(severityGroup, QOverload<int>::of(&QButtonGroup::idClicked), this, [=](int id) {
    _valueBadge->setSeverity(static_cast<NXInfoBadge::Severity>(id));
  });
  severityLayout->addWidget(attentionRadio);
  severityLayout->addWidget(infoRadio);
  severityLayout->addWidget(successRadio);
  severityLayout->addWidget(cautionRadio);
  severityLayout->addWidget(criticalRadio);
  severityLayout->addStretch();

  // ========== NXExpander 示例 ==========
  NXText *expanderText = new NXText(QStringLiteral("NXExpander (向下展开)"), this);
  expanderText->setTextPixelSize(15);
  _expander = new NXExpander(QStringLiteral("网络设置"), this);
  _expander->setSubTitle(QStringLiteral("配置代理和网络参数"));
  _expander->setHeaderIcon(NXIconType::WifiExclamation);
  QWidget *expanderContent           = new QWidget(this);
  QVBoxLayout *expanderContentLayout = new QVBoxLayout(expanderContent);
  expanderContentLayout->setContentsMargins(0, 0, 0, 0);
  expanderContentLayout->addWidget(new NXCheckBox(QStringLiteral("启用代理"), this));
  NXLineEdit *proxyHost = new NXLineEdit(this);
  proxyHost->setPlaceholderText(QStringLiteral("代理地址 (例如: 127.0.0.1)"));
  proxyHost->setFixedHeight(35);
  NXLineEdit *proxyPort = new NXLineEdit(this);
  proxyPort->setPlaceholderText(QStringLiteral("端口 (例如: 7890)"));
  proxyPort->setFixedHeight(35);
  expanderContentLayout->addWidget(proxyHost);
  expanderContentLayout->addWidget(proxyPort);
  _expander->setContentWidget(expanderContent);

  NXText *expanderUpText = new NXText(QStringLiteral("NXExpander (向上展开)"), this);
  expanderUpText->setTextPixelSize(15);
  _expanderUp = new NXExpander(QStringLiteral("高级设置"), this);
  _expanderUp->setExpandDirection(NXExpander::Up);
  _expanderUp->setHeaderIcon(NXIconType::GearComplex);
  QWidget *expanderUpContent           = new QWidget(this);
  QVBoxLayout *expanderUpContentLayout = new QVBoxLayout(expanderUpContent);
  expanderUpContentLayout->setContentsMargins(0, 0, 0, 0);
  QHBoxLayout *dbgLay = new QHBoxLayout();
  dbgLay->addWidget(new NXText(QStringLiteral("调试模式"), 13, this));
  dbgLay->addWidget(new NXToggleSwitch(this));
  dbgLay->addStretch();
  QHBoxLayout *logLay = new QHBoxLayout();
  logLay->addWidget(new NXText(QStringLiteral("日志输出"), 13, this));
  logLay->addWidget(new NXToggleSwitch(this));
  logLay->addStretch();
  expanderUpContentLayout->addLayout(dbgLay);
  expanderUpContentLayout->addLayout(logLay);
  _expanderUp->setContentWidget(expanderUpContent);

  // ========== NXTeachingTip 示例 ==========
  _teachingTip = new NXTeachingTip(this);
  _teachingTip->setTitle(QStringLiteral("截图功能升级"));
  _teachingTip->setSubTitle(QStringLiteral("版本 2.0"));
  _teachingTip->setContent(QStringLiteral("现在支持区域截图、延时截图和 OCR 文字识别。"));
  _teachingTip->setTipIcon(NXIconType::CameraRetro);
  _teachingTip->setTailPosition(NXTeachingTip::Bottom);
  _teachingTip->addAction(QStringLiteral("跳过"), [=]() {
    _teachingTip->closeTip();
  });
  _teachingTip->addAction(QStringLiteral("了解更多"), [=]() {
    _teachingTip->closeTip();
  });

  _teachingTipTarget = new NXPushButton(QStringLiteral("显示 TeachingTip"), this);
  _teachingTipTarget->setFixedSize(160, 38);
  _teachingTip->setTarget(_teachingTipTarget);
  connect(_teachingTipTarget, &NXPushButton::clicked, this, [=]() {
    _teachingTip->showTip();
  });

  NXScrollPageArea *teachingTipArea = new NXScrollPageArea(this);
  QHBoxLayout *tipLayout            = new QHBoxLayout(teachingTipArea);
  tipLayout->addWidget(new NXText(QStringLiteral("NXTeachingTip"), 15, this));
  tipLayout->addWidget(_teachingTipTarget);
  tipLayout->addStretch();

  // TeachingTip 方向切换
  NXScrollPageArea *tipPosArea = new NXScrollPageArea(this);
  QHBoxLayout *tipPosLayout    = new QHBoxLayout(tipPosArea);
  tipPosLayout->addWidget(new NXText(QStringLiteral("TeachingTip 箭头方向"), 15, this));
  QButtonGroup *tipPosGroup  = new QButtonGroup(this);
  NXRadioButton *autoRadio   = new NXRadioButton(QStringLiteral("Auto"), this);
  NXRadioButton *topRadio    = new NXRadioButton(QStringLiteral("Top"), this);
  NXRadioButton *bottomRadio = new NXRadioButton(QStringLiteral("Bottom"), this);
  NXRadioButton *leftRadio   = new NXRadioButton(QStringLiteral("Left"), this);
  NXRadioButton *rightRadio  = new NXRadioButton(QStringLiteral("Right"), this);
  tipPosGroup->addButton(autoRadio, 0);
  tipPosGroup->addButton(topRadio, 1);
  tipPosGroup->addButton(bottomRadio, 2);
  tipPosGroup->addButton(leftRadio, 3);
  tipPosGroup->addButton(rightRadio, 4);
  bottomRadio->setChecked(true);
  connect(tipPosGroup, QOverload<int>::of(&QButtonGroup::idClicked), this, [=](int id) {
    _teachingTip->setTailPosition(static_cast<NXTeachingTip::TailPosition>(id));
  });
  tipPosLayout->addWidget(autoRadio);
  tipPosLayout->addWidget(topRadio);
  tipPosLayout->addWidget(bottomRadio);
  tipPosLayout->addWidget(leftRadio);
  tipPosLayout->addWidget(rightRadio);
  tipPosLayout->addStretch();

  // ========== NXPasswordBox 示例 ==========
  _passwordBox = new NXPasswordBox(this);
  _passwordBox->setFixedSize(280, 38);
  _passwordBox->setPlaceholderText(QStringLiteral("请输入密码..."));

  NXScrollPageArea *passwordArea = new NXScrollPageArea(this);
  QHBoxLayout *pwdLayout         = new QHBoxLayout(passwordArea);
  pwdLayout->addWidget(new NXText(QStringLiteral("NXPasswordBox"), 15, this));
  pwdLayout->addWidget(_passwordBox);
  pwdLayout->addStretch();

  // ========== NXNumberBox 示例 ==========
  _numberBox = new NXNumberBox(this);
  _numberBox->setMinimum(-50);
  _numberBox->setMaximum(200);
  _numberBox->setStep(5);
  _numberBox->setValue(25);

  NXNumberBox *decimalBox = new NXNumberBox(this);
  decimalBox->setMinimum(0);
  decimalBox->setMaximum(10);
  decimalBox->setStep(0.1);
  decimalBox->setDecimals(2);
  decimalBox->setValue(3.14);

  NXText *numberValue = new NXText(QStringLiteral("25"), this);
  numberValue->setTextPixelSize(13);
  connect(_numberBox, &NXNumberBox::valueChanged, this, [=](double v) {
    numberValue->setText(QString::number(v, 'f', 0));
  });

  NXScrollPageArea *numberArea = new NXScrollPageArea(this);
  QHBoxLayout *numberLayout    = new QHBoxLayout(numberArea);
  numberLayout->addWidget(new NXText(QStringLiteral("NXNumberBox"), 15, this));
  numberLayout->addWidget(_numberBox);
  numberLayout->addWidget(numberValue);
  numberLayout->addSpacing(15);
  numberLayout->addWidget(new NXText(QStringLiteral("小数:"), 13, this));
  numberLayout->addWidget(decimalBox);
  numberLayout->addStretch();

  // ========== NXTag 示例 ==========
  NXTag *defaultTag = new NXTag(this);
  defaultTag->setTagText(QStringLiteral("Default"));
  NXTag *primaryTag = new NXTag(this);
  primaryTag->setTagText(QStringLiteral("Primary"));
  primaryTag->setTagColor(NXTag::Primary);
  NXTag *successTag = new NXTag(this);
  successTag->setTagText(QStringLiteral("Success"));
  successTag->setTagColor(NXTag::Success);
  NXTag *warningTag = new NXTag(this);
  warningTag->setTagText(QStringLiteral("Warning"));
  warningTag->setTagColor(NXTag::Warning);
  NXTag *dangerTag = new NXTag(this);
  dangerTag->setTagText(QStringLiteral("Danger"));
  dangerTag->setTagColor(NXTag::Danger);

  NXScrollPageArea *tagArea = new NXScrollPageArea(this);
  QHBoxLayout *tagLayout    = new QHBoxLayout(tagArea);
  tagLayout->addWidget(new NXText(QStringLiteral("NXTag"), 15, this));
  tagLayout->addSpacing(10);
  tagLayout->addWidget(defaultTag);
  tagLayout->addWidget(primaryTag);
  tagLayout->addWidget(successTag);
  tagLayout->addWidget(warningTag);
  tagLayout->addWidget(dangerTag);
  tagLayout->addStretch();

  // Tag 可关闭
  _closableTag = new NXTag(this);
  _closableTag->setTagText(QStringLiteral("可关闭"));
  _closableTag->setIsClosable(true);
  _closableTag->setTagColor(NXTag::Primary);
  NXTag *closableTag2 = new NXTag(this);
  closableTag2->setTagText(QStringLiteral("北京"));
  closableTag2->setIsClosable(true);
  NXTag *closableTag3 = new NXTag(this);
  closableTag3->setTagText(QStringLiteral("上海"));
  closableTag3->setIsClosable(true);
  connect(_closableTag, &NXTag::closed, this, [=]() {
    _closableTag->hide();
  });
  connect(closableTag2, &NXTag::closed, this, [=]() {
    closableTag2->hide();
  });
  connect(closableTag3, &NXTag::closed, this, [=]() {
    closableTag3->hide();
  });

  NXScrollPageArea *tagCloseArea = new NXScrollPageArea(this);
  QHBoxLayout *tagCloseLayout    = new QHBoxLayout(tagCloseArea);
  tagCloseLayout->addWidget(new NXText(QStringLiteral("NXTag 可关闭"), 15, this));
  tagCloseLayout->addSpacing(10);
  tagCloseLayout->addWidget(_closableTag);
  tagCloseLayout->addWidget(closableTag2);
  tagCloseLayout->addWidget(closableTag3);
  tagCloseLayout->addStretch();

  // Tag 可选中
  _checkableTag = new NXTag(this);
  _checkableTag->setTagText(QStringLiteral("周一"));
  _checkableTag->setIsCheckable(true);
  NXTag *chkTag2 = new NXTag(this);
  chkTag2->setTagText(QStringLiteral("周二"));
  chkTag2->setIsCheckable(true);
  chkTag2->setIsChecked(true);
  NXTag *chkTag3 = new NXTag(this);
  chkTag3->setTagText(QStringLiteral("周三"));
  chkTag3->setIsCheckable(true);

  NXScrollPageArea *tagCheckArea = new NXScrollPageArea(this);
  QHBoxLayout *tagCheckLayout    = new QHBoxLayout(tagCheckArea);
  tagCheckLayout->addWidget(new NXText(QStringLiteral("NXTag 可选中"), 15, this));
  tagCheckLayout->addSpacing(10);
  tagCheckLayout->addWidget(_checkableTag);
  tagCheckLayout->addWidget(chkTag2);
  tagCheckLayout->addWidget(chkTag3);
  tagCheckLayout->addStretch();

  // ========== NXSplitButton 示例 ==========
  _splitButton = new NXSplitButton(this);
  _splitButton->setText(QStringLiteral("发送邮件"));
  _splitButton->setNXIcon(NXIconType::Envelope);
  NXMenu *splitMenu = new NXMenu(this);
  splitMenu->addNXIconAction(NXIconType::Reply, QStringLiteral("回复"));
  splitMenu->addNXIconAction(NXIconType::ReplyAll, QStringLiteral("全部回复"));
  splitMenu->addNXIconAction(NXIconType::Share, QStringLiteral("转发"));
  _splitButton->setMenu(splitMenu);

  NXScrollPageArea *splitArea = new NXScrollPageArea(this);
  QHBoxLayout *splitLayout    = new QHBoxLayout(splitArea);
  splitLayout->addWidget(new NXText(QStringLiteral("NXSplitButton"), 15, this));
  splitLayout->addWidget(_splitButton);
  splitLayout->addStretch();

  // ========== NXPersonPicture 示例 ==========
  _personPicture = new NXPersonPicture(this);
  _personPicture->setPicture(QPixmap(QStringLiteral(":/Resource/Image/Cirno.jpg")));
  _personPictureInitials = new NXPersonPicture(this);
  _personPictureInitials->setDisplayName(QStringLiteral("NX"));
  _personPictureDefault = new NXPersonPicture(this);

  NXScrollPageArea *personArea = new NXScrollPageArea(this);
  QHBoxLayout *personLayout    = new QHBoxLayout(personArea);
  personLayout->addWidget(new NXText(QStringLiteral("NXPersonPicture"), 15, this));
  personLayout->addSpacing(10);
  personLayout->addWidget(_personPicture);
  personLayout->addSpacing(10);
  personLayout->addWidget(_personPictureInitials);
  personLayout->addSpacing(10);
  personLayout->addWidget(_personPictureDefault);
  personLayout->addStretch();

  // ========== NXRatingControl 示例 ==========
  _ratingControl = new NXRatingControl(this);
  _ratingControl->setRating(5);
  NXText *ratingValue = new NXText(QStringLiteral("5"), this);
  ratingValue->setTextPixelSize(15);
  connect(_ratingControl, &NXRatingControl::ratingChanged, this, [=](int r) {
    ratingValue->setText(QString::number(r));
  });

  NXScrollPageArea *ratingArea = new NXScrollPageArea(this);
  QHBoxLayout *ratingLayout    = new QHBoxLayout(ratingArea);
  ratingLayout->addWidget(new NXText(QStringLiteral("NXRatingControl"), 15, this));
  ratingLayout->addWidget(_ratingControl);
  ratingLayout->addWidget(ratingValue);
  ratingLayout->addStretch();

  // ========== NXDivider 示例 ==========
  NXDivider *divider1 = new NXDivider(this);
  NXDivider *divider2 = new NXDivider(QStringLiteral("分隔文字"), this);

  // ========== NXFlyout 示例 ==========
  _flyout = new NXFlyout(this);
  _flyout->setTitle(QStringLiteral("提示"));
  _flyout->setContent(QStringLiteral("轻量级弹出面板，点击外部自动关闭。"));
  NXPushButton *flyoutBtn = new NXPushButton(QStringLiteral("显示 Flyout"), this);
  flyoutBtn->setFixedSize(120, 38);
  connect(flyoutBtn, &NXPushButton::clicked, this, [=]() {
    _flyout->showFlyout(flyoutBtn);
  });

  // 自定义 Widget Flyout
  NXFlyout *customFlyout       = new NXFlyout(this);
  QWidget *customContent       = new QWidget();
  QVBoxLayout *customFlyoutLay = new QVBoxLayout(customContent);
  customFlyoutLay->setContentsMargins(0, 0, 0, 0);
  customFlyoutLay->addWidget(new NXText(QStringLiteral("请为本次体验评分:"), 13, customContent));
  customFlyoutLay->addWidget(new NXRatingControl(customContent));
  customFlyout->setContentWidget(customContent);

  NXPushButton *customFlyoutBtn = new NXPushButton(QStringLiteral("自定义 Flyout"), this);
  customFlyoutBtn->setFixedSize(130, 38);
  connect(customFlyoutBtn, &NXPushButton::clicked, this, [=]() {
    customFlyout->showFlyout(customFlyoutBtn);
  });

  NXScrollPageArea *flyoutArea = new NXScrollPageArea(this);
  QHBoxLayout *flyoutLayout    = new QHBoxLayout(flyoutArea);
  flyoutLayout->addWidget(new NXText(QStringLiteral("NXFlyout"), 15, this));
  flyoutLayout->addWidget(flyoutBtn);
  flyoutLayout->addSpacing(10);
  flyoutLayout->addWidget(customFlyoutBtn);
  flyoutLayout->addStretch();

  // ========== NXSkeleton 示例 ==========
  NXSkeleton *skelCircle = new NXSkeleton(this);
  skelCircle->setSkeletonType(NXSkeleton::Circle);
  skelCircle->setFixedSize(40, 40);
  NXSkeleton *skelText1 = new NXSkeleton(this);
  skelText1->setSkeletonType(NXSkeleton::Text);
  skelText1->setFixedSize(250, 16);
  NXSkeleton *skelRect = new NXSkeleton(this);
  skelRect->setSkeletonType(NXSkeleton::Rectangle);
  skelRect->setFixedSize(200, 80);
  skelRect->setBorderRadius(8);

  NXScrollPageArea *skelArea = new NXScrollPageArea(this);
  QHBoxLayout *skelLayout    = new QHBoxLayout(skelArea);
  skelLayout->addWidget(new NXText(QStringLiteral("NXSkeleton"), 15, this));
  skelLayout->addSpacing(10);
  skelLayout->addWidget(skelCircle);
  skelLayout->addSpacing(10);
  skelLayout->addWidget(skelText1);
  skelLayout->addSpacing(10);
  skelLayout->addWidget(skelRect);
  skelLayout->addStretch();

  // ========== NXSteps 示例 ==========
  _steps = new NXSteps(this);
  _steps->setStepCount(4);
  _steps->setStepTitles(
      { QStringLiteral("选择商品"), QStringLiteral("确认订单"), QStringLiteral("支付"), QStringLiteral("完成") });
  _steps->setCurrentStep(1);
  NXPushButton *prevBtn = new NXPushButton(QStringLiteral("上一步"), this);
  prevBtn->setFixedSize(80, 32);
  NXPushButton *nextBtn = new NXPushButton(QStringLiteral("下一步"), this);
  nextBtn->setFixedSize(80, 32);
  connect(prevBtn, &NXPushButton::clicked, _steps, &NXSteps::previous);
  connect(nextBtn, &NXPushButton::clicked, _steps, &NXSteps::next);

  QHBoxLayout *stepsCtrl = new QHBoxLayout();
  stepsCtrl->addWidget(new NXText(QStringLiteral("NXSteps"), 15, this));
  stepsCtrl->addStretch();
  stepsCtrl->addWidget(prevBtn);
  stepsCtrl->addWidget(nextBtn);

  // ========== NXCommandBar 示例 ==========
  _commandBar = new NXCommandBar(this);
  _commandBar->addItem({ NXIconType::Copy, QStringLiteral("复制") });
  _commandBar->addItem({ NXIconType::Paste, QStringLiteral("粘贴") });
  _commandBar->addItem({ NXIconType::Scissors, QStringLiteral("剪切") });
  _commandBar->addSeparator();
  _commandBar->addItem({ NXIconType::ArrowRotateLeft, QStringLiteral("撤销") });
  _commandBar->addItem({ NXIconType::ArrowRotateRight, QStringLiteral("重做") });
  _commandBar->addSeparator();
  _commandBar->addItem({ NXIconType::FloppyDisk, QStringLiteral("保存") });

  NXScrollPageArea *cmdArea = new NXScrollPageArea(this);
  QHBoxLayout *cmdLayout    = new QHBoxLayout(cmdArea);
  cmdLayout->addWidget(new NXText(QStringLiteral("NXCommandBar"), 15, this));
  cmdLayout->addWidget(_commandBar);
  cmdLayout->addStretch();

  // ========== NXPagination 示例 ==========
  QStackedWidget *pageStack = new QStackedWidget(this);
  pageStack->setFixedHeight(160);
  int totalPages = 50;
  for (int p = 1; p <= totalPages; p++) {
    QWidget *page     = new QWidget(this);
    QVBoxLayout *pLay = new QVBoxLayout(page);
    pLay->setContentsMargins(15, 10, 15, 10);
    pLay->addWidget(new NXText(QString(QStringLiteral("第 %1 页")).arg(p), 16, this));
    for (int i = 1; i <= 5; i++)
      pLay->addWidget(new NXText(QString(QStringLiteral("  数据项 #%1")).arg((p - 1) * 4 + i), 13, this));
    pLay->addStretch();
    pageStack->addWidget(page);
  }
  _pagination = new NXPagination(this);
  _pagination->setTotalPages(totalPages);
  _pagination->setCurrentPage(1);
  _pagination->setJumperVisible(true);
  NXText *pageInfo = new NXText(QString(QStringLiteral("第 1 / %1 页")).arg(totalPages), this);
  pageInfo->setTextPixelSize(13);
  connect(_pagination, &NXPagination::currentPageChanged, this, [=](int pg) {
    pageStack->setCurrentIndex(pg - 1);
    pageInfo->setText(QString(QStringLiteral("第 %1 / %2 页")).arg(pg).arg(totalPages));
  });

  NXScrollPageArea *pgArea = new NXScrollPageArea(this);
  QHBoxLayout *pgLayout    = new QHBoxLayout(pgArea);
  pgLayout->addWidget(new NXText(QStringLiteral("NXPagination"), 15, this));
  pgLayout->addWidget(_pagination);
  pgLayout->addWidget(pageInfo);
  pgLayout->addStretch();

  // ========== NXTimeline 示例 ==========
  _timeline = new NXTimeline(this);
  _timeline->addItem({ QStringLiteral("项目启动"), QStringLiteral("完成需求分析和技术选型"), QStringLiteral("2026-01-15"), NXIconType::RocketLaunch });
  _timeline->addItem({ QStringLiteral("开发阶段"), QStringLiteral("完成核心模块开发"), QStringLiteral("2026-02-20"), NXIconType::Code });
  _timeline->addItem(
      { QStringLiteral("测试阶段"), QStringLiteral("集成测试和验收测试"), QStringLiteral("2026-03-01") });
  _timeline->addItem({ QStringLiteral("发布上线"), QStringLiteral("部署到生产环境"), QStringLiteral("2026-03-15") });

  // ========== NXToast 示例 ==========
  NXPushButton *toastSuccessBtn = new NXPushButton(QStringLiteral("成功"), this);
  toastSuccessBtn->setFixedSize(70, 32);
  connect(toastSuccessBtn, &NXPushButton::clicked, this, [=]() {
    NXToast::success(QStringLiteral("操作成功完成!"), 2000, window());
  });
  NXPushButton *toastInfoBtn = new NXPushButton(QStringLiteral("信息"), this);
  toastInfoBtn->setFixedSize(70, 32);
  connect(toastInfoBtn, &NXPushButton::clicked, this, [=]() {
    NXToast::info(QStringLiteral("这是一条提示信息"), 2000, window());
  });
  NXPushButton *toastWarnBtn = new NXPushButton(QStringLiteral("警告"), this);
  toastWarnBtn->setFixedSize(70, 32);
  connect(toastWarnBtn, &NXPushButton::clicked, this, [=]() {
    NXToast::warning(QStringLiteral("请注意检查输入"), 2000, window());
  });
  NXPushButton *toastErrorBtn = new NXPushButton(QStringLiteral("错误"), this);
  toastErrorBtn->setFixedSize(70, 32);
  connect(toastErrorBtn, &NXPushButton::clicked, this, [=]() {
    NXToast::error(QStringLiteral("操作失败，请重试"), 2000, window());
  });

  NXScrollPageArea *toastArea = new NXScrollPageArea(this);
  QHBoxLayout *toastLayout    = new QHBoxLayout(toastArea);
  toastLayout->addWidget(new NXText(QStringLiteral("NXToast"), 15, this));
  toastLayout->addWidget(toastSuccessBtn);
  toastLayout->addWidget(toastInfoBtn);
  toastLayout->addWidget(toastWarnBtn);
  toastLayout->addWidget(toastErrorBtn);
  toastLayout->addStretch();

  // ========== NXWizard 示例 ==========
  _wizard           = new NXWizard(this);
  QWidget *wizPage1 = new QWidget();
  QVBoxLayout *wl1  = new QVBoxLayout(wizPage1);
  wl1->addWidget(new NXText(QStringLiteral("欢迎使用安装向导"), 16, wizPage1));
  wl1->addWidget(new NXText(QStringLiteral("本向导将引导你完成软件安装过程。"), 13, wizPage1));
  wl1->addStretch();
  _wizard->addStep(QStringLiteral("欢迎"), wizPage1);

  QWidget *wizPage2 = new QWidget();
  QVBoxLayout *wl2  = new QVBoxLayout(wizPage2);
  wl2->addWidget(new NXText(QStringLiteral("请选择安装路径"), 16, wizPage2));
  NXLineEdit *pathEdit = new NXLineEdit(wizPage2);
  pathEdit->setText(QStringLiteral("/usr/local/NexUs"));
  pathEdit->setFixedHeight(35);
  wl2->addWidget(pathEdit);
  wl2->addStretch();
  _wizard->addStep(QStringLiteral("路径"), wizPage2);

  QWidget *wizPage3 = new QWidget();
  QVBoxLayout *wl3  = new QVBoxLayout(wizPage3);
  wl3->addWidget(new NXText(QStringLiteral("安装完成！"), 16, wizPage3));
  wl3->addWidget(new NXText(QStringLiteral("所有组件已成功安装。"), 13, wizPage3));
  wl3->addStretch();
  _wizard->addStep(QStringLiteral("完成"), wizPage3);

  NXPushButton *wizardBtn = new NXPushButton(QStringLiteral("打开向导"), this);
  wizardBtn->setFixedSize(100, 38);
  connect(wizardBtn, &NXPushButton::clicked, this, [=]() {
    _wizard->show();
  });

  NXScrollPageArea *wizardArea = new NXScrollPageArea(this);
  QHBoxLayout *wizardLayout    = new QHBoxLayout(wizardArea);
  wizardLayout->addWidget(new NXText(QStringLiteral("NXWizard"), 15, this));
  wizardLayout->addWidget(wizardBtn);
  wizardLayout->addStretch();

  // ========== NXNotificationCenter 示例 ==========
  _notificationCenter = new NXNotificationCenter(this);
  _notificationCenter->addNotification(
      { QStringLiteral("系统更新"), QStringLiteral("NexUs 已更新到 v2.1"), QStringLiteral("10:30") });
  _notificationCenter->addNotification({ QStringLiteral("构建完成"), QStringLiteral("项目编译成功，耗时 12 秒"), QStringLiteral("10:25"), NXIconType::Check });
  _notificationCenter->addNotification({ QStringLiteral("新消息"), QStringLiteral("收到来自 Qt 社区的新回复"), QStringLiteral("09:45"), NXIconType::Envelope });

  NXPushButton *notifBtn = new NXPushButton(QStringLiteral("通知中心"), this);
  notifBtn->setFixedSize(100, 38);
  connect(notifBtn, &NXPushButton::clicked, this, [=]() {
    if (_notificationCenter->isPanelVisible())
      _notificationCenter->hidePanel();
    else
      _notificationCenter->showPanel(window());
  });

  NXScrollPageArea *notifArea = new NXScrollPageArea(this);
  QHBoxLayout *notifLayout    = new QHBoxLayout(notifArea);
  notifLayout->addWidget(new NXText(QStringLiteral("NXNotificationCenter"), 15, this));
  notifLayout->addWidget(notifBtn);
  notifLayout->addStretch();

  // ========== NXMarkdownViewer 示例 ==========
  _markdownViewer = new NXMarkdownViewer(this);
  _markdownViewer->setFixedHeight(200);
  _markdownViewer->setMarkdown(QStringLiteral("# NexUs\n\n"
                                              "一个 **FluentUI** 风格的 Qt 组件库。\n\n"
                                              "## 特性\n\n"
                                              "- 支持 `Light` / `Dark` 主题切换\n"
                                              "- **90+** 个组件\n"
                                              "- 跨平台: Windows / macOS / Linux\n\n"
                                              "## 代码示例\n\n"
                                              "```cpp\nNXToast::success(\"Hello World!\");\n```\n\n"
                                              "> 更多详情请访问 [GitHub](https://github.com/RainbowCandyX/NexUs)\n"));

  // ========== NXCaptcha 示例 ==========
  _captcha = new NXCaptcha(this);
  _captcha->setInputMode(NXCaptcha::DigitOnly);
  NXText *captchaResult = new NXText(QStringLiteral(""), this);
  _captcha->setCodeLength(4);
  captchaResult->setTextPixelSize(13);
  connect(_captcha, &NXCaptcha::codeCompleted, this, [=](QString const &code) {
    captchaResult->setText(QStringLiteral("输入完成: ") + code);
  });
  connect(_captcha, &NXCaptcha::codeChanged, this, [=](QString const &code) {
    if (code.length() < _captcha->getCodeLength())
      captchaResult->setText(QStringLiteral(""));
  });

  NXPushButton *captchaClearBtn = new NXPushButton(QStringLiteral("清除"), this);
  captchaClearBtn->setFixedSize(60, 32);
  connect(captchaClearBtn, &NXPushButton::clicked, _captcha, &NXCaptcha::clear);

  NXScrollPageArea *captchaArea = new NXScrollPageArea(this);
  QHBoxLayout *captchaLayout    = new QHBoxLayout(captchaArea);
  captchaLayout->addWidget(new NXText(QStringLiteral("NXCaptcha"), 15, this));
  captchaLayout->addWidget(_captcha);
  captchaLayout->addWidget(captchaClearBtn);
  captchaLayout->addWidget(captchaResult);
  captchaLayout->addStretch();

  // ========== NXStatCard 示例 ==========
  NXStatCard *statCard1 = new NXStatCard(this);
  statCard1->setTitle(QStringLiteral("今日销售额"));
  statCard1->setValue(QStringLiteral("¥86, 400"));
  statCard1->setCardIcon(NXIconType::CartShopping);
  statCard1->setTrend(NXStatCard::Up);
  statCard1->setTrendText(QStringLiteral("+15.3%"));
  statCard1->setDescription(QStringLiteral("较昨日"));

  NXStatCard *statCard2 = new NXStatCard(this);
  statCard2->setTitle(QStringLiteral("CPU 使用率"));
  statCard2->setValue(QStringLiteral("62%"));
  statCard2->setCardIcon(NXIconType::Gauge);
  statCard2->setTrend(NXStatCard::Down);
  statCard2->setTrendText(QStringLiteral("-8.1%"));
  statCard2->setDescription(QStringLiteral("较上小时"));

  NXStatCard *statCard3 = new NXStatCard(this);
  statCard3->setTitle(QStringLiteral("在线设备"));
  statCard3->setValue(QStringLiteral("1, 024"));
  statCard3->setCardIcon(NXIconType::WifiExclamation);
  statCard3->setTrend(NXStatCard::Neutral);
  statCard3->setTrendText(QStringLiteral("稳定"));
  statCard3->setDescription(QStringLiteral("当前在线"));

  QHBoxLayout *statLayout = new QHBoxLayout();
  statLayout->addWidget(new NXText(QStringLiteral("NXStatCard"), 15, this));
  statLayout->addSpacing(10);
  statLayout->addWidget(statCard1);
  statLayout->addWidget(statCard2);
  statLayout->addWidget(statCard3);
  statLayout->addStretch();

  // ========== NXDropDownButton 示例 ==========
  _dropDownButton = new NXDropDownButton(this);
  _dropDownButton->setText(QStringLiteral("排序方式"));
  _dropDownButton->setNXIcon(NXIconType::BarsSort);
  NXMenu *dropMenu = new NXMenu(this);
  dropMenu->addNXIconAction(NXIconType::ArrowDownAZ, QStringLiteral("按名称"));
  dropMenu->addNXIconAction(NXIconType::Calendar, QStringLiteral("按日期"));
  dropMenu->addNXIconAction(NXIconType::ArrowDown19, QStringLiteral("按大小"));
  _dropDownButton->setMenu(dropMenu);

  NXDropDownButton *dropBtn2 = new NXDropDownButton(this);
  dropBtn2->setText(QStringLiteral("导出"));
  NXMenu *dropMenu2 = new NXMenu(this);
  dropMenu2->addAction(QStringLiteral("PDF"));
  dropMenu2->addAction(QStringLiteral("Excel"));
  dropMenu2->addAction(QStringLiteral("CSV"));
  dropBtn2->setMenu(dropMenu2);

  NXScrollPageArea *dropArea = new NXScrollPageArea(this);
  QHBoxLayout *dropLayout    = new QHBoxLayout(dropArea);
  dropLayout->addWidget(new NXText(QStringLiteral("NXDropDownButton"), 15, this));
  dropLayout->addWidget(_dropDownButton);
  dropLayout->addSpacing(10);
  dropLayout->addWidget(dropBtn2);
  dropLayout->addStretch();

  // ========== NXSelectorBar 示例 ==========
  _selectorBar = new NXSelectorBar(this);
  _selectorBar->addItem(QStringLiteral("全部"));
  _selectorBar->addItem(QStringLiteral("未读"));
  _selectorBar->addItem(QStringLiteral("已标记"));
  _selectorBar->addItem(QStringLiteral("归档"));

  NXSelectorBar *viewSelector = new NXSelectorBar(this);
  viewSelector->addItem(NXIconType::List, QStringLiteral("列表"));
  viewSelector->addItem(NXIconType::Grid2, QStringLiteral("网格"));
  viewSelector->addItem(NXIconType::TableCells, QStringLiteral("详情"));

  NXText *selectorInfo = new NXText(QStringLiteral("选中: 全部"), this);
  selectorInfo->setTextPixelSize(13);
  connect(_selectorBar, &NXSelectorBar::currentIndexChanged, this, [=](int idx) {
    QStringList names = { QStringLiteral("全部"), QStringLiteral("未读"), QStringLiteral("已标记"),
                          QStringLiteral("归档") };
    selectorInfo->setText(QStringLiteral("选中: ") + names.value(idx));
  });

  NXScrollPageArea *selectorArea = new NXScrollPageArea(this);
  QHBoxLayout *selectorLayout    = new QHBoxLayout(selectorArea);
  selectorLayout->addWidget(new NXText(QStringLiteral("NXSelectorBar"), 15, this));
  selectorLayout->addWidget(_selectorBar);
  selectorLayout->addWidget(selectorInfo);
  selectorLayout->addSpacing(10);
  selectorLayout->addWidget(viewSelector);
  selectorLayout->addStretch();

  // ========== NXVirtualList 示例 ==========
  _virtualList                     = new NXVirtualList(this);
  QStandardItemModel *virtualModel = new QStandardItemModel(this);
  for (int i = 0; i < 10000; ++i) {
    QStandardItem *item = new QStandardItem(QString(QStringLiteral("虚拟列表项 #%1 - 支持万级数据量")).arg(i + 1));
    virtualModel->appendRow(item);
  }
  _virtualList->setModel(virtualModel);

  NXText *virtualListInfo = new NXText(QStringLiteral("已加载 10,000 条数据"), this);
  virtualListInfo->setTextPixelSize(13);
  virtualListInfo->setMinimumWidth(200);

  NXScrollPageArea *virtualListArea = new NXScrollPageArea(this);
  virtualListArea->setFixedHeight(200);
  QVBoxLayout *virtualListLayout = new QVBoxLayout(virtualListArea);
  QHBoxLayout *virtualListHeader = new QHBoxLayout();
  virtualListHeader->addWidget(new NXText(QStringLiteral("NXVirtualList"), 15, this));
  virtualListHeader->addWidget(virtualListInfo);
  virtualListHeader->addStretch();
  virtualListLayout->addLayout(virtualListHeader);
  virtualListLayout->addWidget(_virtualList);

  // ========== NXSplashScreen 示例 ==========
  _splashScreen = new NXSplashScreen(this);
  _splashScreen->setTitle(QStringLiteral("NexUs"));
  _splashScreen->setSubTitle(QStringLiteral("FluentUI 风格 Qt 组件库"));
  _splashScreen->setStatusText(QStringLiteral("正在初始化..."));
  _splashScreen->setIsShowProgressRing(true);

  NXPushButton *splashBtn = new NXPushButton(QStringLiteral("显示启动屏"), this);
  splashBtn->setFixedSize(120, 38);
  connect(splashBtn, &NXPushButton::clicked, this, [=]() {
    _splashScreen->setValue(0);
    _splashScreen->setStatusText(QStringLiteral("正在初始化..."));
    _splashScreen->show();
    // 模拟加载过程
    QTimer *timer = new QTimer(this);
    int *progress = new int(0);
    connect(timer, &QTimer::timeout, this, [=]() {
      *progress += 5;
      _splashScreen->setValue(*progress);
      if (*progress >= 30)
        _splashScreen->setStatusText(QStringLiteral("正在加载组件..."));
      if (*progress >= 60)
        _splashScreen->setStatusText(QStringLiteral("正在准备界面..."));
      if (*progress >= 100) {
        timer->stop();
        timer->deleteLater();
        _splashScreen->setStatusText(QStringLiteral("加载完成!"));
        QTimer::singleShot(500, this, [=]() {
          _splashScreen->close();
          delete progress;
        });
      }
    });
    timer->start(80);
  });

  NXScrollPageArea *splashArea = new NXScrollPageArea(this);
  QHBoxLayout *splashLayout    = new QHBoxLayout(splashArea);
  splashLayout->addWidget(new NXText(QStringLiteral("NXSplashScreen"), 15, this));
  splashLayout->addWidget(splashBtn);
  splashLayout->addStretch();

  // ========== NXSheetPanel 示例 ==========
  _sheetPanel = nullptr; // 延迟创建，构造时 window() 可能还未就绪

  NXPushButton *sheetPeekBtn = new NXPushButton(QStringLiteral("Peek"), this);
  sheetPeekBtn->setFixedSize(70, 32);
  connect(sheetPeekBtn, &NXPushButton::clicked, this, [=]() {
    if (!_sheetPanel) {
      _sheetPanel = new NXSheetPanel(window());
      _sheetPanel->setDragHandleVisible(true);
      _sheetPanel->setCloseOnOverlayClick(true);
      QWidget *sheetContent           = new QWidget();
      QVBoxLayout *sheetContentLayout = new QVBoxLayout(sheetContent);
      sheetContentLayout->setContentsMargins(20, 10, 20, 10);
      sheetContentLayout->addWidget(new NXText(QStringLiteral("Sheet 面板内容"), 16, sheetContent));
      sheetContentLayout->addWidget(
          new NXText(QStringLiteral("这是一个从底部滑出的半模态面板，支持拖拽调整高度。"), 13, sheetContent));
      sheetContentLayout->addWidget(new NXText(QStringLiteral("可以放置表单、详情或任意内容。"), 13, sheetContent));
      NXLineEdit *sheetInput = new NXLineEdit(sheetContent);
      sheetInput->setPlaceholderText(QStringLiteral("在 Sheet 中输入内容..."));
      sheetInput->setFixedHeight(35);
      sheetContentLayout->addWidget(sheetInput);
      sheetContentLayout->addStretch();
      _sheetPanel->setCentralWidget(sheetContent);
    }
    _sheetPanel->open(NXSheetPanelType::Peek);
  });
  NXPushButton *sheetHalfBtn = new NXPushButton(QStringLiteral("Half"), this);
  sheetHalfBtn->setFixedSize(70, 32);
  connect(sheetHalfBtn, &NXPushButton::clicked, this, [=]() {
    if (!_sheetPanel) {
      sheetPeekBtn->click();
      return;
    }
    _sheetPanel->open(NXSheetPanelType::Half);
  });
  NXPushButton *sheetFullBtn = new NXPushButton(QStringLiteral("Full"), this);
  sheetFullBtn->setFixedSize(70, 32);
  connect(sheetFullBtn, &NXPushButton::clicked, this, [=]() {
    if (!_sheetPanel) {
      sheetPeekBtn->click();
      return;
    }
    _sheetPanel->open(NXSheetPanelType::Full);
  });

  NXScrollPageArea *sheetArea = new NXScrollPageArea(this);
  QHBoxLayout *sheetLayout    = new QHBoxLayout(sheetArea);
  sheetLayout->addWidget(new NXText(QStringLiteral("NXSheetPanel"), 15, this));
  sheetLayout->addWidget(sheetPeekBtn);
  sheetLayout->addWidget(sheetHalfBtn);
  sheetLayout->addWidget(sheetFullBtn);
  sheetLayout->addStretch();


  // ========== NXTransfer 示例 ==========
  _transfer = new NXTransfer(this);
  _transfer->setFixedHeight(280);
  _transfer->setSourceTitle(QStringLiteral("可选项目"));
  _transfer->setTargetTitle(QStringLiteral("已选项目"));
  QStringList transferItems;
  for (int i = 1; i <= 20; ++i) {
    transferItems.append(QString(QStringLiteral("项目 %1")).arg(i));
  }
  _transfer->setSourceItems(transferItems);

  NXText *transferResult = new NXText(QStringLiteral("已选: 0 项"), this);
  transferResult->setTextPixelSize(13);
  connect(_transfer, &NXTransfer::transferChanged, this, [=](QStringList const &, QStringList const &target) {
    transferResult->setText(QString(QStringLiteral("已选: %1 项")).arg(target.size()));
  });

  QHBoxLayout *transferHeader = new QHBoxLayout();
  transferHeader->addWidget(new NXText(QStringLiteral("NXTransfer"), 15, this));
  transferHeader->addWidget(transferResult);
  transferHeader->addStretch();

  // ========== NXSpotlight 示例 ==========
  _spotlight = nullptr; // 延迟创建

  NXPushButton *spotlightSingleBtn = new NXPushButton(QStringLiteral("单目标聚光"), this);
  spotlightSingleBtn->setFixedSize(110, 38);
  connect(spotlightSingleBtn, &NXPushButton::clicked, this, [=]() {
    if (!_spotlight) {
      _spotlight = new NXSpotlight(window());
    }
    _spotlight->setTitle(QStringLiteral("NXSplashScreen"));
    _spotlight->setContent(QStringLiteral("点击此按钮可以展示启动屏效果。"));
    _spotlight->showSpotlight(splashBtn, QStringLiteral("我知道了"));
  });

  NXPushButton *spotlightTourBtn = new NXPushButton(QStringLiteral("多步骤引导"), this);
  spotlightTourBtn->setFixedSize(110, 38);
  connect(spotlightTourBtn, &NXPushButton::clicked, this, [=]() {
    if (!_spotlight) {
      _spotlight = new NXSpotlight(window());
    }
    QList<NXSpotlight::SpotlightStep> steps;
    steps.append({ splashBtn, QStringLiteral("启动屏"), QStringLiteral("展示应用启动画面效果"), false });
    steps.append({ sheetPeekBtn, QStringLiteral("底部面板"), QStringLiteral("从底部滑出半模态面板"), false });
    steps.append({ spotlightSingleBtn, QStringLiteral("聚光灯"), QStringLiteral("高亮任意目标区域"), false });
    _spotlight->setSteps(steps);
    _spotlight->start();
  });

  NXScrollPageArea *spotlightArea = new NXScrollPageArea(this);
  QHBoxLayout *spotlightLayout    = new QHBoxLayout(spotlightArea);
  spotlightLayout->addWidget(new NXText(QStringLiteral("NXSpotlight"), 15, this));
  spotlightLayout->addWidget(spotlightSingleBtn);
  spotlightLayout->addWidget(spotlightTourBtn);
  spotlightLayout->addStretch();


  // ========== NXCountdown 示例 ==========
  _countdown = new NXCountdown(this);
  _countdown->setRemainingSeconds(86400 + 3661);
  _countdown->setIsShowDays(true);

  NXPushButton *countdownStartBtn = new NXPushButton(QStringLiteral("开始"), this);
  countdownStartBtn->setFixedSize(60, 32);
  NXPushButton *countdownPauseBtn = new NXPushButton(QStringLiteral("暂停"), this);
  countdownPauseBtn->setFixedSize(60, 32);
  NXPushButton *countdownResetBtn = new NXPushButton(QStringLiteral("重置"), this);
  countdownResetBtn->setFixedSize(60, 32);
  connect(countdownStartBtn, &NXPushButton::clicked, this, [=]() {
    if (_countdown->isRunning())
      _countdown->resume();
    else
      _countdown->start();
  });
  connect(countdownPauseBtn, &NXPushButton::clicked, _countdown, &NXCountdown::pause);
  connect(countdownResetBtn, &NXPushButton::clicked, this, [=]() {
    _countdown->stop();
    _countdown->setRemainingSeconds(86400 + 3661);
  });

  NXCountdown *countdownTarget = new NXCountdown(this);
  countdownTarget->setIsShowDays(false);
  countdownTarget->setTargetDateTime(QDateTime::currentDateTime().addSecs(7200));
  countdownTarget->start();

  NXScrollPageArea *countdownArea = new NXScrollPageArea(this);
  countdownArea->setFixedHeight(130);
  QVBoxLayout *countdownMainLayout = new QVBoxLayout(countdownArea);
  QHBoxLayout *countdownHeader     = new QHBoxLayout();
  countdownHeader->addWidget(new NXText(QStringLiteral("NXCountdown"), 15, this));
  countdownHeader->addWidget(countdownStartBtn);
  countdownHeader->addWidget(countdownPauseBtn);
  countdownHeader->addWidget(countdownResetBtn);
  countdownHeader->addStretch();
  countdownMainLayout->addLayout(countdownHeader);
  QHBoxLayout *countdownBody = new QHBoxLayout();
  countdownBody->addWidget(_countdown);
  countdownBody->addSpacing(20);
  countdownBody->addWidget(new NXText(QStringLiteral("目标时间 (2小时后):"), 13, this));
  countdownBody->addWidget(countdownTarget);
  countdownBody->addStretch();
  countdownMainLayout->addLayout(countdownBody);

  // ========== NXPopconfirm 示例 ==========
  _popconfirm = new NXPopconfirm(this);
  _popconfirm->setTitle(QStringLiteral("确认删除"));
  _popconfirm->setContent(QStringLiteral("删除后将无法恢复，是否继续？"));

  NXPushButton *popconfirmBtn = new NXPushButton(QStringLiteral("删除项目"), this);
  popconfirmBtn->setFixedSize(100, 38);
  connect(popconfirmBtn, &NXPushButton::clicked, this, [=]() {
    _popconfirm->showPopconfirm(popconfirmBtn);
  });

  NXText *popconfirmResult = new NXText(QStringLiteral(""), this);
  popconfirmResult->setTextPixelSize(13);
  connect(_popconfirm, &NXPopconfirm::confirmed, this, [=]() {
    popconfirmResult->setText(QStringLiteral("已确认删除"));
  });
  connect(_popconfirm, &NXPopconfirm::cancelled, this, [=]() {
    popconfirmResult->setText(QStringLiteral("已取消"));
  });

  NXPopconfirm *customPopconfirm = new NXPopconfirm(this);
  customPopconfirm->setTitle(QStringLiteral("提交变更"));
  customPopconfirm->setContent(QStringLiteral("确定要提交到远程仓库吗？"));
  customPopconfirm->setConfirmButtonText(QStringLiteral("提交"));
  customPopconfirm->setCancelButtonText(QStringLiteral("再想想"));
  customPopconfirm->setIcon(NXIconType::CloudArrowUp);

  NXPushButton *customPopconfirmBtn = new NXPushButton(QStringLiteral("提交代码"), this);
  customPopconfirmBtn->setFixedSize(100, 38);
  connect(customPopconfirmBtn, &NXPushButton::clicked, this, [=]() {
    customPopconfirm->showPopconfirm(customPopconfirmBtn);
  });

  NXScrollPageArea *popconfirmArea = new NXScrollPageArea(this);
  QHBoxLayout *popconfirmLayout    = new QHBoxLayout(popconfirmArea);
  popconfirmLayout->addWidget(new NXText(QStringLiteral("NXPopconfirm"), 15, this));
  popconfirmLayout->addWidget(popconfirmBtn);
  popconfirmLayout->addWidget(popconfirmResult);
  popconfirmLayout->addSpacing(10);
  popconfirmLayout->addWidget(customPopconfirmBtn);
  popconfirmLayout->addStretch();

  // ========== NXWatermark 示例 ==========
  _watermark = new NXWatermark(QStringLiteral("NexUs"), window());
  _watermark->setVisible(false);

  NXPushButton *watermarkShowBtn = new NXPushButton(QStringLiteral("显示"), this);
  watermarkShowBtn->setFixedSize(60, 36);
  NXPushButton *watermarkHideBtn = new NXPushButton(QStringLiteral("隐藏"), this);
  watermarkHideBtn->setFixedSize(60, 36);
  connect(watermarkShowBtn, &NXPushButton::clicked, this, [=]() {
    _watermark->setVisible(true);
  });
  connect(watermarkHideBtn, &NXPushButton::clicked, this, [=]() {
    _watermark->setVisible(false);
  });

  NXLineEdit *watermarkTextEdit = new NXLineEdit(this);
  watermarkTextEdit->setText(QStringLiteral("NexUs"));
  watermarkTextEdit->setFixedWidth(140);
  connect(watermarkTextEdit, &NXLineEdit::textChanged, this, [=](QString const &text) {
    _watermark->setText(text);
  });

  NXSlider *watermarkOpacitySlider = new NXSlider(this);
  watermarkOpacitySlider->setRange(1, 100);
  watermarkOpacitySlider->setValue(50);
  watermarkOpacitySlider->setFixedWidth(120);
  connect(watermarkOpacitySlider, &NXSlider::valueChanged, this, [=](int v) {
    _watermark->setOpacity(v / 100.0);
  });

  NXSlider *watermarkRotationSlider = new NXSlider(this);
  watermarkRotationSlider->setRange(-90, 90);
  watermarkRotationSlider->setValue(-22);
  watermarkRotationSlider->setFixedWidth(120);
  connect(watermarkRotationSlider, &NXSlider::valueChanged, this, [=](int v) {
    _watermark->setRotation(v);
  });

  NXScrollPageArea *watermarkArea = new NXScrollPageArea(this);
  QHBoxLayout *watermarkLayout    = new QHBoxLayout(watermarkArea);
  watermarkLayout->addWidget(new NXText(QStringLiteral("NXWatermark"), 15, this));
  watermarkLayout->addSpacing(20);
  watermarkLayout->addWidget(watermarkShowBtn);
  watermarkLayout->addWidget(watermarkHideBtn);
  watermarkLayout->addSpacing(10);
  watermarkLayout->addWidget(watermarkTextEdit);
  watermarkLayout->addSpacing(10);
  watermarkLayout->addWidget(new NXText(QStringLiteral("透明度"), 13, this));
  watermarkLayout->addWidget(watermarkOpacitySlider);
  watermarkLayout->addSpacing(10);
  watermarkLayout->addWidget(new NXText(QStringLiteral("角度"), 13, this));
  watermarkLayout->addWidget(watermarkRotationSlider);
  watermarkLayout->addStretch();

  // ========== NXSnackbar 示例 ==========
  NXPushButton *snackSuccessBtn = new NXPushButton(QStringLiteral("成功"), this);
  snackSuccessBtn->setFixedSize(70, 32);
  connect(snackSuccessBtn, &NXPushButton::clicked, this, [=]() {
    NXSnackbar::success(QStringLiteral("文件保存成功!"), QStringLiteral(""), 3000, window());
  });

  NXPushButton *snackUndoBtn = new NXPushButton(QStringLiteral("撤销操作"), this);
  snackUndoBtn->setFixedSize(90, 32);
  connect(snackUndoBtn, &NXPushButton::clicked, this, [=]() {
    NXSnackbar *snackbar = NXSnackbar::info(QStringLiteral("已删除 3 个项目"), QStringLiteral("撤销"), 5000, window());
    connect(snackbar, &NXSnackbar::actionClicked, this, [=]() {
      NXToast::success(QStringLiteral("已撤销删除操作"), 2000, window());
    });
  });

  NXPushButton *snackWarnBtn = new NXPushButton(QStringLiteral("警告"), this);
  snackWarnBtn->setFixedSize(70, 32);
  connect(snackWarnBtn, &NXPushButton::clicked, this, [=]() {
    NXSnackbar::warning(QStringLiteral("磁盘空间不足"), QStringLiteral("查看详情"), 4000, window());
  });

  NXPushButton *snackErrorBtn = new NXPushButton(QStringLiteral("错误"), this);
  snackErrorBtn->setFixedSize(70, 32);
  connect(snackErrorBtn, &NXPushButton::clicked, this, [=]() {
    NXSnackbar::error(QStringLiteral("网络连接失败"), QStringLiteral("重试"), 4000, window());
  });

  NXScrollPageArea *snackbarArea = new NXScrollPageArea(this);
  QHBoxLayout *snackbarLayout    = new QHBoxLayout(snackbarArea);
  snackbarLayout->addWidget(new NXText(QStringLiteral("NXSnackbar"), 15, this));
  snackbarLayout->addWidget(snackSuccessBtn);
  snackbarLayout->addWidget(snackUndoBtn);
  snackbarLayout->addWidget(snackWarnBtn);
  snackbarLayout->addWidget(snackErrorBtn);
  snackbarLayout->addStretch();

  // ========== NXSplitter 示例 ==========
  _splitter = new NXSplitter(Qt::Horizontal, this);
  _splitter->setFixedHeight(200);

  QWidget *splitterLeft = new QWidget(this);
  splitterLeft->setMinimumWidth(120);
  QVBoxLayout *leftLayout = new QVBoxLayout(splitterLeft);
  leftLayout->setContentsMargins(10, 10, 10, 10);
  NXText *leftTitle = new NXText(QStringLiteral("左侧面板"), this);
  leftTitle->setTextPixelSize(14);
  leftLayout->addWidget(leftTitle);
  for (int i = 1; i <= 5; ++i) {
    leftLayout->addWidget(new NXText(QString(QStringLiteral("  列表项 #%1")).arg(i), 13, this));
  }
  leftLayout->addStretch();

  QWidget *splitterRight = new QWidget(this);
  splitterRight->setMinimumWidth(120);
  QVBoxLayout *rightLayout = new QVBoxLayout(splitterRight);
  rightLayout->setContentsMargins(10, 10, 10, 10);
  NXText *rightTitle = new NXText(QStringLiteral("右侧面板"), this);
  rightTitle->setTextPixelSize(14);
  rightLayout->addWidget(rightTitle);
  rightLayout->addWidget(new NXText(QStringLiteral("拖拽中间手柄调整面板大小"), 13, this));
  rightLayout->addStretch();

  _splitter->addWidget(splitterLeft);
  _splitter->addWidget(splitterRight);
  _splitter->setSizes({ 300, 300 });

  NXSplitter *vSplitter = new NXSplitter(Qt::Vertical, this);
  vSplitter->setFixedHeight(200);

  QWidget *splitterTop = new QWidget(this);
  splitterTop->setMinimumHeight(40);
  QVBoxLayout *topLayout = new QVBoxLayout(splitterTop);
  topLayout->setContentsMargins(10, 10, 10, 10);
  topLayout->addWidget(new NXText(QStringLiteral("上方面板"), 14, this));
  topLayout->addStretch();

  QWidget *splitterBottom = new QWidget(this);
  splitterBottom->setMinimumHeight(40);
  QVBoxLayout *bottomLayout = new QVBoxLayout(splitterBottom);
  bottomLayout->setContentsMargins(10, 10, 10, 10);
  bottomLayout->addWidget(new NXText(QStringLiteral("下方面板"), 14, this));
  bottomLayout->addStretch();

  vSplitter->addWidget(splitterTop);
  vSplitter->addWidget(splitterBottom);
  vSplitter->setSizes({ 100, 100 });

  NXScrollPageArea *splitterArea = new NXScrollPageArea(this);
  splitterArea->setFixedHeight(460);
  QVBoxLayout *splitterMainLayout = new QVBoxLayout(splitterArea);
  splitterMainLayout->addWidget(new NXText(QStringLiteral("NXSplitter"), 15, this));
  splitterMainLayout->addSpacing(5);
  splitterMainLayout->addWidget(new NXText(QStringLiteral("水平分割"), 13, this));
  splitterMainLayout->addWidget(_splitter);
  splitterMainLayout->addSpacing(5);
  splitterMainLayout->addWidget(new NXText(QStringLiteral("垂直分割"), 13, this));
  splitterMainLayout->addWidget(vSplitter);

  // ========== NXInfoBar 示例 ==========
  _infoBarInfo = new NXInfoBar(NXInfoBarType::Informational, this);
  _infoBarInfo->setTitle(QStringLiteral("提示"));
  _infoBarInfo->setMessage(QStringLiteral("这是一条信息提示，用于展示一般性的通知内容。"));

  _infoBarSuccess = new NXInfoBar(NXInfoBarType::Success, this);
  _infoBarSuccess->setTitle(QStringLiteral("成功"));
  _infoBarSuccess->setMessage(QStringLiteral("文件已成功保存到指定路径。"));
  _infoBarSuccess->addAction(QStringLiteral("查看"), [=]() {
    qDebug() << "查看按钮被点击";
  });

  _infoBarWarning = new NXInfoBar(NXInfoBarType::Warning, this);
  _infoBarWarning->setTitle(QStringLiteral("警告"));
  _infoBarWarning->setMessage(QStringLiteral("你的许可证将在 7 天后过期。"));
  _infoBarWarning->addAction(QStringLiteral("续费"), [=]() {
    qDebug() << "续费按钮被点击";
  });
  _infoBarWarning->addAction(QStringLiteral("稍后提醒"), [=]() {
    qDebug() << "稍后提醒按钮被点击";
  });

  _infoBarError = new NXInfoBar(NXInfoBarType::Error, this);
  _infoBarError->setTitle(QStringLiteral("错误"));
  _infoBarError->setMessage(QStringLiteral("无法连接到服务器，请检查网络设置。"));
  _infoBarError->setIsClosable(false);
  _infoBarError->addAction(QStringLiteral("重试"), [=]() {
    qDebug() << "重试按钮被点击";
  });

  // ========== 中心布局 ==========
  QVBoxLayout *c = new QVBoxLayout(centralWidget);
  c->setContentsMargins(0, 0, 0, 0);
  c->addWidget(badgeArea);
  c->addWidget(badgeControlArea);
  c->addWidget(severityArea);
  c->addWidget(passwordArea);
  c->addWidget(numberArea);
  c->addWidget(tagArea);
  c->addWidget(tagCloseArea);
  c->addWidget(tagCheckArea);
  c->addWidget(splitArea);
  c->addWidget(personArea);
  c->addWidget(ratingArea);
  c->addWidget(flyoutArea);
  c->addWidget(teachingTipArea);
  c->addWidget(tipPosArea);
  c->addWidget(divider1);
  c->addWidget(skelArea);
  c->addWidget(divider2);
  c->addLayout(stepsCtrl);
  c->addWidget(_steps);
  c->addWidget(cmdArea);
  c->addWidget(pgArea);
  c->addWidget(pageStack);
  c->addWidget(expanderText);
  c->addWidget(_expander);
  c->addWidget(expanderUpText);
  c->addWidget(_expanderUp);
  c->addWidget(captchaArea);
  c->addLayout(statLayout);
  c->addWidget(dropArea);
  c->addWidget(selectorArea);
  c->addWidget(toastArea);
  c->addWidget(wizardArea);
  c->addWidget(notifArea);
  c->addSpacing(5);
  c->addWidget(new NXText(QStringLiteral("NXMarkdownViewer"), 15, this));
  c->addWidget(_markdownViewer);
  c->addSpacing(5);
  c->addWidget(new NXText(QStringLiteral("NXTimeline"), 15, this));
  c->addWidget(_timeline);
  c->addSpacing(10);
  c->addWidget(virtualListArea);
  c->addWidget(splashArea);
  c->addWidget(sheetArea);
  c->addWidget(spotlightArea);
  c->addWidget(countdownArea);
  c->addWidget(popconfirmArea);
  c->addWidget(watermarkArea);
  c->addWidget(snackbarArea);
  c->addWidget(splitterArea);

  // ========== NXQRCode ==========
  _qrCode = new NXQRCode(QStringLiteral("https://github.com/RainbowCandyX/NexUs"), this);
  _qrCode->setFixedSize(180, 180);

  NXLineEdit *qrInput = new NXLineEdit(this);
  qrInput->setText(QStringLiteral("https://github.com/RainbowCandyX/NexUs"));
  qrInput->setFixedHeight(35);
  qrInput->setMinimumWidth(300);
  connect(qrInput, &NXLineEdit::textChanged, this, [=](QString const &text) {
    _qrCode->setText(text);
  });

  NXComboBox *qrEcLevel = new NXComboBox(this);
  qrEcLevel->addItems({ QStringLiteral("低 (7%)"), QStringLiteral("中 (15%)"), QStringLiteral("较高 (25%)"), QStringLiteral("高 (30%)") });
  qrEcLevel->setCurrentIndex(1);
  connect(qrEcLevel, &NXComboBox::currentIndexChanged, this, [=](int index) {
    _qrCode->setErrorCorrectionLevel(static_cast<NXQRCode::ErrorCorrectionLevel>(index));
  });

  NXScrollPageArea *qrArea = new NXScrollPageArea(this);
  qrArea->setFixedHeight(240);
  QVBoxLayout *qrMainLayout = new QVBoxLayout(qrArea);
  QHBoxLayout *qrHeader     = new QHBoxLayout();
  qrHeader->addWidget(new NXText(QStringLiteral("NXQRCode"), 15, this));
  qrHeader->addWidget(qrInput);
  qrHeader->addSpacing(10);
  qrHeader->addWidget(new NXText(QStringLiteral("纠错:"), 13, this));
  qrHeader->addWidget(qrEcLevel);
  qrHeader->addStretch();
  qrMainLayout->addLayout(qrHeader);
  QHBoxLayout *qrBody = new QHBoxLayout();
  qrBody->addWidget(_qrCode);
  qrBody->addStretch();
  qrMainLayout->addLayout(qrBody);
  c->addWidget(qrArea);

  _emojiPicker = new NXEmojiPicker(this);

  NXLineEdit *emojiInput = new NXLineEdit(this);
  emojiInput->setPlaceholderText(QString::fromUtf8("点击表情按钮插入..."));
  emojiInput->setFixedHeight(35);
  emojiInput->setMinimumWidth(300);

  NXPushButton *emojiBtn = new NXPushButton(QString::fromUtf8("😀"), this);
  emojiBtn->setFixedSize(40, 35);
  connect(emojiBtn, &NXPushButton::clicked, this, [=]() {
    _emojiPicker->popup(emojiBtn);
  });
  connect(_emojiPicker, &NXEmojiPicker::emojiSelected, this, [=](QString const &emoji) {
    emojiInput->setText(emojiInput->text() + emoji);
  });

  NXScrollPageArea *emojiArea = new NXScrollPageArea(this);
  QHBoxLayout *emojiLayout    = new QHBoxLayout(emojiArea);
  emojiLayout->addWidget(new NXText(QStringLiteral("NXEmojiPicker"), 15, this));
  emojiLayout->addSpacing(10);
  emojiLayout->addWidget(emojiInput);
  emojiLayout->addWidget(emojiBtn);
  emojiLayout->addStretch();
  c->addWidget(emojiArea);

  _floatButton      = new NXFloatButton(NXIconType::Plus, this);
  NXMenu *floatMenu = new NXMenu(this);
  floatMenu->addNXIconAction(NXIconType::Pen, QStringLiteral("新建笔记"));
  floatMenu->addNXIconAction(NXIconType::Upload, QStringLiteral("上传文件"));
  floatMenu->addNXIconAction(NXIconType::Share, QStringLiteral("分享"));
  _floatButton->setMenu(floatMenu);

  NXScrollPageArea *floatBtnArea = new NXScrollPageArea(this);
  QHBoxLayout *floatBtnLayout    = new QHBoxLayout(floatBtnArea);
  floatBtnLayout->addWidget(new NXText(QStringLiteral("NXFloatButton"), 15, this));
  floatBtnLayout->addSpacing(10);
  floatBtnLayout->addWidget(new NXText(QStringLiteral("位置:"), 13, this));
  NXComboBox *floatPosCombo = new NXComboBox(this);
  floatPosCombo->addItems(
      { QStringLiteral("右下"), QStringLiteral("左下"), QStringLiteral("右上"), QStringLiteral("左上") });
  connect(floatPosCombo, &NXComboBox::currentIndexChanged, this, [=](int index) {
    _floatButton->setPosition(static_cast<NXFloatButton::Position>(index));
  });
  floatBtnLayout->addWidget(floatPosCombo);
  floatBtnLayout->addSpacing(10);
  floatBtnLayout->addWidget(new NXText(QStringLiteral("边距:"), 13, this));
  NXSlider *floatMarginSlider = new NXSlider(this);
  floatMarginSlider->setRange(10, 100);
  floatMarginSlider->setValue(30);
  floatMarginSlider->setFixedWidth(150);
  connect(floatMarginSlider, &NXSlider::valueChanged, this, [=](int value) {
    _floatButton->setMargin(value);
  });
  floatBtnLayout->addWidget(floatMarginSlider);
  floatBtnLayout->addStretch();
  c->addWidget(floatBtnArea);

  c->addSpacing(5);
  c->addWidget(new NXText(QStringLiteral("NXInfoBar"), 15, this));
  c->addWidget(_infoBarInfo);
  c->addWidget(_infoBarSuccess);
  c->addWidget(_infoBarWarning);
  c->addWidget(_infoBarError);

  // ========== NXAutoComplete 示例 ==========
  _autoComplete = new NXAutoComplete(this);
  _autoComplete->setPlaceholderText(QStringLiteral("输入城市名称..."));
  _autoComplete->setCompletions(
      { QStringLiteral("北京"), QStringLiteral("北京大兴机场"), QStringLiteral("北京西站"),
        QStringLiteral("上海"), QStringLiteral("上海虹桥"), QStringLiteral("上海浦东"),
        QStringLiteral("广州"), QStringLiteral("广州南站"), QStringLiteral("深圳"),
        QStringLiteral("深圳北站"), QStringLiteral("杭州"), QStringLiteral("杭州西湖"),
        QStringLiteral("成都"), QStringLiteral("成都天府"), QStringLiteral("武汉"),
        QStringLiteral("南京"), QStringLiteral("南京南站"), QStringLiteral("西安"),
        QStringLiteral("重庆"), QStringLiteral("天津"), QStringLiteral("苏州"),
        QStringLiteral("长沙"), QStringLiteral("Singapore"), QStringLiteral("Hong Kong"),
        QStringLiteral("Taiwan"), QStringLiteral("Tokyo"), QStringLiteral("Seoul"),
        QStringLiteral("Bangkok"), QStringLiteral("New York"), QStringLiteral("London"),
        QStringLiteral("Paris"), QStringLiteral("Sydney"), QStringLiteral("Los Angeles"),
        QStringLiteral("San Francisco"), QStringLiteral("Chicago"), QStringLiteral("Toronto"),
        QStringLiteral("Vancouver"), QStringLiteral("Berlin"), QStringLiteral("Amsterdam"),
        QStringLiteral("Dubai"), QStringLiteral("Mumbai"), QStringLiteral("Melbourne"),
        QStringLiteral("Oslo"), QStringLiteral("Stockholm"), QStringLiteral("Helsinki"),
        QStringLiteral("Lisbon"), QStringLiteral("Barcelona") });

  NXText *autoCompleteResult = new NXText(QStringLiteral(""), this);
  autoCompleteResult->setTextPixelSize(13);
  connect(_autoComplete, &NXAutoComplete::completionSelected, this, [=](QString const &text) {
    autoCompleteResult->setText(QString(QStringLiteral("已选择: %1")).arg(text));
  });

  NXComboBox *matchModeCombo = new NXComboBox(this);
  matchModeCombo->addItems({ QStringLiteral("包含匹配"), QStringLiteral("前缀匹配"), QStringLiteral("后缀匹配"), QStringLiteral("正则表达式") });
  connect(matchModeCombo, &NXComboBox::currentIndexChanged, this, [=](int index) {
    _autoComplete->setMatchMode(static_cast<NXAutoComplete::MatchMode>(index));
  });

  NXScrollPageArea *autoCompleteArea = new NXScrollPageArea(this);
  QHBoxLayout *autoCompleteLayout    = new QHBoxLayout(autoCompleteArea);
  autoCompleteLayout->addWidget(new NXText(QStringLiteral("NXAutoComplete"), 15, this));
  autoCompleteLayout->addSpacing(10);
  autoCompleteLayout->addWidget(_autoComplete);
  autoCompleteLayout->addSpacing(10);
  autoCompleteLayout->addWidget(matchModeCombo);
  autoCompleteLayout->addSpacing(10);
  autoCompleteLayout->addWidget(autoCompleteResult);
  autoCompleteLayout->addStretch();
  c->addWidget(autoCompleteArea);

  // ========== NXTreeSelect 示例 ==========
  _treeSelect = new NXTreeSelect(this);
  _treeSelect->setPlaceholderText(QStringLiteral("请选择分类..."));
  _treeSelect->setMinimumWidth(280);

  QStandardItemModel *treeModel = new QStandardItemModel(this);
  QStandardItem *china          = new QStandardItem(QStringLiteral("中国"));
  QStandardItem *beijing        = new QStandardItem(QStringLiteral("北京"));
  beijing->appendRow(new QStandardItem(QStringLiteral("朝阳区")));
  beijing->appendRow(new QStandardItem(QStringLiteral("海淀区")));
  beijing->appendRow(new QStandardItem(QStringLiteral("西城区")));
  china->appendRow(beijing);
  QStandardItem *shanghai = new QStandardItem(QStringLiteral("上海"));
  shanghai->appendRow(new QStandardItem(QStringLiteral("浦东新区")));
  shanghai->appendRow(new QStandardItem(QStringLiteral("黄浦区")));
  china->appendRow(shanghai);
  QStandardItem *guangdong = new QStandardItem(QStringLiteral("广东"));
  guangdong->appendRow(new QStandardItem(QStringLiteral("深圳")));
  guangdong->appendRow(new QStandardItem(QStringLiteral("广州")));
  china->appendRow(guangdong);
  treeModel->appendRow(china);

  QStandardItem *usa        = new QStandardItem(QStringLiteral("美国"));
  QStandardItem *california = new QStandardItem(QStringLiteral("加利福尼亚"));
  california->appendRow(new QStandardItem(QStringLiteral("洛杉矶")));
  california->appendRow(new QStandardItem(QStringLiteral("旧金山")));
  usa->appendRow(california);
  QStandardItem *newYork = new QStandardItem(QStringLiteral("纽约州"));
  newYork->appendRow(new QStandardItem(QStringLiteral("纽约市")));
  usa->appendRow(newYork);
  treeModel->appendRow(usa);

  _treeSelect->setModel(treeModel);

  NXText *treeSelectResult = new NXText(QStringLiteral(""), this);
  treeSelectResult->setTextPixelSize(13);
  connect(_treeSelect, &NXTreeSelect::currentTextChanged, this, [=](QString const &text) {
    treeSelectResult->setText(QString(QStringLiteral("已选择: %1")).arg(text));
  });

  NXScrollPageArea *treeSelectArea = new NXScrollPageArea(this);
  QHBoxLayout *treeSelectLayout    = new QHBoxLayout(treeSelectArea);
  treeSelectLayout->addWidget(new NXText(QStringLiteral("NXTreeSelect"), 15, this));
  treeSelectLayout->addSpacing(10);
  treeSelectLayout->addWidget(_treeSelect);
  treeSelectLayout->addSpacing(10);
  treeSelectLayout->addWidget(treeSelectResult);
  treeSelectLayout->addStretch();
  c->addWidget(treeSelectArea);

  // ========== NXUploadArea 示例 ==========
  _uploadArea = new NXUploadArea(this);
  _uploadArea->setFixedHeight(200);
  _uploadArea->setAcceptedSuffixes({ QStringLiteral("png"), QStringLiteral("jpg"), QStringLiteral("jpeg"), QStringLiteral("gif"), QStringLiteral("bmp"), QStringLiteral("pdf"), QStringLiteral("txt") });
  _uploadArea->setMaxFileCount(5);
  _uploadArea->setTitle(QStringLiteral("拖拽文件到此处"));
  _uploadArea->setSubTitle(QStringLiteral("支持 png, jpg, gif, bmp, pdf, txt (最多5个)"));
  _uploadArea->setDialogTitle(QStringLiteral("选择要上传的文件"));

  NXText *uploadInfo = new NXText(QStringLiteral("已选择 0 个文件"), this);
  uploadInfo->setTextPixelSize(13);
  connect(_uploadArea, &NXUploadArea::filesSelected, this, [=](QStringList const &files) {
    uploadInfo->setText(QString(QStringLiteral("已选择 %1 个文件")).arg(files.size()));
  });
  connect(_uploadArea, &NXUploadArea::fileRejected, this, [=](QString const &path, QString const &reason) {
    Q_UNUSED(path)
    NXToast::warning(reason, 2000, window());
  });

  NXPushButton *clearUploadBtn = new NXPushButton(QStringLiteral("清空文件"), this);
  clearUploadBtn->setFixedSize(100, 35);
  connect(clearUploadBtn, &NXPushButton::clicked, this, [=]() {
    _uploadArea->clearFiles();
    uploadInfo->setText(QStringLiteral("已选择 0 个文件"));
  });

  NXScrollPageArea *uploadAreaContainer = new NXScrollPageArea(this);
  uploadAreaContainer->setFixedHeight(280);
  QVBoxLayout *uploadMainLayout = new QVBoxLayout(uploadAreaContainer);
  QHBoxLayout *uploadHeader     = new QHBoxLayout();
  uploadHeader->addWidget(new NXText(QStringLiteral("NXUploadArea"), 15, this));
  uploadHeader->addSpacing(10);
  uploadHeader->addWidget(uploadInfo);
  uploadHeader->addSpacing(10);
  uploadHeader->addWidget(clearUploadBtn);
  uploadHeader->addStretch();
  uploadMainLayout->addLayout(uploadHeader);
  uploadMainLayout->addWidget(_uploadArea);
  c->addWidget(uploadAreaContainer);

  _copyButton = new NXCopyButton(this);
  _copyButton->setCopyText(QStringLiteral("https://github.com/RainbowCandyX/NexUs"));

  NXLineEdit *copyInput = new NXLineEdit(this);
  copyInput->setText(QStringLiteral("https://github.com/RainbowCandyX/NexUs"));
  copyInput->setFixedHeight(35);
  copyInput->setMinimumWidth(350);
  connect(copyInput, &NXLineEdit::textChanged, this, [=](QString const &text) {
    _copyButton->setCopyText(text);
  });

  NXText *copyStatus = new NXText(QStringLiteral(""), this);
  copyStatus->setTextPixelSize(13);
  connect(_copyButton, &NXCopyButton::copyCompleted, this, [=](QString const &text) {
    Q_UNUSED(text)
    copyStatus->setText(QStringLiteral("已复制!"));
    QTimer::singleShot(2000, this, [=]() {
      copyStatus->setText(QStringLiteral(""));
    });
  });

  NXScrollPageArea *copyArea = new NXScrollPageArea(this);
  QHBoxLayout *copyLayout    = new QHBoxLayout(copyArea);
  copyLayout->addWidget(new NXText(QStringLiteral("NXCopyButton"), 15, this));
  copyLayout->addSpacing(10);
  copyLayout->addWidget(copyInput);
  copyLayout->addWidget(_copyButton);
  copyLayout->addSpacing(5);
  copyLayout->addWidget(copyStatus);
  copyLayout->addStretch();
  c->addWidget(copyArea);

  c->addSpacing(5);
  c->addLayout(transferHeader);
  c->addWidget(_transfer);
  c->addStretch();
  addCentralWidget(centralWidget, true, false, 0);
}

T_NewComponents::~T_NewComponents()
{
}
