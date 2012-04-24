/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
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


AlwaysDump::sptr AlwaysDump::New()
{
    return AlwaysDump::sptr(new AlwaysDump());
}

//------------------------------------------------------------------------------

void AlwaysDump::allocationRequest( BufferInfo &info, void **buffer, BufferInfo::SizeType size )
{
    this->apply();
}

//------------------------------------------------------------------------------


void AlwaysDump::setRequest( BufferInfo &info, void **buffer, BufferInfo::SizeType size )
{
    this->apply();
}

//------------------------------------------------------------------------------


void AlwaysDump::reallocateRequest( BufferInfo &info, void **buffer, BufferInfo::SizeType newSize )
{
    this->apply();
}

//------------------------------------------------------------------------------


void AlwaysDump::destroyRequest( BufferInfo &info, void **buffer )
{
    this->apply();
}

//------------------------------------------------------------------------------


void AlwaysDump::lockRequest( BufferInfo &info, void **buffer )
{
    this->apply();
}

//------------------------------------------------------------------------------


void AlwaysDump::unlockRequest( BufferInfo &info, void **buffer )
{
    this->apply();
}

//------------------------------------------------------------------------------


void AlwaysDump::dumpSuccess( BufferInfo &info, void **buffer )
{
}

//------------------------------------------------------------------------------


void AlwaysDump::restoreSuccess( BufferInfo &info, void **buffer )
{
}

//------------------------------------------------------------------------------

void AlwaysDump::setManager(::fwTools::IBufferManager::sptr manager)
{
    m_manager = ::fwMemory::BufferManager::dynamicCast(manager);
}

//------------------------------------------------------------------------------

size_t AlwaysDump::dump()
{
    size_t dumped = 0;

    ::fwMemory::BufferManager::sptr manager = m_manager.lock();
    if(manager)
    {

        const fwMemory::BufferInfo::MapType &bufferInfos = manager->getBufferInfos();


        BOOST_FOREACH(const ::fwMemory::BufferInfo::MapType::value_type &elt, bufferInfos)
        {
            const ::fwMemory::BufferInfo &info = elt.second;
            if( ! ( info.size == 0 || *(info.lockCount) > 0 || info.isDumped )  )
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


} // namespace policy

} //namespace fwMemory
