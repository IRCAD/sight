/************************************************************************
 *
 * Copyright (C) 2021-2026 IRCAD France
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

#include <data/series_set.hpp>

#include <io/__/service/reader.hpp>
#include <io/dicom/reader/file.hpp>

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
 * - \b notification_created(core::notification::base::sptr): emitted to display a progress bar while the
 * image is
 * read,
 * it should be connected to a progress bar
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

class reader final : public sight::io::service::reader
{
public:

    SIGHT_DECLARE_SERVICE(reader, sight::io::service::reader);

    reader() noexcept;

    ~reader() noexcept final;

    /// Propose to read a session data file
    void open_location_dialog() final;

    /// Returns managed path type, here service manages only single file
    sight::io::service::path_type_t get_path_type() const final
    {
        return sight::io::service::folder;
    }

protected:

    /// Does nothing
    void starting() final;

    /// Does nothing
    void stopping() final;

    /// Parses the configuration
    void configuring() final;

    /// Read DICOM data from filesystem
    void updating() final;

private:

    /// Clear location and selected series
    void clear();

    [[nodiscard]] bool show_location();
    [[nodiscard]] bool scan();
    [[nodiscard]] bool show_selection();

    /// Dialog policy to use for dialogs. By default, always show dialog
    dialog_policy m_dialog_policy {dialog_policy::always};

    /// Default filters to use when scanning for DICOM files
    sight::data::series::sop_keywords_t m_filters {
        data::series::dicom_types_to_sops(
            static_cast<data::series::dicom_types>(data::series::dicom_t::image)
            | static_cast<data::series::dicom_types>(data::series::dicom_t::model)
            | static_cast<data::series::dicom_types>(data::series::dicom_t::report)
        )
    };

    std::string m_displayed_columns =
        "PatientName/SeriesInstanceUID,PatientSex,PatientBirthDate/Icon,Modality,StudyDescription/SeriesDescription,StudyDate/SeriesDate,StudyTime/SeriesTime,PatientAge,BodyPartExamined,PatientPositionString,ContrastBolusAgent,AcquisitionTime,ContrastBolusStartTime";

    /// The reader to use to read all DICOM files
    sight::io::dicom::reader::file::sptr m_reader;

    /// This will hold the scanned / selected series
    sight::data::series_set::sptr m_selection;
};

} // namespace sight::module::io::dicom
