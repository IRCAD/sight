/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

/**
 * @file fwGui/layoutManager/IMenuBarLayoutManager.hpp
 * @brief This file defines the interface of the base class for managing a menubar.
 *
 *
 * @date 2009-2010
 */

#ifndef __FWGUI_LAYOUTMANAGER_IMENUBARLAYOUTMANAGER_HPP__
#define __FWGUI_LAYOUTMANAGER_IMENUBARLAYOUTMANAGER_HPP__

#include <fwRuntime/ConfigurationElement.hpp>

#include "fwGui/GuiBaseObject.hpp"
#include "fwGui/container/fwMenuBar.hpp"
#include "fwGui/container/fwMenu.hpp"
#include "fwGui/config.hpp"

namespace fwGui
{
namespace layoutManager
{

/**
 * @brief   Defines the menu bar layout manager for IHM.
 * @class   IMenuBarLayoutManager
 *
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API IMenuBarLayoutManager : public ::fwGui::GuiBaseObject
{
public:
    fwCoreNonInstanciableClassDefinitionsMacro( (IMenuBarLayoutManager)(::fwGui::GuiBaseObject) )

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    typedef std::string RegistryKeyType;

    FWGUI_API const static RegistryKeyType REGISTRY_KEY;

    /// Constructor. Do nothing.
    FWGUI_API IMenuBarLayoutManager();

    /// Destructor. Do nothing.
    FWGUI_API virtual ~IMenuBarLayoutManager();

    /**
     * @brief Returns the vector of fwMenu managed by this layout.
     */
    FWGUI_API virtual std::vector< ::fwGui::container::fwMenu::sptr > getMenus();

    /**
     * @brief Initialize layout managers.
     *
     * Example of configuration
     * @code{.xml}
       <service uid="menuBar" type="::fwGui::IMenuBarSrv" impl="::gui::aspect::SDefaultMenuBar" autoConnect="no" >
          <gui>
              <layout>
                  <menu name="My Menu"/>
                  <menu name="My Menu 2"/>
              </layout>
          </gui>
          <registry>
              <menu sid="myMenu" start="yes" />
              <menu sid="myMenu2" start="yes" />
          </registry>
       </service>
       @endcode
     * This method analyzes the gui section of the configuration.
     *  - \<layout\> (mandatory) : give the list of the menu that will appear in the menu bar.
     *  - \<menu name="My Menu"/\> :
     *   - \b name (mandatory) : give the name of the menu that will appear in the interface.
     */

    FWGUI_API virtual void initialize( ConfigurationType configuration);

    /**
     * @brief Instantiate menu with parent menuBar.
     * @pre LayoutManager must be initialized before.
     * @pre parent menuBar must be instanced.
     */
    FWGUI_API virtual void createLayout( ::fwGui::container::fwMenuBar::sptr parent ) = 0;

    /**
     * @brief Destroy local menus.
     * @pre services using this menus must be stopped before.
     */
    FWGUI_API virtual void destroyLayout() = 0;


    /**
     * @brief Set the menu visibility.
     */
    FWGUI_API virtual void menuIsVisible(::fwGui::container::fwMenu::sptr, bool isVisible) = 0;

    /**
     * @brief Set the menu enable or not.
     */
    FWGUI_API virtual void menuIsEnabled(::fwGui::container::fwMenu::sptr, bool isEnabled) = 0;


protected:

    /**
     * @brief Helper to destroy local sub views.
     */
    FWGUI_API virtual void destroyMenus();

    /// All sub containers managed by this layout.
    std::vector< ::fwGui::container::fwMenu::sptr > m_menus;

    /// Save menu name from configuration.
    std::vector< std::string > m_menuNames;

};

} // namespace layoutManager
} // namespace fwGui

#endif /*__FWGUI_LAYOUTMANAGER_IMENUBARLAYOUTMANAGER_HPP__*/


