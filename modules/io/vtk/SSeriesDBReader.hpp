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

#include <data/location/ILocation.hpp>
#include <data/Mesh.hpp>

#include <io/base/service/IReader.hpp>

#include <filesystem>
#include <string>

namespace sight::data
{
class SeriesDB;
}

namespace sight::core::jobs
{
class IJob;
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
 * - \b jobCreated(SPTR(core::jobs::IJob)): emitted to display a progress bar while the image is loading (it should be
 * connected to a SJobBar).
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::io::vtk::SSeriesDBReader">
      <inout key="data" uid="..." />
      <file>...</file>
      <file>...</file>
      <file>...</file>
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b data [sight::data::SeriesDB]: seriesDB to load.
 * @subsection Configuration Configuration
 * - \b file (optional): path of the files to load, if it is not defined, 'openLocationDialog()' should be called to
 * define the path.
 */
class MODULE_IO_VTK_CLASS_API SSeriesDBReader : public sight::io::base::service::IReader
{

public:
    typedef core::com::Signal< void ( SPTR(core::jobs::IJob) ) > JobCreatedSignalType;

    /**
     * @brief Constructor. Do nothing.
     */
    MODULE_IO_VTK_API SSeriesDBReader() noexcept;

    ~SSeriesDBReader() noexcept
    {
    }

    SIGHT_DECLARE_SERVICE(SSeriesDBReader, sight::io::base::service::IReader)

    /**
     * @brief Configure the vtk file path.
     *
     * This method is used to find
     * the file path  using a file selector.
     * @deprecated Will be removed in sight 22.0. Use openLocationDialog() instead.
     */
    [[deprecated("Will be removed in sight 22.0. Use openLocationDialog() instead.")]]
    MODULE_IO_VTK_API void configureWithIHM() override;

    /**
     * @brief Configure the vtk file path.
     *
     * This method is used to find
     * the file path  using a file selector.
     */
    MODULE_IO_VTK_API void openLocationDialog() override;

protected:

    MODULE_IO_VTK_API sight::io::base::service::IOPathType getIOPathType() const override;

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
    MODULE_IO_VTK_API void info(std::ostream& _sstream ) override;

private:

    /**
     * @brief Load VTK mesh or image and push it into SeriesDB.
     * @param[in] vtkFiles vector of std::filesystem::path.
     * @param[out] seriesDB std::shared_ptr< data::SeriesDB >.
     * @return bool.
     *
     * This method is used to load a mesh using the file path.
     */
    void loadSeriesDB( const data::location::ILocation::VectPathType& vtkFiles,
                       const SPTR(data::SeriesDB)& seriesDB );

    SPTR(JobCreatedSignalType) m_sigJobCreated;

};

} // namespace sight::module::io::vtk
