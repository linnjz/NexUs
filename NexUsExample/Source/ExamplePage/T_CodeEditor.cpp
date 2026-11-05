#include "T_CodeEditor.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "NXCodeEditor.h"
#include "NXComboBox.h"
#include "NXScrollPageArea.h"
#include "NXText.h"

static QString
getCppDemo()
{
  return QStringLiteral("#include <QApplication>\n"
                        "#include \"NXWindow.h\"\n"
                        "\n"
                        "// NexUs C++ example\n"
                        "int main(int argc, char *argv[])\n"
                        "{\n"
                        "    QApplication app(argc, argv);\n"
                        "    NXWindow window;\n"
                        "    window.setWindowTitle(\"Hello\");\n"
                        "    window.resize(1200, 740);\n"
                        "    window.show();\n"
                        "    return app.exec();\n"
                        "}\n");
}

static QString
getCDemo()
{
  return QStringLiteral("#include <stdio.h>\n"
                        "#include <stdlib.h>\n"
                        "\n"
                        "/* Classic C example */\n"
                        "int main(int argc, char *argv[])\n"
                        "{\n"
                        "    int count = 10;\n"
                        "    for (int i = 0; i < count; i++)\n"
                        "    {\n"
                        "        printf(\"Hello %d\\n\", i);\n"
                        "    }\n"
                        "    return 0;\n"
                        "}\n");
}

static QString
getCSharpDemo()
{
  return QStringLiteral("using System;\n"
                        "using System.Linq;\n"
                        "\n"
                        "namespace NXDemo\n"
                        "{\n"
                        "    // C# example\n"
                        "    public class Program\n"
                        "    {\n"
                        "        static async Task Main(string[] args)\n"
                        "        {\n"
                        "            var items = new List<int> { 1, 2, 3 };\n"
                        "            foreach (var item in items)\n"
                        "            {\n"
                        "                Console.WriteLine($\"Item: {item}\");\n"
                        "            }\n"
                        "            await Task.Delay(1000);\n"
                        "        }\n"
                        "    }\n"
                        "}\n");
}

static QString
getPythonDemo()
{
  return QStringLiteral("import os\n"
                        "from dataclasses import dataclass\n"
                        "\n"
                        "@dataclass\n"
                        "class Widget:\n"
                        "    name: str\n"
                        "    version: float = 2.0\n"
                        "\n"
                        "# Python example\n"
                        "def main():\n"
                        "    widgets = [Widget(\"Button\"), Widget(\"Slider\", 1.5)]\n"
                        "    for w in widgets:\n"
                        "        print(f\"Widget: {w.name} v{w.version}\")\n"
                        "\n"
                        "    count = len(widgets)\n"
                        "    if count > 0:\n"
                        "        print(f\"Total: {count} widgets\")\n"
                        "\n"
                        "if __name__ == \"__main__\":\n"
                        "    main()\n");
}

static QString
getJavaScriptDemo()
{
  return QStringLiteral("import { createApp } from 'vue';\n"
                        "\n"
                        "// JavaScript example\n"
                        "class WidgetManager {\n"
                        "    constructor() {\n"
                        "        this.widgets = [];\n"
                        "    }\n"
                        "\n"
                        "    async loadWidgets() {\n"
                        "        const response = await fetch('/api/widgets');\n"
                        "        this.widgets = await response.json();\n"
                        "        console.log(`Loaded ${this.widgets.length} widgets`);\n"
                        "    }\n"
                        "}\n"
                        "\n"
                        "const manager = new WidgetManager();\n"
                        "manager.loadWidgets();\n");
}

static QString
getLuaDemo()
{
  return QStringLiteral("-- Lua example\n"
                        "local Widget = {}\n"
                        "Widget.__index = Widget\n"
                        "\n"
                        "function Widget.new(name, version)\n"
                        "    local self = setmetatable({}, Widget)\n"
                        "    self.name = name or \"Unknown\"\n"
                        "    self.version = version or 1.0\n"
                        "    return self\n"
                        "end\n"
                        "\n"
                        "function Widget:display()\n"
                        "    print(string.format(\"%s v%.1f\", self.name, self.version))\n"
                        "end\n"
                        "\n"
                        "local widgets = {\n"
                        "    Widget.new(\"Button\", 2.0),\n"
                        "    Widget.new(\"Slider\", 1.5),\n"
                        "}\n"
                        "\n"
                        "for i, w in ipairs(widgets) do\n"
                        "    w:display()\n"
                        "end\n");
}

