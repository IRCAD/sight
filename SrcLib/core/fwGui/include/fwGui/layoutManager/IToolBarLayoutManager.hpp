/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

/**
 * @file fwGui/layoutManager/IToolBarLayoutManager.hpp
 * @brief This file defines the interface of the base class for managing a toolbar.
 */

#ifndef __FWGUI_LAYOUTMANAGER_ITOOLBARLAYOUTMANAGER_HPP__
#define __FWGUI_LAYOUTMANAGER_ITOOLBARLAYOUTMANAGER_HPP__

#include "fwGui/config.hpp"
#include "fwGui/container/fwContainer.hpp"
#include "fwGui/container/fwMenu.hpp"
#include "fwGui/container/fwMenuItem.hpp"
#include "fwGui/container/fwToolBar.hpp"
#include "fwGui/GuiBaseObject.hpp"
#include "fwGui/IMenuItemCallback.hpp"

#include <fwRuntime/ConfigurationElement.hpp>

#include <boost/filesystem/path.hpp>

namespace fwGui
{
namespace layoutManager
{

/**
 * @brief   Defines the toolBar layout manager for IHM.
 */
class FWGUI_CLASS_API IToolBarLayoutManager : public ::fwGui::GuiBaseObject
{
public:
    fwCoreNonInstanciableClassDefinitionsMacro( (IToolBarLayoutManager)(::fwGui::GuiBaseObject) )

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    typedef std::string RegistryKeyType;
    typedef std::vector< ::fwGui::IMenuItemCallback::sptr > CallbacksType;

    class ActionInfo
    {
    public:

        ActionInfo() :
            m_name(""),
            m_shortcut(""),
            m_icon(""),
            m_icon2(""),
            m_isCheckable(false),
            m_isRadio(false),
            m_isSeparator(false),
            m_isSpacer(false),
            m_isMenu(false),
            m_isEditor(false),
            m_size(0)
        {
        }

        std::string m_name;
        std::string m_shortcut;
        ::boost::filesystem::path m_icon;
        ::boost::filesystem::path m_icon2;
        bool m_isCheckable;
        bool m_isRadio;
        bool m_isSeparator;
        bool m_isSpacer;
        bool m_isMenu;
        bool m_isEditor;
        int m_size;
    };

    FWGUI_API const static RegistryKeyType REGISTRY_KEY;

    /// Constructor. Do nothing.
    FWGUI_API IToolBarLayoutManager();

    /// Destructor. Do nothing.
    FWGUI_API virtual ~IToolBarLayoutManager();

    /**
     * @brief Returns the vector of fwMenuItem managed by this layout.
     */
    FWGUI_API virtual std::vector< ::fwGui::container::fwMenuItem::sptr > getMenuItems();

    /**
     * @brief Returns the vector of fwMenu managed by this layout.
     */
    FWGUI_API virtual std::vector< ::fwGui::container::fwMenu::sptr > getMenus();

    /**
     * @brief Returns the vector of fwContainer managed by this layout.
     */
    FWGUI_API virtual std::vector< ::fwGui::container::fwContainer::sptr > getContainers();

    /**
     * @brief Initialize layout managers.
     *
     * Example of configuration
     * @code{.xml}
       <service uid="toolbar2" type="::fwGui::IToolBarSrv" impl="::gui::aspect::SDefaultToolBar" autoConnect="no" >
           <gui>
               <layout>
                   <menuItem name="My item 2" style="check" icon="TutoGui-0.1/icons/system.png"
     * icon2="TutoGui-0.1/icons/monkey.png" />
                   <menuItem name="My item 3" style="check" icon="TutoGui-0.1/icons/system.png"/>
                   <separator />
                   <menuItem name="My item A" style="radio" icon="TutoGui-0.1/icons/monkey.png"/>
                   <menuItem name="My item B" style="radio" icon="TutoGui-0.1/icons/monkey.png"/>
                   <separator />
                   <menu name="My menu" />
                   <separator />
                   <editor />
               </layout>
           </gui>
           <registry>
               <menuItem sid="item2" />
               <menuItem sid="item3" />
               <menuItem sid="item4" />
               <menuItem sid="item5" />
               <menu sid="menu" />
               <editor sid="editor" />
           </registry>
       </service>
       @endcode
     * This method analyzes the gui section of the configuration.
     *
     *  - \<layout\> (mandatory) : give the list of the menu item that will appear in the toolbar.
     *  - \<menuItem name="My item 2" style="radio" icon="TutoGui-0.1/icons/system.png"/\> :
     *   - \b name (mandatory) : give the name of the menu item that will appear in the interface.
     *   - \b style {check|radio} : give the style of the menu item.
     *   - \b icon : give the path of the icon file
     *   - \b icon2 : give the path of the icon file used when the item is checked
     *  - \<menu name="My menu" /\> :
     *   - \b name (mandatory) : give the name of the menu that will appear in the interface.
     *   - \b icon : give the path of the icon file
     *  - \<editor\> : to add an editor in the toolbar
     *  - \<separator/\> : allow to divide the toolbar by part (draw a line).
     */
    FWGUI_API virtual void initialize( ConfigurationType configuration);

    /**
     * @brief Instantiate actions with parent toolBar.
     */
    FWGUI_API virtual void createLayout( ::fwGui::container::fwToolBar::sptr parent ) = 0;

    /**
     * @brief Destroy local actions.
     * @pre services using this actions must be stopped before.
     */
    FWGUI_API virtual void destroyLayout() = 0;

    /**
     * @brief Set the action visibility.
     */
    FWGUI_API virtual void menuItemSetVisible(::fwGui::container::fwMenuItem::sptr, bool isVisible) = 0;

    /**
     * @brief Set the action enable or not.
     */
    FWGUI_API virtual void menuItemSetEnabled(::fwGui::container::fwMenuItem::sptr, bool isEnabled) = 0;

    /**
     * @brief Set the action checked or not.
     */
    FWGUI_API virtual void menuItemSetChecked(::fwGui::container::fwMenuItem::sptr, bool isChecked) = 0;

    /**
     * @brief Sets callbacks associate with toolBar items.
     */
    virtual void setCallbacks(CallbacksType callbacks)
    {
        m_callbacks = callbacks;
    }

protected:

    /**
     * @brief Helper to destroy local actions.
     */
    FWGUI_API virtual void destroyActions();

    /// All actions managed by this layout.
    std::vector< ::fwGui::container::fwMenuItem::sptr > m_menuItems;

    /// All menus managed by this layout.
    std::vector< ::fwGui::container::fwMenu::sptr > m_menus;

    /// All editors managed by this layout.
    std::vector< ::fwGui::container::fwContainer::sptr > m_containers;

    /// Save action informations from configuration.
    std::vector< ActionInfo > m_actionInfo;

    /// Callbacks associate with toolBar items
    CallbacksType m_callbacks;
};

} // namespace layoutManager
} // namespace fwGui

#endif /*__FWGUI_LAYOUTMANAGER_ITOOLBARLAYOUTMANAGER_HPP__*/

