/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include "core/logic_stamp.hpp"

#include "core/mt/types.hpp"

namespace sight::core
{

//------------------------------------------------------------------------------
static logic_stamp::logic_stamp_type global_logic_stamp = 0;
static core::mt::mutex global_logic_stamp_mutex;

//------------------------------------------------------------------------------

void logic_stamp::modified()
{
    core::mt::scoped_lock lock(global_logic_stamp_mutex);

    m_modified_logical_time = ++global_logic_stamp;

    SIGHT_ASSERT("globalLogicStamp not instanced", global_logic_stamp); // detects overflow
}

} //namespace sight::core
