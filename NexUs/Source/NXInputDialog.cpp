#include "NXInputDialog.h"

#include <QApplication>
#include <QDoubleValidator>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QKeyEvent>
#include <QPainter>
#include <QScreen>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>

#include "DeveloperComponents/NXMaskWidget.h"
#include "DeveloperComponents/NXWinShadowHelper.h"
#include "NXDoubleSpinBox.h"
#include "NXLineEdit.h"
#include "NXPushButton.h"
#include "NXSpinBox.h"
#include "NXText.h"
#include "NXTheme.h"
#include "private/NXInputDialogPrivate.h"

Q_TAKEOVER_NATIVEEVENT_CPP(NXInputDialog, d_func()->m_AppBar);
Q_PROPERTY_CREATE_CPP(NXInputDialog, QPS_SCREF_PAEMIT(QString), TitleText)
Q_PROPERTY_CREATE_CPP(NXInputDialog, QPS_SCREF_PAEMIT(QString), SubTitleText)
Q_PROPERTY_CREATE_CPP(NXInputDialog, QPS_SCREF_PAEMIT(QString), LabelText)
Q_PROPERTY_CREATE_CPP(NXInputDialog, QPS_SCREF_PAEMIT(QString), TextValue)
Q_PROPERTY_CREATE_CPP(NXInputDialog, QPS_SCREF_PAEMIT(QString), OkButtonText)
Q_PROPERTY_CREATE_CPP(NXInputDialog, QPS_SCREF_PAEMIT(QString), CancelButtonText)
Q_PROPERTY_CREATE_CPP(NXInputDialog, QPS_SCREF_PAEMIT(QString), PlaceholderText)
Q_PROPERTY_CREATE_CPP(NXInputDialog, QPS_PAEMIT(double), DoubleValue)
Q_PROPERTY_CREATE_CPP(NXInputDialog, QPS_PAEMIT(int), IntValue)
Q_PROPERTY_CREATE_CPP(NXInputDialog, QPS_PAEMIT(int), InputMinimumWidth)
Q_PROPERTY_CREATE_CPP(NXInputDialog, QPS_PAEMIT(int), InputMaximumWidth)

