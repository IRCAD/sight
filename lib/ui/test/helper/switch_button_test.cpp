/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include "switch_button_test.hpp"

#include <QAbstractButton>

namespace sight::ui::test::helper
{

//------------------------------------------------------------------------------

void switch_button_test::toggle(tester& _tester, const selector& _switch_button)
{
    auto bt = _tester.add_in_backtrace("toggling " + _switch_button.get_description(_tester) + "\" switch_button");
    _switch_button.select(_tester);
    _tester.interact(std::make_unique<sight::ui::test::mouse_click>());
}

//------------------------------------------------------------------------------

void switch_button_test::should_be_checked(tester& _tester, const selector& _switch_button)
{
    auto bt = _tester.add_in_backtrace(
        "checking whether " + _switch_button.get_description(
            _tester
        ) + "\" switch_button is checked"
    );
    _switch_button.select(_tester);
    _tester.doubt<QAbstractButton*>(
        _switch_button.get_description(_tester) + " switch_button should be checked",
        [](QAbstractButton* _obj)
        {
            return _obj->isChecked();
        });
}

//------------------------------------------------------------------------------

void switch_button_test::should_not_be_checked(tester& _tester, const selector& _switch_button)
{
    auto bt =
        _tester.add_in_backtrace(
            "checking whether " + _switch_button.get_description(
                _tester
            ) + "\" switch_button isn't checked"
        );
    _switch_button.select(_tester);
    _tester.doubt<QAbstractButton*>(
        _switch_button.get_description(_tester) + " switch_button should be checked",
        [](QAbstractButton* _obj)
        {
            return !_obj->isChecked();
        });
}

} // namespace sight::ui::test::helper
