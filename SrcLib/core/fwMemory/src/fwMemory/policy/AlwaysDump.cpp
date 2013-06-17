/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <boost/foreach.hpp>

#include "fwMemory/policy/BarrierDump.hpp"
#include "fwMemory/policy/AlwaysDump.hpp"


namespace fwMemory
{

namespace policy
{

static IPolicy::Register<AlwaysDump> registerFactory(AlwaysDump::leafClassname());


//------------------------------------------------------------------------------

void AlwaysDump::allocationRequest( BufferInfo &info, ::fwMemory::BufferManager::BufferPtrType buffer, BufferInfo::SizeType size )
{
    this->apply();
}

//------------------------------------------------------------------------------


void AlwaysDump::setRequest( BufferInfo &info, ::fwMemory::BufferManager::BufferPtrType buffer, BufferInfo::SizeType size )
{
    this->apply();
}

//------------------------------------------------------------------------------


void AlwaysDump::reallocateRequest( BufferInfo &info, ::fwMemory::BufferManager::BufferPtrType buffer, BufferInfo::SizeType newSize )
{
    this->apply();
}

//------------------------------------------------------------------------------


void AlwaysDump::destroyRequest( BufferInfo &info, ::fwMemory::BufferManager::BufferPtrType buffer )
{
    this->apply();
}

//------------------------------------------------------------------------------


void AlwaysDump::lockRequest( BufferInfo &info, ::fwMemory::BufferManager::BufferPtrType buffer )
{
    this->apply();
}

//------------------------------------------------------------------------------


void AlwaysDump::unlockRequest( BufferInfo &info, ::fwMemory::BufferManager::BufferPtrType buffer )
{
    this->apply();
}

//------------------------------------------------------------------------------


void AlwaysDump::dumpSuccess( BufferInfo &info, ::fwMemory::BufferManager::BufferPtrType buffer )
{
}

//------------------------------------------------------------------------------


void AlwaysDump::restoreSuccess( BufferInfo &info, ::fwMemory::BufferManager::BufferPtrType buffer )
{
}

//------------------------------------------------------------------------------

void AlwaysDump::setManager(const ::fwMemory::BufferManager::sptr &manager)
{
    m_manager = manager;
}

//------------------------------------------------------------------------------

size_t AlwaysDump::dump()
{
    size_t dumped = 0;

    ::fwMemory::BufferManager::sptr manager = m_manager.lock();
    if(manager)
    {

        const fwMemory::BufferManager::BufferInfoMapType &bufferInfos = manager->getBufferInfos();


        BOOST_FOREACH(const ::fwMemory::BufferManager::BufferInfoMapType::value_type &elt, bufferInfos)
        {
            const ::fwMemory::BufferInfo &info = elt.second;
            if( ! ( info.size == 0 || info.lockCount() > 0 || !info.loaded )  )
            {
                if( manager->dumpBuffer(elt.first) )
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

std::string AlwaysDump::getParam(const std::string &name, bool *ok  )
{
    if (ok)
    {
        *ok = false;
    }
    return "";
}

//------------------------------------------------------------------------------


} // namespace policy

} //namespace fwMemory
