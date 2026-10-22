#include "NXUploadArea.h"

#include <QApplication>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileInfo>
#include <QMimeData>
#include <QPainter>
#include <QPainterPath>
#include <QUrl>

#include "NXTheme.h"
#include "private/NXUploadAreaPrivate.h"

Q_PROPERTY_CREATE_CPP(NXUploadArea, QS_SET_CREF(QString), Title)
Q_PROPERTY_CREATE_CPP(NXUploadArea, QS_SET_CREF(QString), SubTitle)
Q_PROPERTY_CREATE_CPP(NXUploadArea, QS_SET_CREF(QString), DialogTitle)
Q_PROPERTY_CREATE_CPP(NXUploadArea, QS_SET_CREF(QStringList), AcceptedSuffixes)
Q_PROPERTY_CREATE_CPP(NXUploadArea, qint64, MaxFileSize)
Q_PROPERTY_CREATE_CPP(NXUploadArea, int, BorderRadius)
Q_PROPERTY_CREATE_CPP(NXUploadArea, int, MaxFileCount)
Q_PROPERTY_CREATE_CPP(NXUploadArea, bool, IsMultiple)

NXUploadArea::NXUploadArea(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new NXUploadAreaPrivate())
{
  Q_D(NXUploadArea);
  d->q_ptr          = this;
  d->_pBorderRadius = 8;
  d->_pTitle        = QStringLiteral("拖拽文件到此处");
  d->_pSubTitle     = QStringLiteral("或点击选择文件");
  d->_pMaxFileCount = 0;
  d->_pMaxFileSize  = 0;
  d->_pIsMultiple   = true;
  d->_themeMode     = nxTheme->getThemeMode();
  setObjectName("NXUploadArea");
  setMinimumSize(260, 160);
  setAcceptDrops(true);
  setMouseTracking(true);
  setCursor(Qt::PointingHandCursor);

  connect(nxTheme, &NXTheme::themeModeChanged, d, &NXUploadAreaPrivate::onThemeChanged);
}

NXUploadArea::~NXUploadArea()
{
}

QStringList NXUploadArea::getSelectedFiles() const
{
  return d_ptr->_filePaths;
}

void NXUploadArea::clearFiles()
{
  Q_D(NXUploadArea);
  d->_filePaths.clear();
  update();
}

void NXUploadArea::setAcceptedMimeFilter(const QString &filter)
{
  Q_D(NXUploadArea);
  d->_mimeFilter = filter;
}

QString NXUploadArea::getAcceptedMimeFilter() const
{
  return d_ptr->_mimeFilter;
}

void NXUploadArea::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event)
  Q_D(NXUploadArea);
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  QRectF bgRect(1, 1, width() - 2, height() - 2);
  QPainterPath bgPath;
  bgPath.addRoundedRect(bgRect, d->_pBorderRadius, d->_pBorderRadius);

  if (d->_isDragOver) {
    painter.setPen(QPen(NXThemeColor(d->_themeMode, PrimaryNormal), 2, Qt::DashLine));
    painter.setBrush(NXThemeColor(d->_themeMode, BasicBaseDeepAlpha));
  } else if (d->_isHover) {
    painter.setPen(QPen(NXThemeColor(d->_themeMode, BasicBorderHover), 2, Qt::DashLine));
    painter.setBrush(NXThemeColor(d->_themeMode, BasicHoverAlpha));
  } else {
    painter.setPen(QPen(NXThemeColor(d->_themeMode, BasicBorder), 2, Qt::DashLine));
    painter.setBrush(Qt::NoBrush);
  }
  painter.drawPath(bgPath);

  int contentY  = height() / 2;
  int fileCount = d->_filePaths.size();

  if (fileCount == 0) {
    QFont iconFont = QFont(QStringLiteral("NXAwesome"));
    iconFont.setPixelSize(36);
    painter.setFont(iconFont);
    painter.setPen(d->_isDragOver ? NXThemeColor(d->_themeMode, PrimaryNormal) : NXThemeColor(d->_themeMode, BasicTextNoFocus));
    QRectF iconRect(0, contentY - 48, width(), 40);
    painter.drawText(iconRect, Qt::AlignCenter, QChar((unsigned short) NXIconType::CloudArrowUp));

    QFont titleFont = painter.font();
    titleFont.setFamily(qApp->font().family());
    titleFont.setPixelSize(15);
    titleFont.setBold(true);
    painter.setFont(titleFont);
    painter.setPen(NXThemeColor(d->_themeMode, BasicText));
    QRectF titleRect(0, contentY, width(), 24);
    painter.drawText(titleRect, Qt::AlignCenter, d->_pTitle);

    QFont subFont = titleFont;
    subFont.setPixelSize(12);
    subFont.setBold(false);
    painter.setFont(subFont);
    painter.setPen(NXThemeColor(d->_themeMode, BasicTextNoFocus));
    QRectF subRect(0, contentY + 26, width(), 20);
    painter.drawText(subRect, Qt::AlignCenter, d->_pSubTitle);
  } else {
    QFont iconFont = QFont(QStringLiteral("NXAwesome"));
    iconFont.setPixelSize(24);
    painter.setFont(iconFont);
    painter.setPen(NXThemeColor(d->_themeMode, BasicTextNoFocus));
    QRectF iconRect(0, 12, width(), 28);
    painter.drawText(iconRect, Qt::AlignCenter, QChar((unsigned short) NXIconType::CloudArrowUp));

    QFont fileFont = painter.font();
    fileFont.setFamily(qApp->font().family());
    fileFont.setPixelSize(12);
    painter.setFont(fileFont);

    int startY          = 46;
    int lineHeight      = 22;
    int maxDisplayCount = qMin(fileCount, (height() - startY - 10) / lineHeight);

    for (int i = 0; i < maxDisplayCount; ++i) {
      QFileInfo fileInfo(d->_filePaths[i]);
      QString displayName = fileInfo.fileName();

      painter.setPen(NXThemeColor(d->_themeMode, PrimaryNormal));
      QFont fileIconFont(QStringLiteral("NXAwesome"));
      fileIconFont.setPixelSize(12);
      painter.setFont(fileIconFont);
      QRectF fileIconRect(12, startY + i * lineHeight, 16, lineHeight);
      painter.drawText(fileIconRect, Qt::AlignVCenter | Qt::AlignCenter, QChar((unsigned short) NXIconType::File));

      painter.setFont(fileFont);
      painter.setPen(NXThemeColor(d->_themeMode, BasicText));
      QRectF nameRect(32, startY + i * lineHeight, width() - 60, lineHeight);
      QString elidedName = painter.fontMetrics().elidedText(displayName, Qt::ElideMiddle, nameRect.width());
      painter.drawText(nameRect, Qt::AlignVCenter | Qt::AlignLeft, elidedName);

      painter.setPen(NXThemeColor(d->_themeMode, BasicTextNoFocus));
      QFont xFont(QStringLiteral("NXAwesome"));
      xFont.setPixelSize(10);
      painter.setFont(xFont);
      QRectF xRect(width() - 28, startY + i * lineHeight, 16, lineHeight);
      painter.drawText(xRect, Qt::AlignVCenter | Qt::AlignCenter, QChar((unsigned short) NXIconType::Xmark));
    }

    if (fileCount > maxDisplayCount) {
      painter.setFont(fileFont);
      painter.setPen(NXThemeColor(d->_themeMode, BasicTextNoFocus));
      QRectF moreRect(0, startY + maxDisplayCount * lineHeight, width(), lineHeight);
      painter.drawText(moreRect, Qt::AlignCenter, QString(QStringLiteral("...还有 %1 个文件")).arg(fileCount - maxDisplayCount));
    }
  }

  painter.restore();
}

