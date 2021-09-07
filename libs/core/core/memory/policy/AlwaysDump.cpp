/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "core/memory/policy/AlwaysDump.hpp"

#include "core/memory/policy/BarrierDump.hpp"
#include "core/memory/policy/registry/macros.hpp"

namespace sight::core::memory
{

namespace policy
{

fwMemoryPolicyRegisterMacro(core::memory::policy::AlwaysDump);

//------------------------------------------------------------------------------

void AlwaysDump::allocationRequest(
    BufferInfo& info,
    core::memory::BufferManager::ConstBufferPtrType buffer,
    BufferInfo::SizeType size
)
{
    SIGHT_NOT_USED(info);
    SIGHT_NOT_USED(buffer);
    SIGHT_NOT_USED(size);
    this->apply();
}

//------------------------------------------------------------------------------

void AlwaysDump::setRequest(
    BufferInfo& info,
    core::memory::BufferManager::ConstBufferPtrType buffer,
    BufferInfo::SizeType size
)
{
    SIGHT_NOT_USED(info);
    SIGHT_NOT_USED(buffer);
    SIGHT_NOT_USED(size);
    this->apply();
}

//------------------------------------------------------------------------------

void AlwaysDump::reallocateRequest(
    BufferInfo& info,
    core::memory::BufferManager::ConstBufferPtrType buffer,
    BufferInfo::SizeType newSize
)
{
    SIGHT_NOT_USED(info);
    SIGHT_NOT_USED(buffer);
    SIGHT_NOT_USED(newSize);
    this->apply();
}

//------------------------------------------------------------------------------

void AlwaysDump::destroyRequest(BufferInfo& info, core::memory::BufferManager::ConstBufferPtrType buffer)
{
    SIGHT_NOT_USED(info);
    SIGHT_NOT_USED(buffer);
    this->apply();
}

//------------------------------------------------------------------------------

void AlwaysDump::lockRequest(BufferInfo& info, core::memory::BufferManager::ConstBufferPtrType buffer)
{
    SIGHT_NOT_USED(info);
    SIGHT_NOT_USED(buffer);
    this->apply();
}

//------------------------------------------------------------------------------

void AlwaysDump::unlockRequest(BufferInfo& info, core::memory::BufferManager::ConstBufferPtrType buffer)
{
    SIGHT_NOT_USED(info);
    SIGHT_NOT_USED(buffer);
    this->apply();
}

//------------------------------------------------------------------------------

void AlwaysDump::dumpSuccess(BufferInfo& info, core::memory::BufferManager::ConstBufferPtrType buffer)
{
    SIGHT_NOT_USED(info);
    SIGHT_NOT_USED(buffer);
}

//------------------------------------------------------------------------------

void AlwaysDump::restoreSuccess(BufferInfo& info, core::memory::BufferManager::ConstBufferPtrType buffer)
{
    SIGHT_NOT_USED(info);
    SIGHT_NOT_USED(buffer);
}

//------------------------------------------------------------------------------

size_t AlwaysDump::dump()
{
    size_t dumped = 0;

    core::memory::BufferManager::sptr manager = core::memory::BufferManager::getDefault();
    if(manager)
    {
        const core::memory::BufferManager::BufferInfoMapType bufferInfos = manager->getBufferInfos().get();

        for(const core::memory::BufferManager::BufferInfoMapType::value_type& elt : bufferInfos)
        {
            const core::memory::BufferInfo& info = elt.second;
            if(!(info.size == 0 || info.lockCount() > 0 || !info.loaded))
            {
                if(manager->dumpBuffer(elt.first).get())
                {
                    dumped += elt.second.size;
                }
            }
        }
    }

    return dumped;
}

//------------------------------------------------------------------------------

void AlwaysDump::apply()
{
    this->dump();
}

//------------------------------------------------------------------------------

void AlwaysDump::refresh()
{
    this->apply();
}

//------------------------------------------------------------------------------

std::string AlwaysDump::getParam(const std::string& name, bool* ok) const
{
    SIGHT_NOT_USED(name);
    if(ok)
    {
        *ok = false;
    }

    return "";
}

//------------------------------------------------------------------------------

} // namespace policy

} //namespace sight::core::memory
