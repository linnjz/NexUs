#include "T_Router.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "NXLineEdit.h"
#include "NXPlainTextEdit.h"
#include "NXPushButton.h"
#include "NXRouter.h"
#include "NXScrollPageArea.h"
#include "NXText.h"
#include "NXToast.h"
#include "NXToggleSwitch.h"
#include "NXWindow.h"

T_Router::T_Router(QWidget *parent)
    : T_BasePage(parent)
{
  setWindowTitle(QStringLiteral("NXRouter"));
  createCustomWidget(QStringLiteral("NXRouter 声明式路由演示 - 路由守卫、动态路由、懒加载、导航拦截"));

  QWidget *centralWidget = new QWidget(this);
  centralWidget->setWindowTitle(QStringLiteral("NXRouter"));
  QVBoxLayout *c = new QVBoxLayout(centralWidget);
  c->setContentsMargins(0, 0, 0, 0);

  // ========== 路由注册示例 ==========
  NXScrollPageArea *registerArea = new NXScrollPageArea(this);
  registerArea->setMinimumHeight(100);
  QVBoxLayout *registerLayout = new QVBoxLayout(registerArea);
  registerLayout->addWidget(new NXText(QStringLiteral("路由注册 (addRoutes + installRoutes)"), 15, this));

  NXPushButton *registerBtn = new NXPushButton(QStringLiteral("注册示例路由"), this);
  registerBtn->setFixedSize(130, 38);

  NXPushButton *installBtn = new NXPushButton(QStringLiteral("安装到导航栏"), this);
  installBtn->setFixedSize(130, 38);

  NXText *registerStatus = new NXText(QStringLiteral(""), this);
  registerStatus->setTextPixelSize(13);
  registerStatus->setMinimumWidth(300);

  connect(registerBtn, &NXPushButton::clicked, this, [=]() {
    NXWindow *w = dynamic_cast<NXWindow *>(window());
    if (!w) {
      return;
    }
    eRouter->bindWindow(w);

    NXRouterType::NavigationResult result = eRouter->addRoutes({
        { QStringLiteral("/router-demo"), QStringLiteral("路由演示"), NXIconType::Route, {}, nullptr, {
                                                                                                          { QStringLiteral("/page-a"), QStringLiteral("页面 A"), NXIconType::FileLines, { { QStringLiteral("keepAlive"), true } }, [=]() -> QWidget * {
      QWidget *page    = new QWidget();
      QVBoxLayout *lay = new QVBoxLayout(page);
      lay->addWidget(new NXText(QStringLiteral("这是页面 A (keepAlive: true)"), 16, page));
      lay->addWidget(new NXText(QStringLiteral("通过 NXRouter 懒加载创建，首次导航时才实例化"), 13, page));
      lay->addStretch();
      return page;
    } },
                                                                                                          { QStringLiteral("/page-b"), QStringLiteral("页面 B"), NXIconType::FilePen, { { QStringLiteral("role"), QStringLiteral("user") } }, [=]() -> QWidget * {
      QWidget *page    = new QWidget();
      QVBoxLayout *lay = new QVBoxLayout(page);
      lay->addWidget(new NXText(QStringLiteral("这是页面 B (role: user)"), 16, page));
      lay->addWidget(new NXText(QStringLiteral("需要 user 角色才能访问（守卫可拦截）"), 13, page));
      lay->addStretch();
      return page;
    } },
                                                                                                          { QStringLiteral("/page-c"), QStringLiteral("页面 C"), NXIconType::FileShield, { { QStringLiteral("role"), QStringLiteral("admin") } }, [=]() -> QWidget * {
      QWidget *page    = new QWidget();
      QVBoxLayout *lay = new QVBoxLayout(page);
      lay->addWidget(new NXText(QStringLiteral("这是页面 C (role: admin)"), 16, page));
      lay->addWidget(new NXText(QStringLiteral("需要 admin 角色才能访问"), 13, page));
      lay->addStretch();
      return page;
    } },
                                                                                                      } },
    });
    if (result == NXRouterType::Success) {
      registerStatus->setText(QStringLiteral("路由注册成功! 共 ") + QString::number(eRouter->getRoutePaths().size()) + QStringLiteral(" 条路由"));
      appendLog(QStringLiteral("addRoutes: 注册了 /router-demo 及 3 个子路由"));
    } else if (result == NXRouterType::RouteAlreadyExists) {
      registerStatus->setText(QStringLiteral("路由已存在，无需重复注册"));
    }
  });

  connect(installBtn, &NXPushButton::clicked, this, [=]() {
    if (!eRouter->getBoundWindow()) {
      registerStatus->setText(QStringLiteral("请先注册路由"));
      return;
    }
    eRouter->installRoutes();
    registerStatus->setText(QStringLiteral("已安装到导航栏，请查看左侧 \"路由演示\" 分组"));
    appendLog(QStringLiteral("installRoutes: 路由已安装到 NXWindow 导航栏"));
  });

  QHBoxLayout *registerBtnLayout = new QHBoxLayout();
  registerBtnLayout->addWidget(registerBtn);
  registerBtnLayout->addWidget(installBtn);
  registerBtnLayout->addWidget(registerStatus);
  registerBtnLayout->addStretch();
  registerLayout->addLayout(registerBtnLayout);

  // ========== 路由导航示例 ==========
  NXScrollPageArea *navArea = new NXScrollPageArea(this);
  navArea->setMinimumHeight(100);
  QVBoxLayout *navLayout = new QVBoxLayout(navArea);
  navLayout->addWidget(new NXText(QStringLiteral("路由导航 (push / replace / back / forward)"), 15, this));

  NXPushButton *pushABtn = new NXPushButton(QStringLiteral("push 页面A"), this);
  pushABtn->setFixedSize(110, 32);
  NXPushButton *pushBBtn = new NXPushButton(QStringLiteral("push 页面B"), this);
  pushBBtn->setFixedSize(110, 32);
  NXPushButton *pushCBtn = new NXPushButton(QStringLiteral("push 页面C"), this);
  pushCBtn->setFixedSize(110, 32);
  NXPushButton *backBtn = new NXPushButton(QStringLiteral("back"), this);
  backBtn->setFixedSize(70, 32);
  NXPushButton *forwardBtn = new NXPushButton(QStringLiteral("forward"), this);
  forwardBtn->setFixedSize(70, 32);

  connect(pushABtn, &NXPushButton::clicked, this, [=]() {
    auto r =
        eRouter->push(QStringLiteral("/router-demo/page-a"), {
                                                                 { QStringLiteral("from"), QStringLiteral("demo") }
    });
    appendLog(QStringLiteral("push(/router-demo/page-a) → ") + QString(r == NXRouterType::Success ? QStringLiteral("Success") : QStringLiteral("Failed")));
  });
  connect(pushBBtn, &NXPushButton::clicked, this, [=]() {
    auto r = eRouter->push(QStringLiteral("/router-demo/page-b"));
    appendLog(QStringLiteral("push(/router-demo/page-b) → ") + QString(r == NXRouterType::Success ? QStringLiteral("Success") : QStringLiteral("Failed")));
  });
  connect(pushCBtn, &NXPushButton::clicked, this, [=]() {
    auto r = eRouter->push(QStringLiteral("/router-demo/page-c"));
    if (r == NXRouterType::GuardRejected) {
      appendLog(QStringLiteral("push(/router-demo/page-c) → GuardRejected (需要 admin 角色)"));
    } else {
      appendLog(QStringLiteral("push(/router-demo/page-c) → ") + QString(r == NXRouterType::Success ? QStringLiteral("Success") : QStringLiteral("Failed")));
    }
  });
  connect(backBtn, &NXPushButton::clicked, this, [=]() {
    eRouter->back();
    appendLog(QStringLiteral("back()"));
  });
  connect(forwardBtn, &NXPushButton::clicked, this, [=]() {
    eRouter->forward();
    appendLog(QStringLiteral("forward()"));
  });

  QHBoxLayout *navBtnLayout = new QHBoxLayout();
  navBtnLayout->addWidget(pushABtn);
  navBtnLayout->addWidget(pushBBtn);
  navBtnLayout->addWidget(pushCBtn);
  navBtnLayout->addSpacing(10);
  navBtnLayout->addWidget(backBtn);
  navBtnLayout->addWidget(forwardBtn);
  navBtnLayout->addStretch();
  navLayout->addLayout(navBtnLayout);

  // ========== 路由守卫示例 ==========
  NXScrollPageArea *guardArea = new NXScrollPageArea(this);
  guardArea->setMinimumHeight(100);
  QVBoxLayout *guardLayout = new QVBoxLayout(guardArea);
  guardLayout->addWidget(new NXText(QStringLiteral("路由守卫 (beforeEach)"), 15, this));

  NXToggleSwitch *guardSwitch = new NXToggleSwitch(this);
  NXText *guardDesc           = new NXText(QStringLiteral("关闭时：拦截 role=admin 的路由 (页面C)"), this);
  guardDesc->setTextPixelSize(13);
  guardDesc->setMinimumWidth(300);

  int *guardId = new int(-1);

  auto updateGuard = [=](bool isAdmin) {
    if (*guardId >= 0) {
      eRouter->removeBeforeGuard(*guardId);
      *guardId = -1;
    }
    if (!isAdmin) {
      *guardId = eRouter->beforeEach([=](QString const &to, QString const &, QVariantMap const &) -> bool {
        QVariantMap meta = eRouter->getRouteMeta(to);
        if (meta.value(QStringLiteral("role")).toString() == QStringLiteral("admin")) {
          NXToast::warning(QStringLiteral("权限不足: 需要 admin 角色"), 2000, window());
          return false;
        }
        return true;
      });
      appendLog(QStringLiteral("beforeEach: 已启用角色守卫 (拦截 admin 路由)"));
    } else {
      appendLog(QStringLiteral("beforeEach: 已移除角色守卫 (允许所有路由)"));
    }
    guardDesc->setText(isAdmin ? QStringLiteral("开启时：允许所有路由") : QStringLiteral("关闭时：拦截 role=admin 的路由 (页面C)"));
  };

  connect(guardSwitch, &NXToggleSwitch::toggled, this, updateGuard);
  updateGuard(false);

  QHBoxLayout *guardBtnLayout = new QHBoxLayout();
  NXText *adminLabel          = new NXText(QStringLiteral("模拟 Admin 身份"), 13, this);
  adminLabel->setMinimumWidth(120);
  guardBtnLayout->addWidget(adminLabel);
  guardBtnLayout->addWidget(guardSwitch);
  guardBtnLayout->addSpacing(10);
  guardBtnLayout->addWidget(guardDesc);
  guardBtnLayout->addStretch();
  guardLayout->addLayout(guardBtnLayout);

  // ========== 动态路由示例 ==========
  NXScrollPageArea *dynamicArea = new NXScrollPageArea(this);
  dynamicArea->setMinimumHeight(100);
  QVBoxLayout *dynamicLayout = new QVBoxLayout(dynamicArea);
  NXText *dynamicTitle       = new NXText(QStringLiteral("动态路由 (addDynamicRoute / removeRoute)"), 15, this);
  dynamicTitle->setMinimumWidth(350);
  dynamicLayout->addWidget(dynamicTitle);

  NXLineEdit *dynamicNameEdit = new NXLineEdit(this);
  dynamicNameEdit->setPlaceholderText(QStringLiteral("输入路由名称 (如: page-d)"));
  dynamicNameEdit->setFixedSize(200, 35);

  NXPushButton *addDynamicBtn = new NXPushButton(QStringLiteral("添加动态路由"), this);
  addDynamicBtn->setFixedSize(120, 32);
  NXPushButton *removeDynamicBtn = new NXPushButton(QStringLiteral("移除动态路由"), this);
  removeDynamicBtn->setFixedSize(120, 32);

  NXText *dynamicStatus = new NXText(QStringLiteral(""), this);
  dynamicStatus->setTextPixelSize(13);
  dynamicStatus->setMinimumWidth(300);

  connect(addDynamicBtn, &NXPushButton::clicked, this, [=]() {
    QString name = dynamicNameEdit->text().trimmed();
    if (name.isEmpty()) {
      name = QStringLiteral("page-d");
    }
    QString path = QStringLiteral("/") + name;
    auto r       = eRouter->addDynamicRoute(QStringLiteral("/router-demo"), { path, name, NXIconType::FileDoc, {}, [=]() -> QWidget       *{
      QWidget *page    = new QWidget();
      QVBoxLayout *lay = new QVBoxLayout(page);
      lay->addWidget(new NXText(QStringLiteral("动态页面: ") + name, 16, page));
      lay->addWidget(new NXText(QStringLiteral("通过 addDynamicRoute 在运行时添加"), 13, page));
      lay->addStretch();
      return page;
    } });
    if (r == NXRouterType::Success) {
      dynamicStatus->setText(QStringLiteral("已添加: /router-demo") + path);
      appendLog(QStringLiteral("addDynamicRoute: /router-demo") + path);
    } else {
      dynamicStatus->setText(QStringLiteral("添加失败 (可能已存在或父路由不存在)"));
    }
  });

  connect(removeDynamicBtn, &NXPushButton::clicked, this, [=]() {
    QString name = dynamicNameEdit->text().trimmed();
    if (name.isEmpty()) {
      name = QStringLiteral("page-d");
    }
    QString fullPath = QStringLiteral("/router-demo/") + name;
    auto r           = eRouter->removeRoute(fullPath);
    if (r == NXRouterType::Success) {
      dynamicStatus->setText(QStringLiteral("已移除: ") + fullPath);
      appendLog(QStringLiteral("removeRoute: ") + fullPath);
    } else {
      dynamicStatus->setText(QStringLiteral("移除失败 (路由不存在)"));
    }
  });

  QHBoxLayout *dynamicBtnLayout = new QHBoxLayout();
  dynamicBtnLayout->addWidget(dynamicNameEdit);
  dynamicBtnLayout->addWidget(addDynamicBtn);
  dynamicBtnLayout->addWidget(removeDynamicBtn);
  dynamicBtnLayout->addWidget(dynamicStatus);
  dynamicBtnLayout->addStretch();
  dynamicLayout->addLayout(dynamicBtnLayout);

  // ========== 路由状态 & 日志 ==========
  NXScrollPageArea *logArea = new NXScrollPageArea(this);
  logArea->setFixedHeight(200);
  QVBoxLayout *logLayout = new QVBoxLayout(logArea);

  QHBoxLayout *logHeader = new QHBoxLayout();
  logHeader->addWidget(new NXText(QStringLiteral("路由日志"), 15, this));

  NXText *currentPathText = new NXText(QStringLiteral("当前路由: (无)"), this);
  currentPathText->setTextPixelSize(13);
  currentPathText->setMinimumWidth(400);
  logHeader->addSpacing(20);
  logHeader->addWidget(currentPathText);
  logHeader->addStretch();

  NXPushButton *clearLogBtn = new NXPushButton(QStringLiteral("清除"), this);
  clearLogBtn->setFixedSize(60, 28);
  logHeader->addWidget(clearLogBtn);

  logLayout->addLayout(logHeader);

  _logEdit = new NXPlainTextEdit(this);
  _logEdit->setReadOnly(true);
  logLayout->addWidget(_logEdit);

  connect(clearLogBtn, &NXPushButton::clicked, this, [=]() {
    _logEdit->clear();
  });

  connect(eRouter, &NXRouter::routeChanged, this, [=](QString const &path, QVariantMap const &params) {
    QString paramStr;
    if (!params.isEmpty()) {
      QStringList parts;
      for (auto it = params.begin(); it != params.end(); ++it) {
        parts.append(it.key() + QStringLiteral("=") + it.value().toString());
      }
      paramStr = QStringLiteral(" {") + parts.join(QStringLiteral(", ")) + QStringLiteral("}");
    }
    currentPathText->setText(QStringLiteral("当前路由: ") + path + paramStr);
  });

  connect(eRouter, &NXRouter::navigationBlocked, this, [=](QString const &path) {
    appendLog(QStringLiteral("BLOCKED: ") + path);
  });

  // ========== 中心布局 ==========
  c->addWidget(registerArea);
  c->addWidget(navArea);
  c->addWidget(guardArea);
  c->addWidget(dynamicArea);
  c->addWidget(logArea);
  c->addStretch();
  addCentralWidget(centralWidget, true, false, 0);
}

T_Router::~T_Router()
{
}

void T_Router::appendLog(QString const &text)
{
  if (!_logEdit) {
    return;
  }
  _logEdit->appendPlainText(QStringLiteral("> ") + text);
}
