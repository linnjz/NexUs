#ifndef NXCODEHIGHLIGHTER_H
#define NXCODEHIGHLIGHTER_H

#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>

#include "NXTheme.h"

class NXCodeHighlighter : public QSyntaxHighlighter
{
public:
  explicit NXCodeHighlighter(QTextDocument *parent, NXThemeType::ThemeMode themeMode, int language = 0)
      : QSyntaxHighlighter(parent)
      , _themeMode(themeMode)
      , _language(language)
  {
    setupRules();
  }

  void setThemeMode(NXThemeType::ThemeMode themeMode)
  {
    _themeMode = themeMode;
    setupRules();
    rehighlight();
  }

  void setLanguage(int language)
  {
    _language = language;
    setupRules();
    rehighlight();
  }

protected:
  void highlightBlock(const QString &text) override
  {
    for (const HighlightRule &rule : std::as_const(_rules)) {
      QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
      while (matchIterator.hasNext()) {
        QRegularExpressionMatch match = matchIterator.next();
        setFormat(match.capturedStart(), match.capturedLength(), rule.format);
      }
    }

    if (_hasMultiLineComment()) {
      QTextCharFormat commentFormat;
      commentFormat.setForeground(NXThemeColor(_themeMode, BasicDetailsText));

      setCurrentBlockState(0);
      int startIndex = 0;
      if (previousBlockState() != 1) {
        startIndex = text.indexOf(QRegularExpression(QStringLiteral("/\\*")));
      }
      while (startIndex >= 0) {
        QRegularExpressionMatch endMatch;
        int endIndex = text.indexOf(QRegularExpression(QStringLiteral("\\*/")), startIndex, &endMatch);
        int commentLength;
        if (endIndex == -1) {
          setCurrentBlockState(1);
          commentLength = text.length() - startIndex;
        } else {
          commentLength = endIndex - startIndex + endMatch.capturedLength();
        }
        setFormat(startIndex, commentLength, commentFormat);
        startIndex = text.indexOf(QRegularExpression(QStringLiteral("/\\*")), startIndex + commentLength);
      }
    }
  }

private:
  struct HighlightRule
  {
    QRegularExpression pattern;
    QTextCharFormat format;
  };

