/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
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

#include "fwMemory/ByteSize.hpp"
#include "fwMemory/exception/BadCast.hpp"
#include "fwMemory/policy/registry/macros.hpp"
#include "fwMemory/policy/ValveDump.hpp"

#ifdef _WIN32
#define MEMORYTOOLIMPL Win32MemoryMonitorTools
#include "fwMemory/tools/Win32MemoryMonitorTools.hpp"
#elif defined(__APPLE__)
#define MEMORYTOOLIMPL DarwinMemoryMonitorTools
#include "fwMemory/tools/DarwinMemoryMonitorTools.hpp"
#else
#define MEMORYTOOLIMPL PosixMemoryMonitorTools
#include "fwMemory/tools/PosixMemoryMonitorTools.hpp"
#endif

namespace fwMemory
{

namespace policy
{

fwMemoryPolicyRegisterMacro(::fwMemory::policy::ValveDump);

//------------------------------------------------------------------------------

ValveDump::ValveDump() :
    m_minFreeMem(1024*1024*500LL),
    m_hysteresisOffset(0)
{
}

//------------------------------------------------------------------------------

void ValveDump::allocationRequest( BufferInfo& info, ::fwMemory::BufferManager::ConstBufferPtrType buffer,
                                   BufferInfo::SizeType size )
{
    FwCoreNotUsedMacro(buffer);
    this->apply((size > info.size) ? size - info.size : 0);
}

//------------------------------------------------------------------------------

void ValveDump::setRequest( BufferInfo& info, ::fwMemory::BufferManager::ConstBufferPtrType buffer,
                            BufferInfo::SizeType size )
{
    FwCoreNotUsedMacro(info);
    FwCoreNotUsedMacro(buffer);
    FwCoreNotUsedMacro(size);
    this->apply();
}

//------------------------------------------------------------------------------

void ValveDump::reallocateRequest( BufferInfo& info, ::fwMemory::BufferManager::ConstBufferPtrType buffer,
                                   BufferInfo::SizeType newSize )
{
    FwCoreNotUsedMacro(buffer);
    this->apply((newSize > info.size) ? newSize - info.size : 0);
}

//------------------------------------------------------------------------------

void ValveDump::destroyRequest( BufferInfo& info, ::fwMemory::BufferManager::ConstBufferPtrType buffer )
{
    FwCoreNotUsedMacro(info);
    FwCoreNotUsedMacro(buffer);
}

//------------------------------------------------------------------------------

void ValveDump::lockRequest( BufferInfo& info, ::fwMemory::BufferManager::ConstBufferPtrType buffer )
{
    FwCoreNotUsedMacro(info);
    FwCoreNotUsedMacro(buffer);
}

//------------------------------------------------------------------------------

void ValveDump::unlockRequest( BufferInfo& info, ::fwMemory::BufferManager::ConstBufferPtrType buffer )
{
    FwCoreNotUsedMacro(info);
    FwCoreNotUsedMacro(buffer);
    this->apply();
}

//------------------------------------------------------------------------------

void ValveDump::dumpSuccess( BufferInfo& info, ::fwMemory::BufferManager::ConstBufferPtrType buffer )
{
    FwCoreNotUsedMacro(info);
    FwCoreNotUsedMacro(buffer);
}

//------------------------------------------------------------------------------

void ValveDump::restoreSuccess( BufferInfo& info, ::fwMemory::BufferManager::ConstBufferPtrType buffer )
{
    FwCoreNotUsedMacro(info);
    FwCoreNotUsedMacro(buffer);
}

//------------------------------------------------------------------------------

bool ValveDump::needDump(size_t supplement) const
{
    return ::fwMemory::tools::MEMORYTOOLIMPL::getFreeSystemMemory() <= (m_minFreeMem + supplement);
}

//------------------------------------------------------------------------------

size_t ValveDump::dump(size_t nbOfBytes)
{
    size_t dumped = 0;

    ::fwMemory::BufferManager::sptr manager = ::fwMemory::BufferManager::getDefault();
    if(manager)
    {
        const ::fwMemory::BufferManager::BufferInfoMapType bufferInfos = manager->getBufferInfos().get();

        typedef std::pair<
                fwMemory::BufferManager::BufferInfoMapType::key_type,
                fwMemory::BufferManager::BufferInfoMapType::mapped_type
                > BufferInfosPairType;
        typedef std::vector< BufferInfosPairType > BufferVectorType;

        BufferVectorType buffers;

        for(const ::fwMemory::BufferManager::BufferInfoMapType::value_type& elt :  bufferInfos)
        {
            const ::fwMemory::BufferInfo& info = elt.second;
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
            ::fwMemory::tools::MEMORYTOOLIMPL::getFreeSystemMemory() );
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
    OSLM_INFO("Set " << name << " to " << value);
    try
    {
        if(name == "min_free_mem")
        {
            m_minFreeMem = ::fwMemory::ByteSize(value).getSize();
            return true;

        }
        else if(name == "hysteresis_offet")
        {
            m_hysteresisOffset = ::fwMemory::ByteSize(value).getSize();
            return true;
        }
    }
    catch( ::fwMemory::exception::BadCast const& )
    {
        OSLM_ERROR("Bad value for " << name << " : " << value);
        return false;
    }
    OSLM_ERROR("Bad parameter name " << name );
    return false;
}

//------------------------------------------------------------------------------

const fwMemory::IPolicy::ParamNamesType& ValveDump::getParamNames() const
{
    static const fwMemory::IPolicy::ParamNamesType params = {{ "min_free_mem", "hysteresis_offet" }};
    return params;
}

//------------------------------------------------------------------------------

std::string ValveDump::getParam(const std::string& name, bool* ok ) const
{
    bool isOk = false;
    std::string value;
    if(name == "min_free_mem")
    {
        value = std::string(::fwMemory::ByteSize( ::fwMemory::ByteSize::SizeType(m_minFreeMem) ));
        isOk  = true;
    }
    else if(name == "hysteresis_offet")
    {
        value = std::string(::fwMemory::ByteSize( ::fwMemory::ByteSize::SizeType(m_hysteresisOffset) ));
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

} //namespace fwMemory
