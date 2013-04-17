/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/registry/macros.hpp>

#include <fwData/Object.hpp>
#include <fwData/Composite.hpp>
#include <fwData/Exception.hpp>

#include "fwMedData/ActivitySeries.hpp"

fwDataRegisterMacro( ::fwMedData::ActivitySeries );

namespace fwMedData
{

ActivitySeries::ActivitySeries(::fwData::Object::Key key) : Series(key),
        m_attrData(::fwData::Composite::New())
{}

//------------------------------------------------------------------------------

ActivitySeries::~ActivitySeries()
{}

//------------------------------------------------------------------------------

void ActivitySeries::shallowCopy(const ::fwData::Object::csptr &_source)
{
    ActivitySeries::csptr other = ActivitySeries::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );

    this->fieldShallowCopy( _source );
    m_attrActivityConfigId = other->m_attrActivityConfigId;
    m_attrData = other->m_attrData;
}

//------------------------------------------------------------------------------

void ActivitySeries::cachedDeepCopy(const ::fwData::Object::csptr &_source, DeepCopyCacheType &cache)
{
    ActivitySeries::csptr other = ActivitySeries::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );

    this->fieldDeepCopy( _source, cache );
    m_attrActivityConfigId = other->m_attrActivityConfigId;
    m_attrData = ::fwData::Object::copy(other->m_attrData, cache);
}

//------------------------------------------------------------------------------

} // namespace fwMedData

