#include "NXRollerPicker.h"

#include <QDebug>
#include <QPainter>
#include <QVBoxLayout>

#include "NXApplication.h"
#include "NXTheme.h"
#include "private/NXRollerPickerPrivate.h"

Q_PROPERTY_CREATE_CPP(NXRollerPicker, int, BorderRadius)

NXRollerPicker::NXRollerPicker(QWidget *parent)
    : QPushButton { parent }
    , d_ptr(new NXRollerPickerPrivate())
{
  Q_D(NXRollerPicker);
  d->q_ptr          = this;
  d->_pBorderRadius = 3;
  setMouseTracking(true);
  setFixedSize(90, 30);
  setObjectName("NXRollerPicker");
  setStyleSheet(QStringLiteral("#NXRollerPicker{background-color:transparent;}"));
  QFont font = this->font();
  font.setPixelSize(nxApp->getFontPixelSize() + 3);
  setFont(font);

  d->_rollerPickerContainer = new NXRollerPickerContainer(this);
  d->_rollerPickerContainer->resize(90, d->_pickerRollerHeight + d->_rollerPickerContainer->getButtonAreaHeight());
  d->_rollerPickerContainer->hide();
  connect(this, &QPushButton::clicked, d, &NXRollerPickerPrivate::onRollerPickerClicked);
  connect(d->_rollerPickerContainer, &NXRollerPickerContainer::overButtonClicked, d, &NXRollerPickerPrivate::onOverButtonClicked);
  connect(d->_rollerPickerContainer, &NXRollerPickerContainer::cancelButtonClicked, d, &NXRollerPickerPrivate::onCancelButtonClicked);

  d->_containerLayout = new QHBoxLayout(d->_rollerPickerContainer);
  d->_containerLayout->setSpacing(0);
  d->_containerLayout->setContentsMargins(0, 0, 0, 0);

  d->_themeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
  });
}

NXRollerPicker::~NXRollerPicker()
{
}

void NXRollerPicker::addRoller(const QStringList &itemList, bool isEnableLoop)
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
  roller->setFixedHeight(d->_pickerRollerHeight);
  connect(roller, &NXRoller::currentDataChanged, this, [=]() {
    Q_EMIT currentDataSelectionChanged(getCurrentData());
  });
  d->_rollerPickerContainer->_rollerList.append(roller);
  d->_containerLayout->addWidget(roller);
  setFixedWidth(d->_getRollerTotalWidth());
}

void NXRollerPicker::removeRoller(int index)
{
  Q_D(NXRollerPicker);
  if (index >= d->_rollerPickerContainer->_rollerList.count()) {
    return;
  }
  auto roller = d->_rollerPickerContainer->_rollerList[index];
  d->_rollerPickerContainer->_rollerList.removeAt(index);
  d->_containerLayout->removeWidget(roller);
  roller->deleteLater();
  setFixedWidth(d->_getRollerTotalWidth());
}

void NXRollerPicker::setRollerItemList(int index, const QStringList &itemList)
{
  Q_D(NXRollerPicker);
  if (index >= d->_rollerPickerContainer->_rollerList.count()) {
    return;
  }
  auto roller = d->_rollerPickerContainer->_rollerList[index];
  roller->setItemList(itemList);
}

void NXRollerPicker::setRollerWidth(int index, int width)
{
  Q_D(NXRollerPicker);
  if (index >= d->_rollerPickerContainer->_rollerList.count()) {
    return;
  }
  auto roller = d->_rollerPickerContainer->_rollerList[index];
  roller->setFixedWidth(width);
  setFixedWidth(d->_getRollerTotalWidth());
}

void NXRollerPicker::setCurrentData(int index, const QString &data)
{
  Q_D(NXRollerPicker);
  if (index >= d->_rollerPickerContainer->_rollerList.count()) {
    return;
  }
  d->_rollerPickerContainer->_rollerList[index]->setCurrentData(data);
}

void NXRollerPicker::setCurrentData(const QStringList &dataList)
{
  Q_D(NXRollerPicker);
  for (int i = 0; i < d->_rollerPickerContainer->_rollerList.count(); i++) {
    if (dataList.count() <= i) {
      return;
    }
    auto roller = d->_rollerPickerContainer->_rollerList[i];
    roller->setCurrentData(dataList[i]);
  }
}

QString NXRollerPicker::getCurrentData(int index) const
{
  Q_D(const NXRollerPicker);
  if (index >= d->_rollerPickerContainer->_rollerList.count()) {
    return {};
  }
  return d->_rollerPickerContainer->_rollerList[index]->getCurrentData();
}

QStringList NXRollerPicker::getCurrentData() const
{
  Q_D(const NXRollerPicker);
  QStringList dataList;
  for (auto roller : d->_rollerPickerContainer->_rollerList) {
    dataList.append(roller->getCurrentData());
  }
  return dataList;
}

void NXRollerPicker::setCurrentIndex(int rollerIndex, int index)
{
  Q_D(NXRollerPicker);
  if (rollerIndex >= d->_rollerPickerContainer->_rollerList.count()) {
    return;
  }
  d->_rollerPickerContainer->_rollerList[rollerIndex]->setCurrentIndex(index);
}

void NXRollerPicker::setCurrentIndex(const QList<int> &indexList)
{
  Q_D(NXRollerPicker);
  for (int i = 0; i < d->_rollerPickerContainer->_rollerList.count(); i++) {
    if (indexList.count() <= i) {
      return;
    }
    auto roller = d->_rollerPickerContainer->_rollerList[i];
    roller->setCurrentIndex(indexList[i]);
  }
}

int NXRollerPicker::getCurrentIndex(int rollerIndex) const
{
  Q_D(const NXRollerPicker);
  if (rollerIndex >= d->_rollerPickerContainer->_rollerList.count()) {
    return -1;
  }
  return d->_rollerPickerContainer->_rollerList[rollerIndex]->getCurrentIndex();
}

QList<int> NXRollerPicker::getCurrentIndex() const
{
  Q_D(const NXRollerPicker);
  QList<int> currentIndexList;
  for (auto roller : d->_rollerPickerContainer->_rollerList) {
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
  painter.setPen(NXThemeColor(d->_themeMode, BasicBorder));
  painter.setBrush(underMouse() ? NXThemeColor(d->_themeMode, BasicHover) : NXThemeColor(d->_themeMode, BasicBase));
  painter.drawRoundedRect(baseRect, d->_pBorderRadius, d->_pBorderRadius);

  // Roller内容绘制
  int rollerXOffset = 0;
  for (int i = 0; i < d->_rollerPickerContainer->_rollerList.count(); i++) {
    auto roller = d->_rollerPickerContainer->_rollerList[i];
    painter.setPen(NXThemeColor(d->_themeMode, BasicText));
    int rollerWidth = roller->width();
    painter.drawText(QRect(rollerXOffset, baseRect.y(), rollerWidth, baseRect.height()), Qt::AlignCenter, roller->getCurrentData());
    rollerXOffset += rollerWidth;
    if (i != d->_rollerPickerContainer->_rollerList.count() - 1) {
      painter.setPen(NXThemeColor(d->_themeMode, BasicBorder));
      painter.drawLine(rollerXOffset, 0, rollerXOffset, baseRect.bottom());
    }
  }
  painter.restore();
}
