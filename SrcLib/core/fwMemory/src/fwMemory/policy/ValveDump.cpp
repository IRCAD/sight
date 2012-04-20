/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <boost/foreach.hpp>


#ifdef _WIN32
#define MEMORYTOOLIMPL Win32MemoryMonitorTools
#include "fwMemory/tools/Win32MemoryMonitorTools.hpp"
#elif defined(__MACOSX__)
#define MEMORYTOOLIMPL DarwinMemoryMonitorTools
#include "fwMemory/tools/DarwinMemoryMonitorTools.hpp"
#else
#define MEMORYTOOLIMPL PosixMemoryMonitorTools
#include "fwMemory/tools/PosixMemoryMonitorTools.hpp"
#endif



#include "fwMemory/policy/ValveDump.hpp"


namespace fwMemory
{

namespace policy
{


ValveDump::sptr ValveDump::New()
{
    return ValveDump::sptr(new ValveDump());
}

//------------------------------------------------------------------------------

ValveDump::ValveDump() :
    m_minFreeMem(1024*1024*500LL)
{

}

//------------------------------------------------------------------------------

void ValveDump::allocationRequest( BufferInfo &info, void **buffer, BufferInfo::SizeType size )
{
    this->apply((size > info.size) ? size - info.size : 0);
}

//------------------------------------------------------------------------------


void ValveDump::setRequest( BufferInfo &info, void **buffer, BufferInfo::SizeType size )
{
    this->apply();
}

//------------------------------------------------------------------------------


void ValveDump::reallocateRequest( BufferInfo &info, void **buffer, BufferInfo::SizeType newSize )
{
    this->apply((newSize > info.size) ? newSize - info.size : 0);
}

//------------------------------------------------------------------------------


void ValveDump::destroyRequest( BufferInfo &info, void **buffer )
{
}

//------------------------------------------------------------------------------


void ValveDump::lockRequest( BufferInfo &info, void **buffer )
{
    ::fwMemory::BufferManager::sptr manager = m_manager.lock();
    if(manager)
    {
        bool restored = manager->restoreBuffer( buffer );
        OSLM_ASSERT( "restore not OK ( "<< *buffer <<" ).", !restored || *buffer != 0 );
    }
}

//------------------------------------------------------------------------------


void ValveDump::unlockRequest( BufferInfo &info, void **buffer )
{
    this->apply();
}

//------------------------------------------------------------------------------


void ValveDump::dumpSuccess( BufferInfo &info, void **buffer )
{
}

//------------------------------------------------------------------------------


void ValveDump::restoreSuccess( BufferInfo &info, void **buffer )
{
}

//------------------------------------------------------------------------------



void ValveDump::setManager(::fwTools::IBufferManager::sptr manager)
{
    m_manager = ::fwMemory::BufferManager::dynamicCast(manager);
}

//------------------------------------------------------------------------------

bool ValveDump::needDump(size_t supplement)
{
    return ::fwMemory::tools::MEMORYTOOLIMPL::getFreeSystemMemory() <= (m_minFreeMem + supplement);
}

//------------------------------------------------------------------------------

size_t ValveDump::dump(size_t nbOfBytes)
{
    size_t dumped = 0;

    ::fwMemory::BufferManager::sptr manager = m_manager.lock();
    if(manager)
    {

        const fwMemory::BufferInfo::MapType &bufferInfos = manager->getBufferInfos();

        typedef std::pair<
            fwMemory::BufferInfo::MapType::key_type,
            fwMemory::BufferInfo::MapType::mapped_type
                > BufferInfosPairType;
        typedef std::vector< BufferInfosPairType > BufferVectorType;

        BufferVectorType buffers;

        BOOST_FOREACH(const ::fwMemory::BufferInfo::MapType::value_type &elt, bufferInfos)
        {
            const ::fwMemory::BufferInfo &info = elt.second;
            if( ! ( info.size == 0 || *(info.lockCount) > 0 || info.isDumped )  )
            {
                buffers.push_back(elt);
            }
        }


        BOOST_FOREACH(const BufferVectorType::value_type &pair, bufferInfos)
        {
            if(dumped < nbOfBytes)
            {
                if( manager->dumpBuffer(pair.first) )
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
        this->dump( (m_minFreeMem + supplement) - ::fwMemory::tools::MEMORYTOOLIMPL::getFreeSystemMemory() );
    }
}

//------------------------------------------------------------------------------

} // namespace policy

} //namespace fwMemory