NXInputDialog::NXInputDialog(QWidget *parent)
    : QDialog { parent }
    , d_ptr(new NXInputDialogPrivate())
{
  Q_D(NXInputDialog);
  d->q_ptr = this;

  d->m_MaskWidget = new NXMaskWidget(parent);
  d->m_MaskWidget->move(0, 0);
  d->m_MaskWidget->setFixedSize(parent->size());
  d->m_MaskWidget->setVisible(false);

  resize(400, 250);
  setWindowModality(Qt::ApplicationModal);

  d->m_AppBar = new NXAppBar(this);
  d->m_AppBar->setWindowButtonFlags(NXAppBarType::NoneButtonHint);
  d->m_AppBar->setIsFixedSize(true);
  d->m_AppBar->setAppBarHeight(0);
#ifdef Q_OS_WIN
  createWinId();
#endif

  d->m_TitleText         = QStringLiteral("");
  d->m_SubTitleText      = QStringLiteral("");
  d->m_LabelText         = QStringLiteral("");
  d->m_TextValue         = QStringLiteral("");
  d->m_IntValue          = 0;
  d->m_DoubleValue       = 0.0;
  d->m_OkButtonText      = QStringLiteral("确定");
  d->m_CancelButtonText  = QStringLiteral("取消");
  d->m_PlaceholderText   = QStringLiteral("");
  d->m_InputMinimumWidth = 100;
  d->m_InputMaximumWidth = QWIDGETSIZE_MAX;

  d->m_TitleLabel = new NXText(d->m_TitleText, this);
  d->m_TitleLabel->setTextStyle(NXTextType::Title);
  d->m_SubTitleLabel = new NXText(d->m_SubTitleText, this);
  d->m_SubTitleLabel->setTextStyle(NXTextType::Body);

  d->m_InputLabel = new NXText(d->m_LabelText, this);
  d->m_InputLabel->setTextStyle(NXTextType::Body);
  d->m_LineEdit = new NXLineEdit(this);
  d->m_LineEdit->setText(d->m_TextValue);
  d->m_LineEdit->setPlaceholderText(d->m_PlaceholderText);
  d->m_LineEdit->setIsClearButtonEnable(true);
  connect(d->m_LineEdit, &NXLineEdit::textChanged, this, [=](QString const &text) {
    d->m_TextValue = text;
    Q_EMIT textValueChanged(text);
    bool okInt;
    int intValue = text.toInt(&okInt);
    if (okInt) {
      d->m_IntValue = intValue;
      Q_EMIT intValueChanged(intValue);
    }
    bool okDouble;
    double doubleValue = text.toDouble(&okDouble);
    if (okDouble) {
      d->m_DoubleValue = doubleValue;
      Q_EMIT doubleValueChanged(doubleValue);
    }
  });

  d->m_CancelButton = new NXPushButton(d->m_CancelButtonText, this);
  connect(d->m_CancelButton, &NXPushButton::clicked, this, &NXInputDialog::onCancelButtonClicked);
  d->m_CancelButton->setMinimumSize(0, 0);
  d->m_CancelButton->setMaximumSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX));
  d->m_CancelButton->setFixedHeight(38);
  d->m_CancelButton->setBorderRadius(6);

  d->m_OkButton = new NXPushButton(d->m_OkButtonText, this);
  connect(d->m_OkButton, &NXPushButton::clicked, this, &NXInputDialog::onOkButtonClicked);
  d->m_OkButton->setLightDefaultColor(NXThemeColor(NXThemeType::Light, PrimaryNormal));
  d->m_OkButton->setLightHoverColor(NXThemeColor(NXThemeType::Light, PrimaryHover));
  d->m_OkButton->setLightPressColor(NXThemeColor(NXThemeType::Light, PrimaryPress));
  d->m_OkButton->setLightTextColor(Qt::white);
  d->m_OkButton->setDarkDefaultColor(NXThemeColor(NXThemeType::Dark, PrimaryNormal));
  d->m_OkButton->setDarkHoverColor(NXThemeColor(NXThemeType::Dark, PrimaryHover));
  d->m_OkButton->setDarkPressColor(NXThemeColor(NXThemeType::Dark, PrimaryPress));
  d->m_OkButton->setDarkTextColor(Qt::black);
  d->m_OkButton->setMinimumSize(0, 0);
  d->m_OkButton->setMaximumSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX));
  d->m_OkButton->setFixedHeight(38);
  d->m_OkButton->setBorderRadius(6);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);

  QWidget *contentWidget = new QWidget(this);
  d->m_ContentLayout      = new QVBoxLayout(contentWidget);
  d->m_ContentLayout->setContentsMargins(15, 25, 15, 15);
  d->m_ContentLayout->addWidget(d->m_TitleLabel);
  d->m_ContentLayout->addSpacing(2);
  d->m_ContentLayout->addWidget(d->m_SubTitleLabel);
  d->m_ContentLayout->addSpacing(15);
  d->m_ContentLayout->addWidget(d->m_InputLabel);
  d->m_ContentLayout->addSpacing(5);
  d->m_ContentLayout->addWidget(d->m_LineEdit);
  d->m_ContentLayout->addStretch();

  d->m_ButtonWidget = new QWidget(this);
  d->m_ButtonWidget->setFixedHeight(60);
  QHBoxLayout *buttonLayout = new QHBoxLayout(d->m_ButtonWidget);
  buttonLayout->setContentsMargins(15, 11, 15, 11);
  buttonLayout->addWidget(d->m_CancelButton);
  buttonLayout->addWidget(d->m_OkButton);

  mainLayout->addWidget(contentWidget);
  mainLayout->addWidget(d->m_ButtonWidget);

  d->m_ThemeMode = nxTheme->getThemeMode();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
  });

  QTimer::singleShot(0, d->m_LineEdit, [=]() {
    d->m_LineEdit->setFocus();
  });
}