  QStringList _getKeywords() const
  {
    switch (_language) {
    case 1 : // C
      return {
        QStringLiteral("auto"), QStringLiteral("break"), QStringLiteral("case"), QStringLiteral("char"),
        QStringLiteral("const"), QStringLiteral("continue"), QStringLiteral("default"), QStringLiteral("do"),
        QStringLiteral("double"), QStringLiteral("else"), QStringLiteral("enum"), QStringLiteral("extern"),
        QStringLiteral("float"), QStringLiteral("for"), QStringLiteral("goto"), QStringLiteral("if"),
        QStringLiteral("int"), QStringLiteral("long"), QStringLiteral("register"), QStringLiteral("return"),
        QStringLiteral("short"), QStringLiteral("signed"), QStringLiteral("sizeof"), QStringLiteral("static"),
        QStringLiteral("struct"), QStringLiteral("switch"), QStringLiteral("typedef"), QStringLiteral("union"),
        QStringLiteral("unsigned"), QStringLiteral("void"), QStringLiteral("volatile"), QStringLiteral("while"),
        QStringLiteral("NULL"), QStringLiteral("inline"), QStringLiteral("restrict")
      };
    case 2 : // CSharp
      return { QStringLiteral("abstract"), QStringLiteral("as"), QStringLiteral("base"),
               QStringLiteral("bool"), QStringLiteral("break"), QStringLiteral("byte"),
               QStringLiteral("case"), QStringLiteral("catch"), QStringLiteral("char"),
               QStringLiteral("checked"), QStringLiteral("class"), QStringLiteral("const"),
               QStringLiteral("continue"), QStringLiteral("decimal"), QStringLiteral("default"),
               QStringLiteral("delegate"), QStringLiteral("do"), QStringLiteral("double"),
               QStringLiteral("else"), QStringLiteral("enum"), QStringLiteral("event"),
               QStringLiteral("explicit"), QStringLiteral("extern"), QStringLiteral("false"),
               QStringLiteral("finally"), QStringLiteral("fixed"), QStringLiteral("float"),
               QStringLiteral("for"), QStringLiteral("foreach"), QStringLiteral("goto"),
               QStringLiteral("if"), QStringLiteral("implicit"), QStringLiteral("in"),
               QStringLiteral("int"), QStringLiteral("interface"), QStringLiteral("internal"),
               QStringLiteral("is"), QStringLiteral("lock"), QStringLiteral("long"),
               QStringLiteral("namespace"), QStringLiteral("new"), QStringLiteral("null"),
               QStringLiteral("object"), QStringLiteral("operator"), QStringLiteral("out"),
               QStringLiteral("override"), QStringLiteral("params"), QStringLiteral("private"),
               QStringLiteral("protected"), QStringLiteral("public"), QStringLiteral("readonly"),
               QStringLiteral("ref"), QStringLiteral("return"), QStringLiteral("sbyte"),
               QStringLiteral("sealed"), QStringLiteral("short"), QStringLiteral("sizeof"),
               QStringLiteral("static"), QStringLiteral("string"), QStringLiteral("struct"),
               QStringLiteral("switch"), QStringLiteral("this"), QStringLiteral("throw"),
               QStringLiteral("true"), QStringLiteral("try"), QStringLiteral("typeof"),
               QStringLiteral("uint"), QStringLiteral("ulong"), QStringLiteral("unchecked"),
               QStringLiteral("unsafe"), QStringLiteral("ushort"), QStringLiteral("using"),
               QStringLiteral("var"), QStringLiteral("virtual"), QStringLiteral("void"),
               QStringLiteral("volatile"), QStringLiteral("while"), QStringLiteral("async"),
               QStringLiteral("await"), QStringLiteral("get"), QStringLiteral("set"),
               QStringLiteral("yield") };
    case 3 : // Python
      return {
        QStringLiteral("False"), QStringLiteral("None"), QStringLiteral("True"), QStringLiteral("and"),
        QStringLiteral("as"), QStringLiteral("assert"), QStringLiteral("async"), QStringLiteral("await"),
        QStringLiteral("break"), QStringLiteral("class"), QStringLiteral("continue"), QStringLiteral("def"),
        QStringLiteral("del"), QStringLiteral("elif"), QStringLiteral("else"), QStringLiteral("except"),
        QStringLiteral("finally"), QStringLiteral("for"), QStringLiteral("from"), QStringLiteral("global"),
        QStringLiteral("if"), QStringLiteral("import"), QStringLiteral("in"), QStringLiteral("is"),
        QStringLiteral("lambda"), QStringLiteral("nonlocal"), QStringLiteral("not"), QStringLiteral("or"),
        QStringLiteral("pass"), QStringLiteral("raise"), QStringLiteral("return"), QStringLiteral("try"),
        QStringLiteral("while"), QStringLiteral("with"), QStringLiteral("yield"), QStringLiteral("self"),
        QStringLiteral("print"), QStringLiteral("range"), QStringLiteral("len"), QStringLiteral("int"),
        QStringLiteral("float"), QStringLiteral("str"), QStringLiteral("list"), QStringLiteral("dict"),
        QStringLiteral("tuple"), QStringLiteral("set"), QStringLiteral("bool"), QStringLiteral("type")
      };
    case 4 : // JavaScript
      return { QStringLiteral("await"), QStringLiteral("break"), QStringLiteral("case"),
               QStringLiteral("catch"), QStringLiteral("class"), QStringLiteral("const"),
               QStringLiteral("continue"), QStringLiteral("debugger"), QStringLiteral("default"),
               QStringLiteral("delete"), QStringLiteral("do"), QStringLiteral("else"),
               QStringLiteral("export"), QStringLiteral("extends"), QStringLiteral("false"),
               QStringLiteral("finally"), QStringLiteral("for"), QStringLiteral("function"),
               QStringLiteral("if"), QStringLiteral("import"), QStringLiteral("in"),
               QStringLiteral("instanceof"), QStringLiteral("let"), QStringLiteral("new"),
               QStringLiteral("null"), QStringLiteral("of"), QStringLiteral("return"),
               QStringLiteral("super"), QStringLiteral("switch"), QStringLiteral("this"),
               QStringLiteral("throw"), QStringLiteral("true"), QStringLiteral("try"),
               QStringLiteral("typeof"), QStringLiteral("undefined"), QStringLiteral("var"),
               QStringLiteral("void"), QStringLiteral("while"), QStringLiteral("with"),
               QStringLiteral("yield"), QStringLiteral("async"), QStringLiteral("static"),
               QStringLiteral("get"), QStringLiteral("set"), QStringLiteral("constructor"),
               QStringLiteral("console"), QStringLiteral("window"), QStringLiteral("document") };
    case 5 : // Lua
      return { QStringLiteral("and"), QStringLiteral("break"), QStringLiteral("do"),
               QStringLiteral("else"), QStringLiteral("elseif"), QStringLiteral("end"),
               QStringLiteral("false"), QStringLiteral("for"), QStringLiteral("function"),
               QStringLiteral("goto"), QStringLiteral("if"), QStringLiteral("in"),
               QStringLiteral("local"), QStringLiteral("nil"), QStringLiteral("not"),
               QStringLiteral("or"), QStringLiteral("repeat"), QStringLiteral("return"),
               QStringLiteral("then"), QStringLiteral("true"), QStringLiteral("until"),
               QStringLiteral("while"), QStringLiteral("self"), QStringLiteral("require"),
               QStringLiteral("print"), QStringLiteral("pairs"), QStringLiteral("ipairs"),
               QStringLiteral("type"), QStringLiteral("tostring"), QStringLiteral("tonumber"),
               QStringLiteral("table"), QStringLiteral("string"), QStringLiteral("math"),
               QStringLiteral("io"), QStringLiteral("os"), QStringLiteral("coroutine"),
               QStringLiteral("setmetatable"), QStringLiteral("getmetatable") };
    case 6 : // Rust
      return {
        QStringLiteral("as"), QStringLiteral("async"), QStringLiteral("await"), QStringLiteral("break"),
        QStringLiteral("const"), QStringLiteral("continue"), QStringLiteral("crate"), QStringLiteral("dyn"),
        QStringLiteral("else"), QStringLiteral("enum"), QStringLiteral("extern"), QStringLiteral("false"),
        QStringLiteral("fn"), QStringLiteral("for"), QStringLiteral("if"), QStringLiteral("impl"),
        QStringLiteral("in"), QStringLiteral("let"), QStringLiteral("loop"), QStringLiteral("match"),
        QStringLiteral("mod"), QStringLiteral("move"), QStringLiteral("mut"), QStringLiteral("pub"),
        QStringLiteral("ref"), QStringLiteral("return"), QStringLiteral("self"), QStringLiteral("Self"),
        QStringLiteral("static"), QStringLiteral("struct"), QStringLiteral("super"), QStringLiteral("trait"),
        QStringLiteral("true"), QStringLiteral("type"), QStringLiteral("unsafe"), QStringLiteral("use"),
        QStringLiteral("where"), QStringLiteral("while"), QStringLiteral("bool"), QStringLiteral("char"),
        QStringLiteral("f32"), QStringLiteral("f64"), QStringLiteral("i8"), QStringLiteral("i16"),
        QStringLiteral("i32"), QStringLiteral("i64"), QStringLiteral("i128"), QStringLiteral("isize"),
        QStringLiteral("str"), QStringLiteral("u8"), QStringLiteral("u16"), QStringLiteral("u32"),
        QStringLiteral("u64"), QStringLiteral("u128"), QStringLiteral("usize"), QStringLiteral("Vec"),
        QStringLiteral("String"), QStringLiteral("Option"), QStringLiteral("Result"), QStringLiteral("Some"),
        QStringLiteral("None"), QStringLiteral("Ok"), QStringLiteral("Err"), QStringLiteral("Box"),
        QStringLiteral("println"), QStringLiteral("macro_rules")
      };
    case 7 : // PHP
      return { QStringLiteral("abstract"), QStringLiteral("and"), QStringLiteral("array"),
               QStringLiteral("as"), QStringLiteral("break"), QStringLiteral("callable"),
               QStringLiteral("case"), QStringLiteral("catch"), QStringLiteral("class"),
               QStringLiteral("clone"), QStringLiteral("const"), QStringLiteral("continue"),
               QStringLiteral("declare"), QStringLiteral("default"), QStringLiteral("die"),
               QStringLiteral("do"), QStringLiteral("echo"), QStringLiteral("else"),
               QStringLiteral("elseif"), QStringLiteral("empty"), QStringLiteral("enddeclare"),
               QStringLiteral("endfor"), QStringLiteral("endforeach"), QStringLiteral("endif"),
               QStringLiteral("endswitch"), QStringLiteral("endwhile"), QStringLiteral("eval"),
               QStringLiteral("exit"), QStringLiteral("extends"), QStringLiteral("false"),
               QStringLiteral("final"), QStringLiteral("finally"), QStringLiteral("fn"),
               QStringLiteral("for"), QStringLiteral("foreach"), QStringLiteral("function"),
               QStringLiteral("global"), QStringLiteral("goto"), QStringLiteral("if"),
               QStringLiteral("implements"), QStringLiteral("include"), QStringLiteral("include_once"),
               QStringLiteral("instanceof"), QStringLiteral("insteadof"), QStringLiteral("interface"),
               QStringLiteral("isset"), QStringLiteral("list"), QStringLiteral("match"),
               QStringLiteral("namespace"), QStringLiteral("new"), QStringLiteral("null"),
               QStringLiteral("or"), QStringLiteral("print"), QStringLiteral("private"),
               QStringLiteral("protected"), QStringLiteral("public"), QStringLiteral("readonly"),
               QStringLiteral("require"), QStringLiteral("require_once"), QStringLiteral("return"),
               QStringLiteral("static"), QStringLiteral("switch"), QStringLiteral("this"),
               QStringLiteral("throw"), QStringLiteral("trait"), QStringLiteral("true"),
               QStringLiteral("try"), QStringLiteral("unset"), QStringLiteral("use"),
               QStringLiteral("var"), QStringLiteral("while"), QStringLiteral("xor"),
               QStringLiteral("yield") };
    default : // CPP
      return { QStringLiteral("alignas"),
               QStringLiteral("alignof"),
               QStringLiteral("auto"),
               QStringLiteral("bool"),
               QStringLiteral("break"),
               QStringLiteral("case"),
               QStringLiteral("catch"),
               QStringLiteral("char"),
               QStringLiteral("char8_t"),
               QStringLiteral("char16_t"),
               QStringLiteral("char32_t"),
               QStringLiteral("class"),
               QStringLiteral("concept"),
               QStringLiteral("const"),
               QStringLiteral("consteval"),
               QStringLiteral("constexpr"),
               QStringLiteral("constinit"),
               QStringLiteral("const_cast"),
               QStringLiteral("continue"),
               QStringLiteral("co_await"),
               QStringLiteral("co_return"),
               QStringLiteral("co_yield"),
               QStringLiteral("decltype"),
               QStringLiteral("default"),
               QStringLiteral("delete"),
               QStringLiteral("do"),
               QStringLiteral("double"),
               QStringLiteral("dynamic_cast"),
               QStringLiteral("else"),
               QStringLiteral("emit"),
               QStringLiteral("enum"),
               QStringLiteral("explicit"),
               QStringLiteral("export"),
               QStringLiteral("extern"),
               QStringLiteral("false"),
               QStringLiteral("float"),
               QStringLiteral("for"),
               QStringLiteral("friend"),
               QStringLiteral("goto"),
               QStringLiteral("if"),
               QStringLiteral("inline"),
               QStringLiteral("int"),
               QStringLiteral("long"),
               QStringLiteral("mutable"),
               QStringLiteral("namespace"),
               QStringLiteral("new"),
               QStringLiteral("noexcept"),
               QStringLiteral("nullptr"),
               QStringLiteral("operator"),
               QStringLiteral("override"),
               QStringLiteral("private"),
               QStringLiteral("protected"),
               QStringLiteral("public"),
               QStringLiteral("register"),
               QStringLiteral("reinterpret_cast"),
               QStringLiteral("requires"),
               QStringLiteral("return"),
               QStringLiteral("short"),
               QStringLiteral("signed"),
               QStringLiteral("signal"),
               QStringLiteral("sizeof"),
               QStringLiteral("slot"),
               QStringLiteral("static"),
               QStringLiteral("static_assert"),
               QStringLiteral("static_cast"),
               QStringLiteral("struct"),
               QStringLiteral("switch"),
               QStringLiteral("template"),
               QStringLiteral("this"),
               QStringLiteral("thread_local"),
               QStringLiteral("throw"),
               QStringLiteral("true"),
               QStringLiteral("try"),
               QStringLiteral("typedef"),
               QStringLiteral("typeid"),
               QStringLiteral("typename"),
               QStringLiteral("union"),
               QStringLiteral("unsigned"),
               QStringLiteral("using"),
               QStringLiteral("virtual"),
               QStringLiteral("void"),
               QStringLiteral("volatile"),
               QStringLiteral("wchar_t"),
               QStringLiteral("while"),
               QStringLiteral("Q_OBJECT"),
               QStringLiteral("Q_PROPERTY"),
               QStringLiteral("Q_SIGNAL"),
               QStringLiteral("Q_SLOT"),
               QStringLiteral("Q_EMIT"),
               QStringLiteral("Q_INVOKABLE"),
               QStringLiteral("connect"),
               QStringLiteral("disconnect"),
               QStringLiteral("qDebug"),
               QStringLiteral("qWarning"),
               QStringLiteral("qCritical"),
               QStringLiteral("qFatal"),
               QStringLiteral("include"),
               QStringLiteral("define"),
               QStringLiteral("ifdef"),
               QStringLiteral("ifndef"),
               QStringLiteral("endif"),
               QStringLiteral("pragma") };
    }
  }

