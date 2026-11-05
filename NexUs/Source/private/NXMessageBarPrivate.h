#ifndef NXMESSAGEBARPRIVATE_H
#define NXMESSAGEBARPRIVATE_H

#include <atomic>

#include <QMap>
#include <QObject>

#include "NXDef.h"

class NXIconButton;
class QPainter;
class NXMessageBar;

class NXMessageBarPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXMessageBar)
  Q_PROPERTY_CREATE(QPS_PAEMIT(qreal), Opacity)
  Q_PROPERTY_CREATE(QPS_PAEMIT(qreal), TimePercent)

public:
  explicit NXMessageBarPrivate(QObject *parent = nullptr);
  ~NXMessageBarPrivate();
  void onOtherMessageBarEnd();
  Q_SLOT void messageBarEnd();
  Q_SLOT void onThemeChanged(NXThemeType::ThemeMode themeMode);

private:
  inline static std::atomic<quint64> m_NextCreateSerial { 0 };

  bool m_IsClosing { false };
  bool m_IsNormalDisplay { false };
  bool m_IsOtherMessageBarEnd { false };
  NXThemeType::ThemeMode m_ThemeMode;
  int m_BorderRadius { 6 };
  NXMessageBarType::PositionPolicy m_Policy;
  NXMessageBarType::MessageMode m_MessageMode;
  int m_LeftPadding { 20 };                // 左边框到图标中心
  int m_TitleLeftSpacing { 30 };           // 图标中心到Title左侧
  int m_TextLeftSpacing { 15 };            // Title右侧到Text左侧
  int m_CloseButtonLeftRightMargin { 20 }; // closeButton左右总Margin
  int m_CloseButtonWidth { 30 };
  int m_MessageBarHorizontalMargin { 20 };
  int m_MessageBarVerticalBottomMargin { 20 };
  int m_MessageBarVerticalTopMargin { 25 };
  int m_MessageBarSpacing { 15 };
  int m_ShadowBorderWidth { 6 };
  quint64 m_CreateSerial { 0 };
  qreal m_TimePercentHeight { 2 };
  QString m_Title { QStringLiteral("") };
  QString m_Text { QStringLiteral("") };
  NXIconButton *m_CloseButton { nullptr };
  static QMap<QObject *, QMap<NXMessageBarType::PositionPolicy, QList<NXMessageBar *> *>> m_MessageBarActiveMap;

  // 位置数据

  // 逻辑数据
  void _messageBarCreate(int displayMsec);

  // 初始坐标计算
  void _calculateInitialPos(int &startX, int &startY, int &endX, int &endY);
  //获取总高度和次序信息
  QList<int> _getOtherMessageBarTotalData();
  //计算目标坐标
  qreal _calculateTargetPosY();

  //创建次序判断
  bool _judgeCreateOrder(NXMessageBar *otherMessageBar);
  void _updateActiveMap(bool isActive);

  // 绘制函数
  void _drawSuccess(QPainter *painter);
  void _drawWarning(QPainter *painter);
  void _drawInformation(QPainter *painter);
  void _drawError(QPainter *painter);

  // 计算文本高度
  int _getTextHeight();
};

#endif // NXMESSAGEBARPRIVATE_H
