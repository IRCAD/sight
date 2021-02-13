/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include <io/base/service/IReader.hpp>

#include <filesystem>
#include <string>

namespace sight::core::jobs
{
class IJob;
}

namespace sight::modules::io::vtk
{

/**
 * @brief   VTK Mesh Reader.
 *
 * Service reading a VTK mesh using the fwVtkIO lib.
 *
 * @section XML Configuration
 * @code{.xml}
    <service uid="..." type="::sight::modules::io::vtk::SMeshReader">
        <inout key="data" uid="..." />
        <file>@path/to/file</file>
    </service>
 * @endcode
 *
 * @subsection In-Out In-Out
 * - \b data [sight::data::Mesh]: mesh to read
 * @subsection Configuration Configuration
 * - \b file (optional): path of the file to load, if it is not defined, 'openLocationDialog()' should be called to
 * define the path.
 */
class MODULE_IO_VTK_CLASS_API SMeshReader : public sight::io::base::service::IReader
{

public:
    virtual ~SMeshReader() noexcept
    {
    }

    fwCoreServiceMacro(SMeshReader, sight::io::base::service::IReader)

    typedef core::com::Signal< void ( SPTR(core::jobs::IJob) ) > JobCreatedSignalType;

    /// Constructor
    MODULE_IO_VTK_API SMeshReader() noexcept;

    /**
     * @brief Configure the image path.
     *
     * This method is used to find
     * the file path  using a file selector.
     * @deprecated Will be removed in sight 22.0. Use openLocationDialog() instead.
     */
    [[deprecated("Will be removed in sight 22.0. Use openLocationDialog() instead.")]]
    MODULE_IO_VTK_API virtual void configureWithIHM() override;

    /**
     * @brief Configure the image path.
     *
     * This method is used to find
     * the file path  using a file selector.
     */
    MODULE_IO_VTK_API virtual void openLocationDialog() override;

protected:

    MODULE_IO_VTK_API virtual sight::io::base::service::IOPathType getIOPathType() const override;

    /**
     * @brief Starting method.
     *
     * This method is used to initialize the service.
     */
    MODULE_IO_VTK_API virtual void starting() override;

    /**
     * @brief Stopping method.
     *
     * The stopping method is empty for this service.
     */
    MODULE_IO_VTK_API virtual void stopping() override;

    /**
     * @brief Configuring method.
     *
     * The configuring method only calls the configuring method from the base class
     */
    MODULE_IO_VTK_API virtual void configuring() override;

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
    MODULE_IO_VTK_API void info(std::ostream& _sstream ) override;

private:

    /**
     * @brief This method is used to load a mesh using the file path.
     * @param[in] _vtkFile std::filesystem::path.
     * @return true, if mesh is read, false otherwise(error or exception).
     */
    bool loadMesh(const std::filesystem::path& _vtkFile);

    /**
     * @brief Notification method.
     *
     * This method is used to notify
     * the mesh services.
     */
    void notificationOfUpdate();

    /**
     * @brief the m_bServiceIsConfigured value is \b true
     * if the mesh path is known.
     */
    bool m_bServiceIsConfigured;

    /**
     * @brief Mesh path.
     */
    std::filesystem::path m_fsMeshPath;

    /// Signal triggered when job created
    SPTR(JobCreatedSignalType) m_sigJobCreated;
};

} // namespace sight::modules::io::vtk
