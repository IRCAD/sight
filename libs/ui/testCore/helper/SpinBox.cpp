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

#include "SpinBox.hpp"

#include <QDoubleSpinBox>
#include <QSpinBox>

namespace sight::ui::test_core::helper
{

//------------------------------------------------------------------------------

static void change(Tester& _tester, const QPoint& _pos)
{
    _tester.interact(std::make_unique<MouseClick>(Qt::MouseButton::LeftButton, Qt::NoModifier, _pos));
}

//------------------------------------------------------------------------------

void SpinBox::increment(Tester& _tester, const Select& _spin_box, int _times)
{
    auto bt = _tester.addInBacktrace(
        "incrementing \"" + _spin_box.getDescription(_tester) + "\" spin box " + std::to_string(_times) + "times"
    );
    _spin_box.select(_tester);
    auto* widget = _tester.get<QWidget*>();
    for(int i = 0 ; i < _times ; i++)
    {
        change(_tester, Tester::rightOf(widget));
    }
}

//------------------------------------------------------------------------------

void SpinBox::decrement(Tester& _tester, const Select& _spin_box, int _times)
{
    auto bt = _tester.addInBacktrace(
        "decrementing \"" + _spin_box.getDescription(_tester) + "\" spin box " + std::to_string(_times) + "times"
    );
    _spin_box.select(_tester);
    auto* widget = _tester.get<QWidget*>();
    for(int i = 0 ; i < _times ; i++)
    {
        change(_tester, Tester::leftOf(widget));
    }
}

//------------------------------------------------------------------------------

void SpinBox::valueEquals(Tester& _tester, const Select& _spin_box, const std::string& _expected)
{
    auto bt = _tester.addInBacktrace(
        "checking whether \"" + _spin_box.getDescription(_tester) + "\" spin box has the value " + _expected
    );
    _spin_box.select(_tester);
    QString q_expected = QString::fromStdString(_expected);
    _tester.doubt(
        _spin_box.getDescription(_tester) + " should have the value " + _expected,
        [&q_expected](QObject* _obj)
        {
            if(auto* double_spin_box = qobject_cast<QDoubleSpinBox*>(_obj))
            {
                return double_spin_box->cleanText() == q_expected;
            }

            if(auto* int_spin_box = qobject_cast<QSpinBox*>(_obj))
            {
                return int_spin_box->cleanText() == q_expected;
            }

            return false;
        });
}

//------------------------------------------------------------------------------

void SpinBox::valueEquals(Tester& _tester, const Select& _spin_box, int _expected)
{
    auto bt = _tester.addInBacktrace(
        "checking whether \"" + _spin_box.getDescription(_tester) + "\" spin box has the value "
        + std::to_string(_expected)
    );
    _spin_box.select(_tester);
    _tester.doubt(
        _spin_box.getDescription(_tester) + " should have the value " + std::to_string(_expected),
        [&_expected](QObject* _obj)
        {
            if(auto* double_spin_box = qobject_cast<QDoubleSpinBox*>(_obj))
            {
                return double_spin_box->value() == _expected;
            }

            if(auto* int_spin_box = qobject_cast<QSpinBox*>(_obj))
            {
                return int_spin_box->value() == _expected;
            }

            return false;
        });
}

} // namespace sight::ui::test_core::helper