NXInputDialog::~NXInputDialog()
{
  Q_D(NXInputDialog);
  d->m_MaskWidget->deleteLater();
}

QString NXInputDialog::getText(QWidget *parent, QString const &title, QString const &subtitle, QString const &label, QString const &text, bool *ok, QString const &okButtonText, QString const &cancelButtonText, int inputMinWidth, int inputMaxWidth)
{
  NXInputDialog dialog(parent);
  dialog.setTitleText(title);
  dialog.setSubTitleText(subtitle);
  dialog.setLabelText(label);
  dialog.setTextValue(text);
  dialog.setOkButtonText(okButtonText);
  dialog.setCancelButtonText(cancelButtonText);
  dialog.setInputMinimumWidth(inputMinWidth);
  dialog.setInputMaximumWidth(inputMaxWidth);
  dialog.updateLabels();

  bool accepted = (dialog.exec() == QDialog::Accepted);
  if (ok)
    *ok = accepted;

  return accepted ? dialog.getTextValue() : QString();
}

int NXInputDialog::getInt(QWidget *parent, QString const &title, QString const &subtitle, QString const &label, int value, int minValue, int maxValue, int step, bool *ok, QString const &okButtonText, QString const &cancelButtonText, int inputMinWidth, int inputMaxWidth)
{
  NXInputDialog dialog(parent);
  dialog.setTitleText(title);
  dialog.setSubTitleText(subtitle);
  dialog.setLabelText(label);
  dialog.setIntValue(value);
  dialog.setTextValue(QString::number(value));
  dialog.setOkButtonText(okButtonText);
  dialog.setCancelButtonText(cancelButtonText);
  dialog.setInputMinimumWidth(inputMinWidth);
  dialog.setInputMaximumWidth(inputMaxWidth);
  dialog.updateLabels();
  dialog.setIntRange(minValue, maxValue, step);

  bool accepted = (dialog.exec() == QDialog::Accepted);
  if (ok)
    *ok = accepted;

  return accepted ? dialog.getIntValue() : value;
}

double NXInputDialog::getDouble(QWidget *parent, QString const &title, QString const &subtitle, QString const &label, double value, double minValue, double maxValue, int decimals, bool *ok, QString const &okButtonText, QString const &cancelButtonText, int inputMinWidth, int inputMaxWidth)
{
  NXInputDialog dialog(parent);
  dialog.setTitleText(title);
  dialog.setSubTitleText(subtitle);
  dialog.setLabelText(label);
  dialog.setDoubleValue(value);
  dialog.setTextValue(QString::number(value, 'f', decimals));
  dialog.setOkButtonText(okButtonText);
  dialog.setCancelButtonText(cancelButtonText);
  dialog.setInputMinimumWidth(inputMinWidth);
  dialog.setInputMaximumWidth(inputMaxWidth);
  dialog.updateLabels();
  dialog.setDoubleRange(minValue, maxValue, decimals);

  bool accepted = (dialog.exec() == QDialog::Accepted);
  if (ok)
    *ok = accepted;

  return accepted ? dialog.getDoubleValue() : value;
}

QString NXInputDialog::getMultiLineText(QWidget *parent, QString const &title, QString const &subtitle, QString const &label, QString const &text, bool *ok, QString const &okButtonText, QString const &cancelButtonText, int inputMinWidth, int inputMaxWidth)
{
  NXInputDialog dialog(parent);
  dialog.setTitleText(title);
  dialog.setSubTitleText(subtitle);
  dialog.setLabelText(label);
  dialog.setTextValue(text);
  dialog.setOkButtonText(okButtonText);
  dialog.setCancelButtonText(cancelButtonText);
  dialog.setInputMinimumWidth(inputMinWidth);
  dialog.setInputMaximumWidth(inputMaxWidth);
  dialog.updateLabels();
  dialog.setMultiLine(true);

  bool accepted = (dialog.exec() == QDialog::Accepted);
  if (ok)
    *ok = accepted;

  return accepted ? dialog.getTextValue() : QString();
}

