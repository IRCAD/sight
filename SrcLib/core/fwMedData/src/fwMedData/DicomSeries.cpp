/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwMedData/DicomSeries.hpp"

#include <fwData/Exception.hpp>
#include <fwData/registry/macros.hpp>

#include <boost/filesystem/operations.hpp>

fwDataRegisterMacro( ::fwMedData::DicomSeries );

namespace fwMedData
{

DicomSeries::DicomSeries(::fwData::Object::Key key) :
    Series(key),
    m_dicomAvailability(NONE),
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

    m_dicomAvailability   = other->m_dicomAvailability;
    m_numberOfInstances   = other->m_numberOfInstances;
    m_localDicomPaths     = other->m_localDicomPaths;
    m_dicomBinaries       = other->m_dicomBinaries;
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

    m_dicomAvailability   = other->m_dicomAvailability;
    m_numberOfInstances   = other->m_numberOfInstances;
    m_localDicomPaths     = other->m_localDicomPaths;
    m_SOPClassUIDs        = other->m_SOPClassUIDs;
    m_computedTagValues   = other->m_computedTagValues;
    m_firstInstanceNumber = other->m_firstInstanceNumber;

    m_dicomBinaries.clear();
    for(const auto& elt : other->m_dicomBinaries)
    {
        const ::fwMemory::BufferObject::sptr bufferSrc  = elt.second;
        const ::fwMemory::BufferObject::sptr bufferDest = m_dicomBinaries[elt.first];
        if( !bufferSrc->isEmpty() )
        {
            ::fwMemory::BufferObject::Lock lockerDest(bufferDest);
            bufferDest->allocate(bufferSrc->getSize());
            char* buffDest = static_cast< char* >( lockerDest.getBuffer() );

            ::fwMemory::BufferObject::Lock lockerSource(bufferSrc);
            char* buffSrc = static_cast< char* >( lockerSource.getBuffer() );

            std::copy(buffSrc, buffSrc + bufferSrc->getSize(), buffDest );
        }
    }
}

//------------------------------------------------------------------------------

void DicomSeries::addDicomPath(std::size_t instanceIndex, const ::boost::filesystem::path& path)
{
    m_localDicomPaths[instanceIndex] = path;
}

//------------------------------------------------------------------------------

void DicomSeries::addBinary(const std::string& filename, const ::fwMemory::BufferObject::sptr& buffer)
{
    m_dicomBinaries[filename] = buffer;
}

//------------------------------------------------------------------------------

bool DicomSeries::isInstanceAvailable(std::size_t instanceIndex) const
{
    DicomPathContainerType::const_iterator localPathIter;

    bool available = false;

    switch(m_dicomAvailability)
    {
        case NONE:
        case PATHS:
            localPathIter = m_localDicomPaths.find(instanceIndex);
            available     = localPathIter != m_localDicomPaths.end() && ::boost::filesystem::exists(
                localPathIter->second);
            break;
        case BINARIES:
            available = instanceIndex < m_dicomBinaries.size();
            break;
        default:
            SLM_ASSERT("You shall not pass.", nullptr);
    }

    return available;
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
