/************************************************************************
 *
 * Copyright (C) 2004-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include <ui/qt/util/func_slot.hpp>

namespace sight::ui::qt::util
{

//------------------------------------------------------------------------------

void do_nothing()
{
}

func_slot::func_slot() :
    m_func(do_nothing)
{
}

//------------------------------------------------------------------------------

void func_slot::trigger()
{
    m_func();
}

} // namespace sight::ui::qt::util
