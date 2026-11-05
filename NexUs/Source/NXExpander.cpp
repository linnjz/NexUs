#include "NXExpander.h"

#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QVBoxLayout>
#include <QVariantAnimation>

#include "NXTheme.h"
#include "private/NXExpanderPrivate.h"

Q_PROPERTY_CREATE_CPP(NXExpander, QPS_SCREF_PAEMIT(QString), Title)
Q_PROPERTY_CREATE_CPP(NXExpander, QPS_SCREF_PAEMIT(QString), SubTitle)
Q_PROPERTY_CREATE_CPP(NXExpander, QPS_PAEMIT(int), BorderRadius)
Q_PROPERTY_CREATE_CPP(NXExpander, QPS_PAEMIT(int), AnimationDuration)
Q_PROPERTY_CREATE_CPP(NXExpander, QPS_PAEMIT(NXIconType::IconName), HeaderIcon)

NXExpanderPrivate::NXExpanderPrivate(QObject *parent)
    : QObject(parent)
{
}

NXExpanderPrivate::~NXExpanderPrivate()
{
}

void NXExpanderPrivate::_toggleExpand()
{
  m_IsExpanded = !m_IsExpanded;
  _startExpandAnimation(m_IsExpanded);
  Q_Q(NXExpander);
  Q_EMIT q->expandStateChanged(m_IsExpanded);
}

void NXExpanderPrivate::_startExpandAnimation(bool expand)
{
  Q_Q(NXExpander);
  if (!m_ContentWidget) {
    return;
  }
  if (!m_ExpandAnimation) {
    m_ExpandAnimation = new QVariantAnimation(q);
    m_ExpandAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_ExpandAnimation, &QVariantAnimation::valueChanged, q, [=](QVariant const &value) {
      q->setFixedHeight(value.toInt());
    });
  }
  m_ExpandAnimation->stop();
  m_ExpandAnimation->setDuration(m_AnimationDuration);

  // Ensure content widget is laid out so sizeHint is valid
  m_ContentWidget->adjustSize();
  m_ContentHeight = m_ContentWidget->sizeHint().height() + m_ContentLayout->contentsMargins().top() +
                   m_ContentLayout->contentsMargins().bottom();
  if (m_ContentHeight < 40) {
    m_ContentHeight = 40;
  }
  int collapsedHeight = m_HeaderHeight;
  int expandedHeight  = m_HeaderHeight + m_ContentHeight;

  if (expand) {
    m_ContentContainer->show();
    m_ExpandAnimation->setStartValue(collapsedHeight);
    m_ExpandAnimation->setEndValue(expandedHeight);
  } else {
    m_ContentContainer->hide();
    m_ExpandAnimation->setStartValue(q->height());
    m_ExpandAnimation->setEndValue(collapsedHeight);
  }
  m_ExpandAnimation->start();
}

NXExpander::NXExpander(QWidget *parent)
    : QWidget { parent }
    , d_ptr(new NXExpanderPrivate())
{
  Q_D(NXExpander);
  d->q_ptr               = this;
  d->m_BorderRadius      = 6;
  d->m_Title             = QStringLiteral("");
  d->m_SubTitle          = QStringLiteral("");
  d->m_HeaderIcon        = NXIconType::None;
  d->m_AnimationDuration = 250;
  setObjectName("NXExpander");
  setMouseTracking(true);
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

  d->m_MainLayout = new QVBoxLayout(this);
  d->m_MainLayout->setContentsMargins(0, 0, 0, 0);
  d->m_MainLayout->setSpacing(0);

  // Header area (painted via paintEvent, not a separate widget)
  d->m_HeaderWidget = new QWidget(this);
  d->m_HeaderWidget->setFixedHeight(d->m_HeaderHeight);
  d->m_HeaderWidget->setAttribute(Qt::WA_TransparentForMouseEvents);

  // Content container
  d->m_ContentContainer = new QWidget(this);
  d->m_ContentLayout    = new QVBoxLayout(d->m_ContentContainer);
  d->m_ContentLayout->setContentsMargins(15, 5, 15, 5);
  d->m_ContentContainer->hide();

  d->m_MainLayout->addWidget(d->m_HeaderWidget);
  d->m_MainLayout->addWidget(d->m_ContentContainer);

  setFixedHeight(d->m_HeaderHeight);

  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
    update();
  });
}

NXExpander::NXExpander(QString const &title, QWidget *parent)
    : NXExpander(parent)
{
  Q_D(NXExpander);
  d->m_Title = title;
}

NXExpander::~NXExpander()
{
}

void NXExpander::setExpandDirection(ExpandDirection direction)
{
  Q_D(NXExpander);
  d->m_ExpandDirection = direction;
  // Rebuild layout order
  d->m_MainLayout->removeWidget(d->m_HeaderWidget);
  d->m_MainLayout->removeWidget(d->m_ContentContainer);
  if (direction == Up) {
    d->m_MainLayout->addWidget(d->m_ContentContainer);
    d->m_MainLayout->addWidget(d->m_HeaderWidget);
  } else {
    d->m_MainLayout->addWidget(d->m_HeaderWidget);
    d->m_MainLayout->addWidget(d->m_ContentContainer);
  }
  update();
}

NXExpander::ExpandDirection NXExpander::getExpandDirection() const
{
  return d_ptr->m_ExpandDirection;
}

void NXExpander::setContentWidget(QWidget *widget)
{
  Q_D(NXExpander);
  if (d->m_ContentWidget) {
    d->m_ContentLayout->removeWidget(d->m_ContentWidget);
    d->m_ContentWidget->deleteLater();
  }
  d->m_ContentWidget = widget;
  if (widget) {
    d->m_ContentLayout->addWidget(widget);
  }
}

