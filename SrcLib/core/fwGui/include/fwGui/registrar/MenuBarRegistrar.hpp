/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_REGISTRAR_MENUBARREGISTRYMANAGER_HPP_
#define _FWGUI_REGISTRAR_MENUBARREGISTRYMANAGER_HPP_

#include <fwCore/base.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "fwGui/fwMenuBar.hpp"
#include "fwGui/fwMenu.hpp"
#include "fwGui/config.hpp"


namespace fwGui
{

namespace registrar
{

/**
 * @brief   Defines the menuBar registrar for IHM.
 * @class   MenuBarRegistrar.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API MenuBarRegistrar : public ::fwCore::BaseObject
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (MenuBarRegistrar)(::fwCore::BaseObject), (( (const std::string) )), new MenuBarRegistrar );

    /// Constructor.
    FWGUI_API MenuBarRegistrar( const std::string sid);

    /// Destructor. Do nothing
    FWGUI_API virtual ~MenuBarRegistrar();

    /// Return the parent container
    FWGUI_API virtual ::fwGui::fwMenuBar::sptr getParent();

    /**
     * @brief Return the fwMenu associated with the menuSid.
     * @param menuSid sid of the menu service
     * @param menus  vector containing the fwMenu manages by this registrar.
     */
    FWGUI_API virtual ::fwGui::fwMenu::sptr getFwMenu(std::string menuSid, std::vector< ::fwGui::fwMenu::sptr > menus);

    /**
     * @brief Configure views managed.
     */
    FWGUI_API virtual void initialize( ::fwRuntime::ConfigurationElement::sptr configuration);

    /**
     * @brief Starting menu bar registrar.
     * All services managed in local menus
     * and with start="yes" in configuration will be started.
     * @pre MenuBarRegistrar must be initialized before.
     * @pre sub menus must be instanced before.
     */
    FWGUI_API virtual void manage(std::vector< ::fwGui::fwMenu::sptr > menus );

    /**
     * @brief Stopping view manager.
     * All services managed in local menus will be stopped.
     */
    FWGUI_API virtual void unmanage();

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    typedef std::map< std::string, std::pair<int, bool> > SIDMenuMapType;

    /**
     * @brief All menu services ID managed and associated with pair containing:
     * menu's index vector and boolean describing if is started by the manager.
     */
    SIDMenuMapType m_menuSids;

    /// Main service ID associate with this MenuBarRegistrar
    std::string m_sid;
};

} // namespace registrar
} // namespace fwGui

#endif /*_FWGUI_REGISTRAR_MENUBARREGISTRYMANAGER_HPP_*/


