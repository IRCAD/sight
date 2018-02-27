/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
 *   - Start or stop the service:
 * @code{.xml}
           <service uid="actionUid" type="::fwGui::IActionSrv" impl="::gui::action::SStarter" autoConnect="no">
              <start_or_stop uid="Uid_of_the_service" />
           </service>
   @endcode
 * If the service is stopped, this works as start the service. If the service is started, this works as stop the
 * service.
 *
 * To notice : when the starterActionService is stopped, it stops all the associated services which have been started by
 * itself.
 */

class GUI_CLASS_API SStarter : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceClassDefinitionsMacro( (SStarter)(::fwGui::IActionSrv) );
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
