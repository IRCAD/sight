/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
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

#ifndef __FWMEDDATATOOLS_HELPER_SERIESDB_HPP__
#define __FWMEDDATATOOLS_HELPER_SERIESDB_HPP__

#include "fwMedDataTools/config.hpp"

#include <fwMedData/Series.hpp>
#include <fwMedData/SeriesDB.hpp>

namespace fwMedDataTools
{
namespace helper
{

/**
 * @brief Defines an helper to modify an ::fwMedData::SeriesDB and create in parallel the message to announce this modification.
 */
class FWMEDDATATOOLS_CLASS_API SeriesDB
{

public:

    /// Constructor. Initialize parameters.
    FWMEDDATATOOLS_API SeriesDB( ::fwMedData::SeriesDB::wptr seriesDB );

    /// Destrucotr. Do nothing.
    FWMEDDATATOOLS_API ~SeriesDB();

    /**
     * @brief Add a Series in the SeriesDB.
     * @param[in] newSeries object to add in the SeriesDB
     *
     * Prepare the signal to announce the modification.
     */
    FWMEDDATATOOLS_API void add( ::fwMedData::Series::sptr newSeries );

    /**
     * @brief Remove a Series in the SeriesDB.
     * @param[in] oldSeries object to remove from the SeriesDB.
     *
     * Prepare the signal to announce the modification.
     */
    FWMEDDATATOOLS_API void remove( ::fwMedData::Series::sptr oldSeries );

    /**
     * @brief Clear all series in the SeriesDB.
     *
     * Prepare the signal to announce the modification.
     */
    FWMEDDATATOOLS_API void clear();

    /**
     * @brief Merge seriesDBIn all series from seriesDBIn to the SeriesDB.
     *
     * Prepare the signal to announce the modification.
     */
    FWMEDDATATOOLS_API void merge(::fwMedData::SeriesDB::sptr seriesDBIn);

    /// Send the signal of modification
    FWMEDDATATOOLS_API void notify();

private:

    ::fwMedData::SeriesDB::ContainerType m_addedSeries;
    ::fwMedData::SeriesDB::ContainerType m_removedSeries;
    ::fwMedData::SeriesDB::wptr m_seriesDB;
};

} // namespace helper
} // namespace fwMedDataTools

#endif // __FWMEDDATATOOLS_HELPER_SERIESDB_HPP__
