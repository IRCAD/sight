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

namespace sight::ui::test_core::helper
{

//------------------------------------------------------------------------------

void Label::toolTipMatches(Tester& _tester, const Select& _label, const std::string& _expected)
{
    auto bt = _tester.addInBacktrace(
        "checking if \"" + _label.getDescription(_tester) + "\" label tooltip is equal to \"" + _expected + '"'
    );
    _label.select(_tester);
    _tester.doubt<QLabel*>(
        '"' + _label.getDescription(_tester) + "\" label tooltip should be equal to \"" + _expected + '"',
        [&_expected](QLabel* _obj){return _obj->toolTip() == QString::fromStdString(_expected);});
}

//------------------------------------------------------------------------------

void Label::contain(Tester& _tester, const Select& _label, const std::string& _expected)
{
    auto bt = _tester.addInBacktrace(
        "checking if \"" + _label.getDescription(_tester) + "\" label contains \"" + _expected + '"'
    );
    _label.select(_tester);
    _tester.doubt<QLabel*>(
        '"' + _label.getDescription(_tester) + "\" label should contain \"" + _expected + '"',
        [&_expected](QLabel* _obj)
        {
            return _obj->text().contains(QString::fromStdString(_expected));
        });
}

//------------------------------------------------------------------------------

void Label::exactlyMatch(Tester& _tester, const Select& _label, const std::string& _expected)
{
    auto bt = _tester.addInBacktrace(
        "checking if \"" + _label.getDescription(_tester) + "\" label is equal to \"" + _expected + '"'
    );
    _label.select(_tester);
    _tester.doubt<QLabel*>(
        '"' + _label.getDescription(_tester) + "\" label should be equal to \"" + _expected + '"',
        [&_expected](QLabel* _obj)
        {
            return _obj->text() == QString::fromStdString(_expected);
        });
}

//------------------------------------------------------------------------------

void Label::equal(Tester& _tester, const Select& _label, double _expected, double _tolerance, QRegExp _re)
{
    auto bt = _tester.addInBacktrace(
        "checking if \"" + _label.getDescription(_tester) + "\" label is equal to "
        + std::to_string(_expected) + " (with " + std::to_string(_tolerance) + " tolerance)"
    );
    _label.select(_tester);
    _tester.doubt<QLabel*>(
        '"' + _label.getDescription(_tester) + "\" label should be equal to " + std::to_string(_expected),
        [&_expected, &_tolerance, &_re](QLabel* _obj)
        {
            _obj->text().indexOf(_re);
            return std::abs(_re.cap(1).toDouble() - _expected) <= _tolerance;
        });
}

//------------------------------------------------------------------------------

std::string Label::get(Tester& _tester, const Select& _label)
{
    auto bt = _tester.addInBacktrace("getting the text within " + _label.getDescription(_tester) + " label");
    _label.select(_tester);
    std::string res;
    _tester.doSomething<QLabel*>([&res](QLabel* _l){res = _l->text().toStdString();});
    return res;
}

} // namespace sight::ui::test_core::helper
