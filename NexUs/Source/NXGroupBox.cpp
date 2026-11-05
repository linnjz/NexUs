#include "NXGroupBox.h"

#include <QLayout>

#include "DeveloperComponents/NXGroupBoxStyle.h"
#include "NXTheme.h"
#include "private/NXGroupBoxPrivate.h"

NXGroupBox::NXGroupBox(QWidget *parent)
    : QGroupBox(parent)
    , d_ptr(new NXGroupBoxPrivate())
{
  Q_D(NXGroupBox);
  d->q_ptr = this;

  setMouseTracking(true);
  setObjectName("NXGroupBox");

  QFont font = this->font();
  font.setPixelSize(15);
  setFont(font);

  setStyle(new NXGroupBoxStyle(style()));
  d->onThemeChanged(nxTheme->getThemeMode());
  connect(nxTheme, &NXTheme::themeModeChanged, d, &NXGroupBoxPrivate::onThemeChanged);
}

NXGroupBox::NXGroupBox(QString const &title, QWidget *parent)
    : NXGroupBox(parent)
{
  setTitle(title);
}

NXGroupBox::~NXGroupBox()
{
  delete this->style();
}

void NXGroupBox::paintEvent(QPaintEvent *event)
{
  Q_D(NXGroupBox);
  if (palette().color(QPalette::WindowText) != NXThemeColor(d->m_ThemeMode, BasicText)) {
    d->onThemeChanged(d->m_ThemeMode);
  }
  QGroupBox::paintEvent(event);
}

Q_PROPERTY_CREATE_CPP(NXGroupBox, QPS_PAEMIT(int), BorderRadius)
