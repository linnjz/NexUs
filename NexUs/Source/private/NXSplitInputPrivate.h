#ifndef NXSPLITINPUTPRIVATE_H
#define NXSPLITINPUTPRIVATE_H

#include <QObject>
#include <QVector>

#include "NXDef.h"

class QHBoxLayout;
class QLabel;
class QLineEdit;
class QVariantAnimation;
class NXSplitInput;

class NXSplitInputPrivate : public QObject
{
  Q_OBJECT
  Q_D_CREATE(NXSplitInput)
  Q_PROPERTY_CREATE_D(qreal, SegmentSpacing)
  Q_PROPERTY_CREATE_D(int, BorderRadius)
  Q_PROPERTY_CREATE_D(bool, IsClearButtonEnable)
  Q_PROPERTY_CREATE_D(bool, IsKeepMarkOnCompleted)

public:
  explicit NXSplitInputPrivate(QObject *parent = nullptr);
  ~NXSplitInputPrivate();
  Q_SLOT void onThemeChanged(NXThemeType::ThemeMode themeMode);

private:
  bool m_AllCompleted { false };
  NXThemeType::ThemeMode m_ThemeMode;
  int m_SegmentCount { 1 };
  int m_DefaultMaxLength { 999999 };
  int m_HoveredIndex { -1 };
  QString m_SeparatorText;
  QHBoxLayout *m_BoxLayout { nullptr };
  QVector<QLineEdit *> m_LineEdits;
  QVector<QLabel *> m_Separators;
  QVector<int> m_MaxLengths;
  QVector<qreal> m_SegmentMarks;
  QVector<bool> m_SegmentCompleted;
  QVector<QVariantAnimation *> m_MarkAnimations;

  void _buildWidgets();
  void _updateCompletedState();
  void _updateSeparatorColors();
  void _onChildTextChanged(int index);
  void _onChildEditingFinished(int index);
  void _animateMarkIn(int index);
  void _animateMarkOut(int index);
};

#endif // NXSPLITINPUTPRIVATE_H
