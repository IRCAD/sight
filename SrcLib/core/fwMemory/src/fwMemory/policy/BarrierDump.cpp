/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <boost/foreach.hpp>
#include <fwMemory/policy/BarrierDump.hpp>


namespace fwMemory
{

namespace policy
{


BarrierDump::sptr BarrierDump::New()
{
    return BarrierDump::sptr(new BarrierDump());
}

//------------------------------------------------------------------------------

BarrierDump::BarrierDump() :
    m_totalAllocated(0),
    m_totalDumped(0),
    m_barrier(1024*1024*500)
{

}

//------------------------------------------------------------------------------

void BarrierDump::allocationRequest( BufferInfo &info, void **buffer, BufferInfo::SizeType size )
{
    m_totalAllocated -= info.size;
    m_totalAllocated += size;
    if(info.isDumped)
    {
        m_totalDumped -= info.size;
    }
    this->apply();
}

//------------------------------------------------------------------------------


void BarrierDump::setRequest( BufferInfo &info, void **buffer, BufferInfo::SizeType size )
{
    m_totalAllocated -= info.size;
    m_totalAllocated += size;
    if(info.isDumped)
    {
        m_totalDumped -= info.size;
    }
    this->apply();
}

//------------------------------------------------------------------------------


void BarrierDump::reallocateRequest( BufferInfo &info, void **buffer, BufferInfo::SizeType newSize )
{
    m_totalAllocated -= info.size;
    m_totalAllocated += newSize;
    if(info.isDumped)
    {
        m_totalDumped -= info.size;
    }
    this->apply();
}

//------------------------------------------------------------------------------


void BarrierDump::destroyRequest( BufferInfo &info, void **buffer )
{
    if(info.isDumped)
    {
        m_totalDumped -= info.size;
    }
    m_totalAllocated -= info.size;
}

//------------------------------------------------------------------------------


void BarrierDump::lockRequest( BufferInfo &info, void **buffer )
{

    ::fwMemory::BufferManager::sptr manager = m_manager.lock();
    if(manager)
    {
        bool restored = manager->restoreBuffer( buffer );
        OSLM_ASSERT( "restore not OK ( "<< *buffer <<" ).", !restored || *buffer != 0 );
    }
}

//------------------------------------------------------------------------------


void BarrierDump::unlockRequest( BufferInfo &info, void **buffer )
{
    this->apply();
}

//------------------------------------------------------------------------------


void BarrierDump::dumpSuccess( BufferInfo &info, void **buffer )
{
    m_totalDumped += info.size;
}

//------------------------------------------------------------------------------


void BarrierDump::restoreSuccess( BufferInfo &info, void **buffer )
{
    m_totalDumped -= info.size;
}

//------------------------------------------------------------------------------



void BarrierDump::setManager(::fwTools::IBufferManager::sptr manager)
{
    m_manager = ::fwMemory::BufferManager::dynamicCast(manager);
}

//------------------------------------------------------------------------------

size_t BarrierDump::getTotalAlive()
{
    SLM_ASSERT("More dumped data than allocated data." , m_totalAllocated >= m_totalDumped);
    size_t totalAlive = m_totalAllocated - m_totalDumped;
    return totalAlive;
}

//------------------------------------------------------------------------------

bool BarrierDump::isBarrierCrossed()
{
    return m_barrier < getTotalAlive();
}

//------------------------------------------------------------------------------

size_t BarrierDump::dump(size_t nbOfBytes)
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

void BarrierDump::apply()
{
    if(this->isBarrierCrossed())
    {
        this->dump(this->getTotalAlive() - m_barrier);
    }
}

//------------------------------------------------------------------------------

} // namespace policy

} //namespace fwMemory
