/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwMemory/BufferObject.hpp"

fwCampImplementMacro((fwMemory)(BufferObject))
{
    builder.tag("buffer")
           .function("classname", &::fwMemory::BufferObject::className)
           .function("is_a", (bool (::fwMemory::BufferObject::*)(const std::string &) const) &::fwMemory::BufferObject::isA);
}

namespace fwMemory
{



BufferObject::BufferObject():
    m_buffer(0),
    m_size(0),
    m_bufferManager(::fwMemory::BufferManager::getCurrent()),
    m_allocPolicy(::fwMemory::BufferNoAllocPolicy::New())
{
    m_bufferManager->registerBuffer(&m_buffer);
}

//------------------------------------------------------------------------------

BufferObject::~BufferObject()
{
    OSLM_ASSERT("There is still " << m_count.use_count() << " locks on this BufferObject (" << this << ")",
                m_count.expired());
    m_bufferManager->unregisterBuffer(&m_buffer);
}

//------------------------------------------------------------------------------

void BufferObject::allocate(SizeType size, const ::fwMemory::BufferAllocationPolicy::sptr &policy)
{
    // if(m_bufferManager->allocateBuffer(&m_buffer, size, policy))
    // {
    //     policy->allocate(m_buffer, size);
    //     m_bufferManager->setBuffer(&m_buffer, size, policy);
    // }

    m_bufferManager->allocateBuffer(&m_buffer, size, policy);
    m_allocPolicy = policy;
    m_size = size;
}

//------------------------------------------------------------------------------

void BufferObject::reallocate(SizeType size)
{
    // if(m_bufferManager->reallocateBuffer(&m_buffer, size))
    // {
    //     m_allocPolicy->reallocate(m_buffer, size);
    //     m_bufferManager->setBuffer(&m_buffer, size, m_allocPolicy);
    // }

    m_bufferManager->reallocateBuffer(&m_buffer, size);
    m_size = size;
}

//------------------------------------------------------------------------------

void BufferObject::destroy()
{
    // if(m_bufferManager->destroyBuffer(&m_buffer))
    // {
    //     m_allocPolicy->destroy(m_buffer);
    // }
    m_bufferManager->destroyBuffer(&m_buffer);
    m_allocPolicy = ::fwMemory::BufferNoAllocPolicy::New();
    m_size = 0;
}

//------------------------------------------------------------------------------

void BufferObject::setBuffer(::fwMemory::BufferManager::BufferType buffer, SizeType size,
                             const ::fwMemory::BufferAllocationPolicy::sptr &policy)
{
    // m_buffer = buffer;

    m_bufferManager->setBuffer(&m_buffer, buffer, size, policy);
    m_allocPolicy = policy;
    m_size   = size;
}

//------------------------------------------------------------------------------

BufferObject::Lock BufferObject::lock()
{
    return BufferObject::Lock(this->getSptr());
}

//------------------------------------------------------------------------------

BufferObject::ConstLock BufferObject::lock() const
{
    return BufferObject::ConstLock(this->getConstSptr());
}

//------------------------------------------------------------------------------

void BufferObject::swap( const BufferObject::sptr &_source )
{
    // if (m_bufferManager->swapBuffer(&m_buffer, &(_source->m_buffer)))
    // {
    //     std::swap(m_buffer, _source->m_buffer);
    // }
    m_bufferManager->swapBuffer(&m_buffer, &(_source->m_buffer));

    std::swap(m_size, _source->m_size);
    m_bufferManager.swap(_source->m_bufferManager);
    m_allocPolicy.swap(_source->m_allocPolicy);
}

//------------------------------------------------------------------------------

SPTR(std::istream) BufferObject::getIStream()
{
    SPTR(std::istream) is = m_bufferManager->getIStream(&m_buffer);
    return is;
}

//------------------------------------------------------------------------------

boost::filesystem::path BufferObject::getFile()
{
    return m_bufferManager->getDumpedFilePath(&m_buffer);
}

//------------------------------------------------------------------------------

::fwMemory::FileFormatType BufferObject::getFileFormat()
{
    return m_bufferManager->getDumpedFileFormat(&m_buffer);
}

//------------------------------------------------------------------------------

void BufferObject::setIStreamFactory(const SPTR(::fwMemory::stream::in::IFactory) &factory,
                        SizeType size,
                        const ::boost::filesystem::path &sourceFile,
                        ::fwMemory::FileFormatType format,
                        const ::fwMemory::BufferAllocationPolicy::sptr &policy
                        )
{
    m_size = size;
    m_allocPolicy = policy;
    m_bufferManager->setIStreamFactory(&m_buffer, factory, size, sourceFile, format, policy);
}


} //namespace fwMemory


