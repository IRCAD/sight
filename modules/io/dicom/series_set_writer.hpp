/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <io/__/service/writer.hpp>
#include <io/dicom/writer/series.hpp>

#include <filesystem>
#include <string>

namespace sight::data
{

class series_set;

}

namespace sight::module::io::dicom
{

/**
 * @brief DICOM Writer (ImageSeries/ModelSeries)
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
       <service uid="..." type="sight::module::io::dicom::series_set_writer" >
           <in key="data" uid="..." />
       </service>
 * @endcode
 *
 * @subsection Input Input
 * - \b data [sight::data::vector]: vector containing Series (ImageSeries or ModelSeries) to save in Dicom.
 */
class MODULE_IO_DICOM_CLASS_API series_set_writer : public sight::io::service::writer
{
public:

    SIGHT_DECLARE_SERVICE(series_set_writer, sight::io::service::writer);

    /**
     * @brief Constructor
     */
    MODULE_IO_DICOM_API series_set_writer() noexcept;

    /**
     * @brief Destructor
     */
    MODULE_IO_DICOM_API ~series_set_writer() noexcept override = default;

    /// Propose a directory selection where to save the DICOM files.
    MODULE_IO_DICOM_API void open_location_dialog() override;

protected:

    /// Does nothing
    MODULE_IO_DICOM_API void starting() override;

    /// Does nothing
    MODULE_IO_DICOM_API void stopping() override;

    /// Configuring method. This method is used to configure the service.
    MODULE_IO_DICOM_API void configuring() override;

    /// Write the ImageSeries in DICOM format.
    MODULE_IO_DICOM_API void updating() override;

    /// Return path type managed by the service, here FOLDER
    MODULE_IO_DICOM_API sight::io::service::path_type_t get_path_type() const override;

private:

    /**
     * @brief Write a series_set to a folder in DICOM format
     * @param[in] _folder DICOM folder
     * @param[in] _series_set series_set that must be written
     */
    void save_series_set(const std::filesystem::path _folder, SPTR(data::series_set) _series_set);

    /// Select Fiducial Export Mode using a dialog
    bool select_fiducials_export_mode();

    /// Fiducial Export Mode
    sight::io::dicom::writer::series::fiducials_export_mode m_fiducials_export_mode;
};

} // namespace sight::module::io::dicom
