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
 * @brief   Transformation matrix 3D reader service.
 *
 * Service reading a TransformationMatrix3D object.
 * @li Use setConfiguration(cfg) and configure() methods to configure the matrix filename.
 * @li Use start() to initialize the service.
 * @li Use update() to read the transformation matrix.
 * @li Use stop() to stop service before to destroy it.
 *
 * Service registered details : \n
 * fwServicesRegisterMacro( ::fwIO::IReader , ::ioData::TransformationMatrix3DReaderService ,
 *::fwData::TransformationMatrix3D )
 */
class IODATA_CLASS_API TransformationMatrix3DReaderService : public ::fwIO::IReader
{

public:
    fwCoreServiceClassDefinitionsMacro( (TransformationMatrix3DReaderService)(::fwIO::IReader) );

    /// Super class of reader services
    typedef ::fwIO::IReader SuperClass;

    /** @name Specified reader service methods ( override from ::fwIO::IReader )
     * @{
     */

    /**
     * @brief Configure the image path.
     *
     * This method is used to find
     * the file path  using a file selector.
     */
    IODATA_API virtual void configureWithIHM() override;

    /**
     * @brief   returns  (filename) extension
     */
    IODATA_API virtual std::vector< std::string > getSupportedExtensions() override;
    /// @}

    /// Return path type managed by the service, here FILE
    IODATA_API virtual ::fwIO::IOPathType getIOPathType() const override;

protected:

    /** @name Service methods ( override from ::fwServices::IService )
     * @{
     */

    /**
     * @brief Starting method. This method is called by start() from base service ( ::fwServices::IService )
     *
     * This method is used to initialize the service. The starting method is empty for this service.
     */
    IODATA_API virtual void starting( ) override;

    /**
     * @brief Stopping method. This method is called by stop() from base service ( ::fwServices::IService )
     *
     * The stopping method is empty for this service.
     */
    IODATA_API virtual void stopping( ) override;

    /**
     * @brief Configuring method : calls implementation from `io::IReader`
     */
    IODATA_API virtual void configuring() override;

    /**
     * @brief Updating method. This method is called by update() from base service ( ::fwServices::IService )
     *
     * This method is used to update the service.
     * The transformation matrix is read with the reader ::fwDataIO::reader::TransformationMatrix3DReader.
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

} // namespace ioData
