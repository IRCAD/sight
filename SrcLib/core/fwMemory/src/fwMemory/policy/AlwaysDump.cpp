/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include "fwMemory/policy/registry/macros.hpp"
#include "fwMemory/policy/BarrierDump.hpp"
#include "fwMemory/policy/AlwaysDump.hpp"


namespace fwMemory
{

namespace policy
{

fwMemoryPolicyRegisterMacro(::fwMemory::policy::AlwaysDump);

//------------------------------------------------------------------------------

void AlwaysDump::allocationRequest( BufferInfo &info, ::fwMemory::BufferManager::ConstBufferPtrType buffer,
                                    BufferInfo::SizeType size )
{
    FwCoreNotUsedMacro(info);
    FwCoreNotUsedMacro(buffer);
    FwCoreNotUsedMacro(size);
    this->apply();
}

//------------------------------------------------------------------------------

void AlwaysDump::setRequest( BufferInfo &info, ::fwMemory::BufferManager::ConstBufferPtrType buffer,
                             BufferInfo::SizeType size )
{
    FwCoreNotUsedMacro(info);
    FwCoreNotUsedMacro(buffer);
    FwCoreNotUsedMacro(size);
    this->apply();
}

//------------------------------------------------------------------------------

void AlwaysDump::reallocateRequest( BufferInfo &info, ::fwMemory::BufferManager::ConstBufferPtrType buffer,
                                    BufferInfo::SizeType newSize )
{
    FwCoreNotUsedMacro(info);
    FwCoreNotUsedMacro(buffer);
    FwCoreNotUsedMacro(newSize);
    this->apply();
}

//------------------------------------------------------------------------------

void AlwaysDump::destroyRequest( BufferInfo &info, ::fwMemory::BufferManager::ConstBufferPtrType buffer )
{
    FwCoreNotUsedMacro(info);
    FwCoreNotUsedMacro(buffer);
    this->apply();
}

//------------------------------------------------------------------------------

void AlwaysDump::lockRequest( BufferInfo &info, ::fwMemory::BufferManager::ConstBufferPtrType buffer )
{
    FwCoreNotUsedMacro(info);
    FwCoreNotUsedMacro(buffer);
    this->apply();
}

//------------------------------------------------------------------------------

void AlwaysDump::unlockRequest( BufferInfo &info, ::fwMemory::BufferManager::ConstBufferPtrType buffer )
{
    FwCoreNotUsedMacro(info);
    FwCoreNotUsedMacro(buffer);
    this->apply();
}

//------------------------------------------------------------------------------

void AlwaysDump::dumpSuccess( BufferInfo &info, ::fwMemory::BufferManager::ConstBufferPtrType buffer )
{
    FwCoreNotUsedMacro(info);
    FwCoreNotUsedMacro(buffer);
}

//------------------------------------------------------------------------------

void AlwaysDump::restoreSuccess( BufferInfo &info, ::fwMemory::BufferManager::ConstBufferPtrType buffer )
{
    FwCoreNotUsedMacro(info);
    FwCoreNotUsedMacro(buffer);
}

//------------------------------------------------------------------------------

size_t AlwaysDump::dump()
{
    size_t dumped = 0;

    ::fwMemory::BufferManager::sptr manager = ::fwMemory::BufferManager::getDefault();
    if(manager)
    {
        const ::fwMemory::BufferManager::BufferInfoMapType bufferInfos = manager->getBufferInfos().get();

        BOOST_FOREACH(const ::fwMemory::BufferManager::BufferInfoMapType::value_type &elt, bufferInfos)
        {
            const ::fwMemory::BufferInfo &info = elt.second;
            if( !( info.size == 0 || info.lockCount() > 0 || !info.loaded )  )
            {
                if( manager->dumpBuffer(elt.first).get() )
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

std::string AlwaysDump::getParam(const std::string &name, bool *ok  ) const
{
    FwCoreNotUsedMacro(name);
    if (ok)
    {
        *ok = false;
    }
    return "";
}

//------------------------------------------------------------------------------


} // namespace policy

} //namespace fwMemory
