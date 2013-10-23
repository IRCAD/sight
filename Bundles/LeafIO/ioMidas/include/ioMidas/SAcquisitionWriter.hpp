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
        <url>http://midasserverurl.org/midas</url>   <!-- not mandatory, default value is empty   -->
        <serverVersion>3.2.8</serverVersion>        <!-- not mandatory, default value is "3.2.6" -->
     </config>
     @endverbatim
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

    virtual void updating( ::boost::shared_ptr< const ::fwServices::ObjectMsg > _msg )
        throw(::fwTools::Failed) {} ;
    /**  @} */

    /// Returns managed path type, here service manages only single file
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

