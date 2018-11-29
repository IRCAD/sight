/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

