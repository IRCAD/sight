/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <fwData/registry/macros.hpp>
#include <fwData/Exception.hpp>

#include "fwMedData/Series.hpp"
#include "fwMedData/SeriesDB.hpp"

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

void SeriesDB::deepCopy(const ::fwData::Object::csptr &_source)
{
    SeriesDB::csptr other = SeriesDB::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );

    this->fieldDeepCopy( other );
    m_attrContainer.clear();
    m_attrContainer.resize(other->m_attrContainer.size());
    std::transform(
            other->begin(), other->end(),
            this->begin(),
            &::fwData::Object::copy< ValueType::element_type >
    );
}

//------------------------------------------------------------------------------

} // namespace fwMedData

