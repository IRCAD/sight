/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOTUTO_EXTERNALDATAREADERSERVICE_HPP__
#define __IOTUTO_EXTERNALDATAREADERSERVICE_HPP__

#include <io/IReader.hpp>
#include <boost/filesystem/path.hpp>

#include "ioTuto/config.hpp"

namespace ioTuto
{

class IOTUTO_CLASS_API ExternalDataReaderService : public ::io::IReader
{

public:
    fwCoreServiceClassDefinitionsMacro ( (ExternalDataReaderService)( ::io::IReader) );
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
    IOTUTO_API virtual std::vector< std::string > getSupportedExtensions();

protected:

    /** @name Service methods ( override from ::fwServices::IService )
     * @{
     */

    /**
     * @brief Starting method : default does nothing.
     *
     * This method is used to initialize the service.
     */
    IOTUTO_API virtual void starting()
    {
    }

    /**
     * @brief Stopping method : default does nothing.
     *
     * The stopping method is empty for this service.
     */
    IOTUTO_API virtual void stopping()
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
    IOTUTO_API virtual void configuring( );

    /**
     * @brief Configure the image path.
     *
     * This method is used to find
     * the file path  using a file selector.
     */
    IOTUTO_API void configureWithIHM();

    /**
     * @brief Updating method. This method is called by update() from base service ( ::fwServices::IService )
     *
     * This method is used to update the service.
     */
    IOTUTO_API void updating();

    /**
     * @brief Info method.
     *
     * This method is used to give
     * informations about the service.
     *
     * @param[out] _sstream output stream
     */
    IOTUTO_API virtual void info(std::ostream &_sstream );
    /// @}

    /// Returns managed path type, here service manages only single file
    IOTUTO_API ::io::IOPathType getIOPathType() const;
};
}
#endif /*__IOTUTO_EXTERNALDATAREADERSERVICE_HPP__*/
