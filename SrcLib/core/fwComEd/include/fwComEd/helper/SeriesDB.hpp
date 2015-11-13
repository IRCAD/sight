/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOMED_HELPER_SERIESDB_HPP__
#define __FWCOMED_HELPER_SERIESDB_HPP__


#include "fwComEd/config.hpp"

#include <fwMedData/SeriesDB.hpp>
#include <fwMedData/Series.hpp>

#include <fwServices/IService.hpp>



namespace fwComEd
{
namespace helper
{

/**
 * @brief Defines an helper to modify an ::fwMedData::SeriesDB and create in parallel the message to announce this modification.
 * @class SeriesDB
 */
class FWCOMED_CLASS_API SeriesDB
{

public:

    /// Constructor. Initialize parameters.
    FWCOMED_API SeriesDB( ::fwMedData::SeriesDB::wptr seriesDB );

    /// Destrucotr. Do nothing.
    FWCOMED_API ~SeriesDB();

    /**
     * @brief Add a Series in the SeriesDB.
     * @param[in] newSeries object to add in the SeriesDB
     *
     * Prepare the signal to announce the modification.
     */
    FWCOMED_API void add( ::fwMedData::Series::sptr newSeries );

    /**
     * @brief Remove a Series in the SeriesDB.
     * @param[in] oldSeries object to remove from the SeriesDB.
     *
     * Prepare the signal to announce the modification.
     */
    FWCOMED_API void remove( ::fwMedData::Series::sptr oldSeries );

    /**
     * @brief Clear all series in the SeriesDB.
     *
     * Prepare the signal to announce the modification.
     */
    FWCOMED_API void clear();

    /**
     * @brief Merge seriesDBIn all series from seriesDBIn to the SeriesDB.
     *
     * Prepare the signal to announce the modification.
     */
    FWCOMED_API void merge(::fwMedData::SeriesDB::sptr seriesDBIn);

    /// Send the signal of modification
    FWCOMED_API void notify();

private:

    ::fwMedData::SeriesDB::ContainerType m_addedSeries;
    ::fwMedData::SeriesDB::ContainerType m_removedSeries;
    ::fwMedData::SeriesDB::wptr m_seriesDB;
};

} // namespace helper
} // namespace fwComEd

#endif // __FWCOMED_HELPER_SERIESDB_HPP__
