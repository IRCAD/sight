/************************************************************************
 *
 * Copyright (C) 2025-2026 IRCAD France
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

void reset::starting()
{
}

//-----------------------------------------------------------------------------

void reset::updating()
{
    this->reset_targets();
}

//-----------------------------------------------------------------------------

void reset::reset_targets()
{
    const auto num_targets = m_targets.size();

    // Reset each target object
    for(size_t i = 0 ; i < num_targets ; ++i)
    {
        const auto target = m_targets[i].lock();

        if(target)
        {
            const auto empty_object = sight::data::factory::make(target->get_classname());
            target->deep_copy(empty_object);
            target->async_emit(this, sight::data::signals::MODIFIED);
        }
    }
}

//-----------------------------------------------------------------------------

void reset::stopping()
{
}

//-----------------------------------------------------------------------------

} // namespace sight::module::data.
