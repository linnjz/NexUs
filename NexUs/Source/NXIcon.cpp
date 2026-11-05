#include "NXIcon.h"

#include <QPainter>
#include <QPixmap>

#include "NXApplication.h"

NXIcon::NXIcon()
{
}

NXIcon::~NXIcon()
{
}

QIcon NXIcon::getNXIcon(NXIconType::IconName awesome)
{
  QFont iconFont = QFont(QStringLiteral("NXAwesome"));
  QPixmap pix(30, 30);
  pix.fill(Qt::transparent);
  QPainter painter;
  painter.begin(&pix);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
  // painter.setPen(QColor("#1570A5"));
  // painter.setBrush(QColor("#1570A5"));
  iconFont.setPixelSize(nxApp->getFontPixelSize() + 12);
  painter.setFont(iconFont);
  painter.drawText(pix.rect(), Qt::AlignCenter, QChar((unsigned short) awesome));
  painter.end();
  return QIcon(pix);
}

QIcon NXIcon::getNXIcon(NXIconType::IconName awesome, QColor const &iconColor)
{
  QFont iconFont = QFont(QStringLiteral("NXAwesome"));
  QPixmap pix(30, 30);
  pix.fill(Qt::transparent);
  QPainter painter;
  painter.begin(&pix);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
  painter.setPen(iconColor);
  // painter.setBrush(QColor("#1570A5"));
  iconFont.setPixelSize(nxApp->getFontPixelSize() + 12);
  painter.setFont(iconFont);
  painter.drawText(pix.rect(), Qt::AlignCenter, QChar((unsigned short) awesome));
  painter.end();
  return QIcon(pix);
}

QIcon NXIcon::getNXIcon(NXIconType::IconName awesome, int pixelSize)
{
  QFont iconFont = QFont(QStringLiteral("NXAwesome"));
  QPixmap pix(pixelSize, pixelSize);
  pix.fill(Qt::transparent);
  QPainter painter;
  painter.begin(&pix);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
  iconFont.setPixelSize(pixelSize);
  painter.setFont(iconFont);
  // 画图形字体
  painter.drawText(pix.rect(), Qt::AlignCenter, QChar((unsigned short) awesome));
  painter.end();
  return QIcon(pix);
}

QIcon NXIcon::getNXIcon(NXIconType::IconName awesome, int pixelSize, QColor const &iconColor)
{
  QFont iconFont = QFont(QStringLiteral("NXAwesome"));
  QPixmap pix(pixelSize, pixelSize);
  pix.fill(Qt::transparent);
  QPainter painter;
  painter.begin(&pix);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
  painter.setPen(iconColor);
  iconFont.setPixelSize(pixelSize);
  painter.setFont(iconFont);
  // 画图形字体
  painter.drawText(pix.rect(), Qt::AlignCenter, QChar((unsigned short) awesome));
  painter.end();
  return QIcon(pix);
}

QIcon NXIcon::getNXIcon(NXIconType::IconName awesome, int pixelSize, int fixedWidth, int fixedHeight)
{
  QFont iconFont = QFont(QStringLiteral("NXAwesome"));
  QPixmap pix(fixedWidth, fixedHeight);
  pix.fill(Qt::transparent);
  QPainter painter;
  painter.begin(&pix);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
  iconFont.setPixelSize(pixelSize);
  painter.setFont(iconFont);
  // 画图形字体
  painter.drawText(pix.rect(), Qt::AlignCenter, QChar((unsigned short) awesome));
  painter.end();
  return QIcon(pix);
}

QIcon NXIcon::getNXIcon(NXIconType::IconName awesome, int pixelSize, int fixedWidth, int fixedHeight, QColor const &iconColor)
{
  QFont iconFont = QFont(QStringLiteral("NXAwesome"));
  QPixmap pix(fixedWidth, fixedHeight);
  pix.fill(Qt::transparent);
  QPainter painter;
  painter.begin(&pix);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
  painter.setPen(iconColor);
  iconFont.setPixelSize(pixelSize);
  painter.setFont(iconFont);
  // 画图形字体
  painter.drawText(pix.rect(), Qt::AlignCenter, QChar((unsigned short) awesome));
  painter.end();
  return QIcon(pix);
}
