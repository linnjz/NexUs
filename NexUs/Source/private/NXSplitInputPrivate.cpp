#include "NXSplitInputPrivate.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QVariantAnimation>

#include "NXSplitInput.h"
#include "NXTheme.h"

NXSplitInputPrivate::NXSplitInputPrivate(QObject *parent)
    : QObject { parent }
{
}

NXSplitInputPrivate::~NXSplitInputPrivate()
{
}

void NXSplitInputPrivate::onThemeChanged(NXThemeType::ThemeMode themeMode)
{
  m_ThemeMode = themeMode;
  _updateSeparatorColors();
  Q_Q(NXSplitInput);
  q->update();
}

void NXSplitInputPrivate::_buildWidgets()
{
  Q_Q(NXSplitInput);
  for (int i = 0; i < m_SegmentCount; ++i) {
    QLineEdit *box = new QLineEdit(q);
    box->setMaxLength(m_DefaultMaxLength);
    box->setAlignment(Qt::AlignCenter);
    box->setFrame(false);
    box->setStyleSheet(QStringLiteral("QLineEdit{background:transparent;border:none;padding:0px;}"));
    box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    box->setContextMenuPolicy(Qt::NoContextMenu);
    box->installEventFilter(q);
    m_BoxLayout->addWidget(box, 1);
    m_LineEdits.append(box);
    m_MaxLengths.append(m_DefaultMaxLength);
    m_SegmentMarks.append(0);
    m_SegmentCompleted.append(false);

    connect(box, &QLineEdit::textChanged, q, [=, this](QString const &) {
      _onChildTextChanged(i);
    });
    connect(box, &QLineEdit::editingFinished, q, [=, this]() {
      _onChildEditingFinished(i);
    });
    connect(box, &QLineEdit::returnPressed, q, [=, this]() {
      Q_EMIT q->returnPressed(i);
    });

    QVariantAnimation *anim = new QVariantAnimation(q);
    anim->setDuration(300);
    anim->setEasingCurve(QEasingCurve::InOutSine);
    connect(anim, &QVariantAnimation::valueChanged, q, [=, this](QVariant const &value) {
      m_SegmentMarks[i] = value.toReal();
      q->update();
    });
    m_MarkAnimations.append(anim);

    if (i < m_SegmentCount - 1) {
      QLabel *sep = new QLabel(m_SeparatorText, q);
      sep->setAlignment(Qt::AlignCenter);
      sep->setFixedWidth(sep->fontMetrics().horizontalAdvance(m_SeparatorText) + 4);
      m_BoxLayout->addWidget(sep);
      m_Separators.append(sep);
    }
  }

  _updateSeparatorColors();

  if (!m_LineEdits.isEmpty()) {
    m_LineEdits.first()->setFocus();
  }
}

void NXSplitInputPrivate::_updateCompletedState()
{
  bool allDone = true;
  for (int i = 0; i < m_SegmentCount; ++i) {
    bool hasText         = !m_LineEdits[i]->text().isEmpty();
    m_SegmentCompleted[i] = hasText;
    if (!hasText) {
      allDone = false;
    }
  }
  if (m_AllCompleted != allDone) {
    m_AllCompleted = allDone;
    Q_Q(NXSplitInput);
    q->update();
  }
}

void NXSplitInputPrivate::_updateSeparatorColors()
{
  for (QLabel *label : m_Separators) {
    QPalette pal = label->palette();
    pal.setColor(QPalette::WindowText, NXThemeColor(m_ThemeMode, BasicText));
    label->setPalette(pal);
  }
}

void NXSplitInputPrivate::_onChildTextChanged(int index)
{
  Q_Q(NXSplitInput);
  m_SegmentCompleted[index] = !m_LineEdits[index]->text().isEmpty();

  if (m_LineEdits[index]->text().isEmpty()) {
    _animateMarkOut(index);
  } else if (!m_LineEdits[index]->hasFocus() && m_IsKeepMarkOnCompleted && m_SegmentMarks[index] <= 0) {
    _animateMarkIn(index);
  }

  if (m_LineEdits[index]->text().length() >= m_MaxLengths[index] && index < m_SegmentCount - 1 && m_LineEdits[index]->hasFocus()) {
    m_LineEdits[index + 1]->setFocus();
    m_LineEdits[index + 1]->setCursorPosition(0);
  }

  QString fullText;
  for (int i = 0; i < m_SegmentCount; ++i) {
    if (i > 0) {
      fullText += m_SeparatorText;
    }
    fullText += m_LineEdits[i]->text();
  }
  Q_EMIT q->segmentTextChanged(index, m_LineEdits[index]->text());
  Q_EMIT q->textChanged(fullText);
  _updateCompletedState();
}

void NXSplitInputPrivate::_onChildEditingFinished(int index)
{
  Q_Q(NXSplitInput);
  Q_EMIT q->segmentEditingFinished(index);
  if (m_AllCompleted) {
    Q_EMIT q->editingFinished();
  }
}

void NXSplitInputPrivate::_animateMarkIn(int index)
{
  if (index < 0 || index >= m_MarkAnimations.size()) {
    return;
  }
  QRect boxRect           = m_LineEdits[index]->geometry();
  qreal target            = qMax(qreal(10), (boxRect.width() - 4) / 2.0);
  QVariantAnimation *anim = m_MarkAnimations[index];
  anim->stop();
  anim->setStartValue(m_SegmentMarks[index]);
  anim->setEndValue(target);
  anim->start();
}

void NXSplitInputPrivate::_animateMarkOut(int index)
{
  if (index < 0 || index >= m_MarkAnimations.size()) {
    return;
  }
  QVariantAnimation *anim = m_MarkAnimations[index];
  anim->stop();
  anim->setStartValue(m_SegmentMarks[index]);
  anim->setEndValue(0);
  anim->start();
}
