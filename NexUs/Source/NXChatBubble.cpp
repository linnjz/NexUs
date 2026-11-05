#include "NXChatBubble.h"

#include <functional>

#include <QApplication>
#include <QClipboard>
#include <QFileDialog>
#include <QFontMetrics>
#include <QGestureEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPinchGesture>
#include <QScreen>
#include <QScrollBar>
#include <QShortcut>
#include <QVBoxLayout>
#include <QWheelEvent>

#include "NXIconButton.h"
#include "NXMenu.h"
#include "NXScrollArea.h"
#include "NXText.h"
#include "NXTheme.h"
#include "NXWidget.h"
#include "private/NXChatBubblePrivate.h"

Q_PROPERTY_CREATE_CPP(NXChatBubble, QPS_PAEMIT(int), BorderRadius)
Q_PROPERTY_CREATE_CPP(NXChatBubble, QPS_PAEMIT(int), AvatarSize)
Q_PROPERTY_CREATE_CPP(NXChatBubble, QPS_PAEMIT(int), MaxBubbleWidth)

void NXChatBubble::setMessageText(QString const &text)
{
  Q_D(NXChatBubble);
  d->m_MessageText = text;
  setFixedHeight(sizeHint().height());
  update();
  Q_EMIT signalMessageTextChanged();
}

QString NXChatBubble::getMessageText() const noexcept(std::is_nothrow_copy_constructible_v<QString>)
{
  return d_ptr->m_MessageText;
}

void NXChatBubble::setSenderName(QString const &name)
{
  Q_D(NXChatBubble);
  d->m_SenderName = name;
  setFixedHeight(sizeHint().height());
  update();
  Q_EMIT signalSenderNameChanged();
}

QString NXChatBubble::getSenderName() const noexcept(std::is_nothrow_copy_constructible_v<QString>)
{
  return d_ptr->m_SenderName;
}

void NXChatBubble::setTimestamp(QString const &ts)
{
  Q_D(NXChatBubble);
  d->m_Timestamp = ts;
  setFixedHeight(sizeHint().height());
  update();
  Q_EMIT signalTimestampChanged();
}

QString NXChatBubble::getTimestamp() const noexcept(std::is_nothrow_copy_constructible_v<QString>)
{
  return d_ptr->m_Timestamp;
}

NXChatBubble::NXChatBubble(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new NXChatBubblePrivate())
{
  Q_D(NXChatBubble);
  d->q_ptr            = this;
  d->m_BorderRadius   = 12;
  d->m_AvatarSize     = 40;
  d->m_MaxBubbleWidth = 320;
  setObjectName("NXChatBubble");
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
    update();
  });
}

NXChatBubble::~NXChatBubble()
{
}

void NXChatBubble::setDirection(BubbleDirection direction)
{
  Q_D(NXChatBubble);
  d->m_Direction = direction;
  update();
}

NXChatBubble::BubbleDirection NXChatBubble::getDirection() const
{
  return d_ptr->m_Direction;
}

void NXChatBubble::setStatus(MessageStatus status)
{
  Q_D(NXChatBubble);
  d->m_Status = status;
  update();
}

NXChatBubble::MessageStatus NXChatBubble::getStatus() const
{
  return d_ptr->m_Status;
}

void NXChatBubble::setAvatar(QPixmap const &avatar)
{
  Q_D(NXChatBubble);
  d->m_Avatar = avatar;
  update();
}

QPixmap NXChatBubble::getAvatar() const
{
  return d_ptr->m_Avatar;
}

void NXChatBubble::setBubbleColor(QColor const &color)
{
  Q_D(NXChatBubble);
  d->m_BubbleColor = color;
  update();
}

QColor NXChatBubble::getBubbleColor() const
{
  return d_ptr->m_BubbleColor;
}

void NXChatBubble::setMessageImage(QPixmap const &image)
{
  Q_D(NXChatBubble);
  d->m_MessageImage = image;
  if (!image.isNull()) {
    int maxW = d->m_ImageMaxWidth;
    if (image.width() > maxW) {
      d->m_ScaledImage = image.scaledToWidth(maxW, Qt::SmoothTransformation);
    } else {
      d->m_ScaledImage = image;
    }
  } else {
    d->m_ScaledImage = QPixmap();
  }
  setFixedHeight(sizeHint().height());
  update();
}

