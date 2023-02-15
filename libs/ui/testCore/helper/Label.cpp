/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "Label.hpp"

#include <QLabel>

namespace sight::ui::testCore::helper
{

//------------------------------------------------------------------------------

void Label::toolTipMatches(Tester& tester, const Select& label, const std::string& expected)
{
    auto bt = tester.addInBacktrace(
        "checking if \"" + label.getDescription(tester) + "\" label tooltip is equal to \"" + expected + '"'
    );
    label.select(tester);
    tester.doubt<QLabel*>(
        '"' + label.getDescription(tester) + "\" label tooltip should be equal to \"" + expected + '"',
        [&expected](QLabel* obj){return obj->toolTip() == QString::fromStdString(expected);});
}

//------------------------------------------------------------------------------

void Label::contain(Tester& tester, const Select& label, const std::string& expected)
{
    auto bt = tester.addInBacktrace(
        "checking if \"" + label.getDescription(tester) + "\" label contains \"" + expected + '"'
    );
    label.select(tester);
    tester.doubt<QLabel*>(
        '"' + label.getDescription(tester) + "\" label should contain \"" + expected + '"',
        [&expected](QLabel* obj)
        {
            return obj->text().contains(QString::fromStdString(expected));
        });
}

//------------------------------------------------------------------------------

void Label::exactlyMatch(Tester& tester, const Select& label, const std::string& expected)
{
    auto bt = tester.addInBacktrace(
        "checking if \"" + label.getDescription(tester) + "\" label is equal to \"" + expected + '"'
    );
    label.select(tester);
    tester.doubt<QLabel*>(
        '"' + label.getDescription(tester) + "\" label should be equal to \"" + expected + '"',
        [&expected](QLabel* obj)
        {
            return obj->text() == QString::fromStdString(expected);
        });
}

//------------------------------------------------------------------------------

void Label::equal(Tester& tester, const Select& label, double expected, double tolerance, QRegExp re)
{
    auto bt = tester.addInBacktrace(
        "checking if \"" + label.getDescription(tester) + "\" label is equal to "
        + std::to_string(expected) + " (with " + std::to_string(tolerance) + " tolerance)"
    );
    label.select(tester);
    tester.doubt<QLabel*>(
        '"' + label.getDescription(tester) + "\" label should be equal to " + std::to_string(expected),
        [&expected, &tolerance, &re](QLabel* obj)
        {
            obj->text().indexOf(re);
            return std::abs(re.cap(1).toDouble() - expected) <= tolerance;
        });
}

//------------------------------------------------------------------------------

std::string Label::get(Tester& tester, const Select& label)
{
    auto bt = tester.addInBacktrace("getting the text within " + label.getDescription(tester) + " label");
    label.select(tester);
    std::string res;
    tester.doSomething<QLabel*>([&res](QLabel* l){res = l->text().toStdString();});
    return res;
}

} // namespace sight::ui::testCore::helper
