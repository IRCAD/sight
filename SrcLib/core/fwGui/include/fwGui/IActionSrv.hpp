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

    /// Method called when the action service is stopping
    FWGUI_API void actionServiceStopping();

    /// Method called when the action service is starting
    FWGUI_API void actionServiceStarting();

    /// Set the action service is activated/inactivated.
    FWGUI_API void setIsActive(bool isActive);

    /// Return action service is active.
    FWGUI_API bool getIsActive();

    /// Set the action service executable or not.
    FWGUI_API void setIsExecutable(bool isExecutable);

    /// Return action service is executable.
    FWGUI_API bool getIsExecutable();


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

    /// Initialize the layout manager and registrar.
    FWGUI_API void initialize();


private:

    ::fwGui::registrar::ActionRegistrar::sptr    m_registrar;

    bool m_isActive;
    bool m_isExecutable;
};

} // namespace fwGui

#endif /*_FWGUI_IACTIONSRV_HPP_*/


