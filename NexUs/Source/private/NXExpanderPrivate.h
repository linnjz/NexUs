#ifndef NXEXPANDERPRIVATE_H
#define NXEXPANDERPRIVATE_H

#include <QObject>

#include "NXDef.h"
#include "NXExpander.h"
class QVariantAnimation;
class QVBoxLayout;

class NXExpanderPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXExpander)
  Q_PROPERTY_CREATE_D(QString, Title)
  Q_PROPERTY_CREATE_D(QString, SubTitle)
  Q_PROPERTY_CREATE_D(int, BorderRadius)
  Q_PROPERTY_CREATE_D(int, AnimationDuration)
  Q_PROPERTY_CREATE_D(NXIconType::IconName, HeaderIcon)

public:
  explicit NXExpanderPrivate(QObject *parent = nullptr);
  ~NXExpanderPrivate();

private:
  bool m_IsExpanded { false };
  bool m_IsPressed { false };
  NXExpander::ExpandDirection m_ExpandDirection { NXExpander::Down };
  int m_HeaderHeight { 48 };
  int m_ContentHeight { 0 };
  NXThemeType::ThemeMode m_ThemeMode;
  QWidget *m_ContentWidget { nullptr };
  QWidget *m_HeaderCustomWidget { nullptr };
  QWidget *m_ContentContainer { nullptr };
  QVBoxLayout *m_ContentLayout { nullptr };
  QVBoxLayout *m_MainLayout { nullptr };
  QWidget *m_HeaderWidget { nullptr };
  QVariantAnimation *m_ExpandAnimation { nullptr };
  void _toggleExpand();
  void _startExpandAnimation(bool expand);
};

#endif // NXEXPANDERPRIVATE_H
