#include "NXMarkdownViewer.h"

#include <QTextBrowser>
#include <QVBoxLayout>

#include "NXTheme.h"
#include "private/NXMarkdownViewerPrivate.h"

Q_PROPERTY_CREATE_CPP(NXMarkdownViewer, QS_SET_CREF(QString), Markdown)
Q_PROPERTY_CREATE_CPP(NXMarkdownViewer, int, BorderRadius)

NXMarkdownViewer::NXMarkdownViewer(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new NXMarkdownViewerPrivate())
{
  Q_D(NXMarkdownViewer);
  d->q_ptr          = this;
  d->_pBorderRadius = 0;
  setObjectName("NXMarkdownViewer");

  d->_textBrowser = new QTextBrowser(this);
  d->_textBrowser->setFrameShape(QFrame::NoFrame);
  d->_textBrowser->setReadOnly(true);
  d->_textBrowser->setOpenExternalLinks(true);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->addWidget(d->_textBrowser);

  d->_themeMode = nxTheme->getThemeMode();
  d->_applyThemeStyle();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->_themeMode = themeMode;
    d->_applyThemeStyle();
  });
  connect(this, &NXMarkdownViewer::pMarkdownChanged, this, [=]() {
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    d->_textBrowser->setMarkdown(d->_pMarkdown);
#else
    QString html = d->_pMarkdown;
    html.replace(QStringLiteral("\n\n"), QStringLiteral("<br/><br/>"));
    d->_textBrowser->setHtml(html);
#endif
  });
}

NXMarkdownViewer::~NXMarkdownViewer()
{
}

NXMarkdownViewerPrivate::NXMarkdownViewerPrivate(QObject *parent)
    : QObject(parent)
{
}

NXMarkdownViewerPrivate::~NXMarkdownViewerPrivate()
{
}

void NXMarkdownViewerPrivate::_applyThemeStyle()
{
  if (!_textBrowser) {
    return;
  }
  QColor textColor      = NXThemeColor(_themeMode, BasicText);
  QColor linkColor      = NXThemeColor(_themeMode, PrimaryNormal);
  QColor codeBackground = NXThemeColor(_themeMode, BasicBaseDeep);
  QString styleSheet    = QString(QStringLiteral("QTextBrowser {"
                                                 "    background-color: transparent;"
                                                 "    color: %1;"
                                                 "}"
                                                 "a { color: %2; }"
                                                 "code, pre {"
                                                 "    background-color: %3;"
                                                 "}"))
                              .arg(textColor.name(), linkColor.name(), codeBackground.name());
  _textBrowser->setStyleSheet(styleSheet);
}
