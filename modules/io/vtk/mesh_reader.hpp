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

#include <data/mesh.hpp>

#include <io/__/service/reader.hpp>

#include <filesystem>
#include <string>

namespace sight::core::jobs
{

class base;

} // namespace sight::core::jobs

namespace sight::module::io::vtk
{

/**
 * @brief   VTK Mesh Reader.
 *
 * Service reading a VTK mesh using the fwVtkIO lib.
 *
 * @section XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::io::vtk::mesh_reader">
        <inout key="data" uid="..." />
        <file>@path/to/file</file>
    </service>
 * @endcode
 *
 * @subsection In-Out In-Out
 * - \b data [sight::data::mesh]: mesh to read
 * @subsection Configuration Configuration
 * - \b file (optional): path of the file to load, if it is not defined, 'open_location_dialog()' should be called to
 * define the path.
 */
class mesh_reader : public sight::io::service::reader
{
public:

    ~mesh_reader() noexcept override =
        default;

    SIGHT_DECLARE_SERVICE(mesh_reader, sight::io::service::reader);

    using job_created_signal_t = core::com::signal<void (std::shared_ptr<core::jobs::base>)>;

    /// Constructor
    mesh_reader() noexcept;

    /**
     * @brief Configure the image path.
     *
     * This method is used to find
     * the file path  using a file selector.
     */
    void open_location_dialog() override;

protected:

    sight::io::service::path_type_t get_path_type() const override;

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
     * @brief This method is used to load a mesh using the file path.
     * @param[in] _vtk_file std::filesystem::path.
     * @return true, if mesh is read, false otherwise(error or exception).
     */
    bool load_mesh(const std::filesystem::path& _vtk_file);

    /**
     * @brief Notification method.
     *
     * This method is used to notify
     * the mesh services.
     */
    void notification_of_update();

    /**
     * @brief Mesh path.
     */
    std::filesystem::path m_fs_mesh_path;

    /// Signal triggered when job created
    SPTR(job_created_signal_t) m_sig_job_created;
};

} // namespace sight::module::io::vtk
