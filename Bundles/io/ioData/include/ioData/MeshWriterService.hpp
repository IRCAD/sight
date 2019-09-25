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

#include "ioData/config.hpp"

#include <fwIO/IWriter.hpp>

#include <boost/filesystem/path.hpp>

namespace ioData
{
/**
 * @brief   Mesh writer service.
 *
 * Service writing a mesh object.
 * @li This service has no specified start and stop method.
 * @li Use setConfiguration(cfg) and configure() methods to configure the mesh filename.
 * @li Use update() to write the mesh and notify observers.
 *
 * Service registered details : \n
 * fwServicesRegisterMacro( ::fwIO::IWriter , ::ioData::MeshWriterService , ::fwData::Mesh )
 */
/**
 * @brief This service writes a ::fwData::Mesh (with only triangular cells) on filesystem in format .trian.
 *
 * This format is basic. This is an ascii file which contains :
 * 1) nb of points
 * 2) list of points (x,y,z). Line numbers represent index of point in vector
 * 3) nb of triangles
 * 4) list of triangles with index of points and normals ( often set a -1 )
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::ioData::MeshWriterService">
       <in key="data" uid="..." />
   </service>
   @endcode
 * @subsection Input Input
 * - \b data [::fwData::Mesh]: mesh to write.
 */
class IODATA_CLASS_API MeshWriterService : public ::fwIO::IWriter
{

public:

    fwCoreServiceMacro(MeshWriterService, ::fwIO::IWriter);

    /// Super class of writer services
    typedef ::fwIO::IWriter SuperClass;

    /**
     * @brief   Constructor : does nothing
     */
    IODATA_API MeshWriterService();

    /**
     * @brief   destructor
     */
    IODATA_API ~MeshWriterService() noexcept;

    /** @name Specified writer service methods ( override from ::fwIO::IWriter )
     * @{
     */

    /**
     * @brief Configure the mesh path.
     *
     * This method is used to find
     * the file path  using a file selector.
     */
    IODATA_API void configureWithIHM() override;
    /// @}

    /// Return path type managed by the service, here FILE
    IODATA_API virtual ::fwIO::IOPathType getIOPathType() const override;

protected:

    /** @name Service methods ( override from ::fwServices::IService )
     * @{
     */

    /**
     * @brief Starting method : default does nothing.
     *
     * This method is used to initialize the service.
     */
    virtual void starting() override
    {
    }

    /**
     * @brief Stopping method : default does nothing.
     *
     * The stopping method is empty for this service.
     */
    virtual void stopping() override
    {
    }

    /**
     * @brief Configuring method : calls implementation from `io::IWriter`
     */
    IODATA_API virtual void configuring() override;

    /**
     * @brief Updating method. This method is called by update() from base service ( ::fwServices::IService )
     *
     * This method is used to update the service.
     * The mesh is written with the writer ::fwDataIO::writer::MeshWriter.
     * Notify writing.
     */
    IODATA_API void updating() override;

    /**
     * @brief Info method.
     *
     * This method is used to give
     * informations about the service.
     *
     * @param[out] _sstream output stream
     */
    IODATA_API virtual void info(std::ostream& _sstream ) override;
    /// @}

};

}
