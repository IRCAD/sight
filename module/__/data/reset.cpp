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

#include "reset.hpp"

namespace sight::module::data
{

//-----------------------------------------------------------------------------

void reset::configuring(const config_t& /*_config*/)
{
}

//-----------------------------------------------------------------------------

void reset::starting()
{
}

//-----------------------------------------------------------------------------

void reset::updating()
{
    const auto target = m_target.lock();
    SIGHT_ASSERT("Object empty, can't reset it.", target);

    const auto empty_object = sight::data::factory::make(target->get_classname());
    target->deep_copy(empty_object);
    target->async_emit(sight::data::object::MODIFIED_SIG);
}

//-----------------------------------------------------------------------------

void reset::stopping()
{
}

//-----------------------------------------------------------------------------

} // namespace sight::module::data.
