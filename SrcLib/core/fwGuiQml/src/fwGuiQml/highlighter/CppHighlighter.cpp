/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "fwGuiQml/highlighter/CppHighlighter.hpp"

#include <QtGui>

namespace fwGuiQt
{
namespace highlighter
{

CppHighlighter::CppHighlighter(QTextDocument* parent) :
    QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns << "char" << "class" << "const"
                    << "double" << "enum" << "explicit"
                    << "friend" << "inline" << "int"
                    << "long" << "namespace" << "operator"
                    << "private" << "protected" << "public"
                    << "short" << "signals" << "signed"
                    << "slots" << "static" << "struct"
                    << "template" << "typedef" << "typename"
                    << "union" << "unsigned" << "virtual"
                    << "void" << "volatile";
    for(const QString& pattern :  keywordPatterns)
    {
        rule.pattern = QRegExp("\\b" + pattern + "\\b");
        rule.format  = keywordFormat;
        highlightingRules.append(rule);
    }

    QTextCharFormat classFormat;
    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
    rule.format  = classFormat;
    highlightingRules.append(rule);

    QTextCharFormat singleLineCommentFormat;
    singleLineCommentFormat.setForeground(Qt::red);
    rule.pattern = QRegExp("//[^\n]*");
    rule.format  = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::red);

    QTextCharFormat quotationFormat;
    quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("\".*\"");
    rule.format  = quotationFormat;
    highlightingRules.append(rule);

    QTextCharFormat functionFormat;
    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::blue);
    rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format  = functionFormat;
    highlightingRules.append(rule);

    commentStartExpression = QRegExp("/\\*");
    commentEndExpression   = QRegExp("\\*/");
}

//------------------------------------------------------------------------------

void CppHighlighter::highlightBlock(const QString& text)
{
    for(const HighlightingRule& rule :  highlightingRules)
    {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0)
        {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
    {
        startIndex = commentStartExpression.indexIn(text);
    }

    while (startIndex >= 0)
    {
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1)
        {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        }
        else
        {
            commentLength = endIndex - startIndex
                            + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }
}

//------------------------------------------------------------------------------

} //namespace fwGuiQml
} //namespace highlighter
