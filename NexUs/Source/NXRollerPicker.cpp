#include "NXRollerPicker.h"

#include <QDebug>
#include <QPainter>
#include <QVBoxLayout>

#include "NXApplication.h"
#include "NXTheme.h"
#include "private/NXRollerPickerPrivate.h"

Q_PROPERTY_CREATE_CPP(NXRollerPicker, QPS_PAEMIT(int), BorderRadius)

NXRollerPicker::NXRollerPicker(QWidget *parent)
    : QPushButton { parent }
    , d_ptr(new NXRollerPickerPrivate())
{
  Q_D(NXRollerPicker);
  d->q_ptr          = this;
  d->m_BorderRadius = 3;
  setMouseTracking(true);
  setFixedSize(90, 30);
  setObjectName("NXRollerPicker");
  setStyleSheet(QStringLiteral("#NXRollerPicker{background-color:transparent;}"));
  QFont font = this->font();
  font.setPixelSize(nxApp->getFontPixelSize() + 3);
  setFont(font);

  d->m_RollerPickerContainer = new NXRollerPickerContainer(this);
  d->m_RollerPickerContainer->resize(90, d->m_PickerRollerHeight + d->m_RollerPickerContainer->getButtonAreaHeight());
  d->m_RollerPickerContainer->hide();
  connect(this, &QPushButton::clicked, d, &NXRollerPickerPrivate::onRollerPickerClicked);
  connect(d->m_RollerPickerContainer, &NXRollerPickerContainer::overButtonClicked, d, &NXRollerPickerPrivate::onOverButtonClicked);
  connect(d->m_RollerPickerContainer, &NXRollerPickerContainer::cancelButtonClicked, d, &NXRollerPickerPrivate::onCancelButtonClicked);

  d->m_ContainerLayout = new QHBoxLayout(d->m_RollerPickerContainer);
  d->m_ContainerLayout->setSpacing(0);
  d->m_ContainerLayout->setContentsMargins(0, 0, 0, 0);

  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
  });
}

NXRollerPicker::~NXRollerPicker()
{
}

void NXRollerPicker::addRoller(QStringList const &itemList, bool isEnableLoop)
{
  Q_D(NXRollerPicker);
  if (itemList.isEmpty()) {
    return;
  }
  NXRoller *roller = new NXRoller(this);
  roller->setIsContainer(true);
  roller->setItemList(itemList);
  roller->setMaxVisibleItems(7);
  roller->setIsEnableLoop(isEnableLoop);
  roller->setItemHeight(35);
  roller->setFixedHeight(d->m_PickerRollerHeight);
  connect(roller, &NXRoller::currentDataChanged, this, [=]() {
    Q_EMIT currentDataSelectionChanged(getCurrentData());
  });
  d->m_RollerPickerContainer->m_RollerList.append(roller);
  d->m_ContainerLayout->addWidget(roller);
  setFixedWidth(d->_getRollerTotalWidth());
}

void NXRollerPicker::removeRoller(int index)
{
  Q_D(NXRollerPicker);
  if (index >= d->m_RollerPickerContainer->m_RollerList.count()) {
    return;
  }
  auto roller = d->m_RollerPickerContainer->m_RollerList[index];
  d->m_RollerPickerContainer->m_RollerList.removeAt(index);
  d->m_ContainerLayout->removeWidget(roller);
  roller->deleteLater();
  setFixedWidth(d->_getRollerTotalWidth());
}

void NXRollerPicker::setRollerItemList(int index, QStringList const &itemList)
{
  Q_D(NXRollerPicker);
  if (index >= d->m_RollerPickerContainer->m_RollerList.count()) {
    return;
  }
  auto roller = d->m_RollerPickerContainer->m_RollerList[index];
  roller->setItemList(itemList);
}

void NXRollerPicker::setRollerWidth(int index, int width)
{
  Q_D(NXRollerPicker);
  if (index >= d->m_RollerPickerContainer->m_RollerList.count()) {
    return;
  }
  auto roller = d->m_RollerPickerContainer->m_RollerList[index];
  roller->setFixedWidth(width);
  setFixedWidth(d->_getRollerTotalWidth());
}

