#ifndef NXCODEHIGHLIGHTER_H
#define NXCODEHIGHLIGHTER_H

#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>

#include "NXTheme.h"

class NXCodeHighlighter : public QSyntaxHighlighter
{
public:
  explicit NXCodeHighlighter(QTextDocument *parent, NXThemeType::ThemeMode themeMode, int language = 0);

  void setThemeMode(NXThemeType::ThemeMode themeMode);

  void setLanguage(int language);

protected:
  void highlightBlock(QString const &text) override;

private:
  struct HighlightRule
  {
    QRegularExpression pattern;
    QTextCharFormat format;
  };

  QStringList _getKeywords() const;
  QString _getSingleLineComment() const;
  bool _hasMultiLineComment() const;
  bool _hasPreprocessor() const;
  void setupRules();

  QVector<HighlightRule> m_Rules;
  NXThemeType::ThemeMode m_ThemeMode;
  int m_Language { 0 };
};

#endif // NXCODEHIGHLIGHTER_H
