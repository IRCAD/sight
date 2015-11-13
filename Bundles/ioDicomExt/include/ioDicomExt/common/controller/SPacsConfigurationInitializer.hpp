/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IODICOMEXT_COMMON_CONTROLLER_SPACSCONFIGURATIONINITIALIZER_HPP__
#define __IODICOMEXT_COMMON_CONTROLLER_SPACSCONFIGURATIONINITIALIZER_HPP__

#include <boost/filesystem/path.hpp>

#include <fwServices/IController.hpp>
#include <fwDicomIOExt/data/PacsConfiguration.hpp>

#include "ioDicomExt/config.hpp"

namespace ioDicomExt
{

namespace common
{

namespace controller
{

/**
 * @brief   This service is used to initialize a Pacs Configuration data.
 * @class   SPacsConfigurationInitializer
 * @date    2013.
 */
class IODICOMEXT_CLASS_API SPacsConfigurationInitializer : public ::fwServices::IController
{
public:

    fwCoreServiceClassDefinitionsMacro ( (SPacsConfigurationInitializer)( ::fwServices::IController ) );

    /**
     * @brief Constructor
     */
    IODICOMEXT_API SPacsConfigurationInitializer() throw();

    /**
     * @brief Destructor
     */
    IODICOMEXT_API virtual ~SPacsConfigurationInitializer() throw();

protected:

    /**
     * @brief Configuring method. This method is used to configure the service.
     *
     * XML configuration sample:
       @verbatim
       <service uid="pacsConfigurationInitializer" impl="::ioDicomExt::common::controller::SPacsConfigurationInitializer"
         autoConnect="yes">
         <config
             localApplicationTitle="VRRender"
             pacsHostName="mypacs.mycompany.com"
             pacsApplicationTitle="PACSNAME"
             pacsApplicationPort="11112"
             moveApplicationTitle="MoveApplicationTitle"
             moveApplicationPort="11110"
             retrieveMethod="GET" />
       </service>
       @endverbatim
     */
    IODICOMEXT_API virtual void configuring() throw(::fwTools::Failed);

    /// Override
    IODICOMEXT_API virtual void starting() throw(::fwTools::Failed);

    /// Override
    IODICOMEXT_API virtual void stopping() throw(::fwTools::Failed);

    /// Override
    IODICOMEXT_API void updating() throw(::fwTools::Failed);

    /// Override
    IODICOMEXT_API void info(std::ostream &_sstream );

protected:
    /// Local application title
    std::string m_localApplicationTitle;

    /// Pacs host name
    std::string m_pacsHostName;

    /// Pacs application title
    std::string m_pacsApplicationTitle;

    /// Pacs port
    unsigned int m_pacsApplicationPort;

    /// Move application title
    std::string m_moveApplicationTitle;

    /// Move application port
    unsigned int m_moveApplicationPort;

    /// Request mode
    ::fwDicomIOExt::data::PacsConfiguration::RETRIEVE_METHOD m_retrieveMethod;


};

} // namespace controller
} // namespace common
} // namespace ioDicomExt

#endif // __IODICOMEXT_COMMON_CONTROLLER_SPACSCONFIGURATIONINITIALIZER_HPP__
