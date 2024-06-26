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

#include <data/mesh.hpp>

#include <io/__/service/writer.hpp>

#include <filesystem>
#include <string>

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
 * @brief   VTK Model series writer.
 *
 * Service writing a model series as .vtk files using the fwVtkIO lib.
 *
 * @section Signals Signals
 * - \b job_created(SPTR(core::jobs::base)): emitted to display a progress bar while the image is written (it should be
 * connected to a job_bar).
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::io::vtk::model_series_writer">
       <in key="data" uid="..." />
       <folder>...</folder>
       <extension>vtk</extension>
   </service>
   @endcode
 * @subsection Input Input
 * - \b data [sight::data::model_series]: model to save.
 * @subsection Configuration Configuration
 * - \b folder (optional): path of the folder, if it is not defined, 'open_location_dialog()' should be called to define
 * the path.
 * - \b extension (optional): extension to use when writing files (vtk, vtp, obj, stl, ply). If nothing is set
 * a popup will ask to user to choose one.
 * Accepted extensions are: "vtk", "vtp", "obj" "stl" "ply"
 * Extensions aren't case sensitive but make sure there isn't a dot "." before extension name.
 */
class model_series_writer : public sight::io::service::writer
{
public:

    model_series_writer() noexcept;

    ~model_series_writer() noexcept override =
        default;

    SIGHT_DECLARE_SERVICE(model_series_writer, sight::io::service::writer);

    using job_created_signal_t = core::com::signal<void (std::shared_ptr<core::jobs::base>)>;

    /**
     * @brief Configure the mesh path.
     *
     * This method is used to find the file path  using a file selector.
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
     * @brief write the mesh on disk, using _filename
     * @param[in] _filename : name of the file to write with extension. extension will be used to choose the writer.
     * @param[in] _mesh : const pointer of the data mesh.
     */
    void write_mesh(const std::filesystem::path& _filename, const data::mesh::csptr _mesh);

    /// Job Created signal
    SPTR(job_created_signal_t) m_sig_job_created;

    ///  Stores the selectedExtension from dialog.
    std::string m_selected_extension;
};

} // namespace sight::module::io::vtk
