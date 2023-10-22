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

#include "modules/io/vtk/config.hpp"

#include <data/mesh.hpp>

#include <io/__/service/reader.hpp>

#include <filesystem>
#include <string>

namespace sight::data
{

class series_set;

}

namespace sight::core::jobs
{

class base;

}

namespace sight::module::io::vtk
{

/**
 * @brief   VTK File Reader.
 *
 * Service reading a VTK files (mesh or image) using the fwVtkIO lib.
 * Accepted extensions: *.vtk *.vtp *.vti *.mhd *.vtu *.obj *.ply *.stl
 *
 * @section Signals Signals
 * - \b jobCreated(SPTR(core::jobs::base)): emitted to display a progress bar while the image is loading (it should be
 * connected to a job_bar).
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::io::vtk::series_set_reader">
      <inout key="data" uid="..." />
      <file>...</file>
      <file>...</file>
      <file>...</file>
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b data [sight::data::series_set]: series_set to load.
 * @subsection Configuration Configuration
 * - \b file (optional): path of the files to load, if it is not defined, 'openLocationDialog()' should be called to
 * define the path.
 */
class MODULE_IO_VTK_CLASS_API series_set_reader : public sight::io::service::reader
{
public:

    typedef core::com::signal<void (SPTR(core::jobs::base))> job_created_signal_t;

    /**
     * @brief Constructor. Do nothing.
     */
    MODULE_IO_VTK_API series_set_reader() noexcept;

    ~series_set_reader() noexcept
    {
    }

    SIGHT_DECLARE_SERVICE(series_set_reader, sight::io::service::reader);

    /**
     * @brief Configure the vtk file path.
     *
     * This method is used to find
     * the file path  using a file selector.
     */
    MODULE_IO_VTK_API void openLocationDialog() override;

protected:

    MODULE_IO_VTK_API sight::io::service::IOPathType getIOPathType() const override;

    /**
     * @brief Starting method.
     *
     * This method is used to initialize the service.
     */
    MODULE_IO_VTK_API void starting() override;

    /**
     * @brief Stopping method.
     *
     * The stopping method is empty for this service.
     */
    MODULE_IO_VTK_API void stopping() override;

    /**
     * @brief Configuring method.
     *
     * The configuring method only calls the configuring method from the base class
     */
    MODULE_IO_VTK_API void configuring() override;

    /**
     * @brief Updating method.
     *
     * This method is used to update the service.
     * The image is read.
     */
    MODULE_IO_VTK_API void updating() override;

    /**
     * @brief Info method.
     *
     * This method is used to give
     * informations about the service.
     */
    MODULE_IO_VTK_API void info(std::ostream& _sstream) override;

private:

    /**
     * @brief Load VTK mesh or image and push it into series_set.
     * @param[in] vtkFiles vector of std::filesystem::path.
     * @param[out] series_set std::shared_ptr< data::series_set >.
     * @return bool.
     *
     * This method is used to load a mesh using the file path.
     */
    void loadSeriesSet(
        const std::vector<std::filesystem::path>& _vtk_files,
        const SPTR(data::series_set)& _series_set
    );

    SPTR(job_created_signal_t) m_sigJobCreated;
};

} // namespace sight::module::io::vtk
