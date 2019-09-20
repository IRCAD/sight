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

#include "fwData/Array.hpp"

#include "fwData/Exception.hpp"
#include "fwData/registry/macros.hpp"

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <numeric>

namespace fwData
{

fwDataRegisterMacro( ::fwData::Array );

//------------------------------------------------------------------------------

inline size_t computeSize(
    size_t elementSize,
    const ::fwData::Array::SizeType& size,
    size_t nbOfComponents )
{
    size_t total = 0;
    if (!size.empty())
    {
        total  = elementSize;
        total *=
            std::accumulate(size.begin(),
                            size.end(), nbOfComponents, std::multiplies< ::fwData::Array::SizeType::value_type >() );
    }
    return total;
}

//------------------------------------------------------------------------------

::fwData::Array::OffsetType Array::computeStrides( SizeType size, size_t sizeOfType )
{
    ::fwData::Array::OffsetType strides;
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

Array::Array( ::fwData::Object::Key /*key*/ ) :
    m_strides(0),
    m_type(),
    m_bufferObject(::fwMemory::BufferObject::New()),
    m_size(0),
    m_nbOfComponents(0),
    m_isBufferOwner(true)
{
}

//------------------------------------------------------------------------------

Array::~Array()
{
    this->clear();
}

//------------------------------------------------------------------------------

void Array::swap( Array::sptr _source )
{
    m_fields.swap(_source->m_fields);
    m_strides.swap(_source->m_strides);
    m_size.swap(_source->m_size);
    m_bufferObject->swap(_source->m_bufferObject);

    std::swap(m_type, _source->m_type);
    std::swap(m_nbOfComponents, _source->m_nbOfComponents);
    std::swap(m_isBufferOwner, _source->m_isBufferOwner);
}

//------------------------------------------------------------------------------

void Array::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache)
{
    Array::csptr other = Array::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source, cache );

    this->clear();

    if( !other->m_bufferObject->isEmpty() )
    {
        ::fwMemory::BufferObject::Lock lockerDest(m_bufferObject);
        this->resize(other->m_type, other->m_size, other->m_nbOfComponents, true);
        char* buffDest = static_cast< char* >( lockerDest.getBuffer() );
        ::fwMemory::BufferObject::Lock lockerSource(other->m_bufferObject);
        char* buffSrc = static_cast< char* >( lockerSource.getBuffer() );
        std::copy(buffSrc, buffSrc+other->getSizeInBytes(), buffDest );
    }
    else
    {
        m_strides        = other->m_strides;
        m_type           = other->m_type;
        m_size           = other->m_size;
        m_nbOfComponents = other->m_nbOfComponents;
    }
}

//------------------------------------------------------------------------------

size_t Array::resize(const SizeType& size, bool reallocate)
{
    // TEMPORARY to support deprecated API < 22.0 : to replace by this->resize(size, m_type, reallocate);
    m_nbOfComponents = (m_nbOfComponents == 0) ? 1 : m_nbOfComponents;
    const size_t bufSize = computeSize(m_type.sizeOf(), size, m_nbOfComponents);

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
        FW_RAISE_EXCEPTION_MSG( ::fwData::Exception,
                                "Tried to reallocate a not-owned Buffer.");
    }

    m_strides = computeStrides(size, m_nbOfComponents, m_type.sizeOf());
    m_size    = size;

    return bufSize;
}

//------------------------------------------------------------------------------

