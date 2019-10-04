/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "ioVtkGdcm/config.hpp"

#include <fwIO/IReader.hpp>

#include <filesystem>

#include <string>

namespace fwMedData
{
class SeriesDB;
class Patient;
}

namespace fwJobs
{
class IJob;
}

namespace ioVtkGdcm
{

/**
 * @brief This service reads a dicom dir in lazy mode, and provides a ::fwMedData::SeriesDB.
 *
 * @section Signals Signals
 * - \b jobCreated(SPTR(::fwJobs::IJob)): emitted to display a progress bar while the image is loading (it should be
 * connected to a SJobBar).
 *
 * @section Slots Slots
 * - \b readFolder(std::filesystem::path) : read the given file
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::ioVtkGdcm::SSeriesDBLazyReader">
       <inout key="data" uid="..." />
       <folder>...</folder>
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b data [::fwMedData::SeriesDB]: seriesDB that will contain the loaded image series and modelSeries.
 * @subsection Configuration Configuration
 * - \b folder (optional): path of the folder containing the dicom files, if it is not defined, 'configureWithIHM()'
 *      should be called to define the path.
 */
class IOVTKGDCM_CLASS_API SSeriesDBLazyReader : public ::fwIO::IReader
{

public:
    typedef ::fwCom::Signal< void ( SPTR(::fwJobs::IJob) ) > JobCreatedSignalType;
    typedef std::string ExtensionType;
    typedef std::vector< ExtensionType > ExtensionsType;

    fwCoreServiceMacro(SSeriesDBLazyReader,  ::fwIO::IReader)

    /// constructor
    IOVTKGDCM_API SSeriesDBLazyReader() noexcept;

    /// destructor
    IOVTKGDCM_API virtual ~SSeriesDBLazyReader() noexcept;

protected:

    /// Override
    IOVTKGDCM_API virtual void starting() override;

    /// Override
    IOVTKGDCM_API virtual void stopping() override;

    /// Override
    IOVTKGDCM_API virtual void configuring() override;

    /// Override
    IOVTKGDCM_API void updating() override;

    /// Override
    IOVTKGDCM_API void info(std::ostream& _sstream ) override;

    /// Override
    IOVTKGDCM_API virtual ExtensionsType getSupportedExtensions() override;

    /// Override
    IOVTKGDCM_API virtual std::string getSelectorDialogTitle() override;

    /// Override
    IOVTKGDCM_API virtual void configureWithIHM() override;

    /// Return path type managed by the service, here FOLDER
    IOVTKGDCM_API ::fwIO::IOPathType getIOPathType() const override;

private:

    /// Notifies seriesDB that few series are added
    void notificationOfDBUpdate();

    /// Creates ::fwMedData::SeriesDB from a path
    SPTR(::fwMedData::SeriesDB) createSeriesDB(const std::filesystem::path& dicomDir);

    SPTR(JobCreatedSignalType) m_sigJobCreated;

};

} // namespace ioVtkGdcm
