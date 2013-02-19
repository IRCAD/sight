/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <fwData/registry/macros.hpp>

#include <fwData/Object.hpp>
#include <fwData/Composite.hpp>

#include "fwActivities/ActivitySeries.hpp"

fwDataRegisterMacro( ::fwActivities::ActivitySeries );

namespace fwActivities
{

ActivitySeries::ActivitySeries(::fwData::Object::Key key) : Series(key)
{}

//------------------------------------------------------------------------------

ActivitySeries::~ActivitySeries()
{}

//------------------------------------------------------------------------------

void ActivitySeries::shallowCopy(ActivitySeries::csptr src)
{
    this->fieldShallowCopy( src );
    m_attrActivityConfigId = src->m_attrActivityConfigId;
    m_attrData = src->m_attrData;
}

//------------------------------------------------------------------------------

void ActivitySeries::deepCopy(ActivitySeries::csptr src)
{
    this->fieldDeepCopy( src );
    m_attrActivityConfigId = src->m_attrActivityConfigId;
    m_attrData = ::fwData::Object::copy(src->m_attrData);
}

//------------------------------------------------------------------------------

} // namespace fwActivities

