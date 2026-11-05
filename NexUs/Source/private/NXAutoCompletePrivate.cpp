#include "NXAutoCompletePrivate.h"

#include <QApplication>
#include <QLayout>
#include <QPropertyAnimation>
#include <QRegularExpression>
#include <QScreen>
#include <QSortFilterProxyModel>
#include <QStringListModel>
#if QT_VERSION < QT_VERSION_CHECK(5, 12, 0)
#  include <QRegExp>
#endif

#include "NXAutoComplete.h"
#include "NXAutoCompleteContainer.h"
#include "NXAutoCompleteDelegate.h"
#include "NXBaseListView.h"
#include "NXLineEdit.h"
#include "NXTheme.h"

NXAutoCompletePrivate::NXAutoCompletePrivate(QObject *parent)
    : QObject(parent)
{
}

NXAutoCompletePrivate::~NXAutoCompletePrivate()
{
}

void NXAutoCompletePrivate::onThemeModeChanged(NXThemeType::ThemeMode themeMode)
{
  m_ThemeMode = themeMode;
}

void NXAutoCompletePrivate::onTextEdited(QString const &text)
{
  Q_Q(NXAutoComplete);
  Q_EMIT q->textEdited(text);

  if (text.isEmpty()) {
    _hidePopup();
    return;
  }

  _updateFilter(text);

  if (m_FilterModel->rowCount() > 0) {
    _showPopup();
  } else {
    _hidePopup();
  }
}

void NXAutoCompletePrivate::onItemClicked(QModelIndex const &index)
{
  Q_Q(NXAutoComplete);
  if (!index.isValid()) {
    return;
  }
  QString text = m_FilterModel->data(index, Qt::DisplayRole).toString();
  m_LineEdit->setText(text);
  _hidePopup();
  Q_EMIT q->completionSelected(text);
}

