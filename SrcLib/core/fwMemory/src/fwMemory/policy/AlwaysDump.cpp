/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


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
}

//------------------------------------------------------------------------------


void AlwaysDump::setRequest( BufferInfo &info, void **buffer, BufferInfo::SizeType size )
{
}

//------------------------------------------------------------------------------


void AlwaysDump::reallocateRequest( BufferInfo &info, void **buffer, BufferInfo::SizeType newSize )
{
}

//------------------------------------------------------------------------------


void AlwaysDump::destroyRequest( BufferInfo &info, void **buffer )
{
}

//------------------------------------------------------------------------------


void AlwaysDump::lockRequest( BufferInfo &info, void **buffer )
{
}

//------------------------------------------------------------------------------


void AlwaysDump::unlockRequest( BufferInfo &info, void **buffer )
{
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
}

//------------------------------------------------------------------------------

size_t AlwaysDump::dump(size_t nbOfBytes)
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


} // namespace policy

} //namespace fwMemory
