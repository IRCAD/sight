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

#include <core/com/signal.hpp>

#include <io/__/service/writer.hpp>

#include <filesystem>

namespace sight::data
{

class mesh;

} // namespace sight::data

namespace sight::core::jobs
{

class base;

} // namespace sight::core::jobs

namespace sight::module::io::vtk
{

/**
 * @brief  VTK Mesh Writer.
 *
 * Service writing a VTK Mesh using the fwVtkIO lib.
 *
 * @section Signals Signals
 * - \b job_created(SPTR(core::jobs::base)): emitted to display a progress bar while the image is written (it should be
 * connected to a job_bar).
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::io::vtk::mesh_writer">
       <in key="data" uid="..." />
       <file>...</file>
   </service>
   @endcode
 * @subsection Input Input
 * - \b data [sight::data::mesh]: mesh to save.
 * @subsection Configuration Configuration
 * - \b file (optional): path of the file to save, if it is not defined, 'open_location_dialog()' should be called to
 * define the path.
 */
class mesh_writer : public sight::io::service::writer
{
public:

    ~mesh_writer() noexcept override =
        default;

    SIGHT_DECLARE_SERVICE(mesh_writer, sight::io::service::writer);

    using job_created_signal_t = core::com::signal<void (std::shared_ptr<core::jobs::base>)>;

    /**
     * @brief Constructor. Do nothing.
     */
    mesh_writer() noexcept;

    /**
     * @brief Configure the mesh path.
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
     * The mesh is read.
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
     * @brief Mesh path .
     */
    std::filesystem::path m_fs_mesh_path;

    SPTR(job_created_signal_t) m_sig_job_created;

    /// Extension selected in file dialog
    std::string m_selected_extension;
};

} // namespace sight::module::io::vtk
