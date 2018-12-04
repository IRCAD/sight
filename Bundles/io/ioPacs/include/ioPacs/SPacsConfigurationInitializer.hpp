/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

    fwCoreServiceClassDefinitionsMacro( (SPacsConfigurationInitializer)( ::fwServices::IController ) );

    /**
     * @brief Constructor
     */
    IOPACS_API SPacsConfigurationInitializer() noexcept;

    /**
     * @brief Destructor
     */
    IOPACS_API virtual ~SPacsConfigurationInitializer() noexcept;

protected:

    /// Configuring method. This method is used to configure the service.
    IOPACS_API virtual void configuring() override;

    /// Override
    IOPACS_API virtual void starting() override;

    /// Override
    IOPACS_API virtual void stopping() override;

    /// Override
    IOPACS_API void updating() override;

    /// Override
    IOPACS_API void info(std::ostream& _sstream ) override;

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
