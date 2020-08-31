/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "ioPacs/config.hpp"

#include <fwPacsIO/data/PacsConfiguration.hpp>

#include <fwServices/IController.hpp>

#include <filesystem>

namespace ioPacs
{

/**
 * @brief This service is used to initialize a PACS Configuration data and synchronize it with a preference key.
 *
 * @section XML XML Configuration
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
 *
 * @subsection In-Out In-Out:
 * - \b config [::fwPacsIO::data::PacsConfiguration]: PACS configuration data.
 *
 * @subsection Configuration Configuration:
 * - \b localApplicationTitle (mandatory, string): Name of your local application, it must be known by the PACS.
 * - \b pacsHostName (mandatory, string): Host name of the PACS.
 * - \b pacsApplicationTitle (mandatory, string): Name of the PACS.
 * - \b pacsApplicationPort (mandatory, string): Port of the PACS.
 * - \b moveApplicationTitle (mandatory, string): Move name used by the MOVE protocol but. It should be the same as the
 * local application name.
 * - \b moveApplicationPort (mandatory, string): Port used by the MOVE protocol.
 * - \b retrieveMethod (mandatory, GET/MOVE): Retrieve method protocol.
 * - \b preferenceKey (mandatory, string): Key used to store all these values in preferences.
 */
class IOPACS_CLASS_API SPacsConfigurationInitializer : public ::fwServices::IController
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SPacsConfigurationInitializer,  ::fwServices::IController)

    /// Creates the service.
    IOPACS_API SPacsConfigurationInitializer() noexcept;

    /// Destroyes the service.
    IOPACS_API virtual ~SPacsConfigurationInitializer() noexcept;

protected:

    /// Configures members.
    IOPACS_API void configuring() override;

    /// Retrieves the PACS configuration from preferences.
    IOPACS_API void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect ::fwPacsIO::data::PacsConfiguration::s_MODIFIED_SIG of s_CONFIG_INOUT to
     *::ioPacs::SPacsConfigurationInitializer::s_UPDATE_SLOT.
     */
    IOPACS_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override final;

    /// Stores the PACS configuration into preferences.
    IOPACS_API void updating() override;

    /**
     * @brief Updates preferences.
     *
     * @see updating()
     */
    IOPACS_API void stopping() override;

private:

    /// Defines the AET of the SCU (client name).
    std::string m_SCUAppEntityTitle;

    /// Defines the AET of the SCP (server name).
    std::string m_SCPAppEntityTitle;

    /// Defines the SCP host name (server adress).
    std::string m_SCPHostName;

    /// Defines the SCP port (server port).
    unsigned short m_SCPPort {0};

    /// Defines the request mode, GET or MOVE. Usually, most SCU use the C-MOVE method.
    ::fwPacsIO::data::PacsConfiguration::RETRIEVE_METHOD m_retrieveMethod;

    /**
     * @brief Defines the move AET. This AET is used to receive C-MOVE responses.
     *
     * C-MOVE request are sent from the SCU to the SCP. The SCP will send its response based on its configuration.
     * Usually the configuration contains an IP and a port that match SCU configuration.
     * For more information, see the link bellow:
     * https://book.orthanc-server.com/dicom-guide.html#dicom-network-protocol.
     */
    std::string m_moveAppEntityTitle;

    /// Defines the move port. This port is use to receive C-MOVE responses.
    unsigned short m_movePort {0};

    /// Defines the key to save/load preferences.
    std::string m_preferenceKey;

};

} // namespace ioPacs.
