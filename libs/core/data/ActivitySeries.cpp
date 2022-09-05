/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "data/ActivitySeries.hpp"

#include "data/Exception.hpp"
#include "data/registry/macros.hpp"

SIGHT_REGISTER_DATA(sight::data::ActivitySeries)

namespace sight::data
{

ActivitySeries::ActivitySeries() :

    m_data(data::Composite::New())
{
}

ActivitySeries::ActivitySeries(data::Object::Key _key) :
    Series(_key),
    m_data(data::Composite::New())
{
}

//------------------------------------------------------------------------------

ActivitySeries::~ActivitySeries()
= default;

//------------------------------------------------------------------------------

void ActivitySeries::shallowCopy(const data::Object::csptr& _source)
{
    ActivitySeries::csptr other = ActivitySeries::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );

    this->data::Series::shallowCopy(_source);
    m_activityConfigId = other->m_activityConfigId;
    m_data             = other->m_data;
}

//------------------------------------------------------------------------------

void ActivitySeries::cachedDeepCopy(const data::Object::csptr& _source, DeepCopyCacheType& cache)
{
    ActivitySeries::csptr other = ActivitySeries::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );

    this->data::Series::cachedDeepCopy(_source, cache);

    m_activityConfigId = other->m_activityConfigId;
    m_data             = data::Object::copy(other->m_data, cache);
}

//------------------------------------------------------------------------------

bool ActivitySeries::operator==(const ActivitySeries& other) const noexcept
{
    if(m_activityConfigId != other.m_activityConfigId
       || !core::tools::is_equal(m_data, other.m_data))
    {
        return false;
    }

    // Super class last
    return Series::operator==(other);
}

//------------------------------------------------------------------------------

bool ActivitySeries::operator!=(const ActivitySeries& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data