void NXAutoCompletePrivate::_showPopup()
{
  if (!m_Popup) {
    return;
  }

  int rowCount    = m_FilterModel->rowCount();
  int visibleRows = qMin(rowCount, m_MaxVisibleItems);
  int popupHeight = visibleRows * 36 + 8;
  int popupWidth  = m_LineEdit->width();

  QPoint lineEditGlobal = m_LineEdit->mapToGlobal(QPoint(0, 0));
  QRect screenGeometry  = QApplication::primaryScreen()->availableGeometry();

  bool showAbove = (lineEditGlobal.y() + m_LineEdit->height() + popupHeight > screenGeometry.bottom()) &&
                   (lineEditGlobal.y() - popupHeight >= screenGeometry.top());

  QPoint popupPos;
  if (showAbove) {
    popupPos = QPoint(lineEditGlobal.x(), lineEditGlobal.y() - popupHeight - 3);
  } else {
    popupPos = QPoint(lineEditGlobal.x(), lineEditGlobal.y() + m_LineEdit->height() + 3);
  }

  if (m_IsPopupVisible) {
    m_Popup->setFixedSize(popupWidth, popupHeight);
    m_Popup->move(popupPos);
    m_Popup->update();
    return;
  }
  m_IsPopupVisible = true;

  m_Popup->setFixedSize(popupWidth, popupHeight);
  m_Popup->show();
  m_Popup->move(popupPos);

  m_ListView->resize(popupWidth - 8, popupHeight - 8);

  QLayout *layout = m_Popup->layout();
  while (layout->count()) {
    layout->takeAt(0);
  }

  QPropertyAnimation *heightAnimation = new QPropertyAnimation(m_Popup, "maximumHeight");
  connect(heightAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
    m_Popup->setFixedHeight(value.toUInt());
  });
  heightAnimation->setStartValue(1);
  heightAnimation->setEndValue(popupHeight);
  heightAnimation->setEasingCurve(QEasingCurve::OutCubic);
  heightAnimation->setDuration(400);
  heightAnimation->start(QAbstractAnimation::DeleteWhenStopped);

  QPropertyAnimation *viewAnimation = new QPropertyAnimation(m_ListView, "pos");
  connect(viewAnimation, &QPropertyAnimation::finished, this, [=]() {
    m_IsAllowHidePopup = true;
    layout->addWidget(m_ListView);
  });
  QPoint viewPos = m_ListView->pos();
  viewAnimation->setStartValue(QPoint(viewPos.x(), viewPos.y() - m_ListView->height()));
  viewAnimation->setEndValue(viewPos);
  viewAnimation->setEasingCurve(QEasingCurve::OutCubic);
  viewAnimation->setDuration(400);
  viewAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void NXAutoCompletePrivate::_hidePopup()
{
  if (!m_Popup || !m_IsPopupVisible) {
    return;
  }
  if (!m_IsAllowHidePopup) {
    return;
  }

  m_IsAllowHidePopup   = false;
  int containerHeight = m_Popup->height();

  QLayout *layout = m_Popup->layout();
  while (layout->count()) {
    layout->takeAt(0);
  }

  QPropertyAnimation *viewAnimation = new QPropertyAnimation(m_ListView, "pos");
  QPoint viewPos                    = m_ListView->pos();
  connect(viewAnimation, &QPropertyAnimation::finished, this, [=]() {
    layout->addWidget(m_ListView);
    m_Popup->hide();
    m_Popup->setFixedHeight(containerHeight);
    m_ListView->move(viewPos);
    m_IsPopupVisible = false;
  });
  viewAnimation->setStartValue(viewPos);
  viewAnimation->setEndValue(QPoint(viewPos.x(), viewPos.y() - m_ListView->height()));
  viewAnimation->setEasingCurve(QEasingCurve::InCubic);
  viewAnimation->setDuration(400);
  viewAnimation->start(QAbstractAnimation::DeleteWhenStopped);

  QPropertyAnimation *heightAnimation = new QPropertyAnimation(m_Popup, "maximumHeight");
  connect(heightAnimation, &QPropertyAnimation::valueChanged, this, [=](QVariant const &value) {
    m_Popup->setFixedHeight(value.toInt());
  });
  heightAnimation->setStartValue(m_Popup->height());
  heightAnimation->setEndValue(1);
  heightAnimation->setEasingCurve(QEasingCurve::InCubic);
  heightAnimation->setDuration(400);
  heightAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void NXAutoCompletePrivate::_updateFilter(QString const &text)
{
  Qt::CaseSensitivity cs = m_CaseSensitivity;
  switch (m_MatchMode) {
  case NXAutoComplete::StartsWith : {
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
    QRegularExpression::PatternOption caseOption =
        cs == Qt::CaseInsensitive ? QRegularExpression::CaseInsensitiveOption : QRegularExpression::NoPatternOption;
    m_FilterModel->setFilterRegularExpression(
        QRegularExpression(QStringLiteral("^") + QRegularExpression::escape(text), caseOption));
#else
    m_FilterModel->setFilterCaseSensitivity(cs);
    m_FilterModel->setFilterRegExp(QRegExp(QStringLiteral("^") + QRegExp::escape(text), cs));
#endif
    break;
  }
  case NXAutoComplete::EndsWith : {
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
    QRegularExpression::PatternOption caseOption =
        cs == Qt::CaseInsensitive ? QRegularExpression::CaseInsensitiveOption : QRegularExpression::NoPatternOption;
    m_FilterModel->setFilterRegularExpression(
        QRegularExpression(QRegularExpression::escape(text) + QStringLiteral("$"), caseOption));
#else
    m_FilterModel->setFilterCaseSensitivity(cs);
    m_FilterModel->setFilterRegExp(QRegExp(QRegExp::escape(text) + QStringLiteral("$"), cs));
#endif
    break;
  }
  case NXAutoComplete::RegExp : {
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
    QRegularExpression::PatternOption caseOption =
        cs == Qt::CaseInsensitive ? QRegularExpression::CaseInsensitiveOption : QRegularExpression::NoPatternOption;
    QRegularExpression re(text, caseOption);
    if (re.isValid()) {
      m_FilterModel->setFilterRegularExpression(re);
    }
#else
    QRegExp re(text, cs);
    if (re.isValid()) {
      m_FilterModel->setFilterRegExp(re);
    }
#endif
    break;
  }
  case NXAutoComplete::Contains :
  default :
    m_FilterModel->setFilterCaseSensitivity(cs);
    m_FilterModel->setFilterFixedString(text);
    break;
  }

  NXAutoCompleteDelegate *delegate = qobject_cast<NXAutoCompleteDelegate *>(m_ListView->itemDelegate());
  if (delegate) {
    delegate->setHighlightText(text);
    delegate->setMatchMode(m_MatchMode);
  }
}
