#include "NXEmojiPicker.h"

#include <QApplication>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QScreen>

#include "NXLineEdit.h"
#include "NXScrollArea.h"
#include "NXTheme.h"
#include "private/NXEmojiPickerPrivate.h"

Q_PROPERTY_CREATE_CPP(NXEmojiPicker, QPS_PAEMIT(int), EmojiSize)
Q_PROPERTY_CREATE_CPP(NXEmojiPicker, QPS_PAEMIT(int), Columns)

NXEmojiPicker::NXEmojiPicker(QWidget *parent)
    : QWidget { nullptr }
    , d_ptr(new NXEmojiPickerPrivate())
{
  Q_D(NXEmojiPicker);
  d->q_ptr       = this;
  d->m_EmojiSize = 36;
  d->m_Columns   = 8;
  d->m_ThemeMode  = nxTheme->getThemeMode();

  setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
  setAttribute(Qt::WA_TranslucentBackground);
  setFixedSize(340, 420);

  d->initEmojis();

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
#if defined(Q_OS_WIN) && QT_VERSION == QT_VERSION_CHECK(6, 11, 0)
  mainLayout->setContentsMargins(2, 2, 2, 2);
#else
  mainLayout->setContentsMargins(8, 8, 8, 8);
#endif
  mainLayout->setSpacing(4);

  d->m_SearchEdit = new NXLineEdit(this);
  d->m_SearchEdit->setPlaceholderText(QString::fromUtf8("搜索表情..."));
  d->m_SearchEdit->setFixedHeight(32);
  connect(d->m_SearchEdit, &NXLineEdit::textChanged, this, [d](QString const &text) {
    d->filterEmojis(text);
  });

  QHBoxLayout *tabLayout = new QHBoxLayout();
  tabLayout->setSpacing(2);
  for (int i = 0; i < d->m_Categories.size(); i++) {
    QPushButton *tabBtn = new QPushButton(d->m_Categories[i].icon, this);
    tabBtn->setFixedSize(32, 32);
    tabBtn->setFlat(true);
    tabBtn->setStyleSheet(QStringLiteral("QPushButton{font-size:18px;border:none;border-radius:4px;}"
                                         "QPushButton:hover{background-color:rgba(128,128,128,40);}"));
    connect(tabBtn, &QPushButton::clicked, this, [d, i]() {
      d->m_CurrentCategory = i;
      d->m_SearchEdit->clear();
      d->buildGrid(i);
    });
    tabLayout->addWidget(tabBtn);
  }
  tabLayout->addStretch();

  NXScrollArea *scrollArea = new NXScrollArea(this);
  scrollArea->setWidgetResizable(true);
  scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scrollArea->setStyleSheet(QStringLiteral("QScrollArea{background:transparent;border:none;}"));

  d->m_GridContainer = new QWidget();
  d->m_GridContainer->setStyleSheet(QStringLiteral("background:transparent;"));
  d->m_GridLayout = new QGridLayout(d->m_GridContainer);
  d->m_GridLayout->setSpacing(2);
  d->m_GridLayout->setContentsMargins(0, 0, 0, 0);
  scrollArea->setWidget(d->m_GridContainer);

  mainLayout->addWidget(d->m_SearchEdit);
  mainLayout->addLayout(tabLayout);
  mainLayout->addWidget(scrollArea);

  d->buildGrid(d->m_RecentEmojis.isEmpty() ? 1 : 0);

  connect(nxTheme, &NXTheme::themeModeChanged, this, [this, d](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
    d->buildGrid(d->m_CurrentCategory);
    update();
  });
}

NXEmojiPicker::~NXEmojiPicker()
{
}

void NXEmojiPicker::popup(QWidget *anchor)
{
  if (!anchor) {
    return;
  }
  QPoint pos = anchor->mapToGlobal(QPoint(0, -height() - 5));
  if (pos.y() < 0) {
    pos = anchor->mapToGlobal(QPoint(0, anchor->height() + 5));
  }
  popup(pos);
}

void NXEmojiPicker::popup(QPoint pos)
{
  move(pos);
  show();
  raise();
  activateWindow();
}

void NXEmojiPicker::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);
  Q_D(NXEmojiPicker);
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing);
#if defined(Q_OS_WIN) && QT_VERSION == QT_VERSION_CHECK(6, 11, 0)
  QRect fg = rect();
