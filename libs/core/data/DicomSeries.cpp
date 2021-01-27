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

#include "data/DicomSeries.hpp"

#include <core/memory/stream/in/Raw.hpp>

#include <data/Exception.hpp>
#include <data/registry/macros.hpp>

#include <filesystem>

fwDataRegisterMacro( ::sight::data::DicomSeries )

namespace sight::data
{

DicomSeries::DicomSeries(data::Object::Key _key) :
    Series(_key),
    m_numberOfInstances(0),
    m_firstInstanceNumber(0)
{
}

//------------------------------------------------------------------------------

DicomSeries::~DicomSeries()
{
}

//------------------------------------------------------------------------------

void DicomSeries::shallowCopy(const data::Object::csptr& _source)
{
    DicomSeries::csptr other = DicomSeries::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( data::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->data::Series::shallowCopy(_source);

    m_numberOfInstances   = other->m_numberOfInstances;
    m_dicomContainer      = other->m_dicomContainer;
    m_SOPClassUIDs        = other->m_SOPClassUIDs;
    m_computedTagValues   = other->m_computedTagValues;
    m_firstInstanceNumber = other->m_firstInstanceNumber;
}

//------------------------------------------------------------------------------

void DicomSeries::cachedDeepCopy(const data::Object::csptr& _source, DeepCopyCacheType& cache)
{
    DicomSeries::csptr other = DicomSeries::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( data::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->data::Series::cachedDeepCopy(_source, cache);

    m_numberOfInstances   = other->m_numberOfInstances;
    m_SOPClassUIDs        = other->m_SOPClassUIDs;
    m_computedTagValues   = other->m_computedTagValues;
    m_firstInstanceNumber = other->m_firstInstanceNumber;

    m_dicomContainer.clear();
    for(const auto& elt : other->m_dicomContainer)
    {
        const core::memory::BufferObject::sptr& bufferSrc = elt.second;
        core::memory::BufferObject::Lock lockerSource(bufferSrc);

        if( !bufferSrc->isEmpty() )
        {
            core::memory::BufferObject::sptr bufferDest = core::memory::BufferObject::New();
            core::memory::BufferObject::Lock lockerDest(bufferDest);

            bufferDest->allocate(bufferSrc->getSize());

            char* buffDest = static_cast< char* >( lockerDest.getBuffer() );
            char* buffSrc  = static_cast< char* >( lockerSource.getBuffer() );
            std::copy(buffSrc, buffSrc + bufferSrc->getSize(), buffDest );

            m_dicomContainer[elt.first] = bufferDest;
        }
    }
}

//------------------------------------------------------------------------------

void DicomSeries::addDicomPath(std::size_t _instanceIndex, const std::filesystem::path& _path)
{
    core::memory::BufferObject::sptr buffer = core::memory::BufferObject::New();
    const auto buffSize                     = std::filesystem::file_size(_path);
    buffer->setIStreamFactory( std::make_shared< core::memory::stream::in::Raw >(_path),
                               static_cast< core::memory::BufferObject::SizeType>(buffSize), _path, core::memory::RAW);
    m_dicomContainer[_instanceIndex] = buffer;
}

//------------------------------------------------------------------------------

void DicomSeries::addBinary(std::size_t _instanceIndex, const core::memory::BufferObject::sptr& _buffer)
{
    m_dicomContainer[_instanceIndex] = _buffer;
}

//------------------------------------------------------------------------------

bool DicomSeries::isInstanceAvailable(std::size_t _instanceIndex) const
{
    const auto& dicomContainerIter = m_dicomContainer.find(_instanceIndex);
    return (dicomContainerIter != m_dicomContainer.end());
}

//------------------------------------------------------------------------------

void DicomSeries::addSOPClassUID(const std::string& _sopClassUID)
{
    m_SOPClassUIDs.insert(_sopClassUID);
}

//------------------------------------------------------------------------------

void DicomSeries::addComputedTagValue(const std::string& _tagName, const std::string& _value)
{
    m_computedTagValues[_tagName] = _value;
}

//------------------------------------------------------------------------------

bool DicomSeries::hasComputedValues(const std::string& _tagName) const
{
    return m_computedTagValues.find(_tagName) != m_computedTagValues.end();
}

} // namespace sight::data
