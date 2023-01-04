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

#include "LockDumpSrv.hpp"

#include <core/memory/IBuffered.hpp>

#include <service/macros.hpp>

namespace sight::module::memory
{

static const service::IService::KeyType s_TARGET_INOUT = "target";

//-----------------------------------------------------------------------------

LockDumpSrv::LockDumpSrv() noexcept =
    default;

//-----------------------------------------------------------------------------

LockDumpSrv::~LockDumpSrv() noexcept =
    default;

//-----------------------------------------------------------------------------

void LockDumpSrv::starting()
{
    const auto object = m_target.lock();
    SIGHT_ASSERT("The inout key '" << s_TARGET_INOUT << "' is not correctly set.", object);

    const auto buffered = std::dynamic_pointer_cast<core::memory::IBuffered>(object.get_shared());

    if(buffered)
    {
        m_locks = buffered->dump_lock();
    }
}

//-----------------------------------------------------------------------------

void LockDumpSrv::stopping()
{
    m_locks.clear();
}

//-----------------------------------------------------------------------------

void LockDumpSrv::configuring()
{
}

//-----------------------------------------------------------------------------

void LockDumpSrv::updating()
{
}

//-----------------------------------------------------------------------------

void LockDumpSrv::info(std::ostream& /*_sstream*/)
{
}

//-----------------------------------------------------------------------------

} // namespace sight::module::memory
