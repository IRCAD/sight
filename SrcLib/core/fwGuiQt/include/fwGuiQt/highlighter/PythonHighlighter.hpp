/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIQT_HIGHLIGHTER_PYTHONHIGHLIGHTER_HPP
#define _FWGUIQT_HIGHLIGHTER_PYTHONHIGHLIGHTER_HPP

#include <QSyntaxHighlighter>

#include <QHash>
#include <QTextCharFormat>

#include "fwGuiQt/config.hpp"

class QTextDocument;

namespace fwGuiQt
{
namespace highlighter
{

class FWGUIQT_CLASS_API PythonHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    FWGUIQT_API PythonHighlighter(QTextDocument *parent = 0);

protected:

    FWGUIQT_API void highlightBlock(const QString &text);

private:

    class HighlightingRule
    {
    public:
        HighlightingRule() : nth(0)
        {}
        int nth;
        QRegExp pattern;
        QTextCharFormat format;
    };

    QVector<HighlightingRule> highlightingRules;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;
    QTextCharFormat multiLineCommentFormat;

};

} //namespace fwGuiQt
} //namespace highlighter

#endif // _FWGUIQT_HIGHLIGHTER_PYTHONHIGHLIGHTER_HPP
