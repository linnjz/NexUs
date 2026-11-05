#include "NXSpinBoxPrivate.h"

#include <QClipboard>
#include <QGuiApplication>
#include <QLineEdit>
#include <QTimer>

#include "NXMenu.h"
#include "NXSpinBox.h"
#include "NXTheme.h"

NXSpinBoxPrivate::NXSpinBoxPrivate(QObject *parent)
    : QObject { parent }
{
}

NXSpinBoxPrivate::~NXSpinBoxPrivate()
{
}

void NXSpinBoxPrivate::onThemeChanged(NXThemeType::ThemeMode themeMode)
{
  Q_Q(NXSpinBox);
  m_ThemeMode = themeMode;
  QPalette palette;
  palette.setColor(QPalette::Base, Qt::transparent);
  palette.setColor(QPalette::Text, NXThemeColor(m_ThemeMode, BasicText));
  q->lineEdit()->setPalette(palette);
}

NXMenu *NXSpinBoxPrivate::_createStandardContextMenu()
{
  Q_Q(NXSpinBox);
  QLineEdit *lineEdit = q->lineEdit();
  NXMenu *menu        = new NXMenu(q);
  menu->setMenuItemHeight(27);
  menu->setAttribute(Qt::WA_DeleteOnClose);
  QAction *action = nullptr;
  if (!lineEdit->isReadOnly()) {
    action = menu->addNXIconAction(NXIconType::ArrowRotateLeft, tr("撤销"), QKeySequence::Undo);
    action->setEnabled(lineEdit->isUndoAvailable());
    connect(action, &QAction::triggered, lineEdit, &QLineEdit::undo);

    action = menu->addNXIconAction(NXIconType::ArrowRotateRight, tr("恢复"), QKeySequence::Redo);
    action->setEnabled(lineEdit->isRedoAvailable());
    connect(action, &QAction::triggered, lineEdit, &QLineEdit::redo);
    menu->addSeparator();
  }
#ifndef QT_NO_CLIPBOARD
  if (!lineEdit->isReadOnly()) {
    action = menu->addNXIconAction(NXIconType::KnifeKitchen, tr("剪切"), QKeySequence::Cut);
    action->setEnabled(!lineEdit->isReadOnly() && lineEdit->hasSelectedText() && lineEdit->echoMode() == QLineEdit::Normal);
    connect(action, &QAction::triggered, lineEdit, &QLineEdit::cut);
  }

  action = menu->addNXIconAction(NXIconType::Copy, tr("复制"), QKeySequence::Copy);
  action->setEnabled(lineEdit->hasSelectedText() && lineEdit->echoMode() == QLineEdit::Normal);
  connect(action, &QAction::triggered, lineEdit, &QLineEdit::copy);

  if (!lineEdit->isReadOnly()) {
    action = menu->addNXIconAction(NXIconType::Paste, tr("粘贴"), QKeySequence::Paste);
    action->setEnabled(!lineEdit->isReadOnly() && !QGuiApplication::clipboard()->text().isEmpty());
    connect(action, &QAction::triggered, lineEdit, &QLineEdit::paste);
  }
#endif
  if (!lineEdit->isReadOnly()) {
    action = menu->addNXIconAction(NXIconType::DeleteLeft, tr("删除"));
    action->setEnabled(!lineEdit->isReadOnly() && !lineEdit->text().isEmpty() && lineEdit->hasSelectedText());
    connect(action, &QAction::triggered, this, [=](bool checked) {
      if (lineEdit->hasSelectedText()) {
        int startIndex = lineEdit->selectionStart();
        int endIndex   = lineEdit->selectionEnd();
        lineEdit->setText(lineEdit->text().remove(startIndex, endIndex - startIndex));
      }
    });
  }
  if (!menu->isEmpty()) {
    menu->addSeparator();
  }
  action = menu->addAction(tr("全选"));
  action->setShortcut(QKeySequence::SelectAll);
  action->setEnabled(!lineEdit->text().isEmpty() && !(lineEdit->selectedText() == lineEdit->text()));
  connect(action, &QAction::triggered, q, &NXSpinBox::selectAll);
  return menu;
}
