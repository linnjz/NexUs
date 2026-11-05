#include "NXMarkdownViewer.h"

#include <QTextBrowser>
#include <QVBoxLayout>

#include "NXTheme.h"
#include "private/NXMarkdownViewerPrivate.h"

Q_PROPERTY_CREATE_CPP(NXMarkdownViewer, QPS_SCREF_PAEMIT(QString), Markdown)
Q_PROPERTY_CREATE_CPP(NXMarkdownViewer, QPS_PAEMIT(int), BorderRadius)

NXMarkdownViewer::NXMarkdownViewer(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new NXMarkdownViewerPrivate())
{
  Q_D(NXMarkdownViewer);
  d->q_ptr          = this;
  d->m_BorderRadius = 0;
  setObjectName("NXMarkdownViewer");

  d->m_TextBrowser = new QTextBrowser(this);
  d->m_TextBrowser->setFrameShape(QFrame::NoFrame);
  d->m_TextBrowser->setReadOnly(true);
  d->m_TextBrowser->setOpenExternalLinks(true);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->addWidget(d->m_TextBrowser);

  d->m_ThemeMode = nxTheme->getThemeMode();
  d->_applyThemeStyle();
  connect(nxTheme, &NXTheme::themeModeChanged, this, [=](NXThemeType::ThemeMode themeMode) {
    d->m_ThemeMode = themeMode;
    d->_applyThemeStyle();
  });
  connect(this, &NXMarkdownViewer::signalMarkdownChanged, this, [=]() {
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    d->m_TextBrowser->setMarkdown(d->m_Markdown);
#else
    QString html = d->m_Markdown;
    html.replace(QStringLiteral("\n\n"), QStringLiteral("<br/><br/>"));
    d->m_TextBrowser->setHtml(html);
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
  if (!m_TextBrowser) {
    return;
  }
  QColor textColor      = NXThemeColor(m_ThemeMode, BasicText);
  QColor linkColor      = NXThemeColor(m_ThemeMode, PrimaryNormal);
  QColor codeBackground = NXThemeColor(m_ThemeMode, BasicBaseDeep);
  QString styleSheet    = QString(QStringLiteral("QTextBrowser {"
                                                 "    background-color: transparent;"
                                                 "    color: %1;"
                                                 "}"
                                                 "a { color: %2; }"
                                                 "code, pre {"
                                                 "    background-color: %3;"
                                                 "}"))
                              .arg(textColor.name(), linkColor.name(), codeBackground.name());
  m_TextBrowser->setStyleSheet(styleSheet);
}
