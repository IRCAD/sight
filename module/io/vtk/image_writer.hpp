/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include <core/notification/observer.hpp>

#include <io/__/service/writer.hpp>

#include <filesystem>

namespace sight::data
{

class image;

} // namespace sight::data

namespace sight::module::io::vtk
{

/**
 * @brief   VTK Image Writer
 *
 * Service writing a VTK Image using the fwVtkIO lib.
 *
 * @section Signals Signals
 * - \b notification_created(core::notification::base::sptr): emitted to display a progress bar while the
 * image is
 * written,
 * it should be connected to a progress bar
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::io::vtk::image_writer">
       <data write="..." />
       <path file="..." />
   </service>
   @endcode
 * @subsection Input Input
 * - \b data.write [sight::data::image]: image to save.
 * @subsection Configuration Configuration
 * - \b file (optional): path of the file to save, if it is not defined, 'open_location_dialog()' should be called to
 * define the path.
 */
class image_writer : public sight::io::service::writer
{
public:

    /**
     * @brief Constructor. Do nothing.
     */
    image_writer() noexcept;
    ~image_writer() noexcept override = default;

    SIGHT_DECLARE_SERVICE(image_writer, sight::io::service::writer);

    /**
     * @brief Configure the image path.
     *
     * This method is used to find
     * the file path  using a file selector.
     */
    void open_location_dialog() override;

    /**
     * @brief Save a VTK image.
     * @param[in] _img_file std::filesystem::path.
     * @param[in] _image std::shared_ptr< data::image >.
     * @param[in] _sig_notification_created signal emitted when the image is saved.
     * @return bool.
     *
     * This method is used to save an image using the file path.
     * Returns \b true if the image saving is a success and \b false if it fails
     */
    static bool save_image(
        const std::filesystem::path& _img_file,
        const sight::csptr<data::image>& _image,
        sight::sptr<core::notification::observer> _progress
    );

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
     * This method is used to give
     * informations about the service.
     */
    void info(std::ostream& _sstream) override;

private:

    /**
     * @brief Image path.
     */
    std::filesystem::path m_fs_img_path;
};

} // namespace sight::module::io::vtk
