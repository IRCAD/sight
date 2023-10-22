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

#include "CheckBox.hpp"

#include <QCheckBox>

namespace sight::ui::test_core::helper
{

//------------------------------------------------------------------------------

void CheckBox::toggle(Tester& _tester, const Select& _check_box)
{
    auto bt = _tester.addInBacktrace("toggling " + _check_box.getDescription(_tester) + "\" check box");
    _check_box.select(_tester);
    _tester.interact(std::make_unique<sight::ui::test_core::MouseClick>());
}

//------------------------------------------------------------------------------

void CheckBox::shouldBeChecked(Tester& _tester, const Select& _check_box)
{
    auto bt = _tester.addInBacktrace(
        "checking whether " + _check_box.getDescription(
            _tester
        ) + "\" check box is checked"
    );
    _check_box.select(_tester);
    _tester.doubt<QCheckBox*>(
        _check_box.getDescription(_tester) + " check box should be checked",
        [](QCheckBox* _obj)
        {
            return _obj->isChecked();
        });
}

//------------------------------------------------------------------------------

void CheckBox::shouldNotBeChecked(Tester& _tester, const Select& _check_box)
{
    auto bt =
        _tester.addInBacktrace("checking whether " + _check_box.getDescription(_tester) + "\" check box isn't checked");
    _check_box.select(_tester);
    _tester.doubt<QCheckBox*>(
        _check_box.getDescription(_tester) + " check box should be checked",
        [](QCheckBox* _obj)
        {
            return !_obj->isChecked();
        });
}

} // namespace sight::ui::test_core::helper
