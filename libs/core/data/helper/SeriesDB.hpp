/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#pragma once

#include "data/config.hpp"
#include <data/Series.hpp>
#include <data/SeriesDB.hpp>

namespace sight::data
{

namespace helper
{

/**
 * @brief Defines an helper to modify an data::SeriesDB and create in parallel the message to announce this
 * modification.
 */
class DATA_CLASS_API SeriesDB
{
public:

    /// Constructor. Initialize parameters.
    DATA_API SeriesDB(data::SeriesDB& seriesDB);

    /// Destrucotr. Do nothing.
    DATA_API ~SeriesDB();

    /**
     * @brief Add a Series in the SeriesDB.
     * @param[in] newSeries object to add in the SeriesDB
     *
     * Prepare the signal to announce the modification.
     */
    DATA_API void add(data::Series::sptr newSeries);

    /**
     * @brief Remove a Series in the SeriesDB.
     * @param[in] oldSeries object to remove from the SeriesDB.
     *
     * Prepare the signal to announce the modification.
     */
    DATA_API void remove(data::Series::sptr oldSeries);

    /**
     * @brief Clear all series in the SeriesDB.
     *
     * Prepare the signal to announce the modification.
     */
    DATA_API void clear();

    /**
     * @brief Merge seriesDBIn all series from seriesDBIn to the SeriesDB.
     *
     * Prepare the signal to announce the modification.
     */
    DATA_API void merge(data::SeriesDB::sptr seriesDBIn);

    /// Send the signal of modification
    DATA_API void notify();

private:

    data::SeriesDB::ContainerType m_addedSeries;
    data::SeriesDB::ContainerType m_removedSeries;
    data::SeriesDB& m_seriesDB;
};

} // namespace helper

} // namespace sight::data
