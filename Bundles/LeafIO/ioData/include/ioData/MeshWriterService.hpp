/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IODATA_MESHWRITERSERVICE_HPP__
#define __IODATA_MESHWRITERSERVICE_HPP__

#include "ioData/config.hpp"

#include <io/IWriter.hpp>

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
 * fwServicesRegisterMacro( ::io::IWriter , ::ioData::MeshWriterService , ::fwData::Mesh )
 */
class IODATA_CLASS_API MeshWriterService : public ::io::IWriter
{

public:

    fwCoreServiceClassDefinitionsMacro( (MeshWriterService)(::io::IWriter) );

    /// Super class of writer services
    typedef ::io::IWriter SuperClass;

    /**
     * @brief   Constructor : does nothing
     */
    IODATA_API MeshWriterService();

    /**
     * @brief   destructor
     */
    IODATA_API ~MeshWriterService() noexcept;

    /** @name Specified writer service methods ( override from ::io::IWriter )
     * @{
     */

    /**
     * @brief   returns  (filename) extension
     */
    IODATA_API virtual std::vector< std::string > getSupportedExtensions();

    /**
     * @brief Configure the mesh path.
     *
     * This method is used to find
     * the file path  using a file selector.
     */
    IODATA_API void configureWithIHM();
    /// @}

    /// Return path type managed by the service, here FILE
    IODATA_API virtual ::io::IOPathType getIOPathType() const;

protected:

    /** @name Service methods ( override from ::fwServices::IService )
     * @{
     */

    /**
     * @brief Starting method : default does nothing.
     *
     * This method is used to initialize the service.
     */
    virtual void starting()
    {
    }

    /**
     * @brief Stopping method : default does nothing.
     *
     * The stopping method is empty for this service.
     */
    virtual void stopping()
    {
    }

    /**
     * @brief Configuring method : calls implementation from `io::IWriter`
     */
    IODATA_API virtual void configuring();

    /**
     * @brief Updating method. This method is called by update() from base service ( ::fwServices::IService )
     *
     * This method is used to update the service.
     * The mesh is written with the writer ::fwDataIO::writer::MeshWriter.
     * Notify writing.
     */
    IODATA_API void updating();

    /**
     * @brief Info method.
     *
     * This method is used to give
     * informations about the service.
     *
     * @param[out] _sstream output stream
     */
    IODATA_API virtual void info(std::ostream& _sstream );
    /// @}

};

}

#endif /*__IODATA_MESHWRITERSERVICE_HPP__*/
