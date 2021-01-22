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

#include "core/memory/ByteSize.hpp"
#include "core/memory/exception/BadCast.hpp"
#include "core/memory/policy/registry/macros.hpp"
#include "core/memory/policy/ValveDump.hpp"

#ifdef _WIN32
#define MEMORYTOOLIMPL Win32MemoryMonitorTools
#include "core/memory/tools/Win32MemoryMonitorTools.hpp"
#elif defined(__APPLE__)
#define MEMORYTOOLIMPL DarwinMemoryMonitorTools
#include "core/memory/tools/DarwinMemoryMonitorTools.hpp"
#else
#define MEMORYTOOLIMPL PosixMemoryMonitorTools
#include "core/memory/tools/PosixMemoryMonitorTools.hpp"
#endif

namespace sight::core::memory
{

namespace policy
{

fwMemoryPolicyRegisterMacro(core::memory::policy::ValveDump);

//------------------------------------------------------------------------------

ValveDump::ValveDump() :
    m_minFreeMem(1024*1024*500LL),
    m_hysteresisOffset(0)
{
}

//------------------------------------------------------------------------------

void ValveDump::allocationRequest( BufferInfo& info, core::memory::BufferManager::ConstBufferPtrType buffer,
                                   BufferInfo::SizeType size )
{
    FwCoreNotUsedMacro(buffer);
    this->apply((size > info.size) ? size - info.size : 0);
}

//------------------------------------------------------------------------------

void ValveDump::setRequest( BufferInfo& info, core::memory::BufferManager::ConstBufferPtrType buffer,
                            BufferInfo::SizeType size )
{
    FwCoreNotUsedMacro(info);
    FwCoreNotUsedMacro(buffer);
    FwCoreNotUsedMacro(size);
    this->apply();
}

//------------------------------------------------------------------------------

void ValveDump::reallocateRequest( BufferInfo& info, core::memory::BufferManager::ConstBufferPtrType buffer,
                                   BufferInfo::SizeType newSize )
{
    FwCoreNotUsedMacro(buffer);
    this->apply((newSize > info.size) ? newSize - info.size : 0);
}

//------------------------------------------------------------------------------

void ValveDump::destroyRequest( BufferInfo& info, core::memory::BufferManager::ConstBufferPtrType buffer )
{
    FwCoreNotUsedMacro(info);
    FwCoreNotUsedMacro(buffer);
}

//------------------------------------------------------------------------------

void ValveDump::lockRequest( BufferInfo& info, core::memory::BufferManager::ConstBufferPtrType buffer )
{
    FwCoreNotUsedMacro(info);
    FwCoreNotUsedMacro(buffer);
}

//------------------------------------------------------------------------------

void ValveDump::unlockRequest( BufferInfo& info, core::memory::BufferManager::ConstBufferPtrType buffer )
{
    FwCoreNotUsedMacro(info);
    FwCoreNotUsedMacro(buffer);
    this->apply();
}

//------------------------------------------------------------------------------

void ValveDump::dumpSuccess( BufferInfo& info, core::memory::BufferManager::ConstBufferPtrType buffer )
{
    FwCoreNotUsedMacro(info);
    FwCoreNotUsedMacro(buffer);
}

//------------------------------------------------------------------------------

void ValveDump::restoreSuccess( BufferInfo& info, core::memory::BufferManager::ConstBufferPtrType buffer )
{
    FwCoreNotUsedMacro(info);
    FwCoreNotUsedMacro(buffer);
}

//------------------------------------------------------------------------------

bool ValveDump::needDump(size_t supplement) const
{
    return core::memory::tools::MEMORYTOOLIMPL::getFreeSystemMemory() <= (m_minFreeMem + supplement);
}

//------------------------------------------------------------------------------

size_t ValveDump::dump(size_t nbOfBytes)
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
        typedef std::vector< BufferInfosPairType > BufferVectorType;

        BufferVectorType buffers;

        for(const core::memory::BufferManager::BufferInfoMapType::value_type& elt :  bufferInfos)
        {
            const core::memory::BufferInfo& info = elt.second;
            if( !( info.size == 0 || info.lockCount() > 0 || !info.loaded )  )
            {
                buffers.push_back(elt);
            }
        }

        for(const BufferVectorType::value_type& pair :  bufferInfos)
        {
            if(dumped < nbOfBytes)
            {
                if( manager->dumpBuffer(pair.first).get() )
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

void ValveDump::apply(size_t supplement)
{
    if(this->needDump(supplement))
    {
        this->dump(
            (m_minFreeMem + m_hysteresisOffset + supplement) -
            core::memory::tools::MEMORYTOOLIMPL::getFreeSystemMemory() );
    }
}

//------------------------------------------------------------------------------

void ValveDump::refresh()
{
    this->apply();
}

//------------------------------------------------------------------------------

bool ValveDump::setParam(const std::string& name, const std::string& value)
{
    SLM_INFO("Set " << name << " to " << value);
    try
    {
        if(name == "min_free_mem")
        {
            m_minFreeMem = core::memory::ByteSize(value).getSize();
            return true;

        }
        else if(name == "hysteresis_offet")
        {
            m_hysteresisOffset = core::memory::ByteSize(value).getSize();
            return true;
        }
    }
    catch( core::memory::exception::BadCast const& )
    {
        SLM_ERROR("Bad value for " << name << " : " << value);
        return false;
    }
    SLM_ERROR("Bad parameter name " << name );
    return false;
}

//------------------------------------------------------------------------------

const core::memory::IPolicy::ParamNamesType& ValveDump::getParamNames() const
{
    static const core::memory::IPolicy::ParamNamesType params = {{ "min_free_mem", "hysteresis_offet" }};
    return params;
}

//------------------------------------------------------------------------------

std::string ValveDump::getParam(const std::string& name, bool* ok ) const
{
    bool isOk = false;
    std::string value;
    if(name == "min_free_mem")
    {
        value = std::string(core::memory::ByteSize( core::memory::ByteSize::SizeType(m_minFreeMem) ));
        isOk  = true;
    }
    else if(name == "hysteresis_offet")
    {
        value = std::string(core::memory::ByteSize( core::memory::ByteSize::SizeType(m_hysteresisOffset) ));
        isOk  = true;
    }
    if (ok)
    {
        *ok = isOk;
    }
    return value;
}

//------------------------------------------------------------------------------

} // namespace policy

} //namespace sight::core::memory
