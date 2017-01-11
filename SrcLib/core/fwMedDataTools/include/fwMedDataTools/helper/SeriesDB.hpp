/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
