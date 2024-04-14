/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "lock_dump.hpp"

#include <core/memory/buffered.hpp>

#include <service/macros.hpp>

namespace sight::module::memory
{

//-----------------------------------------------------------------------------

lock_dump::lock_dump() noexcept =
    default;

//-----------------------------------------------------------------------------

lock_dump::~lock_dump() noexcept =
    default;

//-----------------------------------------------------------------------------

void lock_dump::starting()
{
    const auto object = m_target.lock();
    SIGHT_ASSERT("The inout key 'target' is not correctly set.", object);

    const auto buffered = std::dynamic_pointer_cast<core::memory::buffered>(object.get_shared());

    if(buffered)
    {
        m_locks = buffered->dump_lock();
    }
}

//-----------------------------------------------------------------------------

void lock_dump::stopping()
{
    m_locks.clear();
}

//-----------------------------------------------------------------------------

void lock_dump::configuring()
{
}

//-----------------------------------------------------------------------------

void lock_dump::updating()
{
}

//-----------------------------------------------------------------------------

void lock_dump::info(std::ostream& /*_sstream*/)
{
}

//-----------------------------------------------------------------------------

} // namespace sight::module::memory
