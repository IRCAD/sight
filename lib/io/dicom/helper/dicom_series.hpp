/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/io/dicom/config.hpp>

#include <core/macros.hpp>

#include <gdcmDataSet.h>
#include <gdcmScanner.h>

#include <cstdint>
#include <filesystem>
#include <vector>

namespace sight::data
{

class dicom_series;

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
class SIGHT_IO_DICOM_CLASS_API dicom_series
{
public:

    using filename_container_t     = std::vector<std::filesystem::path>;
    using dicom_series_container_t = std::vector<std::shared_ptr<data::dicom_series> >;

    /// Constructor
    SIGHT_IO_DICOM_API dicom_series() noexcept = default;

    /// Destructor
    SIGHT_IO_DICOM_API ~dicom_series() noexcept = default;

    /**
     * @brief Read DicomSeries from paths.
     * @param[in] _filenames instance paths
     * @param[in] _reader_observer reader observer
     * @param[in] _complete_series_observer complete series observer
     * @return container containing DicomSeries
     */
    static SIGHT_IO_DICOM_API dicom_series_container_t read(
        filename_container_t& _filenames,
        const SPTR(core::jobs::observer)& _reader_observer          = nullptr,
        const SPTR(core::jobs::observer)& _complete_series_observer = nullptr
    );
    /**
     * @brief Fill DicomSeries information for series generated using DICOMDIR helper
     * @param[in,out] _series_container List of DicomSeries that must be completed with information
     * @param[in] _complete_series_observer complete series observer
     */
    static SIGHT_IO_DICOM_API void complete(
        dicom_series_container_t& _series_container,
        const SPTR(core::jobs::observer)& _complete_series_observer
    );

protected:

    /**
     * @brief Fill series with information contained in first instance
     * @param[in,out] _series_container List of DicomSeries that must be completed with information
     * @param[in] _complete_series_observer complete series observer
     */
    static void fill_series(
        dicom_series_container_t& _series_container,
        const SPTR(core::jobs::observer)& _complete_series_observer
    );

    /**
     * @brief Create DicomSeries from list of files. Every instance is read in
     * order to retrieve instance information regarding the matching series.
     * @param[in] _filenames List of files
     * @param[in] _reader_observer reader observer
     */
    static dicom_series_container_t split_files(
        filename_container_t& _filenames,
        const SPTR(core::jobs::observer)& _reader_observer
    );

    /**
     * @brief Create a series from the dataset and store it in the series map
     * @param[in,out] _series_container List of DicomSeries that must be completed with information
     * @param[in] _scanner GDCM Scanner used to read information
     * @param[in] _filename Filename from which the information must be read
     */
    static void create_series(
        dicom_series_container_t& _series_container,
        const gdcm::Scanner& _scanner,
        const std::filesystem::path& _filename
    );
};

} // namespace sight::io::dicom::helper
