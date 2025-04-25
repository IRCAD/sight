/************************************************************************
 *
 * Copyright (C) 2023-2025 IRCAD France
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

#include "label.hpp"

#include <QLabel>

namespace sight::ui::test::helper
{

//------------------------------------------------------------------------------

void label::tool_tip_matches(tester& _tester, const selector& _label, const std::string& _expected)
{
    auto bt = _tester.add_in_backtrace(
        "checking if '" + _label.get_description(_tester) + "' label tooltip is equal to '" + _expected + "'"
    );
    _label.select(_tester);
    _tester.doubt<QLabel*>(
        "'" + _label.get_description(_tester) + "' label tooltip should be equal to '" + _expected + "'",
        [&_expected](QLabel* _obj){return _obj->toolTip() == QString::fromStdString(_expected);});
}

//------------------------------------------------------------------------------

void label::contain(tester& _tester, const selector& _label, const std::string& _expected)
{
    auto bt = _tester.add_in_backtrace(
        "checking if '" + _label.get_description(_tester) + "' label contains '" + _expected + "'"
    );
    _label.select(_tester);
    _tester.doubt<QLabel*>(
        "'" + _label.get_description(_tester) + "' label should contain '" + _expected + "'",
        [&_expected](QLabel* _obj)
        {
            return _obj->text().contains(QString::fromStdString(_expected));
        });
}

//------------------------------------------------------------------------------

void label::exactly_match(tester& _tester, const selector& _label, const std::string& _expected)
{
    auto bt = _tester.add_in_backtrace(
        "checking if '" + _label.get_description(_tester) + "' label is equal to '" + _expected + "'"
    );
    _label.select(_tester);
    _tester.doubt<QLabel*>(
        "'" + _label.get_description(_tester) + "' label should be equal to '" + _expected
        + "' (current: " + _tester.get<QLabel*>()->text().toStdString() + ")",
        [&_expected](QLabel* _obj)
        {
            return _obj->text() == QString::fromStdString(_expected);
        });
}

//------------------------------------------------------------------------------

void label::equal(tester& _tester, const selector& _label, double _expected, double _tolerance, QRegularExpression _re)
{
    auto bt = _tester.add_in_backtrace(
        "checking if '" + _label.get_description(_tester) + "' label is equal to "
        + std::to_string(_expected) + " (with " + std::to_string(_tolerance) + " tolerance)"
    );

    _label.select(_tester);

    const auto match  = _re.match(_tester.get<QLabel*>()->text());
    const auto actual = match.captured(1);

    _tester.doubt<QLabel*>(
        "'" + _label.get_description(_tester) + "' label should be equal to '" + std::to_string(_expected)
        + "' (current: " + actual.toStdString() + ")",
        [&_expected, &_tolerance, &actual](QLabel* /*_obj*/)
        {
            return !actual.isEmpty() && std::abs(actual.toDouble() - _expected) <= _tolerance;
        });
}

//------------------------------------------------------------------------------

std::string label::get(tester& _tester, const selector& _label)
{
    auto bt = _tester.add_in_backtrace("getting the text within " + _label.get_description(_tester) + " label");
    _label.select(_tester);
    std::string res;
    _tester.do_something<QLabel*>([&res](QLabel* _l){res = _l->text().toStdString();});
    return res;
}

} // namespace sight::ui::test::helper
