/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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

#include <core/com/signal.hpp>
#include <core/jobs/base.hpp>

#include <io/__/service/reader.hpp>

namespace sight::module::io::dicom
{

/**
 * @brief DICOM generic reader.
 *
 * @details Allow reading a DICOM directory. Part of the configuration is managed by reader class.
 *          After selecting a folder, it is scanned and if more than one DICOM Series are found, a selection dialog is
 *          shown allowing to select one or more Series to load. The DICOM series can be filtered by sopFilter and
 *          typeFilter configuration parameters.
 *
 * @section Signals Signals
 * - \b jobCreated(SPTR(core::jobs::base)): emitted to display a progress bar while the image is read (it should be
 * connected to a job_bar).
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service type="sight::module::io::dicom::reader">
        <inout key="data" uid="..." />
        <config>
            <windowTitle>Open DICOM directory</windowTitle>
            <dialog sopFilter="1.2.840.10008.5.1.4.1.1.2, 1.2.840.10008.5.1.4.1.1.4.1"/>
        </config>
    </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b data [sight::data::object]: object to read to.
 *
 * @subsection Configuration Configuration
 * - \b dialog(optional):
 *      \b sopFilter: The SOP class uids to filter. If empty, and if typeFilter is also empty, the default type filter
 *                    "image", "model", "report" is used.
 *      \b typeFilter: The type of DICOM to filter. Currently, "image", "model", "report" and "fiducials" are allowed.
 *      \b policy:
 *          - \b "never": never show the open dialog (DEFAULT)
 *          - \b "once": show only once, store the location as long as the service is started
 *          - \b "always": always show the location dialog
 *
 *
 * @see sight::io::service::reader
 * @see sight::io::dicom::Reader
 */

class MODULE_IO_DICOM_CLASS_API reader final : public sight::io::service::reader
{
public:

    SIGHT_DECLARE_SERVICE(reader, sight::io::service::reader);

    using job_created_signal_t = core::com::signal<void (core::jobs::base::sptr)>;

    MODULE_IO_DICOM_API reader() noexcept;

    MODULE_IO_DICOM_API ~reader() noexcept;

    /// Propose to read a session data file
    MODULE_IO_DICOM_API void open_location_dialog() override;

protected:

    /// Does nothing
    MODULE_IO_DICOM_API void starting() override;

    /// Does nothing
    MODULE_IO_DICOM_API void stopping() override;

    /// Parses the configuration
    MODULE_IO_DICOM_API void configuring() override;

    /// Read DICOM data from filesystem
    MODULE_IO_DICOM_API void updating() override;

    /// Returns managed path type, here service manages only single file
    MODULE_IO_DICOM_API sight::io::service::path_type_t get_path_type() const override
    {
        return sight::io::service::folder;
    }

private:

    class reader_impl;
    std::unique_ptr<reader_impl> m_pimpl;
};

} // namespace sight::module::io::session
