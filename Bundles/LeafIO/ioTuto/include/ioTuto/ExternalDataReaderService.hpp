/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _IOTUTO_EXTERNALDATAREADER_HPP_
#define _IOTUTO_EXTERNALDATAREADER_HPP_

#include <io/IReader.hpp>
#include <boost/filesystem/path.hpp>
#include "ioTuto/config.hpp"
#include "ioTuto/config.hpp"

namespace ioTuto
{

class IOTUTO_CLASS_API ExternalDataReaderService : public ::io::IReader
{

public:

    /**
     * @brief Constructor : does nothing
    */
    IOTUTO_API ExternalDataReaderService() ;

    /**
     * @brief Destructor
     */
        IOTUTO_API ~ExternalDataReaderService() throw() ;

    /**
     * @brief   returns  (filename) extension
     */
    IOTUTO_API virtual std::vector< std::string > getSupportedExtensions() ;

protected:

    /** @name Service methods ( override from ::fwServices::IService )
     * @{
     */

    /**
     * @brief Starting method : default does nothing.
     *
     * This method is used to initialize the service.
     */
    IOTUTO_API virtual void starting() throw(::fwTools::Failed){};

    /**
     * @brief Stopping method : default does nothing.
     *
     * The stopping method is empty for this service.
     */
IOTUTO_API virtual void stopping() throw(::fwTools::Failed){};


    /**
     * @brief Configure service. This method is called by configure() from base service ( ::fwServices::IService )
     *
     * XML configuration sample:
     * @verbatim
    <service type="::ioTuto::ExternalDataReaderService">
    </service>
     @endverbatim
     *
     * Configure External data info filename.
     */
IOTUTO_API virtual void configuring( ) throw(::fwTools::Failed);

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
    IOTUTO_API void updating() throw(::fwTools::Failed);

    /**
     * @brief React on modifications : default does nothing.
     *
     * @note This method is automaticaly called by update( msg ) method from base service ( ::fwServices::IService ).
     *
     * @param[in] _msg information message for modification
     */
    IOTUTO_API void updating( ::boost::shared_ptr< const ::fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed){};

    /**
     * @brief Info method.
     *
     * This method is used to give
     * informations about the service.
     *
     * @param[out] _sstream output stream
     */
    IOTUTO_API virtual void info(std::ostream &_sstream ) ;
    /// @}

private:

    ::boost::filesystem::path m_fsExternalDataPath;

};
}
#endif /*_IOTUTO_EXTERNALDATAREADER_HPP_*/