  QString _getSingleLineComment() const
  {
    switch (_language) {
    case 3  : return QStringLiteral("#");
    case 5  : return QStringLiteral("--");
    case 7  : return QStringLiteral("//|#");
    default : return QStringLiteral("//");
    }
  }

  bool _hasMultiLineComment() const { return _language != 3 && _language != 5; }

  bool _hasPreprocessor() const { return _language <= 1 || _language == 7; }

  void setupRules()
  {
    _rules.clear();

    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(NXThemeColor(_themeMode, PrimaryNormal));
    keywordFormat.setFontWeight(QFont::Bold);
    for (const QString &kw : _getKeywords()) {
      _rules.append({ QRegularExpression(QString(QStringLiteral("\\b%1\\b")).arg(QRegularExpression::escape(kw))), keywordFormat });
    }

    QTextCharFormat stringFormat;
    stringFormat.setForeground(QColor(0x0F, 0x7B, 0x0F));
    _rules.append({ QRegularExpression(QStringLiteral("\"[^\"]*\"")), stringFormat });
    _rules.append({ QRegularExpression(QStringLiteral("'[^']*'")), stringFormat });
    if (_language == 3 || _language == 4 || _language == 5) {
      _rules.append({ QRegularExpression(QStringLiteral("`[^`]*`")), stringFormat });
    }

    QTextCharFormat numberFormat;
    numberFormat.setForeground(QColor(0xF7, 0x93, 0x0E));
    _rules.append({ QRegularExpression(QStringLiteral("\\b[0-9]+\\.?[0-9]*[fFlLuU]?\\b")), numberFormat });
    _rules.append({ QRegularExpression(QStringLiteral("\\b0[xX][0-9a-fA-F]+\\b")), numberFormat });

    QTextCharFormat commentFormat;
    commentFormat.setForeground(NXThemeColor(_themeMode, BasicDetailsText));
    _rules.append({ QRegularExpression(_getSingleLineComment() + QStringLiteral("[^\n]*")), commentFormat });

    if (_hasPreprocessor()) {
      QTextCharFormat ppFormat;
      ppFormat.setForeground(NXThemeColor(_themeMode, PrimaryNormal));
      ppFormat.setFontItalic(true);
      _rules.append({ QRegularExpression(QStringLiteral("^\\s*#[^\n]*")), ppFormat });
    }

    if (_language == 3) {
      QTextCharFormat decoFormat;
      decoFormat.setForeground(NXThemeColor(_themeMode, PrimaryNormal));
      decoFormat.setFontItalic(true);
      _rules.append({ QRegularExpression(QStringLiteral("@\\w+")), decoFormat });
    }

    if (_language == 6) {
      QTextCharFormat macroFormat;
      macroFormat.setForeground(NXThemeColor(_themeMode, PrimaryNormal));
      macroFormat.setFontItalic(true);
      _rules.append({ QRegularExpression(QStringLiteral("\\b\\w+!")), macroFormat });
    }

    if (_language == 7) {
      QTextCharFormat varFormat;
      varFormat.setForeground(QColor(0xF7, 0x93, 0x0E));
      _rules.append({ QRegularExpression(QStringLiteral("\\$\\w+")), varFormat });
    }
  }

  QVector<HighlightRule> _rules;
  NXThemeType::ThemeMode _themeMode;
  int _language { 0 };
};

#endif // NXCODEHIGHLIGHTER_H
