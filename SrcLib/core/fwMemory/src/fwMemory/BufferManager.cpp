/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iosfwd> // #include <strstream>
#include <iomanip>
#include <algorithm>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>

#include <fwTools/System.hpp>

#include "fwMemory/stream/in/Raw.hpp"
#include "fwMemory/stream/in/Buffer.hpp"
#include "fwMemory/policy/NeverDump.hpp"
#include "fwMemory/BufferManager.hpp"

namespace fwMemory
{

BufferManager::sptr BufferManager::s_currentManager = BufferManager::New();

::fwCore::mt::ReadWriteMutex BufferManager::s_mutex;

//-----------------------------------------------------------------------------

BufferManager::sptr BufferManager::getCurrent()
{
    ::fwCore::mt::ReadLock lock(s_mutex);
    return s_currentManager;
}

//-----------------------------------------------------------------------------

void BufferManager::setCurrent( const BufferManager::sptr &currentManager )
{
    ::fwCore::mt::WriteLock lock(s_mutex);
    s_currentManager = currentManager;
}

//-----------------------------------------------------------------------------

BufferManager::BufferManager() :
    m_dumpPolicy(::fwMemory::policy::NeverDump::New()),
    m_loadingMode(BufferManager::DIRECT)
{
}

//-----------------------------------------------------------------------------

BufferManager::~BufferManager()
{
    // TODO restore dumped buffers
}

//-----------------------------------------------------------------------------

void BufferManager::registerBuffer(BufferManager::BufferPtrType bufferPtr)
{
    m_bufferInfos[bufferPtr];
    m_updated();
}

//-----------------------------------------------------------------------------

void BufferManager::unregisterBuffer(BufferManager::BufferPtrType bufferPtr)
{

    m_bufferInfos.erase(bufferPtr);
    m_updated();
}

//-----------------------------------------------------------------------------

void BufferManager::allocateBuffer(BufferManager::BufferPtrType bufferPtr, SizeType size,
                                   const ::fwMemory::BufferAllocationPolicy::sptr &policy)
{
    BufferInfo & info = m_bufferInfos[bufferPtr];
    SLM_ASSERT("Buffer has already been allocated", info.loaded && (*bufferPtr == NULL));

    if(!info.loaded)
    {
        info.clear();
    }

    m_dumpPolicy->allocationRequest( info, bufferPtr, size );

    try
    {
        policy->allocate(*bufferPtr, size);
    }
    catch( ::fwMemory::exception::Memory & )
    {
        info.clear();
        throw;
    }

    info.lastAccess.modified();
    info.size = size;
    info.bufferPolicy = policy;
    m_updated();
}

//-----------------------------------------------------------------------------

void BufferManager::setBuffer(BufferManager::BufferPtrType bufferPtr, ::fwMemory::BufferManager::BufferType buffer,
                              SizeType size, const ::fwMemory::BufferAllocationPolicy::sptr &policy)
{
    SLM_ASSERT("Buffer must be freed before setting a new one", *bufferPtr == NULL);

    BufferInfo & info = m_bufferInfos[bufferPtr];

    m_dumpPolicy->setRequest( info, bufferPtr, size );

    if(!info.loaded)
    {
        info.clear();
    }

    *bufferPtr = buffer;

    info.lastAccess.modified();
    info.size = size;
    info.bufferPolicy = policy;
    info.fileFormat = ::fwMemory::OTHER;
    info.fsFile.clear();
    info.istreamFactory = ::boost::make_shared< ::fwMemory::stream::in::Buffer >(*bufferPtr, size, info.lockCounter);
    info.userStreamFactory = false;
    m_updated();
}

//-----------------------------------------------------------------------------

void BufferManager::reallocateBuffer(BufferManager::BufferPtrType bufferPtr, SizeType newSize)
{
    BufferInfo & info = m_bufferInfos[bufferPtr];
    SLM_ASSERT("Buffer must be allocated or dumped", (*bufferPtr != NULL) || !info.loaded);

    m_dumpPolicy->reallocateRequest( info, bufferPtr, newSize );

    try
    {
        if(info.loaded)
        {
            info.bufferPolicy->reallocate(*bufferPtr, newSize);
        }
        else
        {
            this->restoreBuffer( info, bufferPtr, newSize );
        }
    }
    catch( ::fwMemory::exception::Memory & )
    {
        m_updated();
        throw;
    }

    info.lastAccess.modified();
    info.size = newSize;

    m_updated();
}

//-----------------------------------------------------------------------------

void BufferManager::destroyBuffer(BufferManager::BufferPtrType bufferPtr)
{
    BufferInfo & info = m_bufferInfos[bufferPtr];
    SLM_ASSERT("Buffer must be allocated or dumped", (*bufferPtr != NULL) || !info.loaded);

    m_dumpPolicy->destroyRequest( info, bufferPtr );

    if(info.loaded)
    {
        info.bufferPolicy->destroy(*bufferPtr);
    }

    info.clear();
    info.lastAccess.modified();
    m_updated();
}

//-----------------------------------------------------------------------------

void BufferManager::swapBuffer(BufferManager::BufferPtrType bufA, BufferManager::BufferPtrType bufB)
{
    BufferInfo & infoA = m_bufferInfos[bufA];
    BufferInfo & infoB = m_bufferInfos[bufB];

    std::swap(*bufA, *bufB);
    std::swap(infoA.size, infoB.size);
    std::swap(infoA.loaded, infoB.loaded);
    std::swap(infoA.fsFile, infoB.fsFile);
    std::swap(infoA.bufferPolicy, infoB.bufferPolicy);
    std::swap(infoA.istreamFactory, infoB.istreamFactory);
    std::swap(infoA.userStreamFactory, infoB.userStreamFactory);
    infoA.lastAccess.modified();
    infoB.lastAccess.modified();
}
//-----------------------------------------------------------------------------

bool BufferManager::lockBuffer(BufferManager::ConstBufferPtrType bufferPtr, const BufferInfo::CounterType &lockCounter)
{

    BufferManager::BufferPtrType castedBuffer = const_cast< BufferManager::BufferPtrType >(bufferPtr);
    BufferInfo & info = m_bufferInfos[castedBuffer];

    info.lockCounter = lockCounter;

    m_dumpPolicy->lockRequest( info, castedBuffer );

    if ( !info.loaded )
    {
        bool restored = this->restoreBuffer( bufferPtr );
        OSLM_ASSERT( "restore not OK ( "<< restored << " && " << *bufferPtr <<" != 0 ).", restored && *bufferPtr != 0 );
        FwCoreNotUsedMacro(restored);
    }

    m_lastAccess.modified();

    m_updated();
    return true;
}

//-----------------------------------------------------------------------------

bool BufferManager::unlockBuffer(BufferManager::ConstBufferPtrType bufferPtr)
{

    BufferManager::BufferPtrType castedBuffer = const_cast< BufferManager::BufferPtrType >(bufferPtr);
    BufferInfo & info = m_bufferInfos[castedBuffer];

    m_dumpPolicy->unlockRequest( info, castedBuffer );

    m_updated();
    return true;
}

//-----------------------------------------------------------------------------

bool BufferManager::dumpBuffer(BufferManager::ConstBufferPtrType  bufferPtr)
{

    BufferManager::BufferPtrType castedBuffer = const_cast< BufferManager::BufferPtrType >(bufferPtr);
    BufferInfo & info = m_bufferInfos[castedBuffer];

    return this->dumpBuffer(info, castedBuffer);
}


//-----------------------------------------------------------------------------

bool BufferManager::dumpBuffer(BufferInfo & info, BufferManager::BufferPtrType bufferPtr)
{
    if ( !info.loaded || info.lockCount() > 0 || info.size == 0 )
    {
        return false;
    }


    ::boost::filesystem::path tmp = ::fwTools::System::getTemporaryFolder();
    ::boost::filesystem::path dumpedFile = ::boost::filesystem::unique_path( tmp/"fwMemory-%%%%-%%%%-%%%%-%%%%.raw" );

    OSLM_TRACE("dumping " << bufferPtr << " " << dumpedFile);

    if ( this->writeBuffer(*bufferPtr, info.size, dumpedFile) )
    {
        info.fsFile = ::fwMemory::FileHolder(dumpedFile, true);
        info.fileFormat = ::fwMemory::RAW;
        info.istreamFactory = ::boost::make_shared< ::fwMemory::stream::in::Raw >(info.fsFile);
        info.userStreamFactory = false;
        info.bufferPolicy->destroy(*bufferPtr);
        *bufferPtr = NULL;
        info.loaded = false;

        m_dumpPolicy->dumpSuccess( info, bufferPtr );

        m_updated();
    }

    return !info.loaded;
}

//-----------------------------------------------------------------------------

bool BufferManager::restoreBuffer(BufferManager::ConstBufferPtrType  bufferPtr)
{
    BufferManager::BufferPtrType castedBuffer = const_cast< BufferManager::BufferPtrType >(bufferPtr);
    BufferInfo & info = m_bufferInfos[castedBuffer];

    return this->restoreBuffer(info, castedBuffer);
}


//-----------------------------------------------------------------------------

bool BufferManager::restoreBuffer(BufferInfo & info,
                                  BufferManager::BufferPtrType bufferPtr, BufferManager::SizeType allocSize)
{

    allocSize = ((allocSize) ? allocSize : info.size);
    if ( !info.loaded )
    {
        OSLM_TRACE("Restoring " << bufferPtr);

        info.bufferPolicy->allocate(*bufferPtr, allocSize);

        char * charBuf = static_cast< char * >(*bufferPtr);
        SizeType size = std::min(allocSize, info.size);
        bool notFailed = false;
        {
            SPTR(std::istream) isptr = (*info.istreamFactory)();
            std::istream &is = *isptr;
            SizeType read = is.read(charBuf, size).gcount();

            FW_RAISE_IF(" Bad file size, expected: " << size << ", was: " << read, size - read != 0);
            notFailed = !is.fail();
        }

        if ( notFailed )
        {
            info.loaded = true;
            info.fsFile.clear();
            info.lastAccess.modified();

            m_dumpPolicy->restoreSuccess( info, bufferPtr );

            info.fileFormat = ::fwMemory::OTHER;
            info.istreamFactory
                = ::boost::make_shared< ::fwMemory::stream::in::Buffer >(*bufferPtr, allocSize, info.lockCounter);
            info.userStreamFactory = false;
            m_updated();
            return true;
        }

    }
    return false;
}

//-----------------------------------------------------------------------------

bool BufferManager::writeBuffer(BufferManager::ConstBufferType buffer, SizeType size, ::boost::filesystem::path &path)
{
    ::boost::filesystem::ofstream fs(path, std::ios::binary|std::ios::trunc);
    FW_RAISE_IF("Memory management : Unable to open " << path, !fs.good());
    const char * charBuf = static_cast< const char * >(buffer);
    OSLM_TRACE("writing " << path);
    fs.write(charBuf, size);
    fs.close();
    return !fs.bad();
}

//-----------------------------------------------------------------------------

bool BufferManager::readBuffer(BufferManager::BufferType buffer, SizeType size, ::boost::filesystem::path &path)
{
    ::boost::filesystem::ifstream fs(path, std::ios::in|std::ios::binary|std::ios::ate);
    FW_RAISE_IF("Unable to read " << path, !fs.good());

    std::streampos fileSize = fs.tellg();
    fs.seekg(0, std::ios::beg);

    FW_RAISE_IF(path << ": Bad file size, expected: " << size << ", was: " << fileSize,
            size - fileSize != 0);

    OSLM_TRACE("reading " << path);
    char * charBuf = static_cast< char * >(buffer);
    fs.read(charBuf, size);

    fs.close();
    return !fs.bad();
}

//-----------------------------------------------------------------------------

std::string BufferManager::toString() const
{
    std::stringstream sstr ("");
    sstr << "nb Elem = " << m_bufferInfos.size() << std::endl;
    sstr    << std::setw(18) << "Buffer" << "->" << std::setw(18) << "Address" << " "
            << std::setw(10) << "Size" << " "
            << std::setw(18) << "Policy" << " "
            << std::setw(6) << "Access" << " "
            << std::setw(4) << "Lock" << " "
            << "DumpStatus" << " "
            << "File" << " "
            << std::endl;
    BOOST_FOREACH( BufferInfoMapType::value_type item, m_bufferInfos )
    {
        BufferInfo & info = item.second;
        sstr    << std::setw(18) << item.first << "->" << std::setw(18) << *(item.first) << " "
                << std::setw(10) << info.size << " "
                << std::setw(18) << info.bufferPolicy << " "
                << std::setw(6) << info.lastAccess << " "
                << std::setw(4) << info.lockCount() << " "
                << ((info.loaded)?"   ":"not") << " loaded "
                << ::boost::filesystem::path(info.fsFile) << " "
                << std::endl;
    }
    return sstr.str();
}

//-----------------------------------------------------------------------------

void BufferManager::setDumpPolicy( const ::fwMemory::IPolicy::sptr &policy )
{
    m_dumpPolicy = policy;
    policy->setManager(this->getSptr());
    policy->refresh();
}

//-----------------------------------------------------------------------------

::fwMemory::IPolicy::sptr BufferManager::getDumpPolicy() const
{
    return m_dumpPolicy;
}

//-----------------------------------------------------------------------------

BufferManager::SizeType BufferManager::getDumpedBufferSize() const
{
    SizeType dumpedBufferSize = 0;
    BOOST_FOREACH( BufferInfoMapType::value_type item, m_bufferInfos )
    {
        BufferInfo & info = item.second;
        if ( !info.loaded )
        {
            dumpedBufferSize += info.size;
        }
    }
    return dumpedBufferSize;
}

//-----------------------------------------------------------------------------

BufferManager::SizeType BufferManager::getManagedBufferSize() const
{
    SizeType managedBufferSize = 0;
    BOOST_FOREACH( BufferInfoMapType::value_type item, m_bufferInfos )
    {
        BufferInfo & info = item.second;
        managedBufferSize += info.size;
    }
    return managedBufferSize;
}

//-----------------------------------------------------------------------------

bool BufferManager::isLoaded(const BufferManager::ConstBufferPtrType bufferPtr) const
{
    BufferManager::BufferPtrType castedBuffer = const_cast< BufferManager::BufferPtrType >(bufferPtr);
    BufferInfoMapType::const_iterator iterInfo = m_bufferInfos.find(castedBuffer);
    FW_RAISE_IF("Buffer is not managed by fwMemory::BufferManager.", iterInfo == m_bufferInfos.end() );
    return iterInfo->second.loaded;
}

//-----------------------------------------------------------------------------

::boost::filesystem::path BufferManager::getDumpedFilePath(const BufferManager::ConstBufferPtrType bufferPtr) const
{
    BufferManager::BufferPtrType castedBuffer = const_cast< BufferManager::BufferPtrType >(bufferPtr);
    BufferInfoMapType::const_iterator iterInfo = m_bufferInfos.find(castedBuffer);
    FW_RAISE_IF("Buffer is not managed by fwMemory::BufferManager.", iterInfo == m_bufferInfos.end() );
    return iterInfo->second.fsFile;
}

//-----------------------------------------------------------------------------

::fwMemory::FileFormatType BufferManager::getDumpedFileFormat(const BufferManager::ConstBufferPtrType bufferPtr) const
{
    BufferManager::BufferPtrType castedBuffer = const_cast< BufferManager::BufferPtrType >(bufferPtr);
    BufferInfoMapType::const_iterator iterInfo = m_bufferInfos.find(castedBuffer);
    FW_RAISE_IF("Buffer is not managed by fwMemory::BufferManager.", iterInfo == m_bufferInfos.end() );
    return iterInfo->second.fileFormat;
}

//-----------------------------------------------------------------------------

SPTR(std::istream) BufferManager::getIStream(const ConstBufferPtrType bufferPtr) const
{
    BufferInfoMapType::const_iterator iterInfo = m_bufferInfos.find(bufferPtr);
    FW_RAISE_IF("Buffer is not managed by fwMemory::BufferManager.", iterInfo == m_bufferInfos.end() );
    const BufferInfo & info = iterInfo->second;
    SPTR(std::istream) isptr = (*info.istreamFactory)();
    return isptr;
}

//-----------------------------------------------------------------------------

void BufferManager::setIStreamFactory(BufferPtrType bufferPtr,
                                      const SPTR(::fwMemory::stream::in::IFactory) &factory,
                                      SizeType size,
                                      ::fwMemory::FileHolder fsFile,
                                      ::fwMemory::FileFormatType format,
                                      const ::fwMemory::BufferAllocationPolicy::sptr &policy
                                     )
{
    BufferInfoMapType::iterator iterInfo = m_bufferInfos.find(bufferPtr);
    FW_RAISE_IF("Buffer is not managed by fwMemory::BufferManager.", iterInfo == m_bufferInfos.end() );
    BufferInfo & info = iterInfo->second;

    if (info.loaded && *bufferPtr)
    {
        info.bufferPolicy->destroy(*bufferPtr);
        *bufferPtr = NULL;
    }

    info.istreamFactory = factory;
    info.userStreamFactory = true;
    info.size = size;
    info.fsFile = fsFile;
    info.fileFormat = format;
    info.bufferPolicy = policy;
    info.loaded = false;

    switch(m_loadingMode)
    {
    case BufferManager::DIRECT:
        this->restoreBuffer(bufferPtr);
        break;
    case BufferManager::LAZY :
        m_dumpPolicy->dumpSuccess( info, bufferPtr );
        m_updated();
        break;
    default:
        SLM_ASSERT("You shall not pass", 0);
    }

}

//-----------------------------------------------------------------------------

bool BufferManager::hasUserStreamFactory(const BufferManager::ConstBufferPtrType bufferPtr) const
{
    BufferManager::BufferPtrType castedBuffer = const_cast< BufferManager::BufferPtrType >(bufferPtr);
    BufferInfoMapType::const_iterator iterInfo = m_bufferInfos.find(castedBuffer);
    FW_RAISE_IF("Buffer is not managed by fwMemory::BufferManager.", iterInfo == m_bufferInfos.end() );
    return iterInfo->second.userStreamFactory;
}

} //namespace fwMemory



