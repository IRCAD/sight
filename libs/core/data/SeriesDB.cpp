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

#include "data/SeriesDB.hpp"

#include "data/Series.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>

#include <data/Exception.hpp>
#include <data/registry/macros.hpp>

SIGHT_REGISTER_DATA(sight::data::SeriesDB)

namespace sight::data
{

const core::com::Signals::SignalKeyType SeriesDB::s_ADDED_SERIES_SIG   = "addedSeries";
const core::com::Signals::SignalKeyType SeriesDB::s_REMOVED_SERIES_SIG = "removedSeries";

//------------------------------------------------------------------------------

SeriesDB::SeriesDB(data::Object::Key /*unused*/)
{
    newSignal<AddedSeriesSignalType>(s_ADDED_SERIES_SIG);
    newSignal<RemovedSeriesSignalType>(s_REMOVED_SERIES_SIG);
}

//------------------------------------------------------------------------------

SeriesDB::~SeriesDB()
= default;

//------------------------------------------------------------------------------

void SeriesDB::shallowCopy(const data::Object::csptr& _source)
{
    SeriesDB::csptr other = SeriesDB::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );

    this->fieldShallowCopy(other);

    m_container = other->m_container;
}

//------------------------------------------------------------------------------

void SeriesDB::cachedDeepCopy(const data::Object::csptr& _source, DeepCopyCacheType& cache)
{
    SeriesDB::csptr other = SeriesDB::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );

    this->fieldDeepCopy(other, cache);
    m_container.clear();
    m_container.reserve(other->m_container.size());
    for(const ValueType& series : other->m_container)
    {
        m_container.push_back(data::Object::copy(series, cache));
    }
}

//------------------------------------------------------------------------------

bool SeriesDB::operator==(const SeriesDB& other) const noexcept
{
    if(!core::tools::is_equal(m_container, other.m_container))
    {
        return false;
    }

    // Super class last
    return Object::operator==(other);
}

//------------------------------------------------------------------------------

bool SeriesDB::operator!=(const SeriesDB& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data
