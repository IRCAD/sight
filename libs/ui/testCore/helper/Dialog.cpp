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

namespace sight::ui::testCore::helper
{

//------------------------------------------------------------------------------

void Dialog::take(Tester& tester, const std::string& desc)
{
    auto bt = tester.addInBacktrace("take " + desc + " dialog");
    take<QWidget*>(tester, desc);
}

//------------------------------------------------------------------------------

void Dialog::maybeTake(Tester& tester, const std::string& desc)
{
    maybeTake<QWidget*>(tester, desc);
}

} // namespace sight::ui::testCore::helper
