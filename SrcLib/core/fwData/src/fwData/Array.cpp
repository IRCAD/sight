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

#include "fwData/registry/macros.hpp"

#include "fwData/Array.hpp"


namespace fwData
{

fwDataRegisterMacro( ::fwData::Array );

inline size_t computeSize(
        size_t elementSize,
        const ::fwData::Array::SizeType &size,
        size_t nbOfComponents )
{
    size_t total = 0;
    if (!size.empty())
    {
        total = elementSize;
        total *= std::accumulate (size.begin(), size.end(), nbOfComponents, std::multiplies< ::fwData::Array::SizeType::value_type >() );
    }
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
    m_attrBufferObject(::fwTools::BufferObject::New()),
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
    m_attrBufferObject->swap(_source->m_attrBufferObject);

    std::swap(m_type, _source->m_type);
    std::swap(m_nbOfComponents, _source->m_nbOfComponents);
    std::swap(m_isBufferOwner, _source->m_isBufferOwner);
}

//------------------------------------------------------------------------------

void Array::deepCopy( Array::csptr _source )
{
    this->fieldDeepCopy( _source );

    this->clear();

    if( !_source->m_attrBufferObject->isEmpty() )
    {
        ::fwTools::BufferObject::Lock lockerDest(m_attrBufferObject);
        this->resize(_source->m_type, _source->m_size, _source->m_nbOfComponents, true);
        char * buffDest = static_cast< char * >( lockerDest.getBuffer() );
        ::fwTools::BufferObject::Lock lockerSource(_source->m_attrBufferObject);
        char * buffSrc = static_cast< char * >( lockerSource.getBuffer() );
        std::copy(buffSrc, buffSrc+_source->getSizeInBytes(), buffDest );
    }
    else
    {
        m_strides        = _source->m_strides;
        m_type           = _source->m_type;
        m_size           = _source->m_size;
        m_nbOfComponents = _source->m_nbOfComponents;
    }
}

//------------------------------------------------------------------------------

size_t Array::resize(
        const ::fwTools::Type &type,
        const SizeType &size,
        size_t nbOfComponents,
        bool reallocate
        ) throw(::fwData::Exception)
{
    nbOfComponents = (nbOfComponents == 0) ? 1 : nbOfComponents;
    size_t oldBufSize = this->getSizeInBytes();
    size_t bufSize = computeSize(type.sizeOf(), size, nbOfComponents);

    if(reallocate && (m_isBufferOwner || m_attrBufferObject->isEmpty()))
    {
        if(m_attrBufferObject->isEmpty())
        {
            m_attrBufferObject->allocate(bufSize);
        }
        else
        {
            m_attrBufferObject->reallocate(bufSize);
        }
        m_isBufferOwner = true;
    }
    else if(reallocate && !m_isBufferOwner)
    {
        FW_RAISE_EXCEPTION_MSG( ::fwData::Exception,
                "Tried to reallocate a not-owned Buffer.");
    }

    m_strides = computeStrides(size, nbOfComponents, type.sizeOf());
    m_type           = type;
    m_size           = size;
    m_nbOfComponents = nbOfComponents;

    return bufSize;
}

//------------------------------------------------------------------------------

size_t Array::resize(const SizeType &size, size_t nbOfComponents, bool reallocate) throw(::fwData::Exception)
{
    return this->resize(m_type, size, nbOfComponents, reallocate);
}

//------------------------------------------------------------------------------

size_t Array::resize(const SizeType &size, bool reallocate) throw(::fwData::Exception)
{
    return this->resize(m_type, size, m_nbOfComponents, reallocate);
}
//------------------------------------------------------------------------------

size_t Array::resize(const std::string &type, const SizeType &size, size_t nbOfComponents, bool reallocate) throw(::fwData::Exception)
{
    ::fwTools::Type fwType = ::fwTools::Type::create(type);
    return this->resize( fwType, size, nbOfComponents, reallocate);
}

//------------------------------------------------------------------------------

void Array::clear()
{
    if ( ! this->m_attrBufferObject->isEmpty() )
    {
        if(m_isBufferOwner)
        {
            this->m_attrBufferObject->destroy();
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


const ::fwData::Array::SizeType &Array::getSize() const
{
    return m_size;
}

//------------------------------------------------------------------------------

const ::fwData::Array::OffsetType &Array::getStrides() const
{
    return m_strides;
}

//------------------------------------------------------------------------------

void Array::setNumberOfComponents(size_t nb)
{
    m_nbOfComponents = (nb == 0) ? 1 : nb;
    this->resize(
            m_type,
            m_size,
            m_nbOfComponents,
            (m_isBufferOwner && !m_attrBufferObject->isEmpty())
            );
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

void Array::setType(const std::string &type)
{
    ::fwTools::Type fwType = ::fwTools::Type::create(type);
    this->setType(fwType);
}

void Array::setType(const ::fwTools::Type &type)
{
    m_type = type;
    this->resize(
            m_type,
            m_size,
            m_nbOfComponents,
            (m_isBufferOwner && !m_attrBufferObject->isEmpty())
            );
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
    offset = std::accumulate(offsets.begin(), offsets.end(), size_t(0));

    offset += component*sizeOfType;

    return offset;
}

//------------------------------------------------------------------------------

}//namespace fwData
