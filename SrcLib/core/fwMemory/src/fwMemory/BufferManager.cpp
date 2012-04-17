/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iosfwd> // #include <strstream>
#include <iomanip>


#include <boost/foreach.hpp>

#include "fwMemory/BufferManager.hpp"

namespace fwMemory
{

//-----------------------------------------------------------------------------

BufferManager::BufferManager()
{}

//-----------------------------------------------------------------------------

BufferManager::~BufferManager()
{}

//-----------------------------------------------------------------------------

bool BufferManager::registerBuffer(void ** buffer, long * lockCount)
{
    SLM_TRACE_FUNC();
    DumpedBufferInfo & info = m_dumpedBufferInfos[buffer];
    info.lockCount = lockCount;
    return false;
}

//-----------------------------------------------------------------------------

bool BufferManager::unregisterBuffer(void ** buffer)
{
    SLM_TRACE_FUNC();
    m_dumpedBufferInfos.erase(buffer);
    return false;
}

//-----------------------------------------------------------------------------

bool BufferManager::allocateBuffer(void ** buffer, SizeType size, ::fwTools::BufferAllocationPolicy::sptr policy)
{
    SLM_TRACE_FUNC();
    DumpedBufferInfo & info = m_dumpedBufferInfos[buffer];
    info.lastAccess.modified();
    info.size = size;
    info.bufferPolicy = policy;
    return true;
}

//-----------------------------------------------------------------------------

bool BufferManager::setBuffer(void ** buffer, SizeType size, ::fwTools::BufferAllocationPolicy::sptr policy)
{
    SLM_TRACE_FUNC();
    DumpedBufferInfo & info = m_dumpedBufferInfos[buffer];
    info.lastAccess.modified();
    info.size = size;
    info.bufferPolicy = policy;
    return true;
}

//-----------------------------------------------------------------------------

bool BufferManager::reallocateBuffer(void ** buffer, SizeType newSize)
{
    SLM_TRACE_FUNC();

    DumpedBufferInfo & info = m_dumpedBufferInfos[buffer];
    info.lastAccess.modified();
    info.size = newSize;
    return true;
}

//-----------------------------------------------------------------------------

bool BufferManager::destroyBuffer(void ** buffer)
{
    SLM_TRACE_FUNC();
    DumpedBufferInfo & info = m_dumpedBufferInfos[buffer];
    info.lastAccess.modified();
    info.size = 0;

    ::fwTools::BufferAllocationPolicy::sptr policy = info.bufferPolicy;
    info.bufferPolicy.reset();

    if(info.isDumped)
    {
        policy->destroy(info.dumpbuf);
        *buffer = 0;
        info.isDumped = false;
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------

bool BufferManager::lockBuffer(const void * const * buffer)
{
    SLM_TRACE_FUNC();
    void **castedBuffer = const_cast<void **>(buffer);
    bool loaded = this->loadBuffer( castedBuffer );
    OSLM_ASSERT( "load not OK ( "<< *castedBuffer <<" ).", !loaded || *castedBuffer != 0 );
    return true;
}

//-----------------------------------------------------------------------------

bool BufferManager::unlockBuffer(const void * const * buffer)
{
    SLM_TRACE_FUNC();
    void **castedBuffer = const_cast<void **>(buffer);
    bool dumped = this->dumpBuffer( castedBuffer );
    OSLM_ASSERT( "Dump not OK ( "<< *castedBuffer <<" ).", !dumped || *castedBuffer == 0 );
    return true;
}

//-----------------------------------------------------------------------------

bool BufferManager::dumpBuffer(void ** buffer)
{
    SLM_TRACE_FUNC();
    DumpedBufferInfoMapType::iterator item = m_dumpedBufferInfos.find(buffer);
    OSLM_ASSERT( "Sorry, try dumping a unreferenced buffer ( "<< buffer <<" ).",item != m_dumpedBufferInfos.end() );
    DumpedBufferInfo & info = item->second;
    if ( info.isDumped || *(info.lockCount) > 0 || info.size == 0 )
    {
        return false;
    }
    OSLM_TRACE("dumping " << buffer);

    info.dumpbuf = *buffer;
    *buffer = 0;

    info.isDumped = true;
    return true;
}

//-----------------------------------------------------------------------------

bool BufferManager::loadBuffer(void ** buffer)
{
    SLM_TRACE_FUNC();
    DumpedBufferInfoMapType::iterator item = m_dumpedBufferInfos.find(buffer);
    OSLM_ASSERT( "Sorry, try dumping a unreferenced buffer ( "<< buffer <<" ).",item != m_dumpedBufferInfos.end() );
    DumpedBufferInfo & info = item->second;

    if ( info.isDumped )
    {
        OSLM_TRACE("loading " << buffer);

        *buffer = info.dumpbuf;
        info.dumpbuf = 0;

        info.isDumped = false;
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------

std::string BufferManager::toString() const
{
    std::stringstream sstr ("");
    sstr << "nb Elem = " << m_dumpedBufferInfos.size() << std::endl;
    sstr    << std::setw(18) << "Buffer" << "->" << std::setw(18) << "Address" << " "
            << std::setw(10) << "Size" << " "
            << std::setw(18) << "Policy" << " "
            << std::setw(6) << "Access" << " "
            << std::setw(4) << "Lock" << " "
            << "DumpStatus" << " "
            << "DumpedFile" << " "
            << std::endl;
    BOOST_FOREACH( DumpedBufferInfoMapType::value_type item, m_dumpedBufferInfos )
    {
        DumpedBufferInfo & info = item.second;
        sstr    << std::setw(18) << item.first << "->" << std::setw(18) << *(item.first) << " "
                << std::setw(10) << info.size << " "
                << std::setw(18) << info.bufferPolicy << " "
                << std::setw(6) << info.lastAccess << " "
                << std::setw(4) << *(info.lockCount) << " "
                << ((info.isDumped)?"    dumped":"not dumped") << " "
                << info.dumpedFile << " "
                << std::endl;
    }
    return sstr.str();
}

//-----------------------------------------------------------------------------

} //namespace fwMemory



