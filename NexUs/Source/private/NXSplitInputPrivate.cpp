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
  _themeMode = themeMode;
  _updateSeparatorColors();
  Q_Q(NXSplitInput);
  q->update();
}

void NXSplitInputPrivate::_buildWidgets()
{
  Q_Q(NXSplitInput);
  for (int i = 0; i < _segmentCount; ++i) {
    QLineEdit *box = new QLineEdit(q);
    box->setMaxLength(_defaultMaxLength);
    box->setAlignment(Qt::AlignCenter);
    box->setFrame(false);
    box->setStyleSheet(QStringLiteral("QLineEdit{background:transparent;border:none;padding:0px;}"));
    box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    box->setContextMenuPolicy(Qt::NoContextMenu);
    box->installEventFilter(q);
    _boxLayout->addWidget(box, 1);
    _lineEdits.append(box);
    _maxLengths.append(_defaultMaxLength);
    _segmentMarks.append(0);
    _segmentCompleted.append(false);

    connect(box, &QLineEdit::textChanged, q, [=, this](const QString &) {
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
    connect(anim, &QVariantAnimation::valueChanged, q, [=, this](const QVariant &value) {
      _segmentMarks[i] = value.toReal();
      q->update();
    });
    _markAnimations.append(anim);

    if (i < _segmentCount - 1) {
      QLabel *sep = new QLabel(_separatorText, q);
      sep->setAlignment(Qt::AlignCenter);
      sep->setFixedWidth(sep->fontMetrics().horizontalAdvance(_separatorText) + 4);
      _boxLayout->addWidget(sep);
      _separators.append(sep);
    }
  }

  _updateSeparatorColors();

  if (!_lineEdits.isEmpty()) {
    _lineEdits.first()->setFocus();
  }
}

void NXSplitInputPrivate::_updateCompletedState()
{
  bool allDone = true;
  for (int i = 0; i < _segmentCount; ++i) {
    bool hasText         = !_lineEdits[i]->text().isEmpty();
    _segmentCompleted[i] = hasText;
    if (!hasText) {
      allDone = false;
    }
  }
  if (_allCompleted != allDone) {
    _allCompleted = allDone;
    Q_Q(NXSplitInput);
    q->update();
  }
}

void NXSplitInputPrivate::_updateSeparatorColors()
{
  for (QLabel *label : _separators) {
    QPalette pal = label->palette();
    pal.setColor(QPalette::WindowText, NXThemeColor(_themeMode, BasicText));
    label->setPalette(pal);
  }
}

void NXSplitInputPrivate::_onChildTextChanged(int index)
{
  Q_Q(NXSplitInput);
  _segmentCompleted[index] = !_lineEdits[index]->text().isEmpty();

  if (_lineEdits[index]->text().isEmpty()) {
    _animateMarkOut(index);
  } else if (!_lineEdits[index]->hasFocus() && _pIsKeepMarkOnCompleted && _segmentMarks[index] <= 0) {
    _animateMarkIn(index);
  }

  if (_lineEdits[index]->text().length() >= _maxLengths[index] && index < _segmentCount - 1 && _lineEdits[index]->hasFocus()) {
    _lineEdits[index + 1]->setFocus();
    _lineEdits[index + 1]->setCursorPosition(0);
  }

  QString fullText;
  for (int i = 0; i < _segmentCount; ++i) {
    if (i > 0) {
      fullText += _separatorText;
    }
    fullText += _lineEdits[i]->text();
  }
  Q_EMIT q->segmentTextChanged(index, _lineEdits[index]->text());
  Q_EMIT q->textChanged(fullText);
  _updateCompletedState();
}

void NXSplitInputPrivate::_onChildEditingFinished(int index)
{
  Q_Q(NXSplitInput);
  Q_EMIT q->segmentEditingFinished(index);
  if (_allCompleted) {
    Q_EMIT q->editingFinished();
  }
}

void NXSplitInputPrivate::_animateMarkIn(int index)
{
  if (index < 0 || index >= _markAnimations.size()) {
    return;
  }
  QRect boxRect           = _lineEdits[index]->geometry();
  qreal target            = qMax(qreal(10), (boxRect.width() - 4) / 2.0);
  QVariantAnimation *anim = _markAnimations[index];
  anim->stop();
  anim->setStartValue(_segmentMarks[index]);
  anim->setEndValue(target);
  anim->start();
}

void NXSplitInputPrivate::_animateMarkOut(int index)
{
  if (index < 0 || index >= _markAnimations.size()) {
    return;
  }
  QVariantAnimation *anim = _markAnimations[index];
  anim->stop();
  anim->setStartValue(_segmentMarks[index]);
  anim->setEndValue(0);
  anim->start();
}
