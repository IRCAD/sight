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

#include "ioVTK/config.hpp"

#include <data/Mesh.hpp>

#include <fwIO/IReader.hpp>

#include <filesystem>
#include <string>

namespace fwJobs
{
class IJob;
}

namespace ioVTK
{

/**
 * @brief  Read VTK files and fill a data::ModelSeries
 * Accepted extensions: *.vtk *.vtp *.obj *.ply *.stl
 *
 * @section Signals Signals
 * - \b jobCreated(SPTR(::fwJobs::IJob)): emitted to display a progress bar while the image is loading (it should be
 * connected to a SJobBar).
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::ioVTK::SModelSeriesReader">
       <inout key="data" uid="..." />
       <file>...</file>
       <file>...</file>
       <file>...</file>
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b data [data::ModelSeries]: model series that will contain the loaded meshes.
 * @subsection Configuration Configuration
 * - \b file (optional): path of the files to load, if it is not defined, 'openLocationDialog()' should be called to
 * define the path.
 */
class IOVTK_CLASS_API SModelSeriesReader : public ::fwIO::IReader
{

public:
    fwCoreServiceMacro(SModelSeriesReader,  ::fwIO::IReader)

    typedef core::com::Signal< void ( SPTR(::fwJobs::IJob) ) > JobCreatedSignalType;

    /**
     * @brief Constructor. Do nothing.
     */
    IOVTK_API SModelSeriesReader() noexcept;

    ~SModelSeriesReader() noexcept
    {
    }

    /**
     * @brief Configure the mesh path.
     *
     * This method is used to find the file path  using a file selector.
     * @deprecated Will be removed in sight 22.0. Use openLocationDialog() instead.
     */
    [[deprecated("Will be removed in sight 22.0. Use openLocationDialog() instead.")]]
    IOVTK_API virtual void configureWithIHM() override;

    /**
     * @brief Configure the mesh path.
     *
     * This method is used to find the file path  using a file selector.
     */
    IOVTK_API virtual void openLocationDialog() override;

protected:

    IOVTK_API virtual ::fwIO::IOPathType getIOPathType() const override;

    /**
     * @brief Starting method.
     *
     * This method is used to initialize the service.
     */
    IOVTK_API virtual void starting() override;

    /**
     * @brief Stopping method.
     *
     * The stopping method is empty for this service.
     */
    IOVTK_API virtual void stopping() override;

    /**
     * @brief Configuring method.
     *
     * The configuring method only calls the configuring method from the base class
     */
    IOVTK_API virtual void configuring() override;

    /**
     * @brief Updating method.
     *
     * This method is used to update the service.
     * The mesh is read.
     */
    IOVTK_API void updating() override;

    /**
     * @brief Info method.
     *
     * This method is used to give
     * informations about the service.
     */
    IOVTK_API void info(std::ostream& _sstream ) override;

private:

    /**
     * @brief loadMesh: read _file to load _mesh data.
     * @param[in] _file path to the file to read.
     * @param[out] _mesh data::Mesh::sptr, the loaded mesh.
     */
    void loadMesh( const std::filesystem::path& _file, data::Mesh::sptr _mesh);

    SPTR(JobCreatedSignalType) m_sigJobCreated;

};

} // namespace ioVTK