void Array::clear()
{
    if ( !this->m_bufferObject->isEmpty() )
    {
        if(m_isBufferOwner)
        {
            this->m_bufferObject->destroy();
        }
        m_strides.clear();
        m_type = ::fwTools::Type();
        m_size.clear();
        m_nbOfComponents = 0;
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
    return m_type.sizeOf() * m_nbOfComponents;
}

//------------------------------------------------------------------------------

size_t Array::getNumberOfElements() const
{
    return computeSize(1, m_size, m_nbOfComponents);
}

//------------------------------------------------------------------------------

size_t Array::getSizeInBytes() const
{
    return computeSize(m_type.sizeOf(), m_size, m_nbOfComponents);
}

//------------------------------------------------------------------------------

const ::fwData::Array::SizeType& Array::getSize() const
{
    return m_size;
}

//------------------------------------------------------------------------------

const ::fwData::Array::OffsetType& Array::getStrides() const
{
    return m_strides;
}

//------------------------------------------------------------------------------

size_t Array::getNumberOfDimensions() const
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

void Array::setType(const std::string& type)
{
    const ::fwTools::Type fwType = ::fwTools::Type::create(type);
    this->setType(fwType);
}

//------------------------------------------------------------------------------

void Array::setType(const ::fwTools::Type& type)
{
    m_type = type;
    this->resize(
        m_size,
        (m_isBufferOwner && !m_bufferObject->isEmpty())
        );
}

//------------------------------------------------------------------------------

::fwTools::Type Array::getType() const
{
    return m_type;
}

//------------------------------------------------------------------------------

size_t Array::getBufferOffset( const ::fwData::Array::IndexType& id ) const
{
    FW_RAISE_EXCEPTION_IF(
        ::fwData::Exception("Given index has " + std::to_string(id.size()) + " dimensions, but Array has " +
                            std::to_string(m_size.size()) + " dimensions."),
        id.size() != m_size.size() );

    OffsetType offsets(id.size());

    std::transform(id.begin(), id.end(), m_strides.begin(), offsets.begin(),
                   std::multiplies<OffsetType::value_type>() );

    const size_t offset = std::accumulate(offsets.begin(), offsets.end(), static_cast<size_t>(0));

    return offset;
}

//------------------------------------------------------------------------------
// New API
//------------------------------------------------------------------------------

size_t Array::resize(
    const SizeType& size,
    const ::fwTools::Type& type,
    bool reallocate
    )
{
    const size_t bufSize = computeSize(type.sizeOf(), size, 1);

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
        FW_RAISE_EXCEPTION_MSG( ::fwData::Exception,
                                "Tried to reallocate a not-owned Buffer.");
    }

    m_strides        = computeStrides(size, type.sizeOf());
    m_type           = type;
    m_size           = size;
    m_nbOfComponents = 1;

    return bufSize;
}

//------------------------------------------------------------------------------

void* Array::getBuffer()
{
    FW_RAISE_EXCEPTION_IF(::fwData::Exception("The buffer cannot be accessed if the array is not locked"),
                          !m_bufferObject->isLocked());
    return m_bufferObject->getBuffer();
}

//-----------------------------------------------------------------------------

const void* Array::getBuffer() const
{
    FW_RAISE_EXCEPTION_IF(::fwData::Exception("The buffer cannot be accessed if the array is not locked"),
                          !m_bufferObject->isLocked());
    return m_bufferObject->getBuffer();
}

//------------------------------------------------------------------------------

void Array::setBuffer(void* buf, bool takeOwnership, ::fwMemory::BufferAllocationPolicy::sptr policy)
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
        ::fwMemory::BufferObject::sptr newBufferObject = ::fwMemory::BufferObject::New();
        m_bufferObject->swap(newBufferObject);
    }
    m_bufferObject->setBuffer(buf, (buf == nullptr) ? 0 : this->getSizeInBytes(), policy);
    this->setIsBufferOwner(takeOwnership);
}

//------------------------------------------------------------------------------

void Array::setBuffer(
    void* buf,
    bool takeOwnership,
    const ::fwData::Array::SizeType& size,
    const ::fwTools::Type& type,
    ::fwMemory::BufferAllocationPolicy::sptr policy)
{
    this->resize( size, type, false);
    this->setBuffer(buf, takeOwnership, policy);
}

//-----------------------------------------------------------------------------

char* Array::getBufferPtr( const ::fwData::Array::IndexType& id)
{
    const size_t offset = this->getBufferOffset(id);
    char* item          = static_cast<char*>(this->getBuffer()) + offset;
    return item;
}

//------------------------------------------------------------------------------

const char* Array::getBufferPtr( const ::fwData::Array::IndexType& id) const
{
    const size_t offset = this->getBufferOffset(id);
    const char* item    = static_cast<const char*>(this->getBuffer()) + offset;
    return item;
}

//------------------------------------------------------------------------------

::fwMemory::BufferObject::Lock Array::lock() const
{
    return m_bufferObject->lock();
}

//------------------------------------------------------------------------------

Array::Iterator<char> Array::begin()
{
    return Iterator<char>(this);
}

//------------------------------------------------------------------------------

Array::Iterator<char> Array::end()
{
    auto itr = Iterator<char>(this);
    itr += static_cast<std::ptrdiff_t>(this->getSizeInBytes());
    return itr;
}

