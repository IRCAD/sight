/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_GUIREGISTRY_HPP_
#define _FWGUI_GUIREGISTRY_HPP_

#include <fwCore/base.hpp>

#include "fwGui/config.hpp"
#include "fwGui/fwContainer.hpp"
#include "fwGui/fwMenuBar.hpp"
#include "fwGui/fwMenu.hpp"
#include "fwGui/fwMenuItem.hpp"

/**
 * @brief   The namespace fwGui contains the base interface for IHM services.
 * @namespace   fwGui
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
namespace fwGui
{

/**
 * @brief   This class contains helpers to manage all fwContainer in the application.
 * @class   GuiRegistry.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API GuiRegistry : public ::fwCore::BaseObject
{

public :

    typedef std::map< std::string , ::fwGui::fwContainer::sptr > ContainerMapType;
    typedef std::map< std::string , ::fwGui::fwMenuBar::sptr >   MenuBarMapType;
    typedef std::map< std::string , ::fwGui::fwMenu::sptr >      MenuMapType;
    typedef std::map< std::string , std::string >                ActionToMenuMapType;

    fwCoreNonInstanciableClassDefinitionsMacro( (GuiRegistry)(::fwCore::BaseObject) )

    /**
     * @brief   Constructor.
     */
    FWGUI_API GuiRegistry() ;

    /**
     * @brief   Destructor. Do nothing.
     */
    FWGUI_API virtual ~GuiRegistry() ;

    //-----------------------------------------------------------------------------

    /**
     * @name    Helper SID-container methods
     */
    ///@{

    /**
     * @brief Registers container associate with service ID.
     */
    FWGUI_API static void registerSIDContainer(std::string sid, ::fwGui::fwContainer::sptr container);

    /**
     * @brief Unregisters container associate with service ID.
     */
    FWGUI_API static void unregisterSIDContainer(std::string sid);

    /**
     * @brief Returns fwContainer associate with service ID.
     */
    FWGUI_API static ::fwGui::fwContainer::sptr getSIDContainer(std::string sid);

    ///@}

    //-----------------------------------------------------------------------------

    /**
     * @name    Helper WID-container methods
     */
    ///@{

    /**
     * @brief Registers container associate with window ID.
     */
    FWGUI_API static void registerWIDContainer(std::string wid, ::fwGui::fwContainer::sptr container);

    /**
     * @brief Unregisters container associate with window ID.
     */
    FWGUI_API static void unregisterWIDContainer(std::string wid);

    /**
     * @brief Returns fwContainer associate with window ID.
     */
    FWGUI_API static ::fwGui::fwContainer::sptr getWIDContainer(std::string wid);

    ///@}

    //-----------------------------------------------------------------------------

    /**
     * @name    Helper SID-MenuBar methods
     */
    ///@{

    /**
     * @brief Registers fwMenuBar associate with service ID.
     */
    FWGUI_API static void registerSIDMenuBar(std::string sid, ::fwGui::fwMenuBar::sptr menuBar);

    /**
     * @brief Unregisters fwMenuBar associate with service ID.
     */
    FWGUI_API static void unregisterSIDMenuBar(std::string sid);
    /**
     * @brief Returns fwMenuBar associate with service ID.
     */
    FWGUI_API static ::fwGui::fwMenuBar::sptr getSIDMenuBar(std::string sid);
    ///@}

    //-----------------------------------------------------------------------------

    /**
     * @name    Helper SID-Menu methods
     */
    ///@{

    /**
     * @brief Registers fwMenu associate with service ID.
     */
    FWGUI_API static void registerSIDMenu(std::string sid, ::fwGui::fwMenu::sptr menu);

    /**
     * @brief Unregisters fwMenu associate with service ID.
     */
    FWGUI_API static void unregisterSIDMenu(std::string sid);
    /**
     * @brief Returns fwMenu associate with service ID.
     */
    FWGUI_API static ::fwGui::fwMenu::sptr getSIDMenu(std::string sid);
    ///@}

    //-----------------------------------------------------------------------------

    /**
     * @name    Helper Action-Menu methods
     */
    ///@{

    /**
     * @brief Registers action sid associted with a parent sid.
     */
    FWGUI_API static void registerActionSIDToParentSID(std::string actionSid, std::string parentSid);

    /**
     * @brief Unregisters action sid associted with a parent sid.
     */
    FWGUI_API static void unregisterActionSIDToParentSID(std::string actionSid);

    /**
     * @brief Method called when the action service is stopping.
     * Call parent service actionServiceStopping() method
     */
    FWGUI_API static void actionServiceStopping(std::string actionSid);

    /**
     * @brief Method called when the action service is starting.
     * Call parent service actionServiceStarting() method
     */
    FWGUI_API static void actionServiceStarting(std::string actionSid);
    ///@}

protected :

    static ContainerMapType     m_globalSIDToFwContainer;
    static ContainerMapType     m_globalWIDToFwContainer;
    static MenuBarMapType       m_globalSIDToFwMenuBar;
    static MenuMapType          m_globalSIDToFwMenu;
    static ActionToMenuMapType  m_actionSIDToParentSID;

};

} // namespace fwGui

#endif /*_FWGUI_GUIREGISTRY_HPP_*/