QWidget *NXExpander::getContentWidget() const
{
  return d_ptr->m_ContentWidget;
}

void NXExpander::setHeaderWidget(QWidget *widget)
{
  Q_D(NXExpander);
  if (d->m_HeaderCustomWidget) {
    d->m_HeaderCustomWidget->deleteLater();
  }
  d->m_HeaderCustomWidget = widget;
  if (widget) {
    widget->setParent(d->m_HeaderWidget);
    widget->setAttribute(Qt::WA_TransparentForMouseEvents);
    widget->move(d->m_HeaderHeight, 0);
    widget->setFixedHeight(d->m_HeaderHeight);
  }
  update();
}

void NXExpander::setIsExpanded(bool expanded)
{
  Q_D(NXExpander);
  if (d->m_IsExpanded != expanded) {
    d->_toggleExpand();
  }
}

bool NXExpander::getIsExpanded() const
{
  return d_ptr->m_IsExpanded;
}

void NXExpander::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);
  Q_D(NXExpander);
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  // Background
  painter.setPen(QPen(NXThemeColor(d->m_ThemeMode, BasicBorder), 1));
  painter.setBrush(NXThemeColor(d->m_ThemeMode, BasicBase));
  painter.drawRoundedRect(QRectF(0.5, 0.5, width() - 1, height() - 1), d->m_BorderRadius, d->m_BorderRadius);

  // Header separator line when expanded
  if (d->m_IsExpanded || (d->m_ExpandAnimation && d->m_ExpandAnimation->state() == QVariantAnimation::Running)) {
    int lineY = (d->m_ExpandDirection == Down) ? d->m_HeaderHeight : height() - d->m_HeaderHeight;
    painter.setPen(QPen(NXThemeColor(d->m_ThemeMode, BasicBorder), 1));
    painter.drawLine(d->m_BorderRadius, lineY, width() - d->m_BorderRadius, lineY);
  }

  // Header area
  int headerY = (d->m_ExpandDirection == Down) ? 0 : height() - d->m_HeaderHeight;

  // Header icon
  int textStartX = 15;
  if (d->m_HeaderIcon != NXIconType::None) {
    QFont iconFont = QFont(QStringLiteral("NXAwesome"));
    iconFont.setPixelSize(16);
    painter.setFont(iconFont);
    painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
    QRect iconRect(15, headerY, 20, d->m_HeaderHeight);
    painter.drawText(iconRect, Qt::AlignVCenter | Qt::AlignLeft, QChar((unsigned short) d->m_HeaderIcon));
    textStartX = 42;
  }

  // Title
  if (!d->m_Title.isEmpty()) {
    QFont titleFont = font();
    titleFont.setPixelSize(14);
    titleFont.setBold(true);
    painter.setFont(titleFont);
    painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
    QRect titleRect(textStartX, headerY, width() - textStartX - 40, d->m_HeaderHeight);
    painter.drawText(titleRect, Qt::AlignVCenter | Qt::AlignLeft, d->m_Title);

    // SubTitle
    if (!d->m_SubTitle.isEmpty()) {
      QFontMetrics fm(titleFont);
      int titleWidth = fm.horizontalAdvance(d->m_Title);
      QFont subFont  = font();
      subFont.setPixelSize(12);
      painter.setFont(subFont);
      painter.setPen(NXThemeColor(d->m_ThemeMode, BasicDetailsText));
      QRect subRect(textStartX + titleWidth + 12, headerY, width() - textStartX - titleWidth - 52, d->m_HeaderHeight);
      painter.drawText(subRect, Qt::AlignVCenter | Qt::AlignLeft, d->m_SubTitle);
    }
  }

  // Expand/collapse arrow
  QFont arrowFont = QFont(QStringLiteral("NXAwesome"));
  arrowFont.setPixelSize(14);
  painter.setFont(arrowFont);
  painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
  QRect arrowRect(width() - 35, headerY, 20, d->m_HeaderHeight);
  NXIconType::IconName arrowIcon;
  if (d->m_ExpandDirection == NXExpander::Down) {
    arrowIcon = d->m_IsExpanded ? NXIconType::AngleUp : NXIconType::AngleDown;
  } else {
    arrowIcon = d->m_IsExpanded ? NXIconType::AngleDown : NXIconType::AngleUp;
  }
  painter.drawText(arrowRect, Qt::AlignVCenter | Qt::AlignCenter, QChar((unsigned short) arrowIcon));
}

void NXExpander::mousePressEvent(QMouseEvent *event)
{
  Q_D(NXExpander);
  int headerY = (d->m_ExpandDirection == Down) ? 0 : height() - d->m_HeaderHeight;
  QRect headerRect(0, headerY, width(), d->m_HeaderHeight);
  if (headerRect.contains(event->pos()) && event->button() == Qt::LeftButton) {
    d->m_IsPressed = true;
  }
  QWidget::mousePressEvent(event);
}

void NXExpander::mouseReleaseEvent(QMouseEvent *event)
{
  Q_D(NXExpander);
  if (d->m_IsPressed && event->button() == Qt::LeftButton) {
    int headerY = (d->m_ExpandDirection == Down) ? 0 : height() - d->m_HeaderHeight;
    QRect headerRect(0, headerY, width(), d->m_HeaderHeight);
    if (headerRect.contains(event->pos())) {
      d->_toggleExpand();
    }
    d->m_IsPressed = false;
  }
  QWidget::mouseReleaseEvent(event);
}

bool NXExpander::event(QEvent *event)
{
  switch (event->type()) {
  case QEvent::Enter :
  case QEvent::Leave : update(); break;
  default            : break;
  }
  return QWidget::event(event);
}
