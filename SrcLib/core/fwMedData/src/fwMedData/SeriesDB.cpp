/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwMedData/SeriesDB.hpp"

#include <fwData/registry/macros.hpp>
#include <fwData/Exception.hpp>

#include "fwMedData/Series.hpp"

fwDataRegisterMacro( ::fwMedData::SeriesDB );

namespace fwMedData
{

SeriesDB::SeriesDB(::fwData::Object::Key key)
{}

//------------------------------------------------------------------------------

SeriesDB::~SeriesDB()
{}

//------------------------------------------------------------------------------

void SeriesDB::shallowCopy(const ::fwData::Object::csptr &_source)
{
    SeriesDB::csptr other = SeriesDB::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );

    this->fieldShallowCopy( other );

    m_attrContainer= other->m_attrContainer;
}

//------------------------------------------------------------------------------

void SeriesDB::cachedDeepCopy(const ::fwData::Object::csptr &_source, DeepCopyCacheType &cache)
{
    SeriesDB::csptr other = SeriesDB::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );

    this->fieldDeepCopy( other, cache );
    m_attrContainer.clear();
    m_attrContainer.reserve(other->m_attrContainer.size());
    BOOST_FOREACH(const ValueType &series, other->m_attrContainer)
    {
        m_attrContainer.push_back(::fwData::Object::copy(series, cache));
    }
}

//------------------------------------------------------------------------------

} // namespace fwMedData

