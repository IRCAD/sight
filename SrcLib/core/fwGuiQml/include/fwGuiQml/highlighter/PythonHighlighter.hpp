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

#pragma once

#include "fwGuiQml/config.hpp"

#include <QHash>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>

class QTextDocument;

namespace fwGuiQml
{
namespace highlighter
{

class FWGUIQML_CLASS_API PythonHighlighter : public QSyntaxHighlighter
{
Q_OBJECT

public:
    FWGUIQML_API PythonHighlighter(QTextDocument* parent = 0);

protected:

    FWGUIQML_API void highlightBlock(const QString& text);

private:

    class HighlightingRule
    {
    public:
        HighlightingRule() :
            nth(0)
        {
        }
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
