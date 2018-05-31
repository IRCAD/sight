/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwMedData/DicomSeries.hpp"

#include <fwData/Exception.hpp>
#include <fwData/registry/macros.hpp>

#include <fwMemory/stream/in/Raw.hpp>

#include <boost/filesystem/operations.hpp>

fwDataRegisterMacro( ::fwMedData::DicomSeries );

namespace fwMedData
{

DicomSeries::DicomSeries(::fwData::Object::Key key) :
    Series(key),
    m_numberOfInstances(0),
    m_firstInstanceNumber(0)
{
}

//------------------------------------------------------------------------------

DicomSeries::~DicomSeries()
{
}

//------------------------------------------------------------------------------

void DicomSeries::shallowCopy(const ::fwData::Object::csptr& _source)
{
    DicomSeries::csptr other = DicomSeries::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->::fwMedData::Series::shallowCopy(_source);

    m_numberOfInstances   = other->m_numberOfInstances;
    m_dicomContainer      = other->m_dicomContainer;
    m_SOPClassUIDs        = other->m_SOPClassUIDs;
    m_computedTagValues   = other->m_computedTagValues;
    m_firstInstanceNumber = other->m_firstInstanceNumber;
}

//------------------------------------------------------------------------------

void DicomSeries::cachedDeepCopy(const ::fwData::Object::csptr& _source, DeepCopyCacheType& cache)
{
    DicomSeries::csptr other = DicomSeries::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->::fwMedData::Series::cachedDeepCopy(_source, cache);

    m_numberOfInstances   = other->m_numberOfInstances;
    m_SOPClassUIDs        = other->m_SOPClassUIDs;
    m_computedTagValues   = other->m_computedTagValues;
    m_firstInstanceNumber = other->m_firstInstanceNumber;

    m_dicomContainer.clear();
    for(const auto& elt : other->m_dicomContainer)
    {
        const ::fwMemory::BufferObject::sptr& bufferSrc = elt.second;
        ::fwMemory::BufferObject::Lock lockerSource(bufferSrc);

        if( !bufferSrc->isEmpty() )
        {
            ::fwMemory::BufferObject::sptr bufferDest = ::fwMemory::BufferObject::New();
            ::fwMemory::BufferObject::Lock lockerDest(bufferDest);

            bufferDest->allocate(bufferSrc->getSize());

            char* buffDest = static_cast< char* >( lockerDest.getBuffer() );
            char* buffSrc  = static_cast< char* >( lockerSource.getBuffer() );
            std::copy(buffSrc, buffSrc + bufferSrc->getSize(), buffDest );

            m_dicomContainer[elt.first] = bufferDest;
        }
    }
}

//------------------------------------------------------------------------------

void DicomSeries::addDicomPath(std::size_t instanceIndex, const ::boost::filesystem::path& path)
{
    ::fwMemory::BufferObject::sptr buffer = ::fwMemory::BufferObject::New();
    const size_t buffSize = ::boost::filesystem::file_size(path);
    buffer->setIStreamFactory( std::make_shared< ::fwMemory::stream::in::Raw >(path),
                               buffSize, path, ::fwMemory::RAW);
    m_dicomContainer[instanceIndex] = buffer;
}

//------------------------------------------------------------------------------

void DicomSeries::addBinary(std::size_t instanceIndex, const ::fwMemory::BufferObject::sptr& buffer)
{
    m_dicomContainer[instanceIndex] = buffer;
}

//------------------------------------------------------------------------------

bool DicomSeries::isInstanceAvailable(std::size_t instanceIndex) const
{
    const auto& dicomContainerIter = m_dicomContainer.find(instanceIndex);
    return (dicomContainerIter != m_dicomContainer.end());
}

//------------------------------------------------------------------------------

void DicomSeries::addSOPClassUID(const std::string& sopClassUID)
{
    m_SOPClassUIDs.insert(sopClassUID);
}

//------------------------------------------------------------------------------

void DicomSeries::addComputedTagValue(const std::string& tagName, const std::string& value)
{
    m_computedTagValues[tagName] = value;
}

//------------------------------------------------------------------------------

bool DicomSeries::hasComputedValues(const std::string& tagName) const
{
    return m_computedTagValues.find(tagName) != m_computedTagValues.end();
}

} // namespace fwMedData
