/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "fwMedDataTools/helper/SeriesDB.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>

#include <data/SeriesDB.hpp>

namespace fwMedDataTools
{
namespace helper
{

//-----------------------------------------------------------------------------

SeriesDB::SeriesDB( data::SeriesDB::wptr seriesDB ) :
    m_seriesDB( seriesDB )
{
}

//-----------------------------------------------------------------------------

SeriesDB::~SeriesDB()
{
}

//-----------------------------------------------------------------------------

void SeriesDB::add( data::Series::sptr newSeries )
{
    data::SeriesDB::sptr seriesDB = m_seriesDB.lock();
    SLM_ASSERT( "The object " << newSeries->getID() << " must not exist in SeriesDB.",
                std::find(seriesDB->begin(), seriesDB->end(), newSeries) == seriesDB->end());

    // Modify SeriesDB
    seriesDB->getContainer().push_back( newSeries );

    m_addedSeries.push_back(newSeries);

}

//-----------------------------------------------------------------------------

void SeriesDB::remove( data::Series::sptr oldSeries )
{
    data::SeriesDB::sptr seriesDB = m_seriesDB.lock();
    data::SeriesDB::iterator iter = std::find(seriesDB->begin(), seriesDB->end(), oldSeries);
    SLM_ASSERT( "The object " << oldSeries->getID() << " must exist in SeriesDB.",
                iter != seriesDB->end());

    // Modify SeriesDB
    seriesDB->getContainer().erase( iter );

    m_removedSeries.push_back( oldSeries );

}

//-----------------------------------------------------------------------------

void SeriesDB::clear()
{
    data::SeriesDB::sptr seriesDB = m_seriesDB.lock();

    while (!seriesDB->empty())
    {
        this->remove(seriesDB->front());
    }
}

//-----------------------------------------------------------------------------

void SeriesDB::merge(data::SeriesDB::sptr seriesDBIn)
{
    data::SeriesDB::ContainerType& vectIn = seriesDBIn->getContainer();
    for(data::Series::sptr series :  vectIn)
    {
        this->add(series);
    }
}

//-----------------------------------------------------------------------------

void SeriesDB::notify()
{
    if (!m_addedSeries.empty())
    {
        auto sig = m_seriesDB.lock()->signal< data::SeriesDB::AddedSeriesSignalType >(
            data::SeriesDB::s_ADDED_SERIES_SIG);
        sig->asyncEmit(m_addedSeries);

    }

    if (!m_removedSeries.empty())
    {
        auto sig = m_seriesDB.lock()->signal< data::SeriesDB::RemovedSeriesSignalType >(
            data::SeriesDB::s_REMOVED_SERIES_SIG);
        sig->asyncEmit(m_removedSeries);

    }

    SLM_INFO_IF("No changes were found on the SeriesDB '" + m_seriesDB.lock()->getID() + "', nothing to notify.",
                m_addedSeries.empty() && m_removedSeries.empty());
}

//-----------------------------------------------------------------------------

} // namespace helper
} // namespace fwMedDataTools
