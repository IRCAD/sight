/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <fwCom/Signal.hpp>

#include <fwIO/IWriter.hpp>

#include <filesystem>

namespace fwData
{
class Mesh;
}

namespace fwJobs
{
class IJob;
}

namespace ioVTK
{

/**
 * @brief  VTK Mesh Writer.
 *
 * Service writing a VTK Mesh using the fwVtkIO lib.
 *
 * @section Signals Signals
 * - \b jobCreated(SPTR(::fwJobs::IJob)): emitted to display a progress bar while the image is written (it should be
 * connected to a SJobBar).
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::ioVTK::SMeshWriter">
       <in key="data" uid="..." />
       <file>...</file>
   </service>
   @endcode
 * @subsection Input Input
 * - \b data [::fwData::Mesh]: mesh to save.
 * @subsection Configuration Configuration
 * - \b file (optional): path of the file to save, if it is not defined, 'configureWithIHM()' should be called to define
 * the path.
 */
class IOVTK_CLASS_API SMeshWriter : public ::fwIO::IWriter
{

public:
    ~SMeshWriter() noexcept
    {
    }

    fwCoreServiceMacro(SMeshWriter,  ::fwIO::IWriter)

    typedef ::fwCom::Signal< void ( SPTR(::fwJobs::IJob) ) > JobCreatedSignalType;

    /**
     * @brief Constructor. Do nothing.
     */
    IOVTK_API SMeshWriter() noexcept;

    /**
     * @brief Configure the mesh path.
     *
     * This method is used to find
     * the file path  using a file selector.
     */
    IOVTK_API virtual void configureWithIHM() override;

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
     * @brief the m_bServiceIsConfigured value is \b true
     * if the image path is known.
     */
    bool m_bServiceIsConfigured;

    /**
     * @brief Mesh path .
     */
    std::filesystem::path m_fsMeshPath;

    SPTR(JobCreatedSignalType) m_sigJobCreated;

};

} // namespace ioVTK