QPixmap NXChatBubble::getMessageImage() const
{
  return d_ptr->m_MessageImage;
}

void NXChatBubble::setImageMaxWidth(int width)
{
  Q_D(NXChatBubble);
  d->m_ImageMaxWidth = width;
  if (!d->m_MessageImage.isNull()) {
    setMessageImage(d->m_MessageImage);
  }
}

int NXChatBubble::getImageMaxWidth() const
{
  return d_ptr->m_ImageMaxWidth;
}

QSize NXChatBubble::sizeHint() const
{
  Q_CONST_D(NXChatBubble);
  int padding         = 10;
  int avatarSpace     = d->m_AvatarSize + 10;
  int nameHeight      = d->m_SenderName.isEmpty() ? 0 : 20;
  int timestampHeight = d->m_Timestamp.isEmpty() ? 0 : 16;
  int textPadding     = 12;

  int bubbleContentHeight = 0;
  int bubbleContentWidth  = 0;

  if (!d->m_ScaledImage.isNull()) {
    bubbleContentWidth  = d->m_ScaledImage.width() + textPadding * 2;
    bubbleContentHeight = d->m_ScaledImage.height() + textPadding * 2;

    if (!d->m_MessageText.isEmpty()) {
      QFont msgFont = font();
      msgFont.setPixelSize(14);
      QFontMetrics fm(msgFont);
      int maxTextW   = bubbleContentWidth - textPadding * 2;
      QRect textRect = fm.boundingRect(0, 0, maxTextW, 10000, Qt::TextWordWrap, d->m_MessageText);
      bubbleContentHeight += textRect.height() + 6;
    }
  } else {
    QFont msgFont = font();
    msgFont.setPixelSize(14);
    QFontMetrics fm(msgFont);
    bubbleContentWidth = qMin(fm.horizontalAdvance(d->m_MessageText) + textPadding * 2, d->m_MaxBubbleWidth);
    QRect textRect =
        fm.boundingRect(0, 0, bubbleContentWidth - textPadding * 2, 10000, Qt::TextWordWrap, d->m_MessageText);
    bubbleContentHeight = textRect.height() + textPadding * 2;
  }

  int totalHeight = padding + nameHeight + bubbleContentHeight + timestampHeight + padding;
  int totalWidth  = avatarSpace + bubbleContentWidth + padding * 2;
  return QSize(totalWidth, totalHeight);
}

