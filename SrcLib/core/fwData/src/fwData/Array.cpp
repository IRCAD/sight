/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <algorithm>
#include <cstdlib>
#include <functional>
#include <numeric>

#include <fwTools/ClassRegistrar.hpp>

#include "fwData/Array.hpp"

namespace fwData
{

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::Array, ::fwData::Array);

inline size_t computeSizeInBytes(
        const ::fwTools::Type &type,
        const ::fwData::Array::SizeType &size,
        const size_t &nbOfComponents )
{
    size_t total = type.sizeOf();
    total *= std::accumulate (size.begin(), size.end(), nbOfComponents, std::multiplies< ::fwData::Array::SizeType::value_type >() );
    return total;
}

::fwData::Array::OffsetType Array::computeStrides( SizeType size, size_t nbOfComponents, size_t sizeOfType )
{
    ::fwData::Array::OffsetType strides;
    strides.reserve(size.size());

    size_t currentStride = sizeOfType*nbOfComponents;
    BOOST_FOREACH(SizeType::value_type s, size)
    {
        strides.push_back(currentStride);
        currentStride *= s;
    }
    return strides;
}

//------------------------------------------------------------------------------

Array::Array():
    m_strides(0),
    m_type(),
    m_buffer(0),
    m_size(0),
    m_nbOfComponents(0),
    m_isBufferOwner(false)
{
}

//------------------------------------------------------------------------------

Array::~Array()
{
}

//------------------------------------------------------------------------------

void Array::shallowCopy( Array::csptr _source )
{
    this->::fwTools::Object::shallowCopyOfChildren( _source );

    this->m_strides        = _source->m_strides;
    this->m_type           = _source->m_type;
    this->m_buffer         = _source->m_buffer;
    this->m_size           = _source->m_size;
    this->m_nbOfComponents = _source->m_nbOfComponents;
    this->m_isBufferOwner  = false;
}

//------------------------------------------------------------------------------

void Array::deepCopy( Array::csptr _source )
{
    this->::fwTools::Object::deepCopyOfChildren( _source );

    this->m_strides        = _source->m_strides;
    this->m_type           = _source->m_type;
    this->m_size           = _source->m_size;
    this->m_nbOfComponents = _source->m_nbOfComponents;

    this->clear();
    size_t length = _source->getSizeInBytes();

    if( _source->m_buffer )
    {
        this->allocate(_source->m_type, _source->m_size, _source->m_nbOfComponents);
        std::copy(_source->begin(), _source->end(), this->begin());
    }
}

//------------------------------------------------------------------------------
char *Array::begin() const
{
    return static_cast<char*>(m_buffer);
}

//------------------------------------------------------------------------------

char *Array::end() const
{
    return reinterpret_cast<char*> (static_cast<char*>(m_buffer) + this->getSizeInBytes());
}


//------------------------------------------------------------------------------

void *Array::getBuffer() const
{
    return m_buffer;
}

//------------------------------------------------------------------------------

size_t Array::allocate(const ::fwTools::Type &type, const SizeType &size, const size_t nbOfComponents) throw(::fwData::Exception)
{
    size_t bufSize = computeSizeInBytes(type, size, nbOfComponents);
    if(m_buffer == NULL)
    {
        m_buffer = malloc( bufSize );
        if (m_buffer == NULL)
        {
            FW_RAISE_EXCEPTION_MSG( ::fwData::Exception,
                    "Was not able to allocate a " << bufSize << " bytes Array." );
        }
        this->m_isBufferOwner  = true;
    }
    else
    {
        FW_RAISE_EXCEPTION_MSG( ::fwData::Exception,
                "Tryed to allocate an already allocated Array ");
    }

    this->m_type = type;
    this->m_size = size;
    this->m_strides = computeStrides(size, nbOfComponents, type.sizeOf());
    this->m_nbOfComponents = nbOfComponents;

    return bufSize;
}

//------------------------------------------------------------------------------

size_t Array::allocate(const std::string &type, const SizeType &size, const size_t nbOfComponents) throw(::fwData::Exception)
{
    ::fwTools::Type type = ::fwTools::Type::create(type);
    return allocate( type, size, nbOfComponents);
}


//------------------------------------------------------------------------------

size_t Array::resize(
        const ::fwTools::Type &type,
        const SizeType &size,
        const size_t nbOfComponents,
        const bool reallocate
        ) throw(::fwData::Exception)
{
    size_t oldBufSize = this->getSizeInBytes();
    size_t bufSize = computeSizeInBytes(type, size, nbOfComponents);

    if(reallocate && m_isBufferOwner)
    {
        if(m_buffer)
        {
            m_buffer = realloc(m_buffer, bufSize);
            if (m_buffer == NULL)
            {
                FW_RAISE_EXCEPTION_MSG( ::fwData::Exception,
                        "Was not able to reallocate " << (bufSize - oldBufSize)
                        << "bytes for the Array." );
            }
        }
        else
        {
            FW_RAISE_EXCEPTION_MSG( ::fwData::Exception,
                    "Tryed to reallocate an non-allocated Array ");
        }
    }
    else if(reallocate && !m_isBufferOwner)
    {
        FW_RAISE_EXCEPTION_MSG( ::fwData::Exception,
                "Tryed to reallocate a not owned Buffer.");
    }

    this->m_strides = computeStrides(size, nbOfComponents, type.sizeOf());
    this->m_type           = type;
    this->m_size           = size;
    this->m_nbOfComponents = nbOfComponents;

    return bufSize;
}


