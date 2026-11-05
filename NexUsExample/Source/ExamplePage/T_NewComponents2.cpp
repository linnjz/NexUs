#include "T_NewComponents2.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "NXChatBubble.h"
#include "NXDashboardGauge.h"
#include "NXScrollPageArea.h"
#include "NXSlider.h"
#include "NXTerminalWidget.h"
#include "NXText.h"

T_NewComponents2::T_NewComponents2(QWidget *parent)
    : T_BasePage(parent)
{
  setWindowTitle(QStringLiteral("NXNewComponents2"));
  createCustomWidget(QStringLiteral("新增组件演示页面 II - ChatBubble / DashboardGauge / TerminalWidget"));

  QWidget *centralWidget = new QWidget(this);
  centralWidget->setWindowTitle(QStringLiteral("NXNewComponents2"));

  // ========== NXChatBubble 示例 ==========
  NXText *chatTitle = new NXText(QStringLiteral("NXChatBubble"), this);
  chatTitle->setTextPixelSize(18);

  _chatBubbleLeft = new NXChatBubble(this);
  _chatBubbleLeft->setDirection(NXChatBubble::Left);
  _chatBubbleLeft->setSenderName(QStringLiteral("Alice"));
  _chatBubbleLeft->setMessageText(QStringLiteral("你好！最近在忙什么项目？听说你在做一个新的组件库？"));
  _chatBubbleLeft->setTimestamp(QStringLiteral("10:30"));
  _chatBubbleLeft->setMinimumWidth(500);

  _chatBubbleRight = new NXChatBubble(this);
  _chatBubbleRight->setDirection(NXChatBubble::Right);
  _chatBubbleRight->setSenderName(QStringLiteral("Bob"));
  _chatBubbleRight->setMessageText(QStringLiteral("是的，我在开发 NexUs 的新组件，包括聊天气泡、仪表盘和终端模拟器！"));
  _chatBubbleRight->setTimestamp(QStringLiteral("10:31"));
  _chatBubbleRight->setStatus(NXChatBubble::Read);
  _chatBubbleRight->setMinimumWidth(500);

  _chatBubbleSystem = new NXChatBubble(this);
  _chatBubbleSystem->setDirection(NXChatBubble::Left);
  _chatBubbleSystem->setMessageText(QStringLiteral("这个组件支持左右对齐、头像、时间戳、发送状态等功能。"));
  _chatBubbleSystem->setTimestamp(QStringLiteral("10:32"));
  _chatBubbleSystem->setStatus(NXChatBubble::Sending);
  _chatBubbleSystem->setMinimumWidth(500);

  _chatBubbleImage = new NXChatBubble(this);
  _chatBubbleImage->setDirection(NXChatBubble::Right);
  _chatBubbleImage->setSenderName(QStringLiteral("Bob"));
  _chatBubbleImage->setMessageImage(QPixmap(QStringLiteral(":/Resource/Image/Cirno.jpg")));
  _chatBubbleImage->setMessageText(QStringLiteral("看看这张图片！"));
  _chatBubbleImage->setTimestamp(QStringLiteral("10:33"));
  _chatBubbleImage->setStatus(NXChatBubble::Sent);
  _chatBubbleImage->setMinimumWidth(500);

  QVBoxLayout *chatLayout = new QVBoxLayout();
  chatLayout->setSpacing(4);
  chatLayout->addWidget(_chatBubbleLeft);
  chatLayout->addWidget(_chatBubbleRight);
  chatLayout->addWidget(_chatBubbleSystem);
  chatLayout->addWidget(_chatBubbleImage);

  NXScrollPageArea *chatArea = new NXScrollPageArea(this);
  chatArea->setFixedHeight(700);
  QVBoxLayout *chatAreaLayout = new QVBoxLayout(chatArea);
  chatAreaLayout->addWidget(chatTitle);
  chatAreaLayout->addLayout(chatLayout);

  // ========== NXDashboardGauge 示例 ==========
  NXText *gaugeTitle = new NXText(QStringLiteral("NXDashboardGauge"), this);
  gaugeTitle->setTextPixelSize(18);

  _gauge = new NXDashboardGauge(this);
  _gauge->setMinimum(0);
  _gauge->setMaximum(200);
  _gauge->setValue(80);
  _gauge->setTitle(QStringLiteral("速度"));
  _gauge->setUnit(QStringLiteral("km/h"));
  _gauge->setValuePixelSize(16);
  _gauge->setTickWarningPercent(0.7);
  _gauge->setDecimals(2);

  _gaugeSmall = new NXDashboardGauge(this);
  _gaugeSmall->setFixedSize(180, 180);
  _gaugeSmall->setMinimum(0);
  _gaugeSmall->setMaximum(100);
  _gaugeSmall->setValue(50);
  _gaugeSmall->setTitle(QStringLiteral("CPU"));
  _gaugeSmall->setUnit(QStringLiteral("%"));
  _gaugeSmall->setValuePixelSize(24);
  _gaugeSmall->setTickWarningPercent(0.6);
  _gaugeSmall->setDecimals(2);

  NXSlider *gaugeSlider = new NXSlider(this);
  gaugeSlider->setRange(static_cast<int>(_gauge->getMinimum()), static_cast<int>(_gauge->getMaximum()));
  gaugeSlider->setValue(72);
  gaugeSlider->setFixedWidth(260);
  NXText *gaugeValueLabel = new NXText(QStringLiteral("80"), this);
  gaugeValueLabel->setTextPixelSize(12);
  connect(gaugeSlider, &NXSlider::valueChanged, this, [=](int v) {
    _gauge->setValue(v);
    gaugeValueLabel->setText(QString::number(v));
  });

  QHBoxLayout *gaugeWidgetLayout = new QHBoxLayout();
  gaugeWidgetLayout->addWidget(_gauge);
  gaugeWidgetLayout->addSpacing(20);
  gaugeWidgetLayout->addWidget(_gaugeSmall);
  gaugeWidgetLayout->addStretch();

  QHBoxLayout *gaugeControlLayout = new QHBoxLayout();
  gaugeControlLayout->addWidget(new NXText(QStringLiteral("值控制:"), 13, this));
  gaugeControlLayout->addWidget(gaugeSlider);
  gaugeControlLayout->addWidget(gaugeValueLabel);
  gaugeControlLayout->addStretch();

  NXScrollPageArea *gaugeArea = new NXScrollPageArea(this);
  gaugeArea->setFixedHeight(340);
  QVBoxLayout *gaugeAreaLayout = new QVBoxLayout(gaugeArea);
  gaugeAreaLayout->addWidget(gaugeTitle);
  gaugeAreaLayout->addLayout(gaugeWidgetLayout);
  gaugeAreaLayout->addLayout(gaugeControlLayout);

  // ========== NXTerminalWidget 示例 ==========
  NXText *terminalTitle = new NXText(QStringLiteral("NXTerminalWidget"), this);
  terminalTitle->setTextPixelSize(18);

  _terminal = new NXTerminalWidget(this);
  _terminal->setFixedHeight(300);
  _terminal->setPrompt(QStringLiteral("ela> "));
  _terminal->appendSuccess(QStringLiteral("NXTerminalWidget 已启动"));
  _terminal->appendOutput(QStringLiteral("输入 help 查看可用命令，输入 clear 清屏。"));

  connect(_terminal, &NXTerminalWidget::commandSubmitted, this, [=](QString const &cmd) {
    if (cmd == QStringLiteral("help")) {
      _terminal->appendOutput(QStringLiteral("可用命令: help, clear, version, echo <text>, gauge <value>"));
    } else if (cmd == QStringLiteral("clear")) {
      _terminal->clear();
    } else if (cmd == QStringLiteral("version")) {
      _terminal->appendOutput(QString(QStringLiteral("NexUs %1")).arg(QStringLiteral("1.0.0")));
    } else if (cmd.startsWith(QStringLiteral("echo "))) {
      _terminal->appendOutput(cmd.mid(5));
    } else if (cmd.startsWith(QStringLiteral("gauge "))) {
      bool ok;
      double val = cmd.mid(6).toDouble(&ok);
      if (ok) {
        qreal clamped = qBound(_gauge->getMinimum(), val, _gauge->getMaximum());
        _gauge->setValue(clamped);
        gaugeSlider->setValue(static_cast<int>(clamped));
        if (val < _gauge->getMinimum()) {
          _terminal->appendOutput(QString(QStringLiteral("值 %1 低于最小值，已设置为 %2")).arg(val).arg(clamped), QColor(0xF7, 0x94, 0x0B));
        } else if (val > _gauge->getMaximum()) {
          _terminal->appendOutput(QString(QStringLiteral("值 %1 超过最大值，已设置为 %2")).arg(val).arg(clamped), QColor(0xF7, 0x94, 0x0B));
        } else {
          _terminal->appendSuccess(QString(QStringLiteral("仪表盘值已设置为 %1")).arg(clamped));
        }
      } else {
        _terminal->appendError(QStringLiteral("无效数值"));
      }
    } else {
      _terminal->appendError(QString(QStringLiteral("未知命令: %1")).arg(cmd));
    }
  });

  NXScrollPageArea *terminalArea = new NXScrollPageArea(this);
  terminalArea->setFixedHeight(360);
  QVBoxLayout *terminalAreaLayout = new QVBoxLayout(terminalArea);
  terminalAreaLayout->addWidget(terminalTitle);
  terminalAreaLayout->addWidget(_terminal);

  // ========== 中心布局 ==========
  QVBoxLayout *c = new QVBoxLayout(centralWidget);
  c->setContentsMargins(0, 0, 0, 0);
  c->addWidget(chatArea);
  c->addWidget(gaugeArea);
  c->addWidget(terminalArea);
  addCentralWidget(centralWidget, true, false, 0);
}

T_NewComponents2::~T_NewComponents2()
{
}
