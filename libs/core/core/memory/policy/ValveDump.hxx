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

#pragma once

#include "core/memory/ByteSize.hpp"
#include "core/memory/exception/BadCast.hpp"
#include "core/memory/policy/registry/macros.hpp"
#include "core/memory/policy/ValveDump.hpp"

#include <cstddef>

namespace sight::core::memory::policy
{

SIGHT_REGISTER_MEMORY_POLICY(core::memory::policy::ValveDump<core::memory::tools::MemoryMonitorTools>);

//------------------------------------------------------------------------------

template<typename TOOL>
ValveDump<TOOL>::ValveDump() :
    m_minFreeMem(1024LL * 1024 * 500LL)
{
}

//------------------------------------------------------------------------------

template<typename TOOL>
void ValveDump<TOOL>::allocationRequest(
    BufferInfo& info,
    core::memory::BufferManager::ConstBufferPtrType buffer,
    BufferInfo::SizeType size
)
{
    SIGHT_NOT_USED(buffer);
    this->apply((size > info.size) ? size - info.size : 0);
}

//------------------------------------------------------------------------------

template<typename TOOL>
void ValveDump<TOOL>::setRequest(
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

template<typename TOOL>
void ValveDump<TOOL>::reallocateRequest(
    BufferInfo& info,
    core::memory::BufferManager::ConstBufferPtrType buffer,
    BufferInfo::SizeType newSize
)
{
    SIGHT_NOT_USED(buffer);
    this->apply((newSize > info.size) ? newSize - info.size : 0);
}

//------------------------------------------------------------------------------

template<typename TOOL>
void ValveDump<TOOL>::destroyRequest(BufferInfo& info, core::memory::BufferManager::ConstBufferPtrType buffer)
{
    SIGHT_NOT_USED(info);
    SIGHT_NOT_USED(buffer);
}

//------------------------------------------------------------------------------

template<typename TOOL>
void ValveDump<TOOL>::lockRequest(BufferInfo& info, core::memory::BufferManager::ConstBufferPtrType buffer)
{
    SIGHT_NOT_USED(info);
    SIGHT_NOT_USED(buffer);
}

//------------------------------------------------------------------------------

template<typename TOOL>
void ValveDump<TOOL>::unlockRequest(BufferInfo& info, core::memory::BufferManager::ConstBufferPtrType buffer)
{
    SIGHT_NOT_USED(info);
    SIGHT_NOT_USED(buffer);
    this->apply();
}

//------------------------------------------------------------------------------

template<typename TOOL>
void ValveDump<TOOL>::dumpSuccess(BufferInfo& info, core::memory::BufferManager::ConstBufferPtrType buffer)
{
    SIGHT_NOT_USED(info);
    SIGHT_NOT_USED(buffer);
}

//------------------------------------------------------------------------------

template<typename TOOL>
void ValveDump<TOOL>::restoreSuccess(BufferInfo& info, core::memory::BufferManager::ConstBufferPtrType buffer)
{
    SIGHT_NOT_USED(info);
    SIGHT_NOT_USED(buffer);
}

//------------------------------------------------------------------------------

template<typename TOOL>
bool ValveDump<TOOL>::needDump(std::size_t supplement) const
{
    return TOOL::getFreeSystemMemory() <= (m_minFreeMem + supplement);
}

//------------------------------------------------------------------------------

template<typename TOOL>
std::size_t ValveDump<TOOL>::dump(std::size_t nbOfBytes)
{
    std::size_t dumped = 0;

    core::memory::BufferManager::sptr manager = core::memory::BufferManager::getDefault();
    if(manager)
    {
        const core::memory::BufferManager::BufferInfoMapType bufferInfos = manager->getBufferInfos().get();

        typedef std::pair<
                core::memory::BufferManager::BufferInfoMapType::key_type,
                core::memory::BufferManager::BufferInfoMapType::mapped_type
        > BufferInfosPairType;
        typedef std::vector<BufferInfosPairType> BufferVectorType;

        BufferVectorType buffers;

        for(const core::memory::BufferManager::BufferInfoMapType::value_type& elt : bufferInfos)
        {
            const core::memory::BufferInfo& info = elt.second;
            if(!(info.size == 0 || info.lockCount() > 0 || !info.loaded))
            {
                buffers.push_back(elt);
            }
        }

        for(const auto& pair : bufferInfos)
        {
            if(dumped < nbOfBytes)
            {
                if(manager->dumpBuffer(pair.first).get())
                {
                    dumped += pair.second.size;
                }
            }
            else
            {
                break;
            }
        }
    }

    return dumped;
}

//------------------------------------------------------------------------------

template<typename TOOL>
void ValveDump<TOOL>::apply(std::size_t supplement)
{
    if(this->needDump(supplement))
    {
        this->dump(
            (m_minFreeMem + m_hysteresisOffset + supplement)
            - TOOL::getFreeSystemMemory()
        );
    }
}

//------------------------------------------------------------------------------

template<typename TOOL>
void ValveDump<TOOL>::refresh()
{
    this->apply();
}

//------------------------------------------------------------------------------

template<typename TOOL>
bool ValveDump<TOOL>::setParam(const std::string& name, const std::string& value)
{
    SIGHT_INFO("Set " << name << " to " << value);
    try
    {
        if(name == "min_free_mem")
        {
            m_minFreeMem = core::memory::ByteSize(value).getSize();
            return true;
        }

        if(name == "hysteresis_offset")
        {
            m_hysteresisOffset = core::memory::ByteSize(value).getSize();
            return true;
        }
    }
    catch(core::memory::exception::BadCast const&)
    {
        SIGHT_ERROR("Bad value for " << name << " : " << value);
        return false;
    }
    SIGHT_ERROR("Bad parameter name " << name);
    return false;
}

//------------------------------------------------------------------------------

template<typename TOOL>
const core::memory::IPolicy::ParamNamesType& ValveDump<TOOL>::getParamNames() const
{
    static const core::memory::IPolicy::ParamNamesType params {"min_free_mem", "hysteresis_offset"};
    return params;
}

//------------------------------------------------------------------------------

template<typename TOOL>
std::string ValveDump<TOOL>::getParam(const std::string& name, bool* ok) const
{
    bool isOk = false;
    std::string value;
    if(name == "min_free_mem")
    {
        value = std::string(core::memory::ByteSize(core::memory::ByteSize::SizeType(m_minFreeMem)));
        isOk  = true;
    }
    else if(name == "hysteresis_offset")
    {
        value = std::string(core::memory::ByteSize(core::memory::ByteSize::SizeType(m_hysteresisOffset)));
        isOk  = true;
    }

    if(ok != nullptr)
    {
        *ok = isOk;
    }

    return value;
}

//------------------------------------------------------------------------------

} // namespace sight::core::memory::policy
