/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#pragma once

#include "vtkGdcmIO/config.hpp"

#include <fwData/location/Folder.hpp>
#include <fwData/location/MultiFiles.hpp>

#include <fwDataIO/reader/GenericObjectReader.hpp>

namespace fwMedData
{
class SeriesDB;
}
namespace fwJobs
{
class Observer;
}

namespace vtkGdcmIO
{

/// Reads a DICOM data from a directory path in order to create a SeriesDB object.
class SeriesDBReader : public ::fwDataIO::reader::GenericObjectReader< ::fwMedData::SeriesDB >,
                       public ::fwData::location::enableFolder< ::fwDataIO::reader::IObjectReader >,
                       public ::fwData::location::enableMultiFiles< ::fwDataIO::reader::IObjectReader >
{

public:

    fwCoreClassMacro(SeriesDBReader, ::fwDataIO::reader::GenericObjectReader< ::fwMedData::SeriesDB >,
                     ::fwDataIO::reader::factory::New< SeriesDBReader >)

    fwCoreAllowSharedFromThis()

    /// Creates the reader.
    VTKGDCMIO_API SeriesDBReader(::fwDataIO::reader::IObjectReader::Key);

    /// Destroyes the reader.
    VTKGDCMIO_API ~SeriesDBReader();

    /// Reads DICOM data from configured path and fills SeriesDB object.
    VTKGDCMIO_API void read() override;

    /// @return internal job.
    VTKGDCMIO_API SPTR(::fwJobs::IJob) getJob() const override;

private:

    /**
     * @brief Creates a SeriesDB from the data contained in the given directory path.
     * @param _dicomDir directory path from which data is read
     * @return a new SeriesDB object
     */
    SPTR(::fwMedData::SeriesDB) createSeriesDB(const std::filesystem::path& _dicomDir);

    /**
     * @brief Fills the related SeriesDB object with DICOM data extracted from the given files.
     * @param _seriesDB SeriesDB object to be filled with DICOM data
     * @param _dicomDir directory path from which data is read
     */
    void addSeries(const SPTR(::fwMedData::SeriesDB)& _seriesDB, const std::vector< std::string >& _filenames);

    /// Contains the internal job.
    SPTR(::fwJobs::Observer) m_job;

};

} // namespace vtkGdcmIO