static QString
getRustDemo()
{
  return QStringLiteral("use std::collections::HashMap;\n"
                        "\n"
                        "/// Rust example\n"
                        "struct Widget {\n"
                        "    name: String,\n"
                        "    version: f64,\n"
                        "}\n"
                        "\n"
                        "impl Widget {\n"
                        "    fn new(name: &str, version: f64) -> Self {\n"
                        "        Widget {\n"
                        "            name: name.to_string(),\n"
                        "            version,\n"
                        "        }\n"
                        "    }\n"
                        "}\n"
                        "\n"
                        "fn main() {\n"
                        "    let widgets = vec![\n"
                        "        Widget::new(\"Button\", 2.0),\n"
                        "        Widget::new(\"Slider\", 1.5),\n"
                        "    ];\n"
                        "    for w in &widgets {\n"
                        "        println!(\"{} v{}\", w.name, w.version);\n"
                        "    }\n"
                        "}\n");
}

static QString
getPhpDemo()
{
  return QStringLiteral("<?php\n"
                        "declare(strict_types=1);\n"
                        "\n"
                        "// PHP example\n"
                        "class Widget\n"
                        "{\n"
                        "    public function __construct(\n"
                        "        private string $name, \n"
                        "        private float $version = 1.0\n"
                        ") {}\n"
                        "\n"
                        "    public function display(): void\n"
                        "    {\n"
                        "        echo \"{$this->name} v{$this->version}\\n\";\n"
                        "    }\n"
                        "}\n"
                        "\n"
                        "$widgets = [\n"
                        "    new Widget(\"Button\", 2.0),\n"
                        "    new Widget(\"Slider\", 1.5),\n"
                        "];\n"
                        "\n"
                        "foreach ($widgets as $w) {\n"
                        "    $w->display();\n"
                        "}\n");
}

T_CodeEditor::T_CodeEditor(QWidget *parent)
    : T_BasePage(parent)
{
  setWindowTitle(QStringLiteral("NXCodeEditor"));
  createCustomWidget(QStringLiteral("代码编辑器，支持行号显示和多语言语法高亮"));

  QWidget *centralWidget = new QWidget(this);
  centralWidget->setWindowTitle(QStringLiteral("NXCodeEditor"));

  // Language selector
  NXComboBox *langCombo = new NXComboBox(this);
  langCombo->addItems({ QStringLiteral("C++"), QStringLiteral("C"), QStringLiteral("C#"), QStringLiteral("Python"), QStringLiteral("JavaScript"), QStringLiteral("Lua"), QStringLiteral("Rust"), QStringLiteral("PHP") });

  NXScrollPageArea *langArea = new NXScrollPageArea(this);
  QHBoxLayout *langLayout    = new QHBoxLayout(langArea);
  langLayout->addWidget(new NXText(QStringLiteral("语言"), 15, this));
  langLayout->addWidget(langCombo);
  langLayout->addStretch();

  // Code editor
  _codeEditor = new NXCodeEditor(this);
  _codeEditor->setIsReadOnly(true);
  _codeEditor->setCode(getCppDemo());

  connect(langCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index) {
    _codeEditor->setLanguage(static_cast<NXCodeEditor::Language>(index));
    switch (index) {
    case 0 : _codeEditor->setCode(getCppDemo()); break;
    case 1 : _codeEditor->setCode(getCDemo()); break;
    case 2 : _codeEditor->setCode(getCSharpDemo()); break;
    case 3 : _codeEditor->setCode(getPythonDemo()); break;
    case 4 : _codeEditor->setCode(getJavaScriptDemo()); break;
    case 5 : _codeEditor->setCode(getLuaDemo()); break;
    case 6 : _codeEditor->setCode(getRustDemo()); break;
    case 7 : _codeEditor->setCode(getPhpDemo()); break;
    }
  });

  QVBoxLayout *centerVLayout = new QVBoxLayout(centralWidget);
  centerVLayout->setContentsMargins(0, 0, 0, 0);
  centerVLayout->addWidget(langArea);
  centerVLayout->addWidget(_codeEditor);
  addCentralWidget(centralWidget, true, false, 0);
}

T_CodeEditor::~T_CodeEditor()
{
}
