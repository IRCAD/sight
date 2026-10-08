/************************************************************************
 *
 * Copyright (C) 2018-2026 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include <io/__/service/reader.hpp>

#include <filesystem>

namespace sight::module::io::vtk
{

/**
 * @brief   VTK ImageSeries Reader.
 *
 * Service reading an image series using the fwVtkIO lib.
 *
 * @section Signals Signals
 * - \b notification_created(core::notification::base::sptr): emitted to display a progress bar while the
 * image is
 * loading,
 * it should be connected to a progress bar
 *
 * @section XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::io::vtk::image_series_reader">
        <data read="${imageSeries}" />
        <path file="@path/to/file" />
    </service>
 * @endcode
 *
 * @subsection In-Out In-Out
 * - \b data.read [sight::data::image_series]: ImageSeries containing read image
 *
 */
class image_series_reader : public sight::io::service::reader
{
public:

    /**
     * @brief Constructor. Do nothing.
     */
    image_series_reader() noexcept;

    SIGHT_DECLARE_SERVICE(image_series_reader, sight::io::service::reader);

    /**
     * @brief Configure the image path.
     */
    void open_location_dialog() override;

    sight::io::service::path_type_t get_path_type() const override;

protected:

    /**
     * @brief Starting method.
     *
     * This method is used to initialize the service.
     */
    void starting() override;

    /**
     * @brief Stopping method.
     *
     * The stopping method is empty for this service.
     */
    void stopping() override;

    /**
     * @brief Configuring method.
     *
     * The configuring method only calls the configuring method from the base class
     */
    void configuring() override;

    /**
     * @brief Updating method.
     *
     * This method is used to update the service.
     * The image is read.
     */
    void updating() override;

    /**
     * @brief Info method.
     *
     * This method is used to give informations about the service.
     */
    void info(std::ostream& _sstream) override;

private:

    /// Image path.
    std::filesystem::path m_fs_img_path;
};

} // namespace sight::module::io::vtk
