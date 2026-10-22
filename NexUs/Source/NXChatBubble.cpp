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

Q_PROPERTY_CREATE_CPP(NXChatBubble, int, BorderRadius)
Q_PROPERTY_CREATE_CPP(NXChatBubble, int, AvatarSize)
Q_PROPERTY_CREATE_CPP(NXChatBubble, int, MaxBubbleWidth)

void NXChatBubble::setMessageText(const QString &text)
{
  Q_D(NXChatBubble);
  d->_pMessageText = text;
  setFixedHeight(sizeHint().height());
  update();
  Q_EMIT pMessageTextChanged();
}

QString NXChatBubble::getMessageText() const
{
  return d_ptr->_pMessageText;
}

void NXChatBubble::setSenderName(const QString &name)
{
  Q_D(NXChatBubble);
  d->_pSenderName = name;
  setFixedHeight(sizeHint().height());
  update();
  Q_EMIT pSenderNameChanged();
}

QString NXChatBubble::getSenderName() const
{
  return d_ptr->_pSenderName;
}

void NXChatBubble::setTimestamp(const QString &ts)
{
  Q_D(NXChatBubble);
  d->_pTimestamp = ts;
  setFixedHeight(sizeHint().height());
  update();
  Q_EMIT pTimestampChanged();
}

QString NXChatBubble::getTimestamp() const
{
  return d_ptr->_pTimestamp;
}

NXChatBubble::NXChatBubble(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new NXChatBubblePrivate())
{
  Q_D(NXChatBubble);
  d->q_ptr            = this;
  d->_pBorderRadius   = 12;
  d->_pAvatarSize     = 40;
  d->_pMaxBubbleWidth = 320;
  setObjectName("NXChatBubble");
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  d->_themeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
    update();
  });
}

NXChatBubble::~NXChatBubble()
{
}

void NXChatBubble::setDirection(BubbleDirection direction)
{
  Q_D(NXChatBubble);
  d->_direction = direction;
  update();
}

NXChatBubble::BubbleDirection NXChatBubble::getDirection() const
{
  return d_ptr->_direction;
}

void NXChatBubble::setStatus(MessageStatus status)
{
  Q_D(NXChatBubble);
  d->_status = status;
  update();
}

NXChatBubble::MessageStatus NXChatBubble::getStatus() const
{
  return d_ptr->_status;
}

void NXChatBubble::setAvatar(const QPixmap &avatar)
{
  Q_D(NXChatBubble);
  d->_avatar = avatar;
  update();
}

QPixmap NXChatBubble::getAvatar() const
{
  return d_ptr->_avatar;
}

void NXChatBubble::setBubbleColor(const QColor &color)
{
  Q_D(NXChatBubble);
  d->_bubbleColor = color;
  update();
}

QColor NXChatBubble::getBubbleColor() const
{
  return d_ptr->_bubbleColor;
}

void NXChatBubble::setMessageImage(const QPixmap &image)
{
  Q_D(NXChatBubble);
  d->_messageImage = image;
  if (!image.isNull()) {
    int maxW = d->_imageMaxWidth;
    if (image.width() > maxW) {
      d->_scaledImage = image.scaledToWidth(maxW, Qt::SmoothTransformation);
    } else {
      d->_scaledImage = image;
    }
  } else {
    d->_scaledImage = QPixmap();
  }
  setFixedHeight(sizeHint().height());
  update();
}

QPixmap NXChatBubble::getMessageImage() const
{
  return d_ptr->_messageImage;
}

void NXChatBubble::setImageMaxWidth(int width)
{
  Q_D(NXChatBubble);
  d->_imageMaxWidth = width;
  if (!d->_messageImage.isNull()) {
    setMessageImage(d->_messageImage);
  }
}

int NXChatBubble::getImageMaxWidth() const
{
  return d_ptr->_imageMaxWidth;
}

