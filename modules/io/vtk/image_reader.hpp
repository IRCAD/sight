/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

// Declaration of class and function export

#include <io/__/service/reader.hpp> // Definition of abstract reader class

#include <filesystem> // Used to save the file system path of loaded image

// Pre-definition of data::image to avoid inclusion file
namespace sight::data
{

class image;

} // namespace sight::data

namespace sight::core::jobs
{

class base;

} // namespace sight::core::jobs

namespace sight::module::io::vtk
{

/**
 * @brief Service reading a VTK image using the fwVtkIO lib.
 *
 * @section Signals Signals
 * - \b job_created(SPTR(core::jobs::base)): emitted to display a progress bar while the image is loading (it should be
 * connected to a job_bar).
 *
 * @section Slots Slots
 * - \b readFile(std::filesystem::path) : read the given file
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::io::vtk::image_reader">
       <inout key="data" uid="..." />
       <file>...</file>
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b data [sight::data::image]: loaded image.
 * @subsection Configuration Configuration
 * - \b file (optional): path of the image to load, if it is not defined, 'open_location_dialog()' should be called to
 * define the path.
 */
class image_reader : public sight::io::service::reader
{
public:

    image_reader() noexcept;

    ~image_reader() noexcept override = default;

    SIGHT_DECLARE_SERVICE(image_reader, sight::io::service::reader);

    using job_created_signal_t = core::com::signal<void (std::shared_ptr<core::jobs::base>)>;

    /**
     * @brief Configure the image path with a dialogBox.
     *
     * This method is used to find the file path using a file selector.
     */
    void open_location_dialog() override;

    /**
     * @brief This method is used to load an vtk image using a file path.
     * @param[in] _vtk_file file system path of vtk image
     * @param[out] _image new empty image that will contain image loaded, if reading process is a success.
     * @return bool \b true if the image loading is a success and \b false if it fails
     */
    static bool load_image(
        const std::filesystem::path& _vtk_file,
        std::shared_ptr<data::image> _image,
        const SPTR(job_created_signal_t)& _sig_job_created
    );

protected:

    sight::io::service::path_type_t get_path_type() const override;

    /// Method called when the service is started, does nothing.
    void starting() override;

    /// Method called when the service is stopped, does nothing.
    void stopping() override;

    /// Method called when the service is stopped, does nothing.
    void configuring() override;

    /**
     * @brief Updating method execute the read process.
     *
     * This method is used to update the service.
     * The image is read.
     */
    void updating() override;

    /// Info method gives some informations on service.
    void info(std::ostream& _sstream) override;

private:

    /// Image path, location of image on filesystem.
    std::filesystem::path m_fs_img_path;

    SPTR(job_created_signal_t) m_sig_job_created;
};

} // namespace sight::module::io::vtk
