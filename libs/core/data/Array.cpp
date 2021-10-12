/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "data/Array.hpp"

#include "data/Exception.hpp"
#include "data/registry/macros.hpp"

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <numeric>

namespace sight::data
{

SIGHT_REGISTER_DATA(sight::data::Array);

//------------------------------------------------------------------------------

inline size_t computeSize(
    size_t elementSize,
    const data::Array::SizeType& size
)
{
    size_t total = 0;
    if(!size.empty())
    {
        total  = elementSize;
        total *=
            std::accumulate(
                size.begin(),
                size.end(),
                static_cast<size_t>(1),
                std::multiplies<data::Array::SizeType::value_type>()
            );
    }

    return total;
}

//------------------------------------------------------------------------------

data::Array::OffsetType Array::computeStrides(SizeType size, size_t sizeOfType)
{
    data::Array::OffsetType strides;
    strides.reserve(size.size());

    size_t currentStride = sizeOfType;
    for(const SizeType::value_type& s : size)
    {
        strides.push_back(currentStride);
        currentStride *= s;
    }

    return strides;
}

//------------------------------------------------------------------------------

Array::Array(data::Object::Key) :
    m_bufferObject(core::memory::BufferObject::New())
{
}

//------------------------------------------------------------------------------

Array::~Array()
{
    this->clear();
}

//------------------------------------------------------------------------------

void Array::swap(Array::sptr _source)
{
    m_fields.swap(_source->m_fields);
    m_strides.swap(_source->m_strides);
    m_size.swap(_source->m_size);
    m_bufferObject->swap(_source->m_bufferObject);

    std::swap(m_type, _source->m_type);
    std::swap(m_isBufferOwner, _source->m_isBufferOwner);
}

//------------------------------------------------------------------------------

void Array::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache)
{
    Array::csptr other = Array::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );
    this->fieldDeepCopy(_source, cache);

    this->clear();

    if(!other->m_bufferObject->isEmpty())
    {
        core::memory::BufferObject::Lock lockerDest(m_bufferObject);
        this->resize(other->m_size, other->m_type);
        char* buffDest = static_cast<char*>(lockerDest.getBuffer());
        core::memory::BufferObject::Lock lockerSource(other->m_bufferObject);
        char* buffSrc = static_cast<char*>(lockerSource.getBuffer());
        std::copy(buffSrc, buffSrc + other->getSizeInBytes(), buffDest);
    }
    else
    {
        m_strides = other->m_strides;
        m_type    = other->m_type;
        m_size    = other->m_size;
    }
}

//------------------------------------------------------------------------------

size_t Array::resize(
    const SizeType& size,
    const core::tools::Type& type,
    bool reallocate
)
{
    const size_t bufSize = computeSize(type.sizeOf(), size);

    if(reallocate && (m_isBufferOwner || m_bufferObject->isEmpty()))
    {
        if(m_bufferObject->isEmpty())
        {
            m_bufferObject->allocate(bufSize);
        }
        else
        {
            m_bufferObject->reallocate(bufSize);
        }

        m_isBufferOwner = true;
    }
    else if(reallocate && !m_isBufferOwner)
    {
        SIGHT_THROW_EXCEPTION_MSG(
            data::Exception,
            "Tried to reallocate a not-owned Buffer."
        );
    }

    m_strides = computeStrides(size, type.sizeOf());
    m_type    = type;
    m_size    = size;

    return bufSize;
}

//------------------------------------------------------------------------------

size_t Array::resize(const SizeType& size, bool reallocate)
{
    SIGHT_ASSERT("Type should have been set by a previous resize() call", !m_type.isOfType<void>());

    return this->resize(size, m_type, reallocate);
}

//------------------------------------------------------------------------------

void Array::clear()
{
    if(!this->m_bufferObject->isEmpty())
    {
        if(m_isBufferOwner)
        {
            this->m_bufferObject->destroy();
        }

        m_strides.clear();
        m_type = core::tools::Type();
        m_size.clear();
    }
}

//------------------------------------------------------------------------------

bool Array::empty() const
{
    return m_size.empty();
}

//------------------------------------------------------------------------------

size_t Array::getElementSizeInBytes() const
{
    return m_type.sizeOf();
}

//------------------------------------------------------------------------------

size_t Array::numElements() const
{
    return computeSize(1, m_size);
}

//------------------------------------------------------------------------------

size_t Array::getSizeInBytes() const
{
    return computeSize(m_type.sizeOf(), m_size);
}

//------------------------------------------------------------------------------