QSize NXChatBubble::sizeHint() const
{
  Q_D(const NXChatBubble);
  int padding         = 10;
  int avatarSpace     = d->_pAvatarSize + 10;
  int nameHeight      = d->_pSenderName.isEmpty() ? 0 : 20;
  int timestampHeight = d->_pTimestamp.isEmpty() ? 0 : 16;
  int textPadding     = 12;

  int bubbleContentHeight = 0;
  int bubbleContentWidth  = 0;

  if (!d->_scaledImage.isNull()) {
    bubbleContentWidth  = d->_scaledImage.width() + textPadding * 2;
    bubbleContentHeight = d->_scaledImage.height() + textPadding * 2;

    if (!d->_pMessageText.isEmpty()) {
      QFont msgFont = font();
      msgFont.setPixelSize(14);
      QFontMetrics fm(msgFont);
      int maxTextW   = bubbleContentWidth - textPadding * 2;
      QRect textRect = fm.boundingRect(0, 0, maxTextW, 10000, Qt::TextWordWrap, d->_pMessageText);
      bubbleContentHeight += textRect.height() + 6;
    }
  } else {
    QFont msgFont = font();
    msgFont.setPixelSize(14);
    QFontMetrics fm(msgFont);
    bubbleContentWidth = qMin(fm.horizontalAdvance(d->_pMessageText) + textPadding * 2, d->_pMaxBubbleWidth);
    QRect textRect =
        fm.boundingRect(0, 0, bubbleContentWidth - textPadding * 2, 10000, Qt::TextWordWrap, d->_pMessageText);
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
  int avatarSpace = d->_pAvatarSize + 10;
  bool isRight    = (d->_direction == Right);

  int avatarX = isRight ? (width() - padding - d->_pAvatarSize) : padding;
  int avatarY = padding;
  if (!d->_avatar.isNull()) {
    QPainterPath clipPath;
    clipPath.addEllipse(avatarX, avatarY, d->_pAvatarSize, d->_pAvatarSize);
    painter.save();
    painter.setClipPath(clipPath);
    painter.drawPixmap(avatarX, avatarY, d->_pAvatarSize, d->_pAvatarSize, d->_avatar);
    painter.restore();
  } else {
    painter.setPen(Qt::NoPen);
    painter.setBrush(NXThemeColor(d->_themeMode, BasicBaseDeep));
    painter.drawEllipse(avatarX, avatarY, d->_pAvatarSize, d->_pAvatarSize);
    QFont iconFont(QStringLiteral("NXAwesome"));
    iconFont.setPixelSize(d->_pAvatarSize / 2);
    painter.setFont(iconFont);
    painter.setPen(NXThemeColor(d->_themeMode, BasicDetailsText));
    painter.drawText(QRect(avatarX, avatarY, d->_pAvatarSize, d->_pAvatarSize), Qt::AlignCenter, QChar(static_cast<int>(NXIconType::User)));
  }

  int bubbleX        = isRight ? padding : (padding + avatarSpace);
  int bubbleMaxWidth = width() - padding * 2 - avatarSpace;
  int currentY       = padding;

  if (!d->_pSenderName.isEmpty()) {
    QFont nameFont = font();
    nameFont.setPixelSize(12);
    nameFont.setBold(true);
    painter.setFont(nameFont);
    painter.setPen(NXThemeColor(d->_themeMode, BasicDetailsText));
    int nameX = bubbleX;
    if (isRight) {
      QFontMetrics nameFm(nameFont);
      nameX = bubbleX + bubbleMaxWidth - nameFm.horizontalAdvance(d->_pSenderName);
    }
    painter.drawText(nameX, currentY + 12, d->_pSenderName);
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

  if (!d->_scaledImage.isNull()) {
    bubbleWidth  = d->_scaledImage.width() + textPadding * 2;
    imageH       = d->_scaledImage.height();
    bubbleHeight = imageH + textPadding * 2;

    if (!d->_pMessageText.isEmpty()) {
      int maxTextW = bubbleWidth - textPadding * 2;
      textBound    = fm.boundingRect(0, 0, maxTextW, 10000, Qt::TextWordWrap, d->_pMessageText);
      captionH     = textBound.height() + 6;
      bubbleHeight += captionH;
    }
  } else {
    int maxTextWidth = qMin(d->_pMaxBubbleWidth - textPadding * 2, bubbleMaxWidth - textPadding * 2);
    textBound        = fm.boundingRect(0, 0, maxTextWidth, 10000, Qt::TextWordWrap, d->_pMessageText);
    bubbleWidth      = textBound.width() + textPadding * 2;
    bubbleHeight     = textBound.height() + textPadding * 2;
  }

  int bx = isRight ? (bubbleX + bubbleMaxWidth - bubbleWidth) : bubbleX;

  QColor bgColor;
  if (d->_bubbleColor.isValid()) {
    bgColor = d->_bubbleColor;
  } else if (isRight) {
    bgColor = NXThemeColor(d->_themeMode, PrimaryNormal);
  } else {
    bgColor = NXThemeColor(d->_themeMode, BasicBase);
  }

  QPainterPath bubblePath;
  bubblePath.addRoundedRect(QRectF(bx, currentY, bubbleWidth, bubbleHeight), d->_pBorderRadius, d->_pBorderRadius);
  painter.setPen(Qt::NoPen);
  painter.fillPath(bubblePath, bgColor);

  if (!isRight) {
    painter.setPen(QPen(NXThemeColor(d->_themeMode, BasicBorder), 1));
    painter.drawPath(bubblePath);
  }

  if (!d->_scaledImage.isNull()) {
    int imgX = bx + textPadding;
    int imgY = currentY + textPadding;

    d->_imageRect = QRect(imgX, imgY, d->_scaledImage.width(), imageH);

    painter.save();
    QPainterPath imgClip;
    int imgRadius = qMax(d->_pBorderRadius - 4, 0);
    imgClip.addRoundedRect(QRectF(imgX, imgY, d->_scaledImage.width(), imageH), imgRadius, imgRadius);
    painter.setClipPath(imgClip);
    painter.drawPixmap(imgX, imgY, d->_scaledImage);
    painter.restore();

    if (!d->_pMessageText.isEmpty()) {
      painter.setFont(msgFont);
      if (isRight) {
        painter.setPen(NXThemeColor(d->_themeMode, BasicTextInvert));
      } else {
        painter.setPen(NXThemeColor(d->_themeMode, BasicText));
      }
      int captionY = imgY + imageH + 6;
      painter.drawText(QRect(imgX, captionY, bubbleWidth - textPadding * 2, textBound.height()), Qt::TextWordWrap, d->_pMessageText);
    }
  } else {
    int maxTextWidth = qMin(d->_pMaxBubbleWidth - textPadding * 2, bubbleMaxWidth - textPadding * 2);
    painter.setFont(msgFont);
    if (isRight) {
      painter.setPen(NXThemeColor(d->_themeMode, BasicTextInvert));
    } else {
      painter.setPen(NXThemeColor(d->_themeMode, BasicText));
    }
    painter.drawText(QRect(bx + textPadding, currentY + textPadding, maxTextWidth, textBound.height()), Qt::TextWordWrap, d->_pMessageText);
  }

  currentY += bubbleHeight + 4;

  QFont timeFont = font();
  timeFont.setPixelSize(11);
  painter.setFont(timeFont);
  painter.setPen(NXThemeColor(d->_themeMode, BasicDetailsText));

  QString bottomText;
  if (!d->_pTimestamp.isEmpty()) {
    bottomText = d->_pTimestamp;
  }

  if (d->_status != None) {
    QString statusText;
    switch (d->_status) {
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

    if (d->_status == Failed) {
      painter.setPen(NXThemeColor(d->_themeMode, StatusDanger));
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
  if (!d->_messageImage.isNull() && d->_imageRect.contains(event->pos())) {
    Q_EMIT imageDoubleClicked(d->_messageImage);

    NXWidget *previewWindow = new NXWidget();
    previewWindow->setAttribute(Qt::WA_DeleteOnClose);
    previewWindow->setWindowButtonFlag(NXAppBarType::StayTopButtonHint, false);
    previewWindow->setIsStayTop(true);
    previewWindow->setIsDefaultClosed(true);

    QScreen *screen  = QApplication::primaryScreen();
    QSize screenSize = screen->availableSize();
    QSize imgSize    = d->_messageImage.size();
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

    QPixmap *currentImage = new QPixmap(d->_messageImage);
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

    connect(imageLabel, &QLabel::customContextMenuRequested, previewWindow, [currentImage, previewWindow](const QPoint &pos) {
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
      ZoomFunc _doZoom;
      qreal *_scale;
      std::function<void(qreal)> _updateImage;
      NXScrollArea *_scrollArea;
      bool _dragging { false };
      QPoint _dragStartPos;
      int _dragStartHBar { 0 };
      int _dragStartVBar { 0 };

      ZoomEventFilter(ZoomFunc doZoom, qreal *scale, std::function<void(qreal)> updateImage, NXScrollArea *scrollArea, QObject *parent)
          : QObject(parent)
          , _doZoom(std::move(doZoom))
          , _scale(scale)
          , _updateImage(std::move(updateImage))
          , _scrollArea(scrollArea)
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
              *_scale    = qBound(0.1, *_scale + step, 10.0);
              _updateImage(*_scale);
              return true;
            }
          }
#endif
          {
            qreal delta = we->angleDelta().y() > 0 ? 0.1 : -0.1;
            *_scale     = qBound(0.1, *_scale + delta, 10.0);
            _updateImage(*_scale);
            return true;
          }
        } else if (ev->type() == QEvent::NativeGesture) {
          QNativeGestureEvent *nge = static_cast<QNativeGestureEvent *>(ev);
          if (nge->gestureType() == Qt::ZoomNativeGesture) {
            qreal factor = nge->value();
            *_scale      = qBound(0.1, *_scale * (1.0 + factor), 10.0);
            _updateImage(*_scale);
            return true;
          }
        } else if (ev->type() == QEvent::MouseButtonPress) {
          QMouseEvent *me = static_cast<QMouseEvent *>(ev);
          if (me->button() == Qt::LeftButton) {
            _dragging = true;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            _dragStartPos = me->globalPosition().toPoint();
#else
            _dragStartPos = me->globalPos();
#endif
            _dragStartHBar = _scrollArea->horizontalScrollBar()->value();
            _dragStartVBar = _scrollArea->verticalScrollBar()->value();
            _scrollArea->viewport()->setCursor(Qt::ClosedHandCursor);
            return true;
          }
        } else if (ev->type() == QEvent::MouseMove) {
          if (_dragging) {
            QMouseEvent *me = static_cast<QMouseEvent *>(ev);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            QPoint delta = me->globalPosition().toPoint() - _dragStartPos;
#else
            QPoint delta = me->globalPos() - _dragStartPos;

#endif
            _scrollArea->horizontalScrollBar()->setValue(_dragStartHBar - delta.x());
            _scrollArea->verticalScrollBar()->setValue(_dragStartVBar - delta.y());
            return true;
          }
        } else if (ev->type() == QEvent::MouseButtonRelease) {
          if (_dragging) {
            _dragging = false;
            _scrollArea->viewport()->setCursor(Qt::OpenHandCursor);
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
    connect(scrollArea, &QWidget::customContextMenuRequested, previewWindow, [currentImage, scrollArea](const QPoint &pos) {
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
