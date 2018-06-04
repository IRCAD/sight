/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "ioTuto/config.hpp"

#include <fwIO/IReader.hpp>

#include <boost/filesystem/path.hpp>

namespace ioTuto
{

/**
 * @deprecated This service is deprecated and will be removed
 */
class IOTUTO_CLASS_API ExternalDataReaderService : public ::fwIO::IReader
{

public:
    fwCoreServiceClassDefinitionsMacro( (ExternalDataReaderService)( ::fwIO::IReader) );
    /**
     * @brief Constructor : does nothing
     */
    IOTUTO_API ExternalDataReaderService();

    /**
     * @brief Destructor
     */
    IOTUTO_API ~ExternalDataReaderService() noexcept;

    /**
     * @brief   returns  (filename) extension
     */
    IOTUTO_API virtual std::vector< std::string > getSupportedExtensions() override;

protected:

    /** @name Service methods ( override from ::fwServices::IService )
     * @{
     */

    /**
     * @brief Starting method : default does nothing.
     *
     * This method is used to initialize the service.
     */
    IOTUTO_API virtual void starting() override
    {
        FW_DEPRECATED_MSG("This service is deprecated", "18.0");
    }

    /**
     * @brief Stopping method : default does nothing.
     *
     * The stopping method is empty for this service.
     */
    IOTUTO_API virtual void stopping() override
    {
    }

    /**
     * @brief Configure service. This method is called by configure() from base service ( ::fwServices::IService )
     *
     * XML configuration sample:
     * @code{.xml}
       <service type="::ioTuto::ExternalDataReaderService">
       </service>
       @endcode
     *
     * Configure External data info filename.
     */
    IOTUTO_API virtual void configuring( ) override;

    /**
     * @brief Configure the image path.
     *
     * This method is used to find
     * the file path  using a file selector.
     */
    IOTUTO_API void configureWithIHM() override;

    /**
     * @brief Updating method. This method is called by update() from base service ( ::fwServices::IService )
     *
     * This method is used to update the service.
     */
    IOTUTO_API void updating() override;

    /**
     * @brief Info method.
     *
     * This method is used to give
     * informations about the service.
     *
     * @param[out] _sstream output stream
     */
    IOTUTO_API virtual void info(std::ostream& _sstream ) override;
    /// @}

    /// Returns managed path type, here service manages only single file
    IOTUTO_API ::fwIO::IOPathType getIOPathType() const override;
};
}