void NXInputDialog::setTextEchoMode(QLineEdit::EchoMode mode)
{
  Q_D(NXInputDialog);
  d->m_LineEdit->setEchoMode(mode);
}

QLineEdit::EchoMode NXInputDialog::textEchoMode() const
{
  Q_CONST_D(NXInputDialog);
  return d->m_LineEdit->echoMode();
}

void NXInputDialog::setIntRange(int minValue, int maxValue, int step)
{
  Q_D(NXInputDialog);

  if (d->m_IsIntMode)
    return;

  d->m_IsIntMode = true;

  if (!d->m_SpinBox) {
    d->m_SpinBox = new NXSpinBox(this);
    d->m_SpinBox->setButtonMode(NXSpinBoxType::PMSide);
    d->m_SpinBox->setMinimumWidth(d->m_InputMinimumWidth);
    d->m_SpinBox->setMaximumWidth(d->m_InputMaximumWidth);
    d->m_SpinBox->setMinimum(minValue);
    d->m_SpinBox->setMaximum(maxValue);
    d->m_SpinBox->setSingleStep(step);
    d->m_SpinBox->setValue(d->m_IntValue);

    connect(d->m_SpinBox, QOverload<int>::of(&NXSpinBox::valueChanged), this, [=](int value) {
      d->m_IntValue  = value;
      d->m_TextValue = QString::number(value);
      Q_EMIT intValueChanged(value);
      Q_EMIT textValueChanged(d->m_TextValue);
    });
  }

  d->m_LineEdit->setVisible(false);
  d->m_SpinBox->setVisible(true);
  d->m_SpinBox->setMinimum(minValue);
  d->m_SpinBox->setMaximum(maxValue);
  d->m_SpinBox->setSingleStep(step);
  d->m_SpinBox->setValue(d->m_IntValue);

  if (d->m_ContentLayout) {
    d->m_ContentLayout->removeWidget(d->m_LineEdit);
    d->m_ContentLayout->insertWidget(6, d->m_SpinBox);
  }
}

void NXInputDialog::setDoubleRange(double minValue, double maxValue, int decimals)
{
  Q_D(NXInputDialog);

  if (d->m_IsDoubleMode)
    return;

  d->m_IsDoubleMode = true;

  if (!d->m_DoubleSpinBox) {
    d->m_DoubleSpinBox = new NXDoubleSpinBox(this);
    d->m_DoubleSpinBox->setButtonMode(NXSpinBoxType::PMSide);
    d->m_DoubleSpinBox->setMinimumWidth(d->m_InputMinimumWidth);
    d->m_DoubleSpinBox->setMaximumWidth(d->m_InputMaximumWidth);
    d->m_DoubleSpinBox->setMinimum(minValue);
    d->m_DoubleSpinBox->setMaximum(maxValue);
    d->m_DoubleSpinBox->setDecimals(decimals);
    d->m_DoubleSpinBox->setValue(d->m_DoubleValue);

    connect(d->m_DoubleSpinBox, QOverload<double>::of(&NXDoubleSpinBox::valueChanged), this, [=](double value) {
      d->m_DoubleValue = value;
      d->m_TextValue   = QString::number(value, 'f', decimals);
      Q_EMIT doubleValueChanged(value);
      Q_EMIT textValueChanged(d->m_TextValue);
    });
  }

  d->m_LineEdit->setVisible(false);
  d->m_DoubleSpinBox->setVisible(true);
  d->m_DoubleSpinBox->setMinimum(minValue);
  d->m_DoubleSpinBox->setMaximum(maxValue);
  d->m_DoubleSpinBox->setDecimals(decimals);
  d->m_DoubleSpinBox->setValue(d->m_DoubleValue);

  if (d->m_ContentLayout) {
    d->m_ContentLayout->removeWidget(d->m_LineEdit);
    d->m_ContentLayout->insertWidget(6, d->m_DoubleSpinBox);
  }
}

