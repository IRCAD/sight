/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_IMENUBARSRV_HPP_
#define _FWGUI_IMENUBARSRV_HPP_

#include <fwServices/IService.hpp>

#include "fwGui/config.hpp"
#include "fwGui/fwContainer.hpp"

#include "fwGui/registrar/MenuBarRegistrar.hpp"
#include "fwGui/layoutManager/IMenuBarLayoutManager.hpp"


namespace fwGui
{

/**
 * @brief   Defines the service interface managing the menu bar.
 * @class   IMenuBarSrv.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API IMenuBarSrv : public ::fwServices::IService
{

public :

    fwCoreServiceClassDefinitionsMacro ( (IMenuBarSrv)(::fwServices::IService) ) ;

    /// Method called when a menu service is stopping
    FWGUI_API void menuServiceStopping(std::string menuSrvSID);

    /// Method called when a menu service is starting
    FWGUI_API void menuServiceStarting(std::string menuSrvSID);

protected :
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    /**
     * @brief   Constructor. Initialize default values.
     */
    FWGUI_API IMenuBarSrv() ;

    /**
     * @brief   Destructor. Do nothing.
     */
    FWGUI_API virtual ~IMenuBarSrv() ;

    /// Initialize the layout and registry managers.
    FWGUI_API void initialize();

private:

    void initializeLayoutManager( ::fwRuntime::ConfigurationElement::sptr layoutConfig );

    ::fwGui::layoutManager::IMenuBarLayoutManager::sptr m_layoutManager;
    ::fwGui::registrar::MenuBarRegistrar::sptr    m_registrar;

    ConfigurationType m_registrarConfig;
    ConfigurationType m_layoutConfig;

    /// Flag to hide or disable the menu if the service is stopped
    bool m_hideMenus;
};

} // namespace fwGui

#endif /*_FWGUI_IMENUBARSRV_HPP_*/