#else
  nxTheme->drawEffectShadow(&painter, rect(), 6, 8);
  QRect fg = rect().adjusted(6, 6, -6, -6);
#endif
  painter.setPen(NXThemeColor(d->m_ThemeMode, PopupBorder));
  painter.setBrush(NXThemeColor(d->m_ThemeMode, PopupBase));
  painter.drawRoundedRect(fg, 8, 8);
}

NXEmojiPickerPrivate::NXEmojiPickerPrivate(QObject *parent)
    : QObject { parent }
{
}

NXEmojiPickerPrivate::~NXEmojiPickerPrivate()
{
}

void NXEmojiPickerPrivate::initEmojis()
{
  m_Categories = {
    { QString::fromUtf8("最近"), QString::fromUtf8("🕐"), {} },
    { QString::fromUtf8("表情"),
     QString::fromUtf8("😀"),
     {
          QString::fromUtf8("😀"),
          QString::fromUtf8("😃"),
          QString::fromUtf8("😄"),
          QString::fromUtf8("😁"),
          QString::fromUtf8("😆"),
          QString::fromUtf8("😅"),
          QString::fromUtf8("🤣"),
          QString::fromUtf8("😂"),
          QString::fromUtf8("🙂"),
          QString::fromUtf8("🙃"),
          QString::fromUtf8("😉"),
          QString::fromUtf8("😊"),
          QString::fromUtf8("😇"),
          QString::fromUtf8("🥰"),
          QString::fromUtf8("😍"),
          QString::fromUtf8("🤩"),
          QString::fromUtf8("😘"),
          QString::fromUtf8("😗"),
          QString::fromUtf8("😚"),
          QString::fromUtf8("😙"),
          QString::fromUtf8("🥲"),
          QString::fromUtf8("😋"),
          QString::fromUtf8("😛"),
          QString::fromUtf8("😜"),
          QString::fromUtf8("🤪"),
          QString::fromUtf8("😝"),
          QString::fromUtf8("🤑"),
          QString::fromUtf8("🤗"),
          QString::fromUtf8("🤭"),
          QString::fromUtf8("🤫"),
          QString::fromUtf8("🤔"),
          QString::fromUtf8("🫡"),
          QString::fromUtf8("🤐"),
          QString::fromUtf8("🤨"),
          QString::fromUtf8("😐"),
          QString::fromUtf8("😑"),
          QString::fromUtf8("😶"),
          QString::fromUtf8("🫥"),
          QString::fromUtf8("😏"),
          QString::fromUtf8("😒"),
          QString::fromUtf8("🙄"),
          QString::fromUtf8("😬"),
          QString::fromUtf8("🤥"),
          QString::fromUtf8("😌"),
          QString::fromUtf8("😔"),
          QString::fromUtf8("😪"),
          QString::fromUtf8("🤤"),
          QString::fromUtf8("😴"),
          QString::fromUtf8("😷"),
          QString::fromUtf8("🤒"),
          QString::fromUtf8("🤕"),
          QString::fromUtf8("🤢"),
          QString::fromUtf8("🤮"),
          QString::fromUtf8("🤧"),
          QString::fromUtf8("🥵"),
          QString::fromUtf8("🥶"),
          QString::fromUtf8("🥴"),
          QString::fromUtf8("😵"),
          QString::fromUtf8("🤯"),
          QString::fromUtf8("🤠"),
          QString::fromUtf8("🥳"),
          QString::fromUtf8("🥸"),
          QString::fromUtf8("😎"),
          QString::fromUtf8("🤓"),
          QString::fromUtf8("🧐"),
          QString::fromUtf8("😕"),
          QString::fromUtf8("🫤"),
          QString::fromUtf8("😟"),
          QString::fromUtf8("🙁"),
          QString::fromUtf8("😮"),
          QString::fromUtf8("😯"),
          QString::fromUtf8("😲"),
          QString::fromUtf8("😳"),
          QString::fromUtf8("🥺"),
          QString::fromUtf8("🥹"),
          QString::fromUtf8("😦"),
          QString::fromUtf8("😧"),
          QString::fromUtf8("😨"),
          QString::fromUtf8("😰"),
          QString::fromUtf8("😥"),
          QString::fromUtf8("😢"),
          QString::fromUtf8("😭"),
          QString::fromUtf8("😱"),
          QString::fromUtf8("😖"),
          QString::fromUtf8("😣"),
          QString::fromUtf8("😞"),
          QString::fromUtf8("😓"),
          QString::fromUtf8("😩"),
          QString::fromUtf8("😫"),
          QString::fromUtf8("🥱"),
          QString::fromUtf8("😤"),
          QString::fromUtf8("😡"),
          QString::fromUtf8("😠"),
          QString::fromUtf8("🤬"),
          QString::fromUtf8("😈"),
          QString::fromUtf8("👿"),
          QString::fromUtf8("💀"),
          QString::fromUtf8("☠️"),
          QString::fromUtf8("💩"),
          QString::fromUtf8("🤡"),
          QString::fromUtf8("👹"),
          QString::fromUtf8("👺"),
          QString::fromUtf8("👻"),
          QString::fromUtf8("👽"),
          QString::fromUtf8("👾"),
          QString::fromUtf8("🤖"),
          QString::fromUtf8("🎃"),
          QString::fromUtf8("😺"),
          QString::fromUtf8("😸"),
          QString::fromUtf8("😹"),
          QString::fromUtf8("😻"),
          QString::fromUtf8("😼"),
      }                                                      },
    { QString::fromUtf8("手势"),
     QString::fromUtf8("👋"),
     {
          QString::fromUtf8("👋"),
          QString::fromUtf8("🤚"),
          QString::fromUtf8("🖐️"),
          QString::fromUtf8("✋"),
          QString::fromUtf8("🖖"),
          QString::fromUtf8("🫱"),
          QString::fromUtf8("🫲"),
          QString::fromUtf8("🫳"),
          QString::fromUtf8("🫴"),
          QString::fromUtf8("👌"),
          QString::fromUtf8("🤌"),
          QString::fromUtf8("🤏"),
          QString::fromUtf8("✌️"),
          QString::fromUtf8("🤞"),
          QString::fromUtf8("🫰"),
          QString::fromUtf8("🤟"),
          QString::fromUtf8("🤘"),
          QString::fromUtf8("🤙"),
          QString::fromUtf8("👈"),
          QString::fromUtf8("👉"),
          QString::fromUtf8("👆"),
          QString::fromUtf8("🖕"),
          QString::fromUtf8("👇"),
          QString::fromUtf8("☝️"),
          QString::fromUtf8("🫵"),
          QString::fromUtf8("👍"),
          QString::fromUtf8("👎"),
          QString::fromUtf8("✊"),
          QString::fromUtf8("👊"),
          QString::fromUtf8("🤛"),
          QString::fromUtf8("🤜"),
          QString::fromUtf8("👏"),
          QString::fromUtf8("🙌"),
          QString::fromUtf8("🫶"),
          QString::fromUtf8("👐"),
          QString::fromUtf8("🤲"),
          QString::fromUtf8("🤝"),
          QString::fromUtf8("🙏"),
          QString::fromUtf8("✍️"),
          QString::fromUtf8("💅"),
      }                                                      },
    { QString::fromUtf8("爱心"),
     QString::fromUtf8("❤️"),
     {
          QString::fromUtf8("❤️"),
          QString::fromUtf8("🧡"),
          QString::fromUtf8("💛"),
          QString::fromUtf8("💚"),
          QString::fromUtf8("💙"),
          QString::fromUtf8("💜"),
          QString::fromUtf8("🖤"),
          QString::fromUtf8("🤍"),
          QString::fromUtf8("🤎"),
          QString::fromUtf8("💔"),
          QString::fromUtf8("❤️‍🔥"),
          QString::fromUtf8("❤️‍🩹"),
          QString::fromUtf8("💕"),
          QString::fromUtf8("💞"),
          QString::fromUtf8("💓"),
          QString::fromUtf8("💗"),
          QString::fromUtf8("💖"),
          QString::fromUtf8("💘"),
          QString::fromUtf8("💝"),
          QString::fromUtf8("💟"),
          QString::fromUtf8("💌"),
          QString::fromUtf8("💋"),
          QString::fromUtf8("💯"),
          QString::fromUtf8("💢"),
          QString::fromUtf8("💥"),
          QString::fromUtf8("💫"),
          QString::fromUtf8("💦"),
          QString::fromUtf8("💨"),
          QString::fromUtf8("🕳️"),
          QString::fromUtf8("💬"),
          QString::fromUtf8("💭"),
          QString::fromUtf8("🗯️"),
      }                                                      },
    { QString::fromUtf8("动物"),
     QString::fromUtf8("🐶"),
     {
          QString::fromUtf8("🐶"),
          QString::fromUtf8("🐱"),
          QString::fromUtf8("🐭"),
          QString::fromUtf8("🐹"),
          QString::fromUtf8("🐰"),
          QString::fromUtf8("🦊"),
          QString::fromUtf8("🐻"),
          QString::fromUtf8("🐼"),
          QString::fromUtf8("🐻‍❄️"),
          QString::fromUtf8("🐨"),
          QString::fromUtf8("🐯"),
          QString::fromUtf8("🦁"),
          QString::fromUtf8("🐮"),
          QString::fromUtf8("🐷"),
          QString::fromUtf8("🐸"),
          QString::fromUtf8("🐵"),
          QString::fromUtf8("🙈"),
          QString::fromUtf8("🙉"),
          QString::fromUtf8("🙊"),
          QString::fromUtf8("🐔"),
          QString::fromUtf8("🐧"),
          QString::fromUtf8("🐦"),
          QString::fromUtf8("🐤"),
          QString::fromUtf8("🦄"),
          QString::fromUtf8("🐝"),
          QString::fromUtf8("🪱"),
          QString::fromUtf8("🐛"),
          QString::fromUtf8("🦋"),
          QString::fromUtf8("🐌"),
          QString::fromUtf8("🐞"),
          QString::fromUtf8("🐜"),
          QString::fromUtf8("🪲"),
          QString::fromUtf8("🐢"),
          QString::fromUtf8("🐍"),
          QString::fromUtf8("🦎"),
          QString::fromUtf8("🦂"),
          QString::fromUtf8("🐙"),
          QString::fromUtf8("🦑"),
          QString::fromUtf8("🦐"),
          QString::fromUtf8("🐠"),
          QString::fromUtf8("🐟"),
          QString::fromUtf8("🐡"),
          QString::fromUtf8("🐬"),
          QString::fromUtf8("🐳"),
          QString::fromUtf8("🐋"),
          QString::fromUtf8("🦈"),
          QString::fromUtf8("🦭"),
          QString::fromUtf8("🐊"),
      }                                                      },
    { QString::fromUtf8("食物"),
     QString::fromUtf8("🍔"),
     {
          QString::fromUtf8("🍎"),
          QString::fromUtf8("🍐"),
          QString::fromUtf8("🍊"),
          QString::fromUtf8("🍋"),
          QString::fromUtf8("🍌"),
          QString::fromUtf8("🍉"),
          QString::fromUtf8("🍇"),
          QString::fromUtf8("🍓"),
          QString::fromUtf8("🫐"),
          QString::fromUtf8("🍈"),
          QString::fromUtf8("🍒"),
          QString::fromUtf8("🍑"),
          QString::fromUtf8("🥭"),
          QString::fromUtf8("🍍"),
          QString::fromUtf8("🥥"),
          QString::fromUtf8("🥝"),
          QString::fromUtf8("🍅"),
          QString::fromUtf8("🥑"),
          QString::fromUtf8("🍆"),
          QString::fromUtf8("🌶️"),
          QString::fromUtf8("🥦"),
          QString::fromUtf8("🥬"),
          QString::fromUtf8("🥒"),
          QString::fromUtf8("🌽"),
          QString::fromUtf8("🍔"),
          QString::fromUtf8("🍟"),
          QString::fromUtf8("🍕"),
          QString::fromUtf8("🌭"),
          QString::fromUtf8("🥪"),
          QString::fromUtf8("🌮"),
          QString::fromUtf8("🌯"),
          QString::fromUtf8("🍜"),
          QString::fromUtf8("🍝"),
          QString::fromUtf8("🍣"),
          QString::fromUtf8("🍱"),
          QString::fromUtf8("🍛"),
          QString::fromUtf8("🍙"),
          QString::fromUtf8("🍚"),
          QString::fromUtf8("🍘"),
          QString::fromUtf8("🥟"),
          QString::fromUtf8("🍰"),
          QString::fromUtf8("🎂"),
          QString::fromUtf8("🧁"),
          QString::fromUtf8("🍩"),
          QString::fromUtf8("🍪"),
          QString::fromUtf8("🍫"),
          QString::fromUtf8("🍬"),
          QString::fromUtf8("🍭"),
          QString::fromUtf8("☕"),
          QString::fromUtf8("🍵"),
          QString::fromUtf8("🧃"),
          QString::fromUtf8("🥤"),
          QString::fromUtf8("🍺"),
          QString::fromUtf8("🍻"),
          QString::fromUtf8("🥂"),
          QString::fromUtf8("🍷"),
      }                                                      },
    { QString::fromUtf8("物品"),
     QString::fromUtf8("⚽"),
     {
          QString::fromUtf8("⚽"),
          QString::fromUtf8("🏀"),
          QString::fromUtf8("🏈"),
          QString::fromUtf8("⚾"),
          QString::fromUtf8("🎾"),
          QString::fromUtf8("🏐"),
          QString::fromUtf8("🎱"),
          QString::fromUtf8("🏓"),
          QString::fromUtf8("🎮"),
          QString::fromUtf8("🕹️"),
          QString::fromUtf8("🎲"),
          QString::fromUtf8("🧩"),
          QString::fromUtf8("🎭"),
          QString::fromUtf8("🎨"),
          QString::fromUtf8("🎬"),
          QString::fromUtf8("🎤"),
          QString::fromUtf8("🎧"),
          QString::fromUtf8("🎵"),
          QString::fromUtf8("🎶"),
          QString::fromUtf8("🎸"),
          QString::fromUtf8("🎹"),
          QString::fromUtf8("🎺"),
          QString::fromUtf8("🎻"),
          QString::fromUtf8("🥁"),
          QString::fromUtf8("📱"),
          QString::fromUtf8("💻"),
          QString::fromUtf8("⌨️"),
          QString::fromUtf8("🖥️"),
          QString::fromUtf8("🖨️"),
          QString::fromUtf8("📷"),
          QString::fromUtf8("📹"),
          QString::fromUtf8("🔍"),
          QString::fromUtf8("💡"),
          QString::fromUtf8("🔦"),
          QString::fromUtf8("📖"),
          QString::fromUtf8("📚"),
          QString::fromUtf8("✏️"),
          QString::fromUtf8("📝"),
          QString::fromUtf8("📌"),
          QString::fromUtf8("📎"),
          QString::fromUtf8("🔑"),
          QString::fromUtf8("🔒"),
          QString::fromUtf8("🔓"),
          QString::fromUtf8("🛠️"),
          QString::fromUtf8("⚙️"),
          QString::fromUtf8("💰"),
          QString::fromUtf8("💎"),
          QString::fromUtf8("🏆"),
      }                                                      },
    { QString::fromUtf8("符号"),
     QString::fromUtf8("✅"),
     {
          QString::fromUtf8("✅"),
          QString::fromUtf8("❌"),
          QString::fromUtf8("❓"),
          QString::fromUtf8("❗"),
          QString::fromUtf8("‼️"),
          QString::fromUtf8("⁉️"),
          QString::fromUtf8("⭕"),
          QString::fromUtf8("🚫"),
          QString::fromUtf8("🔴"),
          QString::fromUtf8("🟠"),
          QString::fromUtf8("🟡"),
          QString::fromUtf8("🟢"),
          QString::fromUtf8("🔵"),
          QString::fromUtf8("🟣"),
          QString::fromUtf8("⚫"),
          QString::fromUtf8("⚪"),
          QString::fromUtf8("🟤"),
          QString::fromUtf8("🔺"),
          QString::fromUtf8("🔻"),
          QString::fromUtf8("🔸"),
          QString::fromUtf8("🔹"),
          QString::fromUtf8("🔶"),
          QString::fromUtf8("🔷"),
          QString::fromUtf8("▪️"),
          QString::fromUtf8("▫️"),
          QString::fromUtf8("◾"),
          QString::fromUtf8("◽"),
          QString::fromUtf8("🔘"),
          QString::fromUtf8("⬆️"),
          QString::fromUtf8("⬇️"),
          QString::fromUtf8("⬅️"),
          QString::fromUtf8("➡️"),
          QString::fromUtf8("↩️"),
          QString::fromUtf8("↪️"),
          QString::fromUtf8("🔄"),
          QString::fromUtf8("🔃"),
          QString::fromUtf8("⭐"),
          QString::fromUtf8("🌟"),
          QString::fromUtf8("✨"),
          QString::fromUtf8("🔥"),
          QString::fromUtf8("🎉"),
          QString::fromUtf8("🎊"),
          QString::fromUtf8("🏁"),
          QString::fromUtf8("🚩"),
          QString::fromUtf8("🏳️"),
          QString::fromUtf8("🏴"),
          QString::fromUtf8("🔔"),
          QString::fromUtf8("🔕"),
      }                                                      },
  };
}

