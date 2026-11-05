#include "NXWizard.h"

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QScreen>
#include <QStackedWidget>
#include <QVBoxLayout>

#include "NXTheme.h"
#include "private/NXWizardPrivate.h"

Q_PROPERTY_CREATE_CPP(NXWizard, QPS_PAEMIT(int), BorderRadius)
Q_PROPERTY_CREATE_CPP(NXWizard, QPS_PAEMIT(int), CurrentStep)

NXWizard::NXWizard(QWidget *parent)
    : QWidget { nullptr }
    , d_ptr(new NXWizardPrivate())
{
  Q_D(NXWizard);
  d->q_ptr          = this;
  d->m_BorderRadius = 10;
  d->m_CurrentStep  = 0;
  setObjectName("NXWizard");
  setFixedSize(600, 450);
  setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
  setAttribute(Qt::WA_TranslucentBackground);

  d->m_StackedWidget = new QStackedWidget(this);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  int sw                  = d->m_ShadowBorderWidth;
  mainLayout->setContentsMargins(sw, sw + 70, sw, sw + 50);
  mainLayout->addWidget(d->m_StackedWidget);

  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
    update();
  });

  // Center on screen
  QScreen *screen = QApplication::primaryScreen();
  if (screen) {
    QRect screenGeometry = screen->geometry();
    int x                = (screenGeometry.width() - width()) / 2 + screenGeometry.x();
    int y                = (screenGeometry.height() - height()) / 2 + screenGeometry.y();
    move(x, y);
  }
}

NXWizard::~NXWizard()
{
}

void NXWizard::addStep(QString const &title, QWidget *page)
{
  Q_D(NXWizard);
  d->m_StepTitles.append(title);
  d->m_StepPages.append(page);
  d->m_StackedWidget->addWidget(page);
  update();
}

void NXWizard::next()
{
  Q_D(NXWizard);
  if (d->m_CurrentStep < d->m_StepPages.count() - 1) {
    d->m_CurrentStep++;
    d->m_StackedWidget->setCurrentIndex(d->m_CurrentStep);
    update();
    Q_EMIT currentStepChanged(d->m_CurrentStep);
  }
}

void NXWizard::previous()
{
  Q_D(NXWizard);
  if (d->m_CurrentStep > 0) {
    d->m_CurrentStep--;
    d->m_StackedWidget->setCurrentIndex(d->m_CurrentStep);
    update();
    Q_EMIT currentStepChanged(d->m_CurrentStep);
  }
}

void NXWizard::finish()
{
  Q_EMIT finished();
  hide();
}

int NXWizard::getStepCount() const
{
  Q_CONST_D(NXWizard);
  return d->m_StepPages.count();
}