//------------------------------------------------------------------------------

size_t Array::resize(const SizeType &size, const size_t nbOfComponents, const bool reallocate) throw(::fwData::Exception)
{
    return this->resize(this->m_type, size, nbOfComponents, reallocate);
}

//------------------------------------------------------------------------------

size_t Array::resize(const SizeType &size, const bool reallocate) throw(::fwData::Exception)
{
    return this->resize(this->m_type, size, this->m_nbOfComponents, reallocate);
}
//------------------------------------------------------------------------------

size_t Array::resize(const std::string &type, const SizeType &size, const size_t nbOfComponents, const bool reallocate) throw(::fwData::Exception)
{
    ::fwTools::Type type = ::fwTools::Type::create(type);
    return this->resize( type, size, nbOfComponents, reallocate);
}


void Array::clear()
{
    if (m_buffer)
    {
        free(m_buffer);
        m_buffer = 0;

        m_strides.clear();
        m_type = ::fwTools::Type();
        m_size.clear();
        m_nbOfComponents = 0;
        m_isBufferOwner = false;
    }
}


//------------------------------------------------------------------------------

size_t Array::getElementSizeInBytes() const
{
    return m_type.sizeOf() * m_nbOfComponents;
}
//------------------------------------------------------------------------------

size_t Array::getSizeInBytes() const
{
    return computeSizeInBytes(m_type, m_size, m_nbOfComponents);
}

//------------------------------------------------------------------------------


::fwData::Array::SizeType   Array::getSize() const
{
    return m_size;
}

//------------------------------------------------------------------------------

::fwData::Array::OffsetType Array::getStrides() const
{
    return m_strides;
}

//------------------------------------------------------------------------------

size_t Array::getNumberOfComponents() const
{
    return m_nbOfComponents;
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


::fwTools::Type Array::getType() const
{
    return m_type;
}

//------------------------------------------------------------------------------

size_t Array::getBufferOffset( const ::fwData::Array::IndexType &id, size_t component, size_t sizeOfType ) const
{
    OSLM_ASSERT(
            "Given index has " << id.size() << " dimensions, but Array has " << m_size.size() << "dimensions.",
            id.size() == m_size.size()
            );

    OffsetType offsets(id.size());

    std::transform(id.begin(), id.end(), m_strides.begin(), offsets.begin(), std::multiplies<OffsetType::value_type>() );

    size_t offset;
    offset = std::accumulate(offsets.begin(), offsets.end(), 0);

    offset += component*sizeOfType;

    return offset;

}

//------------------------------------------------------------------------------

char *Array::getBufferPtr( const ::fwData::Array::IndexType &id, size_t component, size_t sizeOfType ) const
{
    size_t sizeOf = m_type.sizeOf();
    size_t offset = getBufferOffset(id, component, sizeOf);
    char *item = static_cast<char*>(this->m_buffer) + offset;
    return item;
}

//------------------------------------------------------------------------------

void Array::setItem(const ::fwData::Array::IndexType &id, const void *value)
{
    size_t sizeOf = m_type.sizeOf();
    const char *val  = static_cast<const char*>(value);
    char *item = getBufferPtr(id, 0, sizeOf);
    std::copy(val, val + m_nbOfComponents*sizeOf, item);
}
//------------------------------------------------------------------------------


void Array::setItem(const ::fwData::Array::IndexType &id, const size_t component, const void *value)
{
    size_t sizeOf = m_type.sizeOf();
    const char *val  = static_cast<const char*>(value);
    char *item = getBufferPtr(id, component, sizeOf);
    std::copy(val, val + sizeOf, item);
}


//------------------------------------------------------------------------------

void *Array::getItem(const ::fwData::Array::IndexType &id, const size_t component) const
{
    size_t sizeOf = m_type.sizeOf();
    char *item = getBufferPtr(id, component, sizeOf);
    return item;
}

//------------------------------------------------------------------------------

void Array::getItem(const ::fwData::Array::IndexType &id, void *value) const
{
    size_t sizeOf = m_type.sizeOf();
    const char *item = getBufferPtr(id, 0, sizeOf);
    char *val  = static_cast<char*>(value);
    std::copy(item, item + m_nbOfComponents*sizeOf, val);
}

//------------------------------------------------------------------------------

void Array::getItem(const ::fwData::Array::IndexType &id, const size_t component, void *value) const
{
    size_t sizeOf = m_type.sizeOf();
    const char *item = getBufferPtr(id, component, sizeOf);
    char *val  = static_cast<char*>(value);
    std::copy(item, item + m_nbOfComponents*sizeOf, val);
}

//------------------------------------------------------------------------------




}//namespace fwData