void NXChatBubble::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event)
  Q_D(NXChatBubble);

  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

  int padding     = 10;
  int avatarSpace = d->m_AvatarSize + 10;
  bool isRight    = (d->m_Direction == Right);

  int avatarX = isRight ? (width() - padding - d->m_AvatarSize) : padding;
  int avatarY = padding;
  if (!d->m_Avatar.isNull()) {
    QPainterPath clipPath;
    clipPath.addEllipse(avatarX, avatarY, d->m_AvatarSize, d->m_AvatarSize);
    painter.save();
    painter.setClipPath(clipPath);
    painter.drawPixmap(avatarX, avatarY, d->m_AvatarSize, d->m_AvatarSize, d->m_Avatar);
    painter.restore();
  } else {
    painter.setPen(Qt::NoPen);
    painter.setBrush(NXThemeColor(d->m_ThemeMode, BasicBaseDeep));
    painter.drawEllipse(avatarX, avatarY, d->m_AvatarSize, d->m_AvatarSize);
    QFont iconFont(QStringLiteral("NXAwesome"));
    iconFont.setPixelSize(d->m_AvatarSize / 2);
    painter.setFont(iconFont);
    painter.setPen(NXThemeColor(d->m_ThemeMode, BasicDetailsText));
    painter.drawText(QRect(avatarX, avatarY, d->m_AvatarSize, d->m_AvatarSize), Qt::AlignCenter, QChar(static_cast<int>(NXIconType::User)));
  }

  int bubbleX        = isRight ? padding : (padding + avatarSpace);
  int bubbleMaxWidth = width() - padding * 2 - avatarSpace;
  int currentY       = padding;

  if (!d->m_SenderName.isEmpty()) {
    QFont nameFont = font();
    nameFont.setPixelSize(12);
    nameFont.setBold(true);
    painter.setFont(nameFont);
    painter.setPen(NXThemeColor(d->m_ThemeMode, BasicDetailsText));
    int nameX = bubbleX;
    if (isRight) {
      QFontMetrics nameFm(nameFont);
      nameX = bubbleX + bubbleMaxWidth - nameFm.horizontalAdvance(d->m_SenderName);
    }
    painter.drawText(nameX, currentY + 12, d->m_SenderName);
    currentY += 20;
  }

  QFont msgFont = font();
  msgFont.setPixelSize(14);
  QFontMetrics fm(msgFont);
  int textPadding = 12;

  int bubbleWidth  = 0;
  int bubbleHeight = 0;
  int imageH       = 0;
  int captionH     = 0;
  QRect textBound;

  if (!d->m_ScaledImage.isNull()) {
    bubbleWidth  = d->m_ScaledImage.width() + textPadding * 2;
    imageH       = d->m_ScaledImage.height();
    bubbleHeight = imageH + textPadding * 2;

    if (!d->m_MessageText.isEmpty()) {
      int maxTextW = bubbleWidth - textPadding * 2;
      textBound    = fm.boundingRect(0, 0, maxTextW, 10000, Qt::TextWordWrap, d->m_MessageText);
      captionH     = textBound.height() + 6;
      bubbleHeight += captionH;
    }
  } else {
    int maxTextWidth = qMin(d->m_MaxBubbleWidth - textPadding * 2, bubbleMaxWidth - textPadding * 2);
    textBound        = fm.boundingRect(0, 0, maxTextWidth, 10000, Qt::TextWordWrap, d->m_MessageText);
    bubbleWidth      = textBound.width() + textPadding * 2;
    bubbleHeight     = textBound.height() + textPadding * 2;
  }

  int bx = isRight ? (bubbleX + bubbleMaxWidth - bubbleWidth) : bubbleX;

  QColor bgColor;
  if (d->m_BubbleColor.isValid()) {
    bgColor = d->m_BubbleColor;
  } else if (isRight) {
    bgColor = NXThemeColor(d->m_ThemeMode, PrimaryNormal);
  } else {
    bgColor = NXThemeColor(d->m_ThemeMode, BasicBase);
  }

  QPainterPath bubblePath;
  bubblePath.addRoundedRect(QRectF(bx, currentY, bubbleWidth, bubbleHeight), d->m_BorderRadius, d->m_BorderRadius);
  painter.setPen(Qt::NoPen);
  painter.fillPath(bubblePath, bgColor);

  if (!isRight) {
    painter.setPen(QPen(NXThemeColor(d->m_ThemeMode, BasicBorder), 1));
    painter.drawPath(bubblePath);
  }

  if (!d->m_ScaledImage.isNull()) {
    int imgX = bx + textPadding;
    int imgY = currentY + textPadding;

    d->m_ImageRect = QRect(imgX, imgY, d->m_ScaledImage.width(), imageH);

    painter.save();
    QPainterPath imgClip;
    int imgRadius = qMax(d->m_BorderRadius - 4, 0);
    imgClip.addRoundedRect(QRectF(imgX, imgY, d->m_ScaledImage.width(), imageH), imgRadius, imgRadius);
    painter.setClipPath(imgClip);
    painter.drawPixmap(imgX, imgY, d->m_ScaledImage);
    painter.restore();

    if (!d->m_MessageText.isEmpty()) {
      painter.setFont(msgFont);
      if (isRight) {
        painter.setPen(NXThemeColor(d->m_ThemeMode, BasicTextInvert));
      } else {
        painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
      }
      int captionY = imgY + imageH + 6;
      painter.drawText(QRect(imgX, captionY, bubbleWidth - textPadding * 2, textBound.height()), Qt::TextWordWrap, d->m_MessageText);
    }
  } else {
    int maxTextWidth = qMin(d->m_MaxBubbleWidth - textPadding * 2, bubbleMaxWidth - textPadding * 2);
    painter.setFont(msgFont);
    if (isRight) {
      painter.setPen(NXThemeColor(d->m_ThemeMode, BasicTextInvert));
    } else {
      painter.setPen(NXThemeColor(d->m_ThemeMode, BasicText));
    }
    painter.drawText(QRect(bx + textPadding, currentY + textPadding, maxTextWidth, textBound.height()), Qt::TextWordWrap, d->m_MessageText);
  }

  currentY += bubbleHeight + 4;

  QFont timeFont = font();
  timeFont.setPixelSize(11);
  painter.setFont(timeFont);
  painter.setPen(NXThemeColor(d->m_ThemeMode, BasicDetailsText));

  QString bottomText;
  if (!d->m_Timestamp.isEmpty()) {
    bottomText = d->m_Timestamp;
  }

  if (d->m_Status != None) {
    QString statusText;
    switch (d->m_Status) {
    case Sending : statusText = QStringLiteral("发送中..."); break;
    case Sent    : statusText = QStringLiteral("已发送"); break;
    case Read    : statusText = QStringLiteral("已读"); break;
    case Failed  : statusText = QStringLiteral("发送失败"); break;
    default      : break;
    }
    if (!bottomText.isEmpty()) {
      bottomText += QStringLiteral("  ");
    }
    bottomText += statusText;
  }

  if (!bottomText.isEmpty()) {
    QFontMetrics timeFm(timeFont);
    if (isRight) {
      int textW = timeFm.horizontalAdvance(bottomText);
      painter.drawText(bx + bubbleWidth - textW, currentY + 12, bottomText);
    } else {
      painter.drawText(bx, currentY + 12, bottomText);
    }

    if (d->m_Status == Failed) {
      painter.setPen(NXThemeColor(d->m_ThemeMode, StatusDanger));
      QString failText = QStringLiteral("发送失败");
      if (isRight) {
        int failW = timeFm.horizontalAdvance(failText);
        painter.drawText(bx + bubbleWidth - failW, currentY + 12, failText);
      } else {
        int offset = timeFm.horizontalAdvance(bottomText) - timeFm.horizontalAdvance(failText);
        painter.drawText(bx + offset, currentY + 12, failText);
      }
    }
  }
}

