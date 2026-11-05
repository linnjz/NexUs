#include "NXBreadcrumbBar.h"

#include <QHeaderView>
#include <QScroller>
#include <QVBoxLayout>

#include "DeveloperComponents/NXBreadcrumbBarDelegate.h"
#include "DeveloperComponents/NXBreadcrumbBarModel.h"
#include "DeveloperComponents/NXBaseListView.h"
#include "private/NXBreadcrumbBarPrivate.h"
Q_PROPERTY_CREATE_CPP(NXBreadcrumbBar, QPS_PAEMIT(bool), IsAutoRemove)

NXBreadcrumbBar::NXBreadcrumbBar(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXBreadcrumbBarPrivate())
{
  Q_D(NXBreadcrumbBar);
  d->q_ptr           = this;
  d->m_TextPixelSize = 18;
  d->m_IsAutoRemove  = true;
  setFixedHeight(37);
  setObjectName("NXBreadcrumbBar");
  setStyleSheet(QStringLiteral("#NXBreadcrumbBar{background-color:transparent;}"));
  setMouseTracking(true);

  d->m_ListView = new NXBaseListView(this);
  d->m_ListView->setMinimumHeight(0);
  d->m_ListView->setFlow(QListView::LeftToRight);
  d->m_ListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  d->m_ListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  d->m_ListModel = new NXBreadcrumbBarModel(this);
  d->m_ListView->setModel(d->m_ListModel);
  d->m_ListDelegate = new NXBreadcrumbBarDelegate(this);
  d->m_ListView->setItemDelegate(d->m_ListDelegate);
  connect(d->m_ListView, &QListView::clicked, this, [=](QModelIndex const &index) {
    if (d->m_IsAutoRemove) {
      if (d->m_ListModel->getBreadcrumbListCount() != 1 &&
          index.row() != d->m_ListModel->getBreadcrumbListCount() * 2 - 2 &&
          index.data(Qt::DisplayRole).toString() != QStringLiteral(">")) {
        Q_EMIT breadcrumbClicked(index.data(Qt::DisplayRole).toString(), d->m_ListModel->getBreadcrumbList());
        d->m_ListModel->removeBreadcrumb(index.row() / 2 + 1);
      }
    } else {
      Q_EMIT breadcrumbClicked(index.data(Qt::DisplayRole).toString(), d->m_ListModel->getBreadcrumbList());
    }
  });
  QFont textFont = this->font();
  textFont.setLetterSpacing(QFont::AbsoluteSpacing, 0.5);
  textFont.setPixelSize(d->m_TextPixelSize);
  d->m_ListView->setFont(textFont);

  QScroller::grabGesture(d->m_ListView->viewport(), QScroller::LeftMouseButtonGesture);
  QScroller *scroller            = QScroller::scroller(d->m_ListView->viewport());
  QScrollerProperties properties = scroller->scrollerProperties();
  properties.setScrollMetric(QScrollerProperties::MousePressEventDelay, 0);
  properties.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy, QScrollerProperties::OvershootAlwaysOn);
  properties.setScrollMetric(QScrollerProperties::OvershootDragResistanceFactor, 0.35);
  properties.setScrollMetric(QScrollerProperties::OvershootScrollTime, 0.5);
  properties.setScrollMetric(QScrollerProperties::FrameRate, QScrollerProperties::Fps60);
  scroller->setScrollerProperties(properties);

  connect(scroller, &QScroller::stateChanged, this, [=](QScroller::State newstate) {
    if (newstate == QScroller::Pressed) {
      d->m_ListDelegate->setPressIndex(d->m_ListView->indexAt(d->m_ListView->mapFromGlobal(QCursor::pos())));
      d->m_ListView->viewport()->update();
    } else if (newstate == QScroller::Scrolling || newstate == QScroller::Inactive) {
      d->m_ListDelegate->setPressIndex(QModelIndex());
    }
  });

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->addWidget(d->m_ListView);
}

NXBreadcrumbBar::~NXBreadcrumbBar()
{
}

void NXBreadcrumbBar::setTextPixelSize(int textPixelSize)
{
  Q_D(NXBreadcrumbBar);
  if (textPixelSize > 0) {
    d->m_TextPixelSize = textPixelSize;
    QFont textFont     = this->font();
    textFont.setLetterSpacing(QFont::AbsoluteSpacing, 0.5);
    textFont.setPixelSize(d->m_TextPixelSize);
    d->m_ListView->setFont(textFont);
    Q_EMIT signalTextPixelSizeChanged();
  }
}

int NXBreadcrumbBar::getTextPixelSize() const noexcept(std::is_nothrow_copy_constructible_v<int>)
{
  Q_CONST_D(NXBreadcrumbBar);
  return d->m_TextPixelSize;
}

void NXBreadcrumbBar::setBreadcrumbList(QStringList const &breadcrumbList)
{
  Q_D(NXBreadcrumbBar);
  d->m_ListModel->setBreadcrumbList(breadcrumbList);
}

QStringList NXBreadcrumbBar::appendBreadcrumb(QString const &breadcrumb)
{
  Q_D(NXBreadcrumbBar);
  d->m_ListModel->appendBreadcrumb(breadcrumb);
  return d->m_ListModel->getBreadcrumbList();
}

QStringList NXBreadcrumbBar::removeBreadcrumb(QString const &breadcrumb)
{
  Q_D(NXBreadcrumbBar);
  d->m_ListModel->removeBreadcrumb(breadcrumb);
  return d->m_ListModel->getBreadcrumbList();
}

int NXBreadcrumbBar::getBreadcrumbListCount() const
{
  Q_CONST_D(NXBreadcrumbBar);
  return d->m_ListModel->getBreadcrumbListCount();
}

QStringList NXBreadcrumbBar::getBreadcrumbList() const
{
  Q_CONST_D(NXBreadcrumbBar);
  return d->m_ListModel->getBreadcrumbList();
}
