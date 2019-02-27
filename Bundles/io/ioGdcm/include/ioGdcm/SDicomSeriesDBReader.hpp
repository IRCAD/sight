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

#include "ioGdcm/config.hpp"

#include <fwCom/Signal.hpp>

#include <fwIO/IReader.hpp>

#include <boost/filesystem/path.hpp>

#include <string>

namespace fwJobs
{
class IJob;
}

namespace fwMedData
{
class SeriesDB;
class Patient;
}

namespace ioGdcm
{

/**
 * @brief GDCM Reader (DicomSeries)
 *
 * @section Signals Signals
 * - \b jobCreated( SPTR(::fwJobs::IJob) ) : Emitted when a job is created.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::ioGdcm::SDicomSeriesDBReader">
            <inout key="data" uid="..." />
            <dicomdirSupport>user_selection</dicomdirSupport>
       </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b data [::fwMedData::SeriesDB]: Destination container for DicomSeries
 * @subsection Configuration Configuration:
 * - \b dicomdirSupport (optional) : DicomDir support mode.
 * dicomdirSupport available mode:
 *    - always (always use the DicomDir if present)
 *    - never (never use the DicomDir)
 *    - user_selection (let the user decide whether using the DicomDir or not)
 */
class IOGDCM_CLASS_API SDicomSeriesDBReader : public ::fwIO::IReader
{

public:

    typedef ::fwCom::Signal< void ( SPTR(::fwJobs::IJob) ) > JobCreatedSignal;

    fwCoreServiceClassDefinitionsMacro( (SDicomSeriesDBReader)( ::fwIO::IReader) )

    /**
     * @brief   constructor
     *
     */
    IOGDCM_API SDicomSeriesDBReader() noexcept;

    /**
     * @brief   destructor
     */
    IOGDCM_API virtual ~SDicomSeriesDBReader() noexcept override;

protected:

    /// Enum for DicomDir support mode
    enum DicomDirSupport
    {
        ALWAYS = 0,     /*! Always use the DicomDir if present */
        NEVER,          /*! Never use the DicomDir */
        USER_SELECTION  /*! Let the user decide whether using the DicomDir or not */
    };

    /// Override
    IOGDCM_API virtual void starting() override;

    /// Override
    IOGDCM_API virtual void stopping() override;

    /// Override
    IOGDCM_API void updating() override;

    /// Override
    IOGDCM_API void info(std::ostream& _sstream ) override;

    /// Override
    IOGDCM_API virtual std::string getSelectorDialogTitle() override;

    /// Configuring method. This method is used to configure the service.
    IOGDCM_API virtual void configuring() override;

    /// Override
    IOGDCM_API virtual void configureWithIHM() override;

    /// Return path type managed by the service, here FOLDER
    IOGDCM_API ::fwIO::IOPathType getIOPathType() const override;

private:

    /**
     * @brief Create a seriesDB and fill it using the data of the DICOM files
     * @param[in] dicomDir DICOM folder
     */
    SPTR(::fwMedData::SeriesDB) createSeriesDB(const ::boost::filesystem::path& dicomDir);

    /// Signal emitted when a job is created
    SPTR(JobCreatedSignal) m_sigJobCreated;

    /// Cancel information for jobs
    bool m_cancelled;

    /// Show log dialog
    bool m_showLogDialog;

    /// Specify how to use dicomdir files
    DicomDirSupport m_dicomDirSupport;
};

} // namespace ioGdcm
