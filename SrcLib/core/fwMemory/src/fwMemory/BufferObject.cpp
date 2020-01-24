/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#define CAMP_COMPILATION

#include "fwMemory/BufferObject.hpp"

fwCampImplementMacro((fwMemory)(BufferObject))
{
    builder.tag("buffer")
    .function("classname", &::fwMemory::BufferObject::className)
    .function("is_a", (bool(::fwMemory::BufferObject::*)(const std::string&) const) & ::fwMemory::BufferObject::isA);
}

namespace fwMemory
{

BufferObject::BufferObject() :
    m_buffer(0),
    m_size(0),
    m_bufferManager(::fwMemory::BufferManager::getDefault()),
    m_allocPolicy(::fwMemory::BufferNoAllocPolicy::New())
{
    m_bufferManager->registerBuffer(&m_buffer).get();
}

//------------------------------------------------------------------------------

BufferObject::~BufferObject()
{
    // In the past we asserted that m_count was expired, but it can not be ensured because the unlock is asynchronous
    // So we simply unregister the buffer and we will check the counter value on the buffer manager thread instead
    m_bufferManager->unregisterBuffer(&m_buffer).get();
}

//------------------------------------------------------------------------------

void BufferObject::allocate(SizeType size, const ::fwMemory::BufferAllocationPolicy::sptr& policy)
{
    m_bufferManager->allocateBuffer(&m_buffer, size, policy).get();
    m_allocPolicy = policy;
    m_size        = size;
}

//------------------------------------------------------------------------------

void BufferObject::reallocate(SizeType size)
{
    m_bufferManager->reallocateBuffer(&m_buffer, size).get();
    m_size = size;
}

//------------------------------------------------------------------------------

void BufferObject::destroy()
{
    m_bufferManager->destroyBuffer(&m_buffer).get();
    m_allocPolicy = ::fwMemory::BufferNoAllocPolicy::New();
    m_size        = 0;
}

//------------------------------------------------------------------------------

void BufferObject::setBuffer(::fwMemory::BufferManager::BufferType buffer, SizeType size,
                             const ::fwMemory::BufferAllocationPolicy::sptr& policy)
{
    m_bufferManager->setBuffer(&m_buffer, buffer, size, policy).get();
    m_allocPolicy = policy;
    m_size        = size;
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

void BufferObject::swap( const BufferObject::sptr& _source )
{
    m_bufferManager->swapBuffer(&m_buffer, &(_source->m_buffer)).get();

    std::swap(m_size, _source->m_size);
    m_bufferManager.swap(_source->m_bufferManager);
    m_allocPolicy.swap(_source->m_allocPolicy);
}

//------------------------------------------------------------------------------

BufferManager::StreamInfo BufferObject::getStreamInfo() const
{
    return m_bufferManager->getStreamInfo(&m_buffer).get();
}

//------------------------------------------------------------------------------

void BufferObject::setIStreamFactory(const SPTR(::fwMemory::stream::in::IFactory)& factory,
                                     SizeType size,
                                     const std::filesystem::path& sourceFile,
                                     ::fwMemory::FileFormatType format,
                                     const ::fwMemory::BufferAllocationPolicy::sptr& policy
                                     )
{
    m_size        = size;
    m_allocPolicy = policy;
    m_bufferManager->setIStreamFactory(&m_buffer, factory, size, sourceFile, format, policy).get();
}

} //namespace fwMemory