void NXEmojiPickerPrivate::buildGrid(int categoryIndex)
{
  Q_Q(NXEmojiPicker);
  QLayoutItem *item;
  while ((item = m_GridLayout->takeAt(0)) != nullptr) {
    delete item->widget();
    delete item;
  }

  QStringList const &emojis = (categoryIndex == 0) ? m_RecentEmojis : m_Categories[categoryIndex].emojis;
  if (emojis.isEmpty() && categoryIndex == 0) {
    QLabel *hint = new QLabel(QString::fromUtf8("暂无最近使用"), m_GridContainer);
    hint->setAlignment(Qt::AlignCenter);
    hint->setStyleSheet(QStringLiteral("color:gray;padding:20px;"));
    m_GridLayout->addWidget(hint, 0, 0, 1, m_Columns);
    return;
  }

  int row = 0, col = 0;
  for (QString const &emoji : emojis) {
    QPushButton *btn = new QPushButton(emoji, m_GridContainer);
    btn->setFixedSize(m_EmojiSize, m_EmojiSize);
    btn->setFlat(true);
    btn->setStyleSheet(QString(QStringLiteral("QPushButton{font-size:%1px;border:none;border-radius:4px;}"
                                              "QPushButton:hover{background-color:rgba(128,128,128,40);}"))
                           .arg(m_EmojiSize - 10));
    QObject::connect(btn, &QPushButton::clicked, q, [this, q, emoji]() {
      m_RecentEmojis.removeAll(emoji);
      m_RecentEmojis.prepend(emoji);
      if (m_RecentEmojis.size() > 32) {
        m_RecentEmojis.removeLast();
      }
      Q_EMIT q->emojiSelected(emoji);
      q->hide();
    });
    m_GridLayout->addWidget(btn, row, col);
    col++;
    if (col >= m_Columns) {
      col = 0;
      row++;
    }
  }
}

