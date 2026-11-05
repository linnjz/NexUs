#include "NXMessageDialog.h"

#include <QHBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QVBoxLayout>

#include "NXTheme.h"
#include "private/NXMessageDialogPrivate.h"

Q_PROPERTY_CREATE_CPP(NXMessageDialog, QPS_SCREF_PAEMIT(QString), Title)
Q_PROPERTY_CREATE_CPP(NXMessageDialog, QPS_SCREF_PAEMIT(QString), Content)
Q_PROPERTY_CREATE_CPP(NXMessageDialog, QPS_PAEMIT(int), BorderRadius)
Q_PROPERTY_CREATE_CPP(NXMessageDialog, QPS_PAEMIT(int), TitlePixelSize)
Q_PROPERTY_CREATE_CPP(NXMessageDialog, QPS_PAEMIT(int), ContentPixelSize)

NXMessageDialog::NXMessageDialog(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new NXMessageDialogPrivate())
{
  Q_D(NXMessageDialog);
  d->q_ptr = this;

  d->m_BorderRadius     = 8;
  d->m_Title            = QStringLiteral("标题");
  d->m_Content          = QStringLiteral("");
  d->m_TitlePixelSize   = 15;
  d->m_ContentPixelSize = 13;

  setMinimumSize(280, 150);

  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
    update();
  });

  d->m_ConfirmButton = new NXMessageDialogButton(NXMessageDialogButton::Confirm, this);
  d->m_ConfirmButton->setFixedHeight(40);
  d->m_CancelButton = new NXMessageDialogButton(NXMessageDialogButton::Cancel, this);
  d->m_CancelButton->setFixedHeight(40);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);

  mainLayout->addStretch();

  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->setContentsMargins(0, 0, 0, 0);
  buttonLayout->setSpacing(0);
  buttonLayout->addWidget(d->m_ConfirmButton, 1);
  buttonLayout->addWidget(d->m_CancelButton, 1);

  mainLayout->addLayout(buttonLayout);

  connect(d->m_ConfirmButton, &NXMessageDialogButton::clicked, this, [=]() {
    Q_EMIT confirmed();
    hide();
  });
  connect(d->m_CancelButton, &NXMessageDialogButton::clicked, this, [=]() {
    Q_EMIT cancelled();
    hide();
  });
}

NXMessageDialog::~NXMessageDialog()
{
}

void NXMessageDialog::paintEvent(QPaintEvent *event)
{
  Q_D(NXMessageDialog);
  QPainter painter(this);
  painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing | QPainter::TextAntialiasing);

  painter.setPen(Qt::NoPen);
  painter.setBrush(NXThemeColor(d->m_ThemeMode, BasicBase));
  painter.drawRoundedRect(rect(), d->m_BorderRadius, d->m_BorderRadius);

  QFont titleFont = this->font();
  titleFont.setPixelSize(d->m_TitlePixelSize);
  titleFont.setWeight(QFont::Bold);
  painter.setFont(titleFont);
  painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
  painter.drawText(QRect(15, 15, width() - 30, 25), Qt::AlignLeft | Qt::AlignTop, d->m_Title);

  QFont contentFont = this->font();
  contentFont.setPixelSize(d->m_ContentPixelSize);
  contentFont.setWeight(QFont::Normal);
  painter.setFont(contentFont);
  painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
  int contentHeight = height() - 40 - 45 - 15;
  painter.drawText(QRect(15, 45, width() - 30, contentHeight), Qt::TextWordWrap | Qt::AlignLeft | Qt::AlignTop, d->m_Content);

  painter.setPen(QPen(NXThemeColor(d->m_ThemeMode, BasicBorder), 1));
  painter.drawLine(0, height() - 40, width(), height() - 40);
}
