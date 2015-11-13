/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwMedData/Series.hpp"
#include "fwMedData/SeriesDB.hpp"

#include <fwData/Exception.hpp>
#include <fwData/registry/macros.hpp>


fwDataRegisterMacro( ::fwMedData::SeriesDB );

namespace fwMedData
{

SeriesDB::SeriesDB(::fwData::Object::Key key)
{
}

//------------------------------------------------------------------------------

SeriesDB::~SeriesDB()
{
}

//------------------------------------------------------------------------------

void SeriesDB::shallowCopy(const ::fwData::Object::csptr &_source)
{
    SeriesDB::csptr other = SeriesDB::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->fieldShallowCopy( other );

    m_container = other->m_container;
}

//------------------------------------------------------------------------------

void SeriesDB::cachedDeepCopy(const ::fwData::Object::csptr &_source, DeepCopyCacheType &cache)
{
    SeriesDB::csptr other = SeriesDB::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->fieldDeepCopy( other, cache );
    m_container.clear();
    m_container.reserve(other->m_container.size());
    for(const ValueType &series :  other->m_container)
    {
        m_container.push_back(::fwData::Object::copy(series, cache));
    }
}

//------------------------------------------------------------------------------

} // namespace fwMedData

