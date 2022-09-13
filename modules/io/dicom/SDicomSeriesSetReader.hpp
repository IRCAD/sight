/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "modules/io/dicom/config.hpp"

#include <core/com/Signal.hpp>

#include <io/base/service/IReader.hpp>

#include <filesystem>
#include <string>

namespace sight::core::jobs
{

class IJob;

}

namespace sight::data
{

class SeriesSet;
class Patient;

}

namespace sight::module::io::dicom
{

/**
 * @brief GDCM Reader (DicomSeries)
 *
 * @section Signals Signals
 * - \b jobCreated( SPTR(core::jobs::IJob) ) : Emitted when a job is created.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::module::io::dicom::SDicomSeriesSetReader">
            <inout key="data" uid="..." />
            <dicomdirSupport>user_selection</dicomdirSupport>
            <showLogDialog>true or false</showLogDialog>
       </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b data [sight::data::SeriesSet]: Destination container for DicomSeries
 * @subsection Configuration Configuration:
 * - \b dicomdirSupport (optional) : DicomDir support mode.
 * dicomdirSupport available mode:
 *    - always (always use the DicomDir if present)
 *    - never (never use the DicomDir)
 *    - user_selection (let the user decide whether using the DicomDir or not)
 *  - \b showLogDialog (optional) : Show log dialog. should be true or false
 */
class MODULE_IO_DICOM_CLASS_API SDicomSeriesSetReader : public sight::io::base::service::IReader
{
public:

    typedef core::com::Signal<void (SPTR(core::jobs::IJob))> JobCreatedSignal;

    SIGHT_DECLARE_SERVICE(SDicomSeriesSetReader, sight::io::base::service::IReader);

    /**
     * @brief   constructor
     *
     */
    MODULE_IO_DICOM_API SDicomSeriesSetReader() noexcept;

    /**
     * @brief   destructor
     */
    MODULE_IO_DICOM_API ~SDicomSeriesSetReader() noexcept override = default;

protected:

    /// Enum for DicomDir support mode
    enum DicomDirSupport
    {
        ALWAYS = 0,    /*! Always use the DicomDir if present */
        NEVER,         /*! Never use the DicomDir */
        USER_SELECTION /*! Let the user decide whether using the DicomDir or not */
    };

    /// Override
    MODULE_IO_DICOM_API void starting() override;

    /// Override
    MODULE_IO_DICOM_API void stopping() override;

    /// Override
    MODULE_IO_DICOM_API void updating() override;

    /// Override
    MODULE_IO_DICOM_API void info(std::ostream& _sstream) override;

    /// Override
    MODULE_IO_DICOM_API std::string getSelectorDialogTitle() override;

    /// Configuring method. This method is used to configure the service.
    MODULE_IO_DICOM_API void configuring() override;

    /// Configure using GUI.
    MODULE_IO_DICOM_API void openLocationDialog() override;

    /// Return path type managed by the service, here FOLDER
    MODULE_IO_DICOM_API sight::io::base::service::IOPathType getIOPathType() const override;

private:

    /**
     * @brief Create a SeriesSet and fill it using the data of the DICOM files
     * @param[in] dicomDir DICOM folder
     */
    SPTR(data::SeriesSet) createSeriesSet(const std::filesystem::path& dicomDir);

    /// Signal emitted when a job is created
    SPTR(JobCreatedSignal) m_sigJobCreated;

    /// Cancel information for jobs
    bool m_cancelled;

    /// Show log dialog
    bool m_showLogDialog;

    /// Specify how to use dicomdir files
    DicomDirSupport m_dicomDirSupport;
};

} // namespace sight::module::io::dicom
