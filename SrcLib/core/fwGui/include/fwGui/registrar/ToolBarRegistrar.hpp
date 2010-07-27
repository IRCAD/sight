/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_REGISTRAR_TOOLBARREGISTRYMANAGER_HPP_
#define _FWGUI_REGISTRAR_TOOLBARREGISTRYMANAGER_HPP_

#include <fwCore/base.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "fwGui/fwToolBar.hpp"
#include "fwGui/fwMenuItem.hpp"
#include "fwGui/ActionCallbackBase.hpp"
#include "fwGui/config.hpp"


namespace fwGui
{

namespace registrar
{

/**
 * @brief   Defines the toolBar registrar for IHM.
 * @class   ToolBarRegistrar.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API ToolBarRegistrar : public ::fwCore::BaseObject
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (ToolBarRegistrar)(::fwCore::BaseObject), (( (const std::string) )), new ToolBarRegistrar );
    typedef std::vector< ::fwGui::IMenuItemCallback::sptr > CallbacksType;

    /// Constructor.
    FWGUI_API ToolBarRegistrar( const std::string sid);

    /// Destructor. Do nothing
    FWGUI_API virtual ~ToolBarRegistrar();

    /// Return the parent container
    FWGUI_API virtual ::fwGui::fwToolBar::sptr getParent();

    /**
     * @brief Return the fwMenuItem associated with the toolBarSid.
     * @param actionSid sid of the action service
     * @param toolBarItems  vector containing the fwMenuItem manages by this registrar.
     */
    FWGUI_API virtual ::fwGui::fwMenuItem::sptr getFwMenuItem(std::string toolBarSid, std::vector< ::fwGui::fwMenuItem::sptr > menuItems);

    /**
     * @brief Configure views managed.
     */
    FWGUI_API virtual void initialize( ::fwRuntime::ConfigurationElement::sptr configuration);

    /**
     * @brief Starting toolBar registrar.
     * All services managed in local toolBar items
     * and with start="yes" in configuration will be started.
     * @pre ToolBarRegistrar must be initialized before.
     * @pre sub toolBar items must be instanced before.
     */
    FWGUI_API virtual void manage(std::vector< ::fwGui::fwMenuItem::sptr > toolBarItems );

    /**
     * @brief Stopping toolBar items manager.
     * All services managed in local toolBar items will be stopped.
     */
    FWGUI_API virtual void unmanage();

    /**
     * @brief This method is called when an action is clicked.
     */
    FWGUI_API virtual void onItemAction();

    /**
     * @brief Returns callbacks associate with toolBar items.
     */
    FWGUI_API virtual CallbacksType getCallbacks(){ return m_callbacks;};

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    typedef std::map< std::string, std::pair<int, bool> > SIDToolBarMapType;

    /**
     * @brief All toolBar services ID managed and associated with pair containing:
     * action's index vector and boolean describing if is started by the manager.
     */
    SIDToolBarMapType m_actionSids;

    /// Main service ID associate with this ToolBarRegistrar
    std::string m_sid;

    /// Callback associate with toolBar items
    CallbacksType m_callbacks;
};

} // namespace registrar
} // namespace fwGui

#endif /*_FWGUI_REGISTRAR_TOOLBARREGISTRYMANAGER_HPP_*/