void NXWizard::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);
  Q_D(NXWizard);
  QPainter painter(this);
  painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing | QPainter::TextAntialiasing);

  int sw = d->m_ShadowBorderWidth;

  // Shadow
  nxTheme->drawEffectShadow(&painter, rect(), sw, d->m_BorderRadius);

  // Background
  QRect foregroundRect = rect().adjusted(sw, sw, -sw, -sw);
  painter.setPen(Qt::NoPen);
  painter.setBrush(NXThemeColor(d->m_ThemeMode, DialogBase));
  painter.drawRoundedRect(foregroundRect, d->m_BorderRadius, d->m_BorderRadius);

  int fgX = foregroundRect.x();
  int fgY = foregroundRect.y();
  int fgW = foregroundRect.width();
  int fgH = foregroundRect.height();

  // --- Top area: step indicator (70px) ---
  int topAreaHeight = 70;
  int stepCount     = d->m_StepPages.count();

  if (stepCount > 0) {
    int circleDiameter = 28;
    int circleRadius   = circleDiameter / 2;
    int circleY        = fgY + 8;
    int titleY         = fgY + 42;
    int lineCenterY    = circleY + circleRadius;

    int margin        = 50;
    qreal usableWidth = fgW - margin * 2;
    qreal spacing     = (stepCount > 1) ? usableWidth / (stepCount - 1) : 0;

    QColor primaryNormal    = NXThemeColor(d->m_ThemeMode, PrimaryNormal);
    QColor basicBase        = NXThemeColor(d->m_ThemeMode, BasicBase);
    QColor basicBorder      = NXThemeColor(d->m_ThemeMode, BasicBorder);
    QColor basicDetailsText = NXThemeColor(d->m_ThemeMode, BasicDetailsText);
    QColor whiteColor       = Qt::white;

    QFont iconFont(QStringLiteral("NXAwesome"));
    iconFont.setPixelSize(14);

    QFont numberFont = this->font();
    numberFont.setPixelSize(14);
    numberFont.setWeight(QFont::Bold);

    QFont titleFont = this->font();
    titleFont.setPixelSize(13);

    for (int i = 0; i < stepCount; i++) {
      qreal cx = (stepCount > 1) ? (fgX + margin + i * spacing) : (fgX + fgW / 2.0);

      // Connecting line
      if (i < stepCount - 1) {
        qreal lineStartX = cx + circleRadius;
        qreal nextCx     = (fgX + margin + (i + 1) * spacing);
        qreal lineEndX   = nextCx - circleRadius;

        if (i < d->m_CurrentStep) {
          painter.setPen(QPen(primaryNormal, 2));
        } else {
          painter.setPen(QPen(basicBorder, 2));
        }
        painter.drawLine(QPointF(lineStartX, lineCenterY), QPointF(lineEndX, lineCenterY));
      }

      // Circle
      QRectF circleRect(cx - circleRadius, circleY, circleDiameter, circleDiameter);
      painter.setPen(Qt::NoPen);

      if (i < d->m_CurrentStep) {
        // Completed
        painter.setBrush(primaryNormal);
        painter.drawEllipse(circleRect);
        painter.setPen(whiteColor);
        painter.setFont(iconFont);
        painter.drawText(circleRect, Qt::AlignCenter, QChar((unsigned short) 0xea'6c));
      } else if (i == d->m_CurrentStep) {
        // Active
        painter.setBrush(primaryNormal);
        painter.drawEllipse(circleRect);
        painter.setPen(whiteColor);
        painter.setFont(numberFont);
        painter.drawText(circleRect, Qt::AlignCenter, QString::number(i + 1));
      } else {
        // Pending
        painter.setBrush(basicBase);
        painter.drawEllipse(circleRect);
        painter.setPen(QPen(basicBorder, 1));
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(circleRect);
        painter.setPen(basicDetailsText);
        painter.setFont(numberFont);
        painter.drawText(circleRect, Qt::AlignCenter, QString::number(i + 1));
      }

      // Title
      if (i < d->m_StepTitles.size()) {
        painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
        painter.setFont(titleFont);
        int titleWidth = 120;
        QRectF titleRect(cx - titleWidth / 2.0, titleY, titleWidth, 20);
        painter.drawText(titleRect, Qt::AlignHCenter | Qt::AlignTop, d->m_StepTitles[i]);
      }
    }
  }

  // --- Separator line: top / middle ---
  QColor separatorColor = NXThemeColor(d->m_ThemeMode, BasicBorder);
  painter.setPen(QPen(separatorColor, 1));
  int topSepY = fgY + topAreaHeight;
  painter.drawLine(fgX + 1, topSepY, fgX + fgW - 1, topSepY);

  // --- Separator line: middle / bottom ---
  int bottomAreaHeight = 50;
  int bottomSepY       = fgY + fgH - bottomAreaHeight;
  painter.drawLine(fgX + 1, bottomSepY, fgX + fgW - 1, bottomSepY);

  // --- Bottom area: buttons (50px) ---
  int btnH       = 30;
  int btnY       = bottomSepY + (bottomAreaHeight - btnH) / 2;
  int btnMargin  = 15;
  int btnSpacing = 10;

  QFont btnFont = this->font();
  btnFont.setPixelSize(13);
  painter.setFont(btnFont);
  QFontMetrics fm(btnFont);

  QColor basicBase     = NXThemeColor(d->m_ThemeMode, BasicBase);
  QColor basicText     = NXThemeColor(d->m_ThemeMode, BasicText);
  QColor basicBorder   = NXThemeColor(d->m_ThemeMode, BasicBorder);
  QColor primaryNormal = NXThemeColor(d->m_ThemeMode, PrimaryNormal);

  // Cancel button (left)
  QString cancelText = QStringLiteral("\u53D6\u6D88");
  int cancelW        = fm.horizontalAdvance(cancelText) + 24;
  QRect cancelRect(fgX + btnMargin, btnY, cancelW, btnH);
  d->m_CancelButtonRect = cancelRect;
  painter.setPen(QPen(basicBorder, 1));
  painter.setBrush(basicBase);
  painter.drawRoundedRect(cancelRect, 4, 4);
  painter.setPen(basicText);
  painter.drawText(cancelRect, Qt::AlignCenter, cancelText);

  // Next/Finish button (rightmost)
  bool isLastStep  = (d->m_CurrentStep >= d->m_StepPages.count() - 1) && (d->m_StepPages.count() > 0);
  QString nextText = isLastStep ? QStringLiteral("\u5B8C\u6210") : QStringLiteral("\u4E0B\u4E00\u6B65");
  int nextW        = fm.horizontalAdvance(nextText) + 24;
  QRect nextRect(fgX + fgW - btnMargin - nextW, btnY, nextW, btnH);
  d->m_NextButtonRect = nextRect;
  painter.setPen(Qt::NoPen);
  painter.setBrush(primaryNormal);
  painter.drawRoundedRect(nextRect, 4, 4);
  painter.setPen(Qt::white);
  painter.drawText(nextRect, Qt::AlignCenter, nextText);

  // Previous button (before next)
  QString prevText = QStringLiteral("\u4E0A\u4E00\u6B65");
  int prevW        = fm.horizontalAdvance(prevText) + 24;
  QRect prevRect(nextRect.x() - btnSpacing - prevW, btnY, prevW, btnH);
  d->m_PrevButtonRect = prevRect;
  bool prevDisabled  = (d->m_CurrentStep == 0);
  painter.setPen(QPen(basicBorder, 1));
  painter.setBrush(basicBase);
  painter.drawRoundedRect(prevRect, 4, 4);
  if (prevDisabled) {
    QColor disabledText = basicText;
    disabledText.setAlpha(80);
    painter.setPen(disabledText);
  } else {
    painter.setPen(basicText);
  }
  painter.drawText(prevRect, Qt::AlignCenter, prevText);
}

void NXWizard::mousePressEvent(QMouseEvent *event)
{
  Q_D(NXWizard);
  QPoint pos = event->pos();
  if (d->m_CancelButtonRect.contains(pos)) {
    Q_EMIT cancelled();
    hide();
    return;
  }
  if (d->m_PrevButtonRect.contains(pos) && d->m_CurrentStep > 0) {
    previous();
    return;
  }
  if (d->m_NextButtonRect.contains(pos)) {
    bool isLastStep = (d->m_CurrentStep >= d->m_StepPages.count() - 1) && (d->m_StepPages.count() > 0);
    if (isLastStep) {
      finish();
    } else {
      next();
    }
    return;
  }
  QWidget::mousePressEvent(event);
}

// NXWizardPrivate

NXWizardPrivate::NXWizardPrivate(QObject *parent)
    : QObject(parent)
{
}

NXWizardPrivate::~NXWizardPrivate()
{
}
