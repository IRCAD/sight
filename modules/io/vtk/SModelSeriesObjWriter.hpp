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

#include <data/Mesh.hpp>

#include <io/__/service/writer.hpp>

#include <filesystem>
#include <string>

namespace sight::data
{

class Mesh;

} // namespace sight::data

namespace sight::core::jobs
{

class base;

} // namespace sight::core::jobs

namespace sight::module::io::vtk
{

/**
 * @brief   Wavefront OBJ files.
 *
 * Service writing a model series as .obj files using the fwVtkIO lib.
 *
 * @section Signals Signals
 * - \b jobCreated(SPTR(core::jobs::base)): emitted to display a progress bar while the image is written (it should be
 * connected to a SJobBar).
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::io::vtk::SModelSeriesObjWriter">
       <in key="data" uid="..." />
       <folder>...</folder>
   </service>
   @endcode
 * @subsection Input Input
 * - \b data [sight::data::ModelSeries]: model to save.
 * @subsection Configuration Configuration
 * - \b folder (optional): path of the folder, if it is not defined, 'openLocationDialog()' should be called to define
 * the path.
 */
class MODULE_IO_VTK_CLASS_API SModelSeriesObjWriter : public sight::io::service::writer
{
public:

    typedef core::com::signal<void (SPTR(core::jobs::base))> JobCreatedSignalType;

    /**
     * @brief Constructor. Do nothing.
     */
    MODULE_IO_VTK_API SModelSeriesObjWriter() noexcept;

    ~SModelSeriesObjWriter() noexcept override =
        default;

    SIGHT_DECLARE_SERVICE(SModelSeriesObjWriter, sight::io::service::writer);

    /**
     * @brief Configure the mesh path.
     *
     * This method is used to find the file path  using a file selector.
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
     * The mesh is read.
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
     * @brief Mesh path.
     */
    std::filesystem::path m_fsMeshPath;

    SPTR(JobCreatedSignalType) m_sigJobCreated;
};

} // namespace sight::module::io::vtk