void NXUploadArea::dragEnterEvent(QDragEnterEvent *event)
{
  Q_D(NXUploadArea);
  if (event->mimeData()->hasUrls()) {
    event->setDropAction(Qt::CopyAction);
    event->accept();
    d->_isDragOver = true;
    update();
  }
}

void NXUploadArea::dragMoveEvent(QDragMoveEvent *event)
{
  if (event->mimeData()->hasUrls()) {
    event->setDropAction(Qt::CopyAction);
    event->accept();
  }
}

void NXUploadArea::dragLeaveEvent(QDragLeaveEvent *event)
{
  Q_UNUSED(event)
  Q_D(NXUploadArea);
  d->_isDragOver = false;
  update();
}

void NXUploadArea::dropEvent(QDropEvent *event)
{
  Q_D(NXUploadArea);
  event->accept();
  d->_isDragOver            = false;
  const QMimeData *mimeData = event->mimeData();
  if (mimeData->hasUrls()) {
    QStringList filePaths;
    for (const QUrl &url : mimeData->urls()) {
      if (url.isLocalFile()) {
        filePaths.append(url.toLocalFile());
      }
    }
    if (!filePaths.isEmpty()) {
      if (!d->_pIsMultiple && filePaths.size() > 1) {
        filePaths = filePaths.mid(0, 1);
      }
      d->_addFiles(filePaths);
    }
  }
  update();
}

void NXUploadArea::mousePressEvent(QMouseEvent *event)
{
  Q_D(NXUploadArea);
  d->_isPressed = true;

  if (!d->_filePaths.isEmpty()) {
    int startY          = 46;
    int lineHeight      = 22;
    int maxDisplayCount = qMin(d->_filePaths.size(), (height() - startY - 10) / lineHeight);
    int clickY          = event->pos().y();
    int clickX          = event->pos().x();

    for (int i = 0; i < maxDisplayCount; ++i) {
      int itemY = startY + i * lineHeight;
      if (clickY >= itemY && clickY < itemY + lineHeight && clickX >= width() - 28 && clickX <= width() - 12) {
        d->_removeFile(i);
        d->_isPressed = false;
        return;
      }
    }
  }

  update();
  QWidget::mousePressEvent(event);
}

void NXUploadArea::mouseReleaseEvent(QMouseEvent *event)
{
  Q_D(NXUploadArea);
  if (d->_isPressed && rect().contains(event->pos())) {
    d->_openFileDialog();
  }
  d->_isPressed = false;
  update();
  QWidget::mouseReleaseEvent(event);
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void NXUploadArea::enterEvent(QEnterEvent *event)
#else
void NXUploadArea::enterEvent(QEvent *event)
#endif
{
  Q_D(NXUploadArea);
  d->_isHover = true;
  update();
  QWidget::enterEvent(event);
}

void NXUploadArea::leaveEvent(QEvent *event)
{
  Q_D(NXUploadArea);
  d->_isHover = false;
  update();
  QWidget::leaveEvent(event);
}
