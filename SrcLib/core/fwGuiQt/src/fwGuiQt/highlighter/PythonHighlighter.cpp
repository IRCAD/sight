/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>
#include <QtGui>

#include "fwGuiQt/highlighter/PythonHighlighter.hpp"

namespace fwGuiQt
{
namespace highlighter
{

PythonHighlighter::PythonHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{
    // http://diotavelli.net/PyQtWiki/Python%20syntax%20highlighting
    HighlightingRule rule;

    // Python keywords
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns << "and" << "assert" << "break" << "class" << "continue" << "def"
                    << "del" << "elif" << "else" << "except" << "exec" << "finally"
                    << "for" << "from" << "global" << "if" << "import" << "in"
                    << "is" << "lambda" << "not" << "or" << "pass" << "print"
                    << "raise" << "return" << "try" << "while" << "yield"
                    << "None" << "True" << "False";
    rule.nth = 0;
    BOOST_FOREACH(const QString &pattern, keywordPatterns)
    {
        rule.pattern = QRegExp( "\\b" + pattern + "\\b");
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    // Python operators
    QTextCharFormat operatorsFormat;
    operatorsFormat.setForeground(Qt::red);
    QStringList operatorsPatterns;
    operatorsPatterns << "="
                    // Comparison
                    << "==" << "!=" << "<" << "<=" << ">" << ">="
                    // Arithmetic
                    << "\\+" << "-" << "\\*" << "/" << "//" << "\\%" << "\\*\\*"
                    // In-place
                    << "\\+=" << "-=" << "\\*=" << "/=" << "\\%="
                    // Bitwise
                    << "\\^" << "\\|" << "\\&" << "\\~" << ">>" << "<<";
    rule.nth = 0;
    BOOST_FOREACH(const QString &pattern, operatorsPatterns)
    {
        rule.pattern = QRegExp( pattern );
        rule.format = operatorsFormat;
        highlightingRules.append(rule);
    }

    // Python braces
    QTextCharFormat bracesFormat;
    bracesFormat.setForeground(Qt::darkGray);
    QStringList bracesPatterns;
    bracesPatterns << "\\{" << "\\}" << "\\(" << "\\)" << "\\[" << "\\]";
    rule.nth = 0;
    BOOST_FOREACH(const QString &pattern, bracesPatterns)
    {
        rule.pattern = QRegExp( pattern );
        rule.format = bracesFormat;
        highlightingRules.append(rule);
    }

    // 'class' followed by an identifier
    QTextCharFormat classFormat;
    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegExp("\\bclass\\b\\s*(\\w+)");
    rule.format = classFormat;
    rule.nth = 1;
    highlightingRules.append(rule);

    // 'def' followed by an identifier
    QTextCharFormat defFormat;
    defFormat.setFontWeight(QFont::Bold);
    defFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegExp("\\bdef\\b\\s*(\\w+)");
    rule.format = defFormat;
    rule.nth = 1;
    highlightingRules.append(rule);

    // Python self
    QTextCharFormat selfFormat;
    selfFormat.setFontItalic(true);
    selfFormat.setForeground(Qt::black);
    rule.pattern = QRegExp("\\bself\\b");
    rule.format = selfFormat;
    rule.nth = 0;
    highlightingRules.append(rule);

    // Double-quoted string, possibly containing escape sequences : "[^"\\]*(\\.[^"\\]*)*"
    QTextCharFormat stringFormat;
    stringFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegExp("\".*\"");
    rule.format = stringFormat;
    rule.nth = 0;
    highlightingRules.append(rule);
    // Single-quoted string, possibly containing escape sequences : '[^'\\]*(\\.[^'\\]*)*'
    rule.pattern = QRegExp("'.*'");
    highlightingRules.append(rule);

    // Python comment (single line)
    QTextCharFormat singleLineCommentFormat;
    singleLineCommentFormat.setFontItalic(true);
    singleLineCommentFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("#[^\\n]*");
    rule.format = singleLineCommentFormat;
    rule.nth = 0;
    highlightingRules.append(rule);

    // Numeric literals
    QTextCharFormat numericFormat;
    QColor col;
    col.setNamedColor("brown");
    numericFormat.setForeground(col);
    rule.format = numericFormat;
    rule.nth = 0;
    rule.pattern = QRegExp("\\b[+-]?[0-9]+[lL]?\\b");
    highlightingRules.append(rule);
    rule.pattern = QRegExp("\\b[+-]?0[xX][0-9A-Fa-f]+[lL]?\\b");
    highlightingRules.append(rule);
    rule.pattern = QRegExp("\\b[+-]?[0-9]+(?:\\.[0-9]+)?(?:[eE][+-]?[0-9]+)?\\b");
    highlightingRules.append(rule);
}

//------------------------------------------------------------------------------

void PythonHighlighter::highlightBlock(const QString &text)
{
    BOOST_FOREACH(const HighlightingRule &rule, highlightingRules)
    {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text, 0);
        while (index >= 0)
        {
            index = expression.pos(rule.nth);
            int length = expression.cap(rule.nth).length();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
    setCurrentBlockState(0);
}

//------------------------------------------------------------------------------

} //namespace fwGuiQt
} //namespace highlighter
