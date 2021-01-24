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

#include <data/location/ILocation.hpp>
#include <data/Mesh.hpp>

#include <fwIO/IReader.hpp>

#include <filesystem>
#include <string>

namespace sight::data
{
class SeriesDB;
}

namespace fwJobs
{
class IJob;
}

namespace ioVTK
{

/**
 * @brief   VTK File Reader.
 *
 * Service reading a VTK files (mesh or image) using the fwVtkIO lib.
 * Accepted extensions: *.vtk *.vtp *.vti *.mhd *.vtu *.obj *.ply *.stl
 *
 * @section Signals Signals
 * - \b jobCreated(SPTR(::fwJobs::IJob)): emitted to display a progress bar while the image is loading (it should be
 * connected to a SJobBar).
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::ioVTK::SSeriesDBReader">
      <inout key="data" uid="..." />
      <file>...</file>
      <file>...</file>
      <file>...</file>
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b data [data::SeriesDB]: seriesDB to load.
 * @subsection Configuration Configuration
 * - \b file (optional): path of the files to load, if it is not defined, 'openLocationDialog()' should be called to
 * define the path.
 */
class IOVTK_CLASS_API SSeriesDBReader : public ::fwIO::IReader
{

public:
    typedef core::com::Signal< void ( SPTR(::fwJobs::IJob) ) > JobCreatedSignalType;

    /**
     * @brief Constructor. Do nothing.
     */
    IOVTK_API SSeriesDBReader() noexcept;

    ~SSeriesDBReader() noexcept
    {
    }

    fwCoreServiceMacro(SSeriesDBReader,  ::fwIO::IReader)

    /**
     * @brief Configure the vtk file path.
     *
     * This method is used to find
     * the file path  using a file selector.
     * @deprecated Will be removed in sight 22.0. Use openLocationDialog() instead.
     */
    [[deprecated("Will be removed in sight 22.0. Use openLocationDialog() instead.")]]
    IOVTK_API virtual void configureWithIHM() override;

    /**
     * @brief Configure the vtk file path.
     *
     * This method is used to find
     * the file path  using a file selector.
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
     * The image is read.
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

} // namespace ioVTK
