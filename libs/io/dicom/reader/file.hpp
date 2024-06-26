/************************************************************************
 *
 * Copyright (C) 2022-2024 IRCAD France
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

#include <core/jobs/job.hpp>
#include <core/location/multiple_files.hpp>
#include <core/location/single_folder.hpp>

#include <data/series_set.hpp>

#include <io/__/reader/generic_object_reader.hpp>

namespace sight::io::dicom::reader
{

class SIGHT_IO_DICOM_CLASS_API file final : public io::reader::generic_object_reader<data::series_set>,
                                            public core::location::single_folder,
                                            public core::location::multiple_files,
                                            public core::com::has_signals
{
public:

    SIGHT_DECLARE_CLASS(file, io::reader::generic_object_reader<data::series_set>);

    SIGHT_IO_DICOM_API file();

    SIGHT_IO_DICOM_API ~file() noexcept override;

    /// Reads DICOM data
    SIGHT_IO_DICOM_API void read() override;

    inline std::string extension() const override;

    /// Returns a list of DICOM series by scanning files, either using get_files() or recursively using get_folder() as
    /// root directory.
    /// The files are NOT sorted!
    /// @return data::series_set::sptr: A set of series, with their associated files
    /// @throw std::runtime_error if the root directory is not an existing folder
    /// @throw std::runtime_error if there is no dicom files are found
    SIGHT_IO_DICOM_API data::series_set::sptr scan();

    /// Returns a list of DICOM series with their associated files sorted.
    /// Also call scan() if there is no scanned series.
    /// @return data::series_set::sptr and double: A set of series, with their associated files sorted, and the
    /// z_spacing
    SIGHT_IO_DICOM_API data::series_set::sptr sort();

    /// Getters/Setters
    /// @{

    /// Set SOP class filters to use when scanning for DICOM files (CTImageStorage, SpatialFiducialsStorage, ...).
    /// @param[in] _filters SOP class filters
    SIGHT_IO_DICOM_API void set_filters(const data::series::SopKeywords& _filters);

    /// Set the scanned Series list, unsorted
    /// @param[in] _scanned The Series with their associated files
    SIGHT_IO_DICOM_API void set_scanned(const data::series_set::sptr& _scanned);

    /// Set the sorted Series list. These are the series that will be read
    /// @param[in] _sorted The Series with their associated files
    SIGHT_IO_DICOM_API void set_sorted(const data::series_set::sptr& _sorted);

    /// Set/get the current job
    SIGHT_IO_DICOM_API core::jobs::base::sptr get_job() const override;
    SIGHT_IO_DICOM_API void set_job(core::jobs::job::sptr _job);

    /// @}

private:

    class reader_impl;
    std::unique_ptr<reader_impl> m_pimpl;
};

//------------------------------------------------------------------------------

inline std::string file::extension() const
{
    return ".dcm";
}

} // namespace sight::io::dicom::reader
