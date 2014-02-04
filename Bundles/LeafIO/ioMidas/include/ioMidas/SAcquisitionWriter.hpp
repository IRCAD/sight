/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOMIDAS_SACQUISITIONWRITER_HPP__
#define __IOMIDAS_SACQUISITIONWRITER_HPP__

#include <string>
#include <vector>

#include <io/IWriter.hpp>

#include <midasIO/IConfiguration.hpp>

#include "ioMidas/config.hpp"


namespace ioMidas
{

/**
 * @class SAcquisitionWriter
 * @brief Writer exporting ::fwData::Acquisition to a Midas server.
 * @note This writer cannot be used without calling configureWithIHM().
 */
class IOMIDAS_CLASS_API SAcquisitionWriter : public ::io::IWriter
{

public :

    fwCoreServiceClassDefinitionsMacro ( (SAcquisitionWriter)( ::io::IWriter) );

    /**
     * @brief Constructor
     */
    IOMIDAS_API SAcquisitionWriter() throw();

    /**
     * @brief Destructor
     */
    IOMIDAS_API virtual ~SAcquisitionWriter() throw();

protected:

    /**
     * @brief Configures the service
     *
     * Example :
     @verbatim
     <config>
        <url>http://midasserverurl.org/midas</url>  <!-- mandatory -->
        <serverVersion>3.2.8</serverVersion>        <!-- not mandatory, default value is "3.2.6" -->
        <appName>Default</appName>                  <!-- not mandatory, default value is "Default" -->
        <license>3</license>                        <!-- not mandatory, default value is "3" -->
        <privacy>Private</privacy>                  <!-- not mandatory, default value is "Private" -->
        <rootFolder>Patients</rootFolder>           <!-- not mandatory, default value is "Patients" -->
     </config>
     @endverbatim
     *
     * XML attributes :
     * - url : URL of the server
     * - serverVersion : server version (supported versions are "3.2.6" and "3.2.8")
     * - appName : Midas application name defined by server
     * - license : index of uploaded files license defined in ::midasIO::IConfiguration::LicenseIDType
     * - privacy : privacy of uploaded files (supported values are "Public", "Parent", "Private")
     * - rootFolder : main folder in Midas community where files are uploaded.
     *   This folder is automatically created if it doesn't exists.
     */
    IOMIDAS_API virtual void configuring() throw(::fwTools::Failed) ;

    /**
     * @name Overrides ::fwServices::IService
     * @{ */
    IOMIDAS_API virtual void starting() throw(::fwTools::Failed);
    IOMIDAS_API virtual void stopping() throw(::fwTools::Failed);
    IOMIDAS_API void updating() throw(::fwTools::Failed);
    IOMIDAS_API void info(std::ostream &_sstream ) ;
    IOMIDAS_API virtual std::vector< std::string > getSupportedExtensions() ;
    IOMIDAS_API virtual std::string getSelectorDialogTitle();
    IOMIDAS_API virtual void configureWithIHM();
    /**  @} */

    /// Returns managed path type, here service manages a list of files
    IOMIDAS_API ::io::IOPathType getIOPathType() const;

private :

    /**
     * @brief Midas configuration.
     * @see ::midasIO::IConfiguration
     */
    ::midasIO::IConfiguration::sptr m_config;
};

} // namespace ioMidas

#endif //__IOMIDAS_SACQUISITIONWRITER_HPP__