void NXEmojiPickerPrivate::filterEmojis(QString const &text)
{
  Q_Q(NXEmojiPicker);
  if (text.isEmpty()) {
    buildGrid(m_CurrentCategory);
    return;
  }

  QLayoutItem *item;
  while ((item = m_GridLayout->takeAt(0)) != nullptr) {
    delete item->widget();
    delete item;
  }

  QStringList all;
  for (int i = 1; i < m_Categories.size(); i++) {
    all.append(m_Categories[i].emojis);
  }

  int row = 0, col = 0;
  for (QString const &emoji : all) {
    QPushButton *btn = new QPushButton(emoji, m_GridContainer);
    btn->setFixedSize(m_EmojiSize, m_EmojiSize);
    btn->setFlat(true);
    btn->setStyleSheet(QString(QStringLiteral("QPushButton{font-size:%1px;border:none;border-radius:4px;}"
                                              "QPushButton:hover{background-color:rgba(128,128,128,40);}"))
                           .arg(m_EmojiSize - 10));
    QObject::connect(btn, &QPushButton::clicked, q, [this, q, emoji]() {
      m_RecentEmojis.removeAll(emoji);
      m_RecentEmojis.prepend(emoji);
      if (m_RecentEmojis.size() > 32) {
        m_RecentEmojis.removeLast();
      }
      Q_EMIT q->emojiSelected(emoji);
      q->hide();
    });
    m_GridLayout->addWidget(btn, row, col);
    col++;
    if (col >= m_Columns) {
      col = 0;
      row++;
    }
  }
}