void NXRollerPicker::setCurrentData(int index, QString const &data)
{
  Q_D(NXRollerPicker);
  if (index >= d->m_RollerPickerContainer->m_RollerList.count()) {
    return;
  }
  d->m_RollerPickerContainer->m_RollerList[index]->setCurrentData(data);
}

void NXRollerPicker::setCurrentData(QStringList const &dataList)
{
  Q_D(NXRollerPicker);
  for (int i = 0; i < d->m_RollerPickerContainer->m_RollerList.count(); i++) {
    if (dataList.count() <= i) {
      return;
    }
    auto roller = d->m_RollerPickerContainer->m_RollerList[i];
    roller->setCurrentData(dataList[i]);
  }
}

QString NXRollerPicker::getCurrentData(int index) const
{
  Q_CONST_D(NXRollerPicker);
  if (index >= d->m_RollerPickerContainer->m_RollerList.count()) {
    return {};
  }
  return d->m_RollerPickerContainer->m_RollerList[index]->getCurrentData();
}

QStringList NXRollerPicker::getCurrentData() const
{
  Q_CONST_D(NXRollerPicker);
  QStringList dataList;
  for (auto roller : d->m_RollerPickerContainer->m_RollerList) {
    dataList.append(roller->getCurrentData());
  }
  return dataList;
}

void NXRollerPicker::setCurrentIndex(int rollerIndex, int index)
{
  Q_D(NXRollerPicker);
  if (rollerIndex >= d->m_RollerPickerContainer->m_RollerList.count()) {
    return;
  }
  d->m_RollerPickerContainer->m_RollerList[rollerIndex]->setCurrentIndex(index);
}

void NXRollerPicker::setCurrentIndex(QList<int> const &indexList)
{
  Q_D(NXRollerPicker);
  for (int i = 0; i < d->m_RollerPickerContainer->m_RollerList.count(); i++) {
    if (indexList.count() <= i) {
      return;
    }
    auto roller = d->m_RollerPickerContainer->m_RollerList[i];
    roller->setCurrentIndex(indexList[i]);
  }
}

int NXRollerPicker::getCurrentIndex(int rollerIndex) const
{
  Q_CONST_D(NXRollerPicker);
  if (rollerIndex >= d->m_RollerPickerContainer->m_RollerList.count()) {
    return -1;
  }
  return d->m_RollerPickerContainer->m_RollerList[rollerIndex]->getCurrentIndex();
}

QList<int> NXRollerPicker::getCurrentIndex() const
{
  Q_CONST_D(NXRollerPicker);
  QList<int> currentIndexList;
  for (auto roller : d->m_RollerPickerContainer->m_RollerList) {
    currentIndexList.append(roller->getCurrentIndex());
  }
  return currentIndexList;
}

void NXRollerPicker::paintEvent(QPaintEvent *event)
{
  Q_D(NXRollerPicker);
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  // 边框和背景绘制
  QRect baseRect = rect();
  baseRect.adjust(1, 1, -1, -1);
  painter.setPen(NXThemeColor(d->m_ThemeMode, BasicBorder));
  painter.setBrush(underMouse() ? NXThemeColor(d->m_ThemeMode, BasicHover) : NXThemeColor(d->m_ThemeMode, BasicBase));
  painter.drawRoundedRect(baseRect, d->m_BorderRadius, d->m_BorderRadius);

  // Roller内容绘制
  int rollerXOffset = 0;
  for (int i = 0; i < d->m_RollerPickerContainer->m_RollerList.count(); i++) {
    auto roller = d->m_RollerPickerContainer->m_RollerList[i];
    painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
    int rollerWidth = roller->width();
    painter.drawText(QRect(rollerXOffset, baseRect.y(), rollerWidth, baseRect.height()), Qt::AlignCenter, roller->getCurrentData());
    rollerXOffset += rollerWidth;
    if (i != d->m_RollerPickerContainer->m_RollerList.count() - 1) {
      painter.setPen(NXThemeColor(d->m_ThemeMode, BasicBorder));
      painter.drawLine(rollerXOffset, 0, rollerXOffset, baseRect.bottom());
    }
  }
  painter.restore();
}
