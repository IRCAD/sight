/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "ioData/config.hpp"

#include <fwIO/IReader.hpp>

#include <boost/filesystem/path.hpp>

namespace ioData
{
/**
 * @brief   Mesh reader service.
 *
 * Service reading a trian file into a fwData::Mesh object.
 * @li This service has no specified start and stop method.
 * @li Use setConfiguration(cfg) and configure() methods to configure the mesh filename.
 * @li Use update() to read the mesh and notify observers.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::ioData::STrianMeshReader">
       <inout key="mesh" uid="..." />
       <file>....</file>
   </service>
   @endcode
 * @subsection InOut InOut
 * - \b mesh [::fwData::Mesh]: mesh.
 * @subsubsection Configuration Configuration
 * - \b file : .trian file to read
 */
class IODATA_CLASS_API STrianMeshReader : public ::fwIO::IReader
{

public:

    fwCoreServiceClassDefinitionsMacro( (STrianMeshReader)(::fwIO::IReader) );

    /// Super class of reader services
    typedef ::fwIO::IReader SuperClass;

    /** @name Specified reader service methods ( override from ::fwIO::IReader )
     * @{
     */

    /**
     * @brief   returns  (filename) extension
     */
    IODATA_API virtual std::vector< std::string > getSupportedExtensions() override;

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
     * @brief Configuring method : calls implementation from `io::IReader`
     */
    IODATA_API virtual void configuring() override;

    /**
     * @brief Updating method. This method is called by update() from base service ( ::fwServices::IService )
     *
     * This method is used to update the service.
     * The mesh is read with the reader ::fwDataIO::reader::MeshReader.
     * Notify reading.
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
