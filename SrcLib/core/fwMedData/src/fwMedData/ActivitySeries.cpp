/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwMedData/ActivitySeries.hpp"

#include <fwData/Composite.hpp>
#include <fwData/Exception.hpp>
#include <fwData/registry/macros.hpp>

fwDataRegisterMacro( ::fwMedData::ActivitySeries );

namespace fwMedData
{

ActivitySeries::ActivitySeries(::fwData::Object::Key key) :
    Series(key),
    m_data(::fwData::Composite::New())
{
}

//------------------------------------------------------------------------------

ActivitySeries::~ActivitySeries()
{
}

//------------------------------------------------------------------------------

void ActivitySeries::shallowCopy(const ::fwData::Object::csptr& _source)
{
    ActivitySeries::csptr other = ActivitySeries::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->::fwMedData::Series::shallowCopy(_source);
    m_activityConfigId = other->m_activityConfigId;
    m_data             = other->m_data;
}

//------------------------------------------------------------------------------

void ActivitySeries::cachedDeepCopy(const ::fwData::Object::csptr& _source, DeepCopyCacheType& cache)
{
    ActivitySeries::csptr other = ActivitySeries::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->::fwMedData::Series::cachedDeepCopy(_source, cache);

    m_activityConfigId = other->m_activityConfigId;
    m_data             = ::fwData::Object::copy(other->m_data, cache);
}

//------------------------------------------------------------------------------

} // namespace fwMedData

