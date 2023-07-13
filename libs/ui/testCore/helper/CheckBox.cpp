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

namespace sight::ui::testCore::helper
{

//------------------------------------------------------------------------------

void CheckBox::toggle(Tester& tester, const Select& checkBox)
{
    auto bt = tester.addInBacktrace("toggling " + checkBox.getDescription(tester) + "\" check box");
    checkBox.select(tester);
    tester.interact(std::make_unique<sight::ui::testCore::MouseClick>());
}

//------------------------------------------------------------------------------

void CheckBox::shouldBeChecked(Tester& tester, const Select& checkBox)
{
    auto bt = tester.addInBacktrace("checking whether " + checkBox.getDescription(tester) + "\" check box is checked");
    checkBox.select(tester);
    tester.doubt<QCheckBox*>(
        checkBox.getDescription(tester) + " check box should be checked",
        [](QCheckBox* obj)
        {
            return obj->isChecked();
        });
}

//------------------------------------------------------------------------------

void CheckBox::shouldNotBeChecked(Tester& tester, const Select& checkBox)
{
    auto bt =
        tester.addInBacktrace("checking whether " + checkBox.getDescription(tester) + "\" check box isn't checked");
    checkBox.select(tester);
    tester.doubt<QCheckBox*>(
        checkBox.getDescription(tester) + " check box should be checked",
        [](QCheckBox* obj)
        {
            return !obj->isChecked();
        });
}

} // namespace sight::ui::testCore::helper