void NXInputDialog::setMultiLine(bool multiLine)
{
  Q_D(NXInputDialog);
  if (d->m_IsMultiLine == multiLine)
    return;

  d->m_IsMultiLine = multiLine;

  if (multiLine) {
    if (!d->m_TextEdit) {
      d->m_TextEdit = new QTextEdit(this);
      d->m_TextEdit->setPlainText(d->m_TextValue);
      d->m_TextEdit->setMinimumHeight(150);
      connect(d->m_TextEdit, &QTextEdit::textChanged, this, [=]() {
        d->m_TextValue = d->m_TextEdit->toPlainText();
        Q_EMIT textValueChanged(d->m_TextValue);
      });
    }
    d->m_LineEdit->setVisible(false);
    d->m_TextEdit->setVisible(true);
    d->m_TextEdit->setPlainText(d->m_TextValue);

    if (d->m_ContentLayout) {
      d->m_ContentLayout->removeWidget(d->m_LineEdit);
      d->m_ContentLayout->insertWidget(6, d->m_TextEdit);
    }
    resize(400, 350);
  } else {
    if (d->m_TextEdit) {
      d->m_TextEdit->setVisible(false);
    }
    d->m_LineEdit->setVisible(true);
    d->m_LineEdit->setText(d->m_TextValue);

    if (d->m_ContentLayout && d->m_TextEdit) {
      d->m_ContentLayout->removeWidget(d->m_TextEdit);
      d->m_ContentLayout->insertWidget(4, d->m_LineEdit);
    }
    resize(400, 250);
  }
}

void NXInputDialog::showEvent(QShowEvent *event)
{
  Q_D(NXInputDialog);
  if (parentWidget()) {
    d->m_MaskWidget->setVisible(true);
    d->m_MaskWidget->raise();
    d->m_MaskWidget->setFixedSize(parentWidget()->size());
    d->m_MaskWidget->doMaskAnimation(90);
  }
  QDialog::showEvent(event);
}

void NXInputDialog::paintEvent(QPaintEvent *event)
{
  Q_D(NXInputDialog);
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  painter.setPen(Qt::NoPen);
  painter.setBrush(NXThemeColor(d->m_ThemeMode, DialogBase));
  painter.drawRect(rect());
  painter.setBrush(NXThemeColor(d->m_ThemeMode, DialogLayoutArea));
  painter.drawRoundedRect(QRectF(0, height() - 60, width(), 60), 8, 8);
  painter.restore();
}

void NXInputDialog::keyPressEvent(QKeyEvent *event)
{
  if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
    onOkButtonClicked();
    event->accept();
    return;
  } else if (event->key() == Qt::Key_Escape) {
    onCancelButtonClicked();
    event->accept();
    return;
  }
  QDialog::keyPressEvent(event);
}

void NXInputDialog::onOkButtonClicked()
{
  Q_D(NXInputDialog);
  d->_doCloseAnimation(true);
}

void NXInputDialog::onCancelButtonClicked()
{
  Q_D(NXInputDialog);
  d->_doCloseAnimation(false);
}

void NXInputDialog::updateLabels()
{
  Q_D(NXInputDialog);
  d->m_TitleLabel->setText(d->m_TitleText);
  d->m_SubTitleLabel->setText(d->m_SubTitleText);
  d->m_InputLabel->setText(d->m_LabelText);
  d->m_CancelButton->setText(d->m_CancelButtonText);
  d->m_OkButton->setText(d->m_OkButtonText);
}
