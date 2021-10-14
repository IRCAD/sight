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

#include "core/memory/policy/BarrierDump.hpp"

#include "core/memory/ByteSize.hpp"
#include "core/memory/exception/BadCast.hpp"
#include "core/memory/policy/registry/macros.hpp"

namespace sight::core::memory
{

namespace policy
{

fwMemoryPolicyRegisterMacro(core::memory::policy::BarrierDump);

//------------------------------------------------------------------------------

BarrierDump::BarrierDump() :
    m_totalAllocated(0),
    m_totalDumped(0),
    m_barrier(1024 * 1024 * 500)
{
}

//------------------------------------------------------------------------------

void BarrierDump::allocationRequest(
    BufferInfo& info,
    core::memory::BufferManager::ConstBufferPtrType,
    BufferInfo::SizeType size
)
{
    SIGHT_ASSERT("Memory allocation inconsistency", m_totalAllocated >= info.size);
    m_totalAllocated -= info.size;
    m_totalAllocated += size;
    if(!info.loaded)
    {
        SIGHT_ASSERT("Memory dump inconsistency", m_totalDumped >= info.size);
        m_totalDumped -= info.size;
    }

    this->apply();
}

//------------------------------------------------------------------------------

void BarrierDump::setRequest(
    BufferInfo& info,
    core::memory::BufferManager::ConstBufferPtrType,
    BufferInfo::SizeType size
)
{
    SIGHT_ASSERT("Memory allocation inconsistency", m_totalAllocated >= info.size);
    m_totalAllocated -= info.size;
    m_totalAllocated += size;
    if(!info.loaded)
    {
        SIGHT_ASSERT("Memory dump inconsistency", m_totalDumped >= info.size);
        m_totalDumped -= info.size;
    }

    this->apply();
}

//------------------------------------------------------------------------------

void BarrierDump::reallocateRequest(
    BufferInfo& info,
    core::memory::BufferManager::ConstBufferPtrType,
    BufferInfo::SizeType newSize
)
{
    SIGHT_ASSERT("Memory allocation inconsistency", m_totalAllocated >= info.size);
    m_totalAllocated -= info.size;
    m_totalAllocated += newSize;
    if(!info.loaded)
    {
        SIGHT_ASSERT("Memory dump inconsistency", m_totalDumped >= info.size);
        m_totalDumped -= info.size;
    }

    this->apply();
}

//------------------------------------------------------------------------------

void BarrierDump::destroyRequest(BufferInfo& info, core::memory::BufferManager::ConstBufferPtrType)
{
    if(!info.loaded)
    {
        SIGHT_ASSERT("Memory dump inconsistency", m_totalDumped >= info.size);
        m_totalDumped -= info.size;
    }

    SIGHT_ASSERT("Memory allocation inconsistency", m_totalAllocated >= info.size);
    m_totalAllocated -= info.size;
}

//------------------------------------------------------------------------------

void BarrierDump::lockRequest(BufferInfo&, core::memory::BufferManager::ConstBufferPtrType)
{
}

//------------------------------------------------------------------------------

void BarrierDump::unlockRequest(BufferInfo&, core::memory::BufferManager::ConstBufferPtrType)
{
    this->apply();
}

//------------------------------------------------------------------------------

void BarrierDump::dumpSuccess(BufferInfo& info, core::memory::BufferManager::ConstBufferPtrType)
{
    m_totalDumped += info.size;
}

//------------------------------------------------------------------------------

void BarrierDump::restoreSuccess(BufferInfo& info, core::memory::BufferManager::ConstBufferPtrType)
{
    SIGHT_ASSERT("Memory dump inconsistency", m_totalDumped >= info.size);
    m_totalDumped -= info.size;
}

//------------------------------------------------------------------------------

size_t BarrierDump::getTotalAlive() const
{
    SIGHT_ASSERT("More dumped data than allocated data.", m_totalAllocated >= m_totalDumped);
    size_t totalAlive = m_totalAllocated - m_totalDumped;
    return totalAlive;
}

//------------------------------------------------------------------------------

bool BarrierDump::isBarrierCrossed() const
{
    return m_barrier < getTotalAlive();
}

//------------------------------------------------------------------------------

size_t BarrierDump::dump(size_t nbOfBytes)
{
    size_t dumped = 0;

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

void BarrierDump::apply()
{
    if(this->isBarrierCrossed())
    {
        this->dump(this->getTotalAlive() - m_barrier);
    }
}

//------------------------------------------------------------------------------

void BarrierDump::refresh()
{
    core::memory::BufferManager::sptr manager      = core::memory::BufferManager::getDefault();
    core::memory::BufferManager::BufferStats stats = manager->getBufferStats().get();
    m_totalAllocated = stats.totalManaged;
    m_totalDumped    = stats.totalDumped;
    this->apply();
}

//------------------------------------------------------------------------------

bool BarrierDump::setParam(const std::string& name, const std::string& value)
{
    try
    {
        if(name == "barrier")
        {
            m_barrier = core::memory::ByteSize(value).getSize();
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

const core::memory::IPolicy::ParamNamesType& BarrierDump::getParamNames() const
{
    static const core::memory::IPolicy::ParamNamesType params = {{"barrier"}};
    return params;
}

//------------------------------------------------------------------------------

std::string BarrierDump::getParam(const std::string& name, bool* ok) const
{
    bool isOk = false;
    std::string value;
    if(name == "barrier")
    {
        value = std::string(core::memory::ByteSize(core::memory::ByteSize::SizeType(m_barrier)));
        isOk  = true;
    }

    if(ok)
    {
        *ok = isOk;
    }

    return value;
}

//------------------------------------------------------------------------------

} // namespace policy

} //namespace sight::core::memory
