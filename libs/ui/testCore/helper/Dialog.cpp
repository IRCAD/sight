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

#include "Dialog.hpp"

namespace sight::ui::test_core::helper
{

//------------------------------------------------------------------------------

void Dialog::take(Tester& _tester, const std::string& _desc, const std::string& _child_name)
{
    auto bt = _tester.addInBacktrace("take " + _desc + " dialog");
    take<QWidget*>(_tester, _desc, _child_name);
}

//------------------------------------------------------------------------------

void Dialog::maybeTake(Tester& _tester, const std::string& _desc, const std::string& _child_name)
{
    maybeTake<QWidget*>(_tester, _desc, _child_name);
}

} // namespace sight::ui::test_core::helper
