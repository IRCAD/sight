/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#pragma once

#include "io/dicom/config.hpp"

#include <core/macros.hpp>

#include <gdcmDataSet.h>
#include <gdcmScanner.h>

#include <cstdint>
#include <filesystem>
#include <vector>

namespace sight::data
{

class DicomSeries;

} // namespace sight::data

namespace sight::data
{

class Patient;
class Study;
class Equipment;

} // namespace sight::data

namespace sight::core::jobs
{

class observer;

} // namespace sight::core::jobs

namespace sight::io::dicom::helper
{

/**
 * @brief DicomSeries Helper. This class is used to generate/fill DicomSeries.
 * - It can be used to read DicomSeries from a list of files (using read function).
 * - It can be used to complete DicomSeries that have been created using the
 *   DicomDir helper (using complete function).
 */
class IO_DICOM_CLASS_API DicomSeries
{
public:

    typedef std::vector<std::filesystem::path> FilenameContainerType;
    typedef std::vector<SPTR(data::DicomSeries)> DicomSeriesContainerType;

    /// Constructor
    IO_DICOM_API DicomSeries() noexcept = default;

    /// Destructor
    IO_DICOM_API ~DicomSeries() noexcept = default;

    /**
     * @brief Read DicomSeries from paths.
     * @param[in] filenames instance paths
     * @param[in] readerObserver reader observer
     * @param[in] completeSeriesObserver complete series observer
     * @return container containing DicomSeries
     */
    static IO_DICOM_API DicomSeriesContainerType read(
        FilenameContainerType& filenames,
        const SPTR(core::jobs::observer)& readerObserver         = nullptr,
        const SPTR(core::jobs::observer)& completeSeriesObserver = nullptr
    );
    /**
     * @brief Fill DicomSeries information for series generated using DICOMDIR helper
     * @param[in,out] series_set List of DicomSeries that must be completed with information
     * @param[in] completeSeriesObserver complete series observer
     */
    static IO_DICOM_API void complete(
        DicomSeriesContainerType& seriesContainer,
        const SPTR(core::jobs::observer)& completeSeriesObserver
    );

protected:

    /**
     * @brief Fill series with information contained in first instance
     * @param[in,out] series_set List of DicomSeries that must be completed with information
     * @param[in] completeSeriesObserver complete series observer
     */
    static void fillSeries(
        DicomSeriesContainerType& seriesContainer,
        const SPTR(core::jobs::observer)& completeSeriesObserver
    );

    /**
     * @brief Create DicomSeries from list of files. Every instance is read in
     * order to retrieve instance information regarding the matching series.
     * @param[in] filenames List of files
     * @param[in] readerObserver reader observer
     */
    static DicomSeriesContainerType splitFiles(
        FilenameContainerType& filenames,
        const SPTR(core::jobs::observer)& readerObserver
    );

    /**
     * @brief Create a series from the dataset and store it in the series map
     * @param[in,out] series_set List of DicomSeries that must be completed with information
     * @param[in] scanner GDCM Scanner used to read information
     * @param[in] filename Filename from which the information must be read
     */
    static void createSeries(
        DicomSeriesContainerType& seriesContainer,
        const gdcm::Scanner& scanner,
        const std::filesystem::path& filename
    );
};

} // namespace sight::io::dicom::helper
