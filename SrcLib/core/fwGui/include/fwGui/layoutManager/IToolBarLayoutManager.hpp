/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_LAYOUTMANAGER_ITOOLBARLAYOUTMANAGER_HPP_
#define _FWGUI_LAYOUTMANAGER_ITOOLBARLAYOUTMANAGER_HPP_

#include <fwCore/base.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "fwGui/fwToolBar.hpp"
#include "fwGui/fwMenuItem.hpp"
#include "fwGui/IMenuItemCallback.hpp"
#include "fwGui/config.hpp"

namespace fwGui
{
namespace layoutManager
{

/**
 * @brief   Defines the toolBar layout manager for IHM.
 * @class   IToolBarLayoutManager.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API IToolBarLayoutManager : public ::fwCore::BaseObject
{
public:
    fwCoreNonInstanciableClassDefinitionsMacro( (IToolBarLayoutManager)(::fwCore::BaseObject) )

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    typedef std::string RegistryKeyType;
    typedef std::vector< ::fwGui::IMenuItemCallback::sptr > CallbacksType;

    class ActionInfo
         {
         public :

            ActionInfo() :
                m_name(""),
                m_icon(""),
                m_isSeparator(false),
                m_isCheckable (false),
                m_isRadio(false)
            {}

            std::string m_name;
            std::string m_icon;
            bool        m_isCheckable;
            bool        m_isRadio;
            bool        m_isSeparator;
         };

    FWGUI_API const static RegistryKeyType REGISTRY_KEY;

    /// Constructor. Do nothing.
    FWGUI_API IToolBarLayoutManager();

    /// Destructor. Do nothing.
    FWGUI_API virtual ~IToolBarLayoutManager();

    /**
     * @brief Returns the vector of fwMenuItem managed by this layout.
     */
    FWGUI_API virtual std::vector< ::fwGui::fwMenuItem::sptr > getMenuItems();

    /**
     * @brief Initialize layout managers.
     *
     * Example of configuration
     * @verbatim
       <service uid="toolbar2" type="::fwGui::IToolBarSrv" implementation="::gui::aspect::DefaultToolBarSrv" autoComChannel="no" >
           <gui>
               <layout>
                   <menuItem name="My item 2" style="radio" icon="Bundles/TutoGui_0-1/icons/system.png"/>
                   <menuItem name="My item 3" style="radio" icon="Bundles/TutoGui_0-1/icons/system.png"/>
                   <separator />
                   <menuItem name="My item A" style="radio" icon="Bundles/TutoGui_0-1/icons/monkey.png"/>
                   <menuItem name="My item B" style="radio" icon="Bundles/TutoGui_0-1/icons/monkey.png"/>
               </layout>
           </gui>
           <registry>
               <menuItem sid="item2" />
               <menuItem sid="item3" />
               <menuItem sid="item4" />
               <menuItem sid="item5" />
           </registry>
       </service>
       @endverbatim
     * This method analyzes the gui section of the configuration.
     *
     *  - <layout> (mandatory) : give the list of the menu item that will appear in the toolbar.
     *  - <menuItem name="My item 2" style="radio" icon="Bundles/TutoGui_0-1/icons/system.png"/> :
     *   - \b name (mandatory) : give the name of the menu item that will appear in the interface.
     *   - \b style {check|radio} : give the style of the menu item.
     *   - \b icon : give the path of the icon file
     *  - <separator/> : allow to divide the toolbar by part (draw a line).
     */
    FWGUI_API virtual void initialize( ConfigurationType configuration);

    /**
     * @brief Instantiate actions with parent toolBar.
     */
    FWGUI_API virtual void createLayout( ::fwGui::fwToolBar::sptr parent ) = 0;

    /**
     * @brief Destroy local actions.
     * @pre services using this actions must be stopped before.
     */
    FWGUI_API virtual void destroyLayout() = 0;


    /**
     * @brief Set the action visibility.
     */
    FWGUI_API virtual void menuItemSetVisible(::fwGui::fwMenuItem::sptr, bool isVisible) = 0;

    /**
     * @brief Set the action enable or not.
     */
    FWGUI_API virtual void menuItemSetEnabled(::fwGui::fwMenuItem::sptr, bool isEnabled) = 0;

    /**
     * @brief Set the action checked or not.
     */
    FWGUI_API virtual void menuItemSetChecked(::fwGui::fwMenuItem::sptr, bool isChecked) = 0;

    /**
     * @brief Sets callbacks associate with toolBar items.
     */
    FWGUI_API virtual void setCallbacks(CallbacksType callbacks) {m_callbacks = callbacks;};

protected:

    /**
     * @brief Helper to destroy local actions.
     */
    FWGUI_API virtual void destroyActions();

    /// All actions managed by this layout.
    std::vector< ::fwGui::fwMenuItem::sptr > m_menuItems;

    /// Save action informations from configuration.
    std::vector< ActionInfo > m_actionInfo;

    /// Callbacks associate with toolBar items
    CallbacksType m_callbacks;
};

} // namespace layoutManager
} // namespace fwGui

#endif /*_FWGUI_LAYOUTMANAGER_ITOOLBARLAYOUTMANAGER_HPP_*/