//------------------------------------------------------------------------------

Array::ConstIterator<char> Array::begin() const
{
    return ConstIterator<char>(this);
}

//------------------------------------------------------------------------------

Array::ConstIterator<char> Array::end() const
{
    auto itr = ConstIterator<char>(this);
    itr += static_cast<std::ptrdiff_t>(this->getSizeInBytes());
    return itr;
}

//------------------------------------------------------------------------------
// Deprecated API
//------------------------------------------------------------------------------

::fwData::Array::OffsetType Array::computeStrides( SizeType size, size_t nbOfComponents, size_t sizeOfType )
{
    FW_DEPRECATED_MSG("computeStrides(SizeType size, size_t nbOfComponents, size_t sizeOfType) is deprecated, use "
                      "computeStrides(SizeType size, size_t sizeOfType) instead",
                      "22.0");
    ::fwData::Array::OffsetType strides;
    strides.reserve(size.size());

    size_t currentStride = sizeOfType*nbOfComponents;
    for(const SizeType::value_type& s : size)
    {
        strides.push_back(currentStride);
        currentStride *= s;
    }
    return strides;
}

//------------------------------------------------------------------------------

size_t Array::resize(
    const ::fwTools::Type& type,
    const SizeType& size,
    size_t nbOfComponents,
    bool reallocate
    )
{
    FW_DEPRECATED("resize(const std::string &type, const SizeType &size, size_t nbOfComponents, bool reallocate",
                  "resize(const SizeType &size, const ::fwTools::Type &type, bool reallocate)",
                  "22.0")
    m_nbOfComponents = nbOfComponents;
    m_type           = type;

    return this->resize(size, reallocate);
}

//------------------------------------------------------------------------------

size_t Array::resize(const SizeType& size, size_t nbOfComponents, bool reallocate)
{
    FW_DEPRECATED("resize(const SizeType &size, size_t nbOfComponents, bool reallocate",
                  "resize(const SizeType &size, const ::fwTools::Type &type, bool reallocate)",
                  "22.0")
    m_nbOfComponents = nbOfComponents;
    return this->resize(size, reallocate);
}

//------------------------------------------------------------------------------

size_t Array::resize(const std::string& type, const SizeType& size, size_t nbOfComponents,
                     bool reallocate)
{
    FW_DEPRECATED("resize(const std::string &type, const SizeType &size, size_t nbOfComponents, bool reallocate)",
                  "resize(const SizeType &size, const ::fwTools::Type &type, bool reallocate)",
                  "22.0");
    m_nbOfComponents = nbOfComponents;
    m_type           = ::fwTools::Type::create(type);
    return this->resize( size, reallocate);
}

//------------------------------------------------------------------------------

void Array::setNumberOfComponents(size_t nb)
{
    FW_DEPRECATED_MSG("Component attribute is deprecated, increase array dimension instead of using component", "22.0");

    m_nbOfComponents = (nb == 0) ? 1 : nb;
    this->resize(
        m_type,
        m_size,
        m_nbOfComponents,
        (m_isBufferOwner && !m_bufferObject->isEmpty())
        );
}

//------------------------------------------------------------------------------

size_t Array::getNumberOfComponents() const
{
    FW_DEPRECATED_MSG("Component attribute is deprecated, increase array dimension instead of using component", "22.0");
    return m_nbOfComponents;
}

//------------------------------------------------------------------------------

size_t Array::getBufferOffset( const ::fwData::Array::IndexType& id, size_t component, size_t sizeOfType) const
{
    FW_DEPRECATED("getBufferOffset( const ::fwData::Array::IndexType& id, size_t component, size_t sizeOfType )",
                  "getBufferOffset( const ::fwData::Array::IndexType& id, size_t sizeOfType )",
                  "22.0");

    OSLM_ASSERT(
        "Given index has " << id.size() << " dimensions, but Array has " << m_size.size() << "dimensions.",
            id.size() == m_size.size()
        );

    OffsetType offsets(id.size());

    std::transform(id.begin(), id.end(), m_strides.begin(), offsets.begin(),
                   std::multiplies<OffsetType::value_type>() );

    size_t offset;
    offset  = std::accumulate(offsets.begin(), offsets.end(), size_t(0));
    offset += component*sizeOfType;

    return offset;
}

//------------------------------------------------------------------------------

} //namespace fwData
