/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "ioData/config.hpp"

#include <fwIO/IWriter.hpp>

#include <boost/filesystem/path.hpp>

namespace ioData
{
/**
 * @brief   Transformation matrix 3D writer service.
 *
 * Service writing a TransformationMatrix3D object
 * @li Use setConfiguration(cfg) and configure() methods to configure the matrix filename.
 * @li Use start() to initialize the service.
 * @li Use update() to write the transformation matrix.
 * @li Use stop() to stop service before to destroy it.
 *
 * Service registered details : \n
 * fwServicesRegisterMacro( ::fwIO::IWriter , ::ioData::TransformationMatrix3DWriterService ,
 *::fwData::TransformationMatrix3D )
 */
class IODATA_CLASS_API TransformationMatrix3DWriterService : public ::fwIO::IWriter
{

public:

    fwCoreServiceClassDefinitionsMacro( (TransformationMatrix3DWriterService)(::fwIO::IWriter) );

    /// Super class of writer services
    typedef ::fwIO::IWriter SuperClass;

    /** @name Specified writer service methods ( override from ::fwIO::IWriter )
     * @{
     */

    /**
     * @brief Configure the matrix path.
     *
     * This method is used to find
     * the file path  using a file selector.
     */
    IODATA_API virtual void configureWithIHM() override;

    /// Return path type managed by the service, here FILE
    IODATA_API virtual ::fwIO::IOPathType getIOPathType() const override;

    ///@}

protected:

    /** @name Service methods ( override from ::fwServices::IService )
     * @{
     */

    /**
     * @brief Starting method. This method is called by start() from base service ( ::fwServices::IService )
     *
     * This method is used to initialize the service. The starting method is empty for this service.
     */
    IODATA_API virtual void starting() override;

    /**
     * @brief Stopping method. This method is called by stop() from base service ( ::fwServices::IService )
     *
     * The stopping method is empty for this service.
     */
    IODATA_API virtual void stopping() override;

    /**
     * @brief Configuring method : calls implementation from `io::IWriter`
     */
    IODATA_API virtual void configuring() override;

    /**
     * @brief Updating method. This method is called by update() from base service ( ::fwServices::IService )
     *
     * This method is used to update the service.
     * The transformation matrix is written with the writer ::fwDataIO::writer::TransformationMatrix3DWriter.
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

} // namespace ioData
