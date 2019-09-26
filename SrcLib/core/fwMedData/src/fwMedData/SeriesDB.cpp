/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwMedData/SeriesDB.hpp"

#include "fwMedData/Series.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwData/Exception.hpp>
#include <fwData/registry/macros.hpp>

fwDataRegisterMacro( ::fwMedData::SeriesDB );

namespace fwMedData
{

const ::fwCom::Signals::SignalKeyType SeriesDB::s_ADDED_SERIES_SIG   = "addedSeries";
const ::fwCom::Signals::SignalKeyType SeriesDB::s_REMOVED_SERIES_SIG = "removedSeries";

//------------------------------------------------------------------------------

SeriesDB::SeriesDB(::fwData::Object::Key)
{
    newSignal< AddedSeriesSignalType >(s_ADDED_SERIES_SIG);
    newSignal< RemovedSeriesSignalType >(s_REMOVED_SERIES_SIG);
}

//------------------------------------------------------------------------------

SeriesDB::~SeriesDB()
{
}

//------------------------------------------------------------------------------

void SeriesDB::shallowCopy(const ::fwData::Object::csptr& _source)
{
    SeriesDB::csptr other = SeriesDB::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->fieldShallowCopy( other );

    m_container = other->m_container;
}

//------------------------------------------------------------------------------

void SeriesDB::cachedDeepCopy(const ::fwData::Object::csptr& _source, DeepCopyCacheType& cache)
{
    SeriesDB::csptr other = SeriesDB::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->fieldDeepCopy( other, cache );
    m_container.clear();
    m_container.reserve(other->m_container.size());
    for(const ValueType& series :  other->m_container)
    {
        m_container.push_back(::fwData::Object::copy(series, cache));
    }
}

//------------------------------------------------------------------------------

} // namespace fwMedData
