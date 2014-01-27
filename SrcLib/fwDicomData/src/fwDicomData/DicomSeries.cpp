/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>
#include <boost/filesystem/operations.hpp>

#include <fwData/Array.hpp>
#include <fwData/registry/macros.hpp>
#include <fwData/Exception.hpp>

#include "fwDicomData/DicomSeries.hpp"

fwDataRegisterMacro( ::fwDicomData::DicomSeries );

namespace fwDicomData
{

DicomSeries::DicomSeries(::fwData::Object::Key key): Series(key),
        m_attrDicomAvailability(NONE),
        m_attrNumberOfInstances(0)
{
}

//------------------------------------------------------------------------------

DicomSeries::~DicomSeries()
{}

//------------------------------------------------------------------------------

void DicomSeries::shallowCopy(const ::fwData::Object::csptr &_source)
{
    DicomSeries::csptr other = DicomSeries::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );

    this->::fwMedData::Series::shallowCopy(_source);

    m_attrDicomAvailability = other->m_attrDicomAvailability;
    m_attrNumberOfInstances = other->m_attrNumberOfInstances;
    m_attrLocalDicomPaths = other->m_attrLocalDicomPaths;
    m_attrDicomBinaries = other->m_attrDicomBinaries;

}

//------------------------------------------------------------------------------

void DicomSeries::cachedDeepCopy(const ::fwData::Object::csptr &_source, DeepCopyCacheType &cache)
{
    DicomSeries::csptr other = DicomSeries::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );

    this->::fwMedData::Series::cachedDeepCopy(_source, cache);

    m_attrDicomAvailability = other->m_attrDicomAvailability;
    m_attrNumberOfInstances = other->m_attrNumberOfInstances;
    m_attrLocalDicomPaths = other->m_attrLocalDicomPaths;

    m_attrDicomBinaries.clear();
    BOOST_FOREACH(DicomBinaryContainerType::value_type array, other->m_attrDicomBinaries)
    {
        m_attrDicomBinaries[array.first] = ::fwData::Object::copy(array.second);
    }

}

//------------------------------------------------------------------------------

void DicomSeries::addDicomPath(unsigned int instanceIndex, ::boost::filesystem::path path)
{
    m_attrLocalDicomPaths[instanceIndex] = path;
}

//------------------------------------------------------------------------------

void DicomSeries::addBinary(const std::string &filename, SPTR(::fwData::Array) binary)
{
    m_attrDicomBinaries[filename] = binary;
}

//------------------------------------------------------------------------------

bool DicomSeries::isInstanceAvailable(unsigned int instanceIndex)
{
    return m_attrLocalDicomPaths.find(instanceIndex) != m_attrLocalDicomPaths.end()
            && ::boost::filesystem::exists(m_attrLocalDicomPaths[instanceIndex]);
}

} // namespace fwDicomData