void NXChatBubble::mouseDoubleClickEvent(QMouseEvent *event)
{
  Q_D(NXChatBubble);
  if (!d->m_MessageImage.isNull() && d->m_ImageRect.contains(event->pos())) {
    Q_EMIT imageDoubleClicked(d->m_MessageImage);

    NXWidget *previewWindow = new NXWidget();
    previewWindow->setAttribute(Qt::WA_DeleteOnClose);
    previewWindow->setWindowButtonFlag(NXAppBarType::StayTopButtonHint, false);
    previewWindow->setIsStayTop(true);
    previewWindow->setIsDefaultClosed(true);

    QScreen *screen  = QApplication::primaryScreen();
    QSize screenSize = screen->availableSize();
    QSize imgSize    = d->m_MessageImage.size();
    int appBarH      = previewWindow->getAppBarHeight();
    int bottomBarH   = 40;

    int maxW = qMin(static_cast<int>(screenSize.width() * 0.75), 1200);
    int maxH = qMin(static_cast<int>(screenSize.height() * 0.75), 900) - appBarH - bottomBarH;

    qreal initScale = 1.0;
    if (imgSize.width() > maxW || imgSize.height() > maxH) {
      qreal sw  = static_cast<qreal>(maxW) / imgSize.width();
      qreal sh  = static_cast<qreal>(maxH) / imgSize.height();
      initScale = qMin(sw, sh);
    }

    int winW = qMax(static_cast<int>(imgSize.width() * initScale), 400) + 12;
    int winH = qMax(static_cast<int>(imgSize.height() * initScale), 300) + appBarH + bottomBarH + 12;

    QLabel *imageLabel = new QLabel(previewWindow);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setMinimumSize(1, 1);
    imageLabel->setContextMenuPolicy(Qt::CustomContextMenu);

    QPixmap *currentImage = new QPixmap(d->m_MessageImage);
    qreal *currentScale   = new qreal(initScale);
    NXText *scaleLabel    = new NXText(previewWindow);
    scaleLabel->setTextPixelSize(13);

    auto updateImage = [imageLabel, currentImage, previewWindow, scaleLabel](qreal zoomScale) {
      int w = static_cast<int>(currentImage->width() * zoomScale);
      int h = static_cast<int>(currentImage->height() * zoomScale);
      imageLabel->setPixmap(currentImage->scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation));
      imageLabel->resize(w, h);
      int percent = static_cast<int>(zoomScale * 100);
      previewWindow->setWindowTitle(QString(QStringLiteral("图片预览 - %1%")).arg(percent));
      scaleLabel->setText(QString(QStringLiteral("%1%")).arg(percent));
    };
    updateImage(initScale);

    NXScrollArea *scrollArea = new NXScrollArea(previewWindow);
    scrollArea->setWidgetResizable(false);
    scrollArea->setAlignment(Qt::AlignCenter);
    scrollArea->setWidget(imageLabel);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    NXIconButton *zoomOutBtn = new NXIconButton(NXIconType::MagnifyingGlassMinus, 18, previewWindow);
    zoomOutBtn->setFixedSize(32, 32);
    NXIconButton *zoomInBtn = new NXIconButton(NXIconType::MagnifyingGlassPlus, 18, previewWindow);
    zoomInBtn->setFixedSize(32, 32);
    NXIconButton *zoomResetBtn = new NXIconButton(NXIconType::Expand, 18, previewWindow);
    zoomResetBtn->setFixedSize(32, 32);
    NXIconButton *rotateLeftBtn = new NXIconButton(NXIconType::RotateLeft, 18, previewWindow);
    rotateLeftBtn->setFixedSize(32, 32);
    NXIconButton *rotateRightBtn = new NXIconButton(NXIconType::RotateRight, 18, previewWindow);
    rotateRightBtn->setFixedSize(32, 32);
    NXIconButton *copyBtn = new NXIconButton(NXIconType::Copy, 18, previewWindow);
    copyBtn->setFixedSize(32, 32);
    NXIconButton *saveBtn = new NXIconButton(NXIconType::Download, 18, previewWindow);
    saveBtn->setFixedSize(32, 32);

    auto doZoom = [currentScale, updateImage](qreal delta) {
      *currentScale = qBound(0.1, *currentScale + delta, 10.0);
      updateImage(*currentScale);
    };

    auto doRotate = [currentImage, currentScale, updateImage](int degrees) {
      QTransform transform;
      transform.rotate(degrees);
      *currentImage = currentImage->transformed(transform, Qt::SmoothTransformation);
      updateImage(*currentScale);
    };

    connect(zoomInBtn, &NXIconButton::clicked, previewWindow, [doZoom]() {
      doZoom(0.1);
    });
    connect(zoomOutBtn, &NXIconButton::clicked, previewWindow, [doZoom]() {
      doZoom(-0.1);
    });
    connect(zoomResetBtn, &NXIconButton::clicked, previewWindow, [currentScale, updateImage, initScale]() {
      *currentScale = initScale;
      updateImage(initScale);
    });
    connect(rotateLeftBtn, &NXIconButton::clicked, previewWindow, [doRotate]() {
      doRotate(-90);
    });
    connect(rotateRightBtn, &NXIconButton::clicked, previewWindow, [doRotate]() {
      doRotate(90);
    });
    connect(copyBtn, &NXIconButton::clicked, previewWindow, [currentImage]() {
      QApplication::clipboard()->setPixmap(*currentImage);
    });
    connect(saveBtn, &NXIconButton::clicked, previewWindow, [currentImage, previewWindow]() {
      QString filePath =
          QFileDialog::getSaveFileName(previewWindow, QStringLiteral("保存图片"), QStringLiteral("image.png"), QStringLiteral("PNG (*.png);;JPEG (*.jpg *.jpeg);;BMP (*.bmp);;所有文件 (*)"));
      if (!filePath.isEmpty()) {
        currentImage->save(filePath);
      }
    });

    connect(imageLabel, &QLabel::customContextMenuRequested, previewWindow, [currentImage, previewWindow](QPoint const &pos) {
      NXMenu *menu        = new NXMenu(previewWindow);
      QAction *copyAction = menu->addNXIconAction(NXIconType::Copy, QStringLiteral("复制图片"));
      QAction *saveAction = menu->addNXIconAction(NXIconType::Download, QStringLiteral("保存图片"));
      connect(copyAction, &QAction::triggered, previewWindow, [currentImage]() {
        QApplication::clipboard()->setPixmap(*currentImage);
      });
      connect(saveAction, &QAction::triggered, previewWindow, [currentImage, previewWindow]() {
        QString filePath =
            QFileDialog::getSaveFileName(previewWindow, QStringLiteral("保存图片"), QStringLiteral("image.png"), QStringLiteral("PNG (*.png);;JPEG (*.jpg *.jpeg);;BMP (*.bmp);;所有文件 (*)"));
        if (!filePath.isEmpty()) {
          currentImage->save(filePath);
        }
      });
      connect(menu, &NXMenu::aboutToHide, menu, &NXMenu::deleteLater);
      menu->popup(QCursor::pos());
    });

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->setContentsMargins(10, 4, 10, 6);
    bottomLayout->addStretch();
    bottomLayout->addWidget(rotateLeftBtn);
    bottomLayout->addSpacing(6);
    bottomLayout->addWidget(rotateRightBtn);
    bottomLayout->addSpacing(12);
    bottomLayout->addWidget(zoomOutBtn);
    bottomLayout->addSpacing(6);
    bottomLayout->addWidget(scaleLabel);
    bottomLayout->addSpacing(6);
    bottomLayout->addWidget(zoomInBtn);
    bottomLayout->addSpacing(12);
    bottomLayout->addWidget(zoomResetBtn);
    bottomLayout->addSpacing(12);
    bottomLayout->addWidget(copyBtn);
    bottomLayout->addSpacing(6);
    bottomLayout->addWidget(saveBtn);
    bottomLayout->addStretch();

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(5, 5, 5, 0);
    layout->setSpacing(0);
    layout->addWidget(scrollArea, 1);
    layout->addLayout(bottomLayout);
    previewWindow->setLayout(layout);

    connect(previewWindow, &QObject::destroyed, previewWindow, [currentScale, currentImage]() {
      delete currentScale;
      delete currentImage;
    });

    class ZoomEventFilter : public QObject
    {
    public:
      using ZoomFunc = std::function<void(qreal)>;
      ZoomFunc m_DoZoom;
      qreal *m_Scale;
      std::function<void(qreal)> m_UpdateImage;
      NXScrollArea *m_ScrollArea;
      bool m_Dragging { false };
      QPoint m_DragStartPos;
      int m_DragStartHBar { 0 };
      int m_DragStartVBar { 0 };

      ZoomEventFilter(ZoomFunc doZoom, qreal *scale, std::function<void(qreal)> updateImage, NXScrollArea *scrollArea, QObject *parent)
          : QObject(parent)
          , m_DoZoom(std::move(doZoom))
          , m_Scale(scale)
          , m_UpdateImage(std::move(updateImage))
          , m_ScrollArea(scrollArea)
      {
      }

      bool eventFilter(QObject *obj, QEvent *ev) override
      {
        if (ev->type() == QEvent::Wheel) {
          QWheelEvent *we = static_cast<QWheelEvent *>(ev);
#ifdef Q_OS_MAC
          if (we->phase() != Qt::NoScrollPhase || (we->modifiers() & Qt::ControlModifier)) {
            qreal pixelDelta = we->pixelDelta().y();
            if (qAbs(pixelDelta) > 0) {
              qreal step = pixelDelta / 600.0;
              *m_Scale    = qBound(0.1, *m_Scale + step, 10.0);
              m_UpdateImage(*m_Scale);
              return true;
            }
          }
#endif
          {
            qreal delta = we->angleDelta().y() > 0 ? 0.1 : -0.1;
            *m_Scale     = qBound(0.1, *m_Scale + delta, 10.0);
            m_UpdateImage(*m_Scale);
            return true;
          }
        } else if (ev->type() == QEvent::NativeGesture) {
          QNativeGestureEvent *nge = static_cast<QNativeGestureEvent *>(ev);
          if (nge->gestureType() == Qt::ZoomNativeGesture) {
            qreal factor = nge->value();
            *m_Scale      = qBound(0.1, *m_Scale * (1.0 + factor), 10.0);
            m_UpdateImage(*m_Scale);
            return true;
          }
        } else if (ev->type() == QEvent::MouseButtonPress) {
          QMouseEvent *me = static_cast<QMouseEvent *>(ev);
          if (me->button() == Qt::LeftButton) {
            m_Dragging = true;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            m_DragStartPos = me->globalPosition().toPoint();
#else
            m_DragStartPos = me->globalPos();
#endif
            m_DragStartHBar = m_ScrollArea->horizontalScrollBar()->value();
            m_DragStartVBar = m_ScrollArea->verticalScrollBar()->value();
            m_ScrollArea->viewport()->setCursor(Qt::ClosedHandCursor);
            return true;
          }
        } else if (ev->type() == QEvent::MouseMove) {
          if (m_Dragging) {
            QMouseEvent *me = static_cast<QMouseEvent *>(ev);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            QPoint delta = me->globalPosition().toPoint() - m_DragStartPos;
#else
            QPoint delta = me->globalPos() - m_DragStartPos;

#endif
            m_ScrollArea->horizontalScrollBar()->setValue(m_DragStartHBar - delta.x());
            m_ScrollArea->verticalScrollBar()->setValue(m_DragStartVBar - delta.y());
            return true;
          }
        } else if (ev->type() == QEvent::MouseButtonRelease) {
          if (m_Dragging) {
            m_Dragging = false;
            m_ScrollArea->viewport()->setCursor(Qt::OpenHandCursor);
            return true;
          }
        }
        return false;
      }
    };

    auto *filter = new ZoomEventFilter(doZoom, currentScale, updateImage, scrollArea, previewWindow);
    scrollArea->viewport()->installEventFilter(filter);
    scrollArea->installEventFilter(filter);
    scrollArea->viewport()->setCursor(Qt::OpenHandCursor);

    QShortcut *escShortcut = new QShortcut(Qt::Key_Escape, previewWindow);
    connect(escShortcut, &QShortcut::activated, previewWindow, &NXWidget::close);

    QShortcut *copyShortcut = new QShortcut(QKeySequence::Copy, previewWindow);
    connect(copyShortcut, &QShortcut::activated, previewWindow, [currentImage]() {
      QApplication::clipboard()->setPixmap(*currentImage);
    });

    scrollArea->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(scrollArea, &QWidget::customContextMenuRequested, previewWindow, [currentImage, scrollArea](QPoint const &pos) {
      NXMenu contextMenu;
      QAction *copyAction = contextMenu.addNXIconAction(NXIconType::Copy, QStringLiteral("复制图片"));
      connect(copyAction, &QAction::triggered, [currentImage]() {
        QApplication::clipboard()->setPixmap(*currentImage);
      });
      contextMenu.exec(scrollArea->mapToGlobal(pos));
    });

    previewWindow->resize(winW, winH);
    previewWindow->moveToCenter();
    previewWindow->show();
    return;
  }
  QWidget::mouseDoubleClickEvent(event);
}

NXChatBubblePrivate::NXChatBubblePrivate(QObject *parent)
    : QObject(parent)
{
}

NXChatBubblePrivate::~NXChatBubblePrivate()
{
}
