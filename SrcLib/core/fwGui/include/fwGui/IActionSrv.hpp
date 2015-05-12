/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUI_IACTIONSRV_HPP__
#define __FWGUI_IACTIONSRV_HPP__

#include <fwServices/IService.hpp>

#include "fwGui/config.hpp"

#include "fwGui/registrar/ActionRegistrar.hpp"

namespace fwGui
{

/**
 * @brief   Defines the service interface managing the menu items.
 * @class   IActionSrv
 *
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API IActionSrv : public ::fwServices::IService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (IActionSrv)(::fwServices::IService) );
    fwCoreAllowSharedFromThis();

    /// Method called when the action service is stopping
    FWGUI_API void actionServiceStopping();

    /// Method called when the action service is starting
    FWGUI_API void actionServiceStarting();

    /// Set the action service is active/inactive.
    FWGUI_API virtual void setIsActive(bool isActive);

    /// Return true if action service is active.
    FWGUI_API bool getIsActive();

    /// Return true if action 'check' feedback is inverted.
    bool getActiveStateValue()
    {
        return m_activeStateValue;
    }

    /// Set the action service executable or not.
    FWGUI_API void setIsExecutable(bool isExecutable);

    /// Return action service is executable.
    FWGUI_API bool getIsExecutable();

    /**
     * @brief Confirm that the action must be executed.
     *
     * If action is configured to be confirmed : show a dialog box to confirm execution.
     * Else return true
     *
     * @return true if user click on 'yes' button.
     */
    FWGUI_API bool confirmAction();

protected:

    FWGUI_API IActionSrv();

    FWGUI_API virtual ~IActionSrv();

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    /**
     * @brief Initialize the action.
     *
     * Example of configuration
     * @verbatim
           <service uid="item" type="::fwGui::IActionSrv" impl="::gui::action::NothingActionSrv" autoConnect="no" >
              <state active="false" executable="false" inverse="true" />
              <confirmation value="yes" message="..." />
           </service>
       @endverbatim
     * - \<state active="false" executable="false" /\> : fix the state of the action in the menu and Toolbar.
     *   - \b executable not mandatory (Default value true ) : allows to enable/disable the execution of the action.
     *     If the action appears in the interface it will be enable/disable.
     *   - \b active not mandatory (Default value false ):
     *     If the action appears in the interface it will be check/uncheck.
     *   - \b inverse not mandatory (Default value true) : allow to invert the state of the action (if "check")
     * - \<confirmation value="yes" message="..." /\> : configure if the action must be confirm by user before to execute it.
     *   - \b value : if true the action will show a confirmation message before to be executed.
     *   - \b message not mandatory : if not empty the message is shown in dialog box.
     *   - \b defaultbutton not mandatory (default defined by underlying gui backend) :
     *                      specify the dialog default button (yes or no)
     */
    FWGUI_API void initialize();


private:

    ::fwGui::registrar::ActionRegistrar::sptr m_registrar;

    /// Value for which the action will be considered 'active'
    bool m_activeStateValue;
    bool m_isActive;
    bool m_isExecutable;
    bool m_confirmAction;
    std::string m_confirmMessage;
    std::string m_defaultButton;
};

//-----------------------------------------------------------------------------

class LockAction
{
public:
    LockAction(IActionSrv::wptr action) : m_action(action)
    {
        m_action.lock()->setIsExecutable(false);
    }
    ~LockAction()
    {
        m_action.lock()->setIsExecutable(true);
    }
private:
    IActionSrv::wptr m_action;
};

//-----------------------------------------------------------------------------

} // namespace fwGui

#endif /*__FWGUI_IACTIONSRV_HPP__*/


