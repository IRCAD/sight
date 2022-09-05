/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "core/memory/policy/NeverDump.hpp"

#include "core/memory/policy/registry/macros.hpp"

namespace sight::core::memory::policy
{

fwMemoryPolicyRegisterMacro(core::memory::policy::NeverDump);

//------------------------------------------------------------------------------

void NeverDump::allocationRequest(
    BufferInfo& info,
    core::memory::BufferManager::ConstBufferPtrType buffer,
    BufferInfo::SizeType size
)
{
    SIGHT_NOT_USED(info);
    SIGHT_NOT_USED(buffer);
    SIGHT_NOT_USED(size);
}

//------------------------------------------------------------------------------

void NeverDump::setRequest(
    BufferInfo& info,
    core::memory::BufferManager::ConstBufferPtrType buffer,
    BufferInfo::SizeType size
)
{
    SIGHT_NOT_USED(info);
    SIGHT_NOT_USED(buffer);
    SIGHT_NOT_USED(size);
}

//------------------------------------------------------------------------------

void NeverDump::reallocateRequest(
    BufferInfo& info,
    core::memory::BufferManager::ConstBufferPtrType buffer,
    BufferInfo::SizeType newSize
)
{
    SIGHT_NOT_USED(info);
    SIGHT_NOT_USED(buffer);
    SIGHT_NOT_USED(newSize);
}

//------------------------------------------------------------------------------

void NeverDump::destroyRequest(BufferInfo& info, core::memory::BufferManager::ConstBufferPtrType buffer)
{
    SIGHT_NOT_USED(info);
    SIGHT_NOT_USED(buffer);
}

//------------------------------------------------------------------------------

void NeverDump::lockRequest(BufferInfo& info, core::memory::BufferManager::ConstBufferPtrType buffer)
{
    SIGHT_NOT_USED(info);
    SIGHT_NOT_USED(buffer);
}

//------------------------------------------------------------------------------

void NeverDump::unlockRequest(BufferInfo& info, core::memory::BufferManager::ConstBufferPtrType buffer)
{
    SIGHT_NOT_USED(info);
    SIGHT_NOT_USED(buffer);
}

//------------------------------------------------------------------------------

void NeverDump::dumpSuccess(BufferInfo& info, core::memory::BufferManager::ConstBufferPtrType buffer)
{
    SIGHT_NOT_USED(info);
    SIGHT_NOT_USED(buffer);
}

//------------------------------------------------------------------------------

void NeverDump::restoreSuccess(BufferInfo& info, core::memory::BufferManager::ConstBufferPtrType buffer)
{
    SIGHT_NOT_USED(info);
    SIGHT_NOT_USED(buffer);
}

//------------------------------------------------------------------------------

void NeverDump::refresh()
{
}

//------------------------------------------------------------------------------

std::string NeverDump::getParam(const std::string& name, bool* ok) const
{
    SIGHT_NOT_USED(name);
    if(ok != nullptr)
    {
        *ok = false;
    }

    return "";
}

//------------------------------------------------------------------------------

} // namespace sight::core::memory::policy
