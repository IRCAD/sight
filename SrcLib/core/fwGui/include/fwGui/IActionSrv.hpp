/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_IACTIONSRV_HPP_
#define _FWGUI_IACTIONSRV_HPP_

#include <fwServices/IService.hpp>

#include "fwGui/config.hpp"

#include "fwGui/registrar/ActionRegistrar.hpp"

namespace fwGui
{

/**
 * @brief   Defines the service interface managing the menu items.
 * @class   IActionSrv.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API IActionSrv : public ::fwServices::IService
{

public :

    fwCoreServiceClassDefinitionsMacro ( (IActionSrv)(::fwServices::IService) ) ;
    fwCoreAllowSharedFromThis();

    /// Method called when the action service is stopping
    FWGUI_API void actionServiceStopping();

    /// Method called when the action service is starting

    FWGUI_API void actionServiceStarting();

    /// Set the action service is activated/inactivated.
    FWGUI_API virtual void setIsActive(bool isActive);

    /// Return action service is active.
    FWGUI_API bool getIsActive();

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

protected :
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    /**
     * @brief   Constructor. Initialize default values.
     */
    FWGUI_API IActionSrv() ;

    /**
     * @brief   Destructor. Do nothing.
     */
    FWGUI_API virtual ~IActionSrv() ;

    /**
     * @brief Initialize the action.
     *
     * Example of configuration
     * @verbatim
           <service uid="item" type="::fwGui::IActionSrv" implementation="::gui::action::NothingActionSrv" autoComChannel="no" >
              <state active="false" executable="false" />
              <confirmation value="yes" message="..." />
           </service>
       @endverbatim
     * - <state active="false" executable="false" /> : fix the state of the action in the menu and Toolbar.
     *
     * \b executable not mandatory (Default value true ) : allows to enable/disable the execution of the action.
     * If the action appears in the interface it will be enable/disable.
     *
     * \b active not mandatory (Default value false ):
     *  If the action appears in the interface it will be check/uncheck.
     *
     *  - <confirmation value="yes" message="..." /> : configure if the action must be confirm by user before to exeute it
     *  \b value : if true the action will show a confirmation message before to be executed
     *  \b message not mandatory : if not empty the message is shown in dialog box.
     */
    FWGUI_API void initialize();


private:

    ::fwGui::registrar::ActionRegistrar::sptr    m_registrar;

    bool m_isActive;
    bool m_isExecutable;
    bool m_confirmAction;
    std::string m_confirmMessage;
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

#endif /*_FWGUI_IACTIONSRV_HPP_*/


