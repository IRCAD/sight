/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "gui/config.hpp"

#include <fwGui/IActionSrv.hpp>

#include <fwServices/IService.hpp>

#include <vector>

namespace gui
{
namespace action
{

/**
 * @brief   Apply an action (start, stop, ...) on a service specify by uid.
 *
 * This action works on a ::fwData::Object. It does the action specify by the specify config.
 * This action can be :
 *   - Start a service :
 * @code{.xml}
           <service uid="actionUid" type="::fwGui::IActionSrv" impl="::gui::action::SStarter" autoConnect="no">
              <start uid="Uid_of_the_service" />
           </service>
   @endcode
 * If the service is stopped, the service specified by "Uid_of_the_service" is started and updated. Otherwise it is just
 * updated.
 *
 *   - Only start a service :
 * @code{.xml}
           <service uid="actionUid" type="::fwGui::IActionSrv" impl="::gui::action::SStarter" autoConnect="no">
              <start_only uid="Uid_of_the_service" />
           </service>
   @endcode
 * If the service is stopped, the service specified by "Uid_of_the_service" is started. Otherwise nothing happens.
 *
 *   - Start a service if exists :
 * @code{.xml}
           <service uid="actionUid" type="::fwGui::IActionSrv" impl="::gui::action::SStarter" autoConnect="no">
              <start_if_exists uid="Uid_of_the_service" />
           </service>
   @endcode
 * Test if the service specified by "Uid_of_the_service" exists before starting it.
 *
 *   - Stop a service :
 * @code{.xml}
           <service uid="actionUid" type="::fwGui::IActionSrv" impl="::gui::action::SStarter" autoConnect="no">
              <stop uid="Uid_of_the_service" />
           </service>
   @endcode
 *  If the service specified by "Uid_of_the_service" has been started, it is stopped. Otherwise, nothing appends.
 *
 *   - Stop a service if exists :  Test if the service exist before stopping it
 * @code{.xml}
           <service uid="actionUid" type="::fwGui::IActionSrv" impl="::gui::action::SStarter" autoConnect="no">
              <stop_if_exists uid="Uid_of_the_service" />
           </service>
   @endcode
 *  Test if the service specified by "Uid_of_the_service" exists before stopping it.
 *
 *   - Start and update or stop the service:
 * @code{.xml}
           <service uid="actionUid" type="::fwGui::IActionSrv" impl="::gui::action::SStarter" autoConnect="no">
              <start_or_stop uid="Uid_of_the_service" />
           </service>
   @endcode
 * If the service is stopped, this starts and updates the service. If the service is started, this stops the
 * service.
 *
 *   - Only start or stop the service:
 * @code{.xml}
           <service uid="actionUid" type="::fwGui::IActionSrv" impl="::gui::action::SStarter" autoConnect="no">
              <start_only_or_stop uid="Uid_of_the_service" />
           </service>
   @endcode
 * If the service is stopped, this starts the service. If the service is started, this stops the
 * service.
 * To notice : when the starterActionService is stopped, it stops all the associated services which have been started by
 * itself.
 */

class GUI_CLASS_API SStarter : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceMacro(SStarter, ::fwGui::IActionSrv);
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    /**
     * @brief Constructor. Do nothing.
     */
    GUI_API SStarter() noexcept;

    /**
     * @brief Destructor. Do nothing.
     */
    GUI_API virtual ~SStarter() noexcept;

protected:

    enum ActionType
    {
        START,
        STOP,
        START_OR_STOP,
        START_IF_EXISTS,
        START_ONLY,
        START_ONLY_OR_STOP,
        STOP_IF_EXISTS,
        DO_NOTHING
    };

    /**
     * @brief This method gives information about the class. Do nothing.
     */
    GUI_API virtual void info(std::ostream& _sstream ) override;

    /**
     * @brief This method starts-updates or stops the specified services
     */
    GUI_API void updating() override;

    /**
     * @brief This method is used to configure the service parameters: specifies which services must be started or
     * stopped
     */
    GUI_API void configuring() override;

    GUI_API virtual void starting() override;

    /**
     * @brief Uninitialized the service activity.
     * All services started by this action are stopped.
     */
    GUI_API virtual void stopping() override;

private:
    typedef ::fwTools::fwID::IDType IDSrvType;
    typedef std::pair< IDSrvType, ActionType > PairIDActionType;
    typedef std::vector<PairIDActionType> VectPairIDActionType;
    typedef std::set<IDSrvType> SetIDSrvType;

    // vector representing uuid's services that must be started (true) or stopped (false)
    VectPairIDActionType m_uuidServices;
    SetIDSrvType m_idStartedSrvSet;
};

} // namespace action
} // namespace gui
