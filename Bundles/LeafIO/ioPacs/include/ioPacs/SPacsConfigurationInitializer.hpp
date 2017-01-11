/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOPACS_SPACSCONFIGURATIONINITIALIZER_HPP__
#define __IOPACS_SPACSCONFIGURATIONINITIALIZER_HPP__

#include "ioPacs/config.hpp"

#include <fwPacsIO/data/PacsConfiguration.hpp>
#include <fwServices/IController.hpp>

#include <boost/filesystem/path.hpp>

namespace ioPacs
{

/**
 * @brief   This service is used to initialize a Pacs Configuration data.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::ioPacs::SPacsConfigurationInitializer">
            <inout key="config" uid="..." />
             <config
                 localApplicationTitle="VRRender"
                 pacsHostName="mypacs.mycompany.com"
                 pacsApplicationTitle="PACSNAME"
                 pacsApplicationPort="11112"
                 moveApplicationTitle="MoveApplicationTitle"
                 moveApplicationPort="11110"
                 retrieveMethod="GET"
                 preferenceKey="PACS_CONFIGURATION"
             />
       </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b config [::fwPacsIO::data::PacsConfiguration]: PACS configuration data.
 */

class IOPACS_CLASS_API SPacsConfigurationInitializer : public ::fwServices::IController
{
public:

    fwCoreServiceClassDefinitionsMacro ( (SPacsConfigurationInitializer)( ::fwServices::IController ) );

    /**
     * @brief Constructor
     */
    IOPACS_API SPacsConfigurationInitializer() throw();

    /**
     * @brief Destructor
     */
    IOPACS_API virtual ~SPacsConfigurationInitializer() throw();

protected:

    /// Configuring method. This method is used to configure the service.
    IOPACS_API virtual void configuring() throw(::fwTools::Failed);

    /// Override
    IOPACS_API virtual void starting() throw(::fwTools::Failed);

    /// Override
    IOPACS_API virtual void stopping() throw(::fwTools::Failed);

    /// Override
    IOPACS_API void updating() throw(::fwTools::Failed);

    /// Override
    IOPACS_API void info(std::ostream& _sstream );

protected:
    /// Local application title
    std::string m_localApplicationTitle;

    /// Pacs host name
    std::string m_pacsHostName;

    /// Pacs application title
    std::string m_pacsApplicationTitle;

    /// Pacs port
    unsigned short m_pacsApplicationPort;

    /// Move application title
    std::string m_moveApplicationTitle;

    /// Move application port
    unsigned short m_moveApplicationPort;

    /// Request mode
    ::fwPacsIO::data::PacsConfiguration::RETRIEVE_METHOD m_retrieveMethod;

    /// The key to save/load the preferences
    std::string m_preferenceKey;

};

} // namespace ioPacs

#endif // __IOPACS_SPACSCONFIGURATIONINITIALIZER_HPP__
