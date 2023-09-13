/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include <core/jobs/Job.hpp>
#include <core/location/MultipleFiles.hpp>
#include <core/location/SingleFolder.hpp>

#include <data/SeriesSet.hpp>

#include <io/base/reader/GenericObjectReader.hpp>

namespace sight::io::dicom
{

class IO_DICOM_CLASS_API Reader final : public base::reader::GenericObjectReader<data::SeriesSet>,
                                        public core::location::SingleFolder,
                                        public core::location::MultipleFiles,
                                        public core::com::HasSignals
{
public:

    SIGHT_DECLARE_CLASS(
        Reader,
        io::base::reader::GenericObjectReader<data::SeriesSet>,
        io::base::reader::factory::New<Reader>
    );

    IO_DICOM_API Reader(io::base::reader::IObjectReader::Key key);

    IO_DICOM_API ~Reader() noexcept override;

    /// Reads DICOM data
    IO_DICOM_API void read() override;

    inline std::string extension() const override;

    /// Returns a list of DICOM series by scanning files, either using getFiles() or recursively using getFolder() as
    /// root directory.
    /// The files are NOT sorted!
    /// @return data::SeriesSet::sptr: A set of series, with their associated files
    /// @throw std::runtime_error if the root directory is not an existing folder
    /// @throw std::runtime_error if there is no dicom files are found
    IO_DICOM_API data::SeriesSet::sptr scan();

    /// Returns a list of DICOM series with their associated files sorted.
    /// Also call scan() if there is no scanned series.
    /// @return data::SeriesSet::sptr and double: A set of series, with their associated files sorted, and the z_spacing
    IO_DICOM_API data::SeriesSet::sptr sort();

    /// Getters/Setters
    /// @{

    /// Set SOP class filters to use when scanning for DICOM files (CTImageStorage, SpatialFiducialsStorage, ...).
    /// @param[in] filters SOP class filters
    IO_DICOM_API void setFilters(const data::Series::SopKeywords& filters);

    /// Set the scanned Series list, unsorted
    /// @param[in] scanned The Series with their associated files
    IO_DICOM_API void setScanned(const data::SeriesSet::sptr& scanned);

    /// Set the sorted Series list. These are the series that will be read
    /// @param[in] sorted The Series with their associated files
    IO_DICOM_API void setSorted(const data::SeriesSet::sptr& sorted);

    /// Set/get the current job
    IO_DICOM_API core::jobs::IJob::sptr getJob() const override;
    IO_DICOM_API void setJob(core::jobs::Job::sptr job);

    /// @}

private:

    class ReaderImpl;
    std::unique_ptr<ReaderImpl> m_pimpl;
};

//------------------------------------------------------------------------------

inline std::string Reader::extension() const
{
    return ".dcm";
}

} // namespace sight::io::dicom
