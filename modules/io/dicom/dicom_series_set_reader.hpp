/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <io/__/service/reader.hpp>

#include <filesystem>
#include <string>

namespace sight::core::jobs
{

class base;

}

namespace sight::data
{

class series_set;
class Patient;

}

namespace sight::module::io::dicom
{

/**
 * @brief GDCM Reader (DicomSeries)
 *
 * @section Signals Signals
 * - \b jobCreated( SPTR(core::jobs::base) ) : Emitted when a job is created.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::module::io::dicom::dicom_series_set_reader">
            <inout key="data" uid="..." />
            <dicomdirSupport>user_selection</dicomdirSupport>
            <showLogDialog>true or false</showLogDialog>
       </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b data [sight::data::series_set]: Destination container for DicomSeries
 * @subsection Configuration Configuration:
 * - \b dicomdirSupport (optional) : DicomDir support mode.
 * dicomdirSupport available mode:
 *    - always (always use the DicomDir if present)
 *    - never (never use the DicomDir)
 *    - user_selection (let the user decide whether using the DicomDir or not)
 *  - \b showLogDialog (optional) : Show log dialog. should be true or false
 */
class MODULE_IO_DICOM_CLASS_API dicom_series_set_reader : public sight::io::service::reader
{
public:

    using job_created_signal_t = core::com::signal<void (std::shared_ptr<core::jobs::base>)>;

    SIGHT_DECLARE_SERVICE(dicom_series_set_reader, sight::io::service::reader);

    /**
     * @brief   constructor
     *
     */
    MODULE_IO_DICOM_API dicom_series_set_reader() noexcept;

    /**
     * @brief   destructor
     */
    MODULE_IO_DICOM_API ~dicom_series_set_reader() noexcept override = default;

protected:

    /// Enum for DicomDir support mode
    enum dicom_dir_support
    {
        always = 0,    /*! Always use the DicomDir if present */
        never,         /*! Never use the DicomDir */
        user_selection /*! Let the user decide whether using the DicomDir or not */
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
    MODULE_IO_DICOM_API std::string get_selector_dialog_title() override;

    /// Configuring method. This method is used to configure the service.
    MODULE_IO_DICOM_API void configuring() override;

    /// Configure using GUI.
    MODULE_IO_DICOM_API void open_location_dialog() override;

    /// Return path type managed by the service, here FOLDER
    MODULE_IO_DICOM_API sight::io::service::path_type_t get_path_type() const override;

private:

    /**
     * @brief Create a series_set and fill it using the data of the DICOM files
     * @param[in] dicomDir DICOM folder
     */
    SPTR(data::series_set) create_series_set(const std::filesystem::path& _dicom_dir);

    /// Signal emitted when a job is created
    SPTR(job_created_signal_t) m_sig_job_created;

    /// Cancel information for jobs
    bool m_cancelled;

    /// Show log dialog
    bool m_show_log_dialog;

    /// Specify how to use dicomdir files
    dicom_dir_support m_dicom_dir_support;
};

} // namespace sight::module::io::dicom