const data::Array::SizeType& Array::getSize() const
{
    return m_size;
}

//------------------------------------------------------------------------------

const data::Array::OffsetType& Array::getStrides() const
{
    return m_strides;
}

//------------------------------------------------------------------------------

size_t Array::numDimensions() const
{
    return m_size.size();
}

//------------------------------------------------------------------------------

void Array::setIsBufferOwner(const bool own)
{
    m_isBufferOwner = own;
}

//------------------------------------------------------------------------------

bool Array::getIsBufferOwner() const
{
    return m_isBufferOwner;
}

//------------------------------------------------------------------------------

core::tools::Type Array::getType() const
{
    return m_type;
}

//------------------------------------------------------------------------------

size_t Array::getBufferOffset(const data::Array::IndexType& id) const
{
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Given index has " + std::to_string(id.size()) + " dimensions, but Array has "
            + std::to_string(m_size.size()) + " dimensions."
        ),
        id.size() != m_size.size()
    );

    OffsetType offsets(id.size());

    std::transform(
        id.begin(),
        id.end(),
        m_strides.begin(),
        offsets.begin(),
        std::multiplies<OffsetType::value_type>()
    );

    const size_t offset = std::accumulate(offsets.begin(), offsets.end(), static_cast<size_t>(0));

    return offset;
}

//------------------------------------------------------------------------------

void* Array::getBuffer()
{
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "The buffer cannot be accessed if the array is not locked for dump "
            "(see lock())"
        ),
        !m_bufferObject->isLocked()
    );
    return m_bufferObject->getBuffer();
}

//-----------------------------------------------------------------------------

const void* Array::getBuffer() const
{
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception("The buffer cannot be accessed if the array is not locked"),
        !m_bufferObject->isLocked()
    );
    return m_bufferObject->getBuffer();
}

//------------------------------------------------------------------------------

void Array::setBuffer(void* buf, bool takeOwnership, core::memory::BufferAllocationPolicy::sptr policy)
{
    if(m_bufferObject)
    {
        if(!m_bufferObject->isEmpty())
        {
            m_bufferObject->destroy();
        }
    }
    else
    {
        core::memory::BufferObject::sptr newBufferObject = core::memory::BufferObject::New();
        m_bufferObject->swap(newBufferObject);
    }

    m_bufferObject->setBuffer(buf, (buf == nullptr) ? 0 : this->getSizeInBytes(), policy);
    this->setIsBufferOwner(takeOwnership);
}

//------------------------------------------------------------------------------

void Array::setBuffer(
    void* buf,
    bool takeOwnership,
    const data::Array::SizeType& size,
    const core::tools::Type& type,
    core::memory::BufferAllocationPolicy::sptr policy
)
{
    this->resize(size, type, false);
    this->setBuffer(buf, takeOwnership, policy);
}

//-----------------------------------------------------------------------------

char* Array::getBufferPtr(const data::Array::IndexType& id)
{
    const size_t offset = this->getBufferOffset(id);
    char* item          = static_cast<char*>(this->getBuffer()) + offset;
    return item;
}

//------------------------------------------------------------------------------

const char* Array::getBufferPtr(const data::Array::IndexType& id) const
{
    const size_t offset = this->getBufferOffset(id);
    const char* item    = static_cast<const char*>(this->getBuffer()) + offset;
    return item;
}

//------------------------------------------------------------------------------

core::memory::BufferObject::Lock Array::lock() const
{
    return m_bufferObject->lock();
}

//------------------------------------------------------------------------------

void Array::lockBuffer(std::vector<core::memory::BufferObject::Lock>& locks) const
{
    locks.push_back(this->lock());
}

//------------------------------------------------------------------------------

Array::iterator<char> Array::begin()
{
    return iterator<char>(static_cast<char*>(getBuffer()));
}

//------------------------------------------------------------------------------

Array::iterator<char> Array::end()
{
    auto itr = iterator<char>(static_cast<char*>(getBuffer()));
    itr += static_cast<std::ptrdiff_t>(this->getSizeInBytes());
    return itr;
}

//------------------------------------------------------------------------------

Array::const_iterator<char> Array::begin() const
{
    return const_iterator<char>(static_cast<const char*>(getBuffer()));
}

//------------------------------------------------------------------------------

Array::const_iterator<char> Array::end() const
{
    auto itr = const_iterator<char>(static_cast<const char*>(getBuffer()));
    itr += static_cast<std::ptrdiff_t>(this->getSizeInBytes());
    return itr;
}

//------------------------------------------------------------------------------

} //namespace sight::data
