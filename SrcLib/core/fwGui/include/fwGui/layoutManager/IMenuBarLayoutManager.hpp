/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_LAYOUTMANAGER_IMENUBARLAYOUTMANAGER_HPP_
#define _FWGUI_LAYOUTMANAGER_IMENUBARLAYOUTMANAGER_HPP_

#include <fwCore/base.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "fwGui/fwMenuBar.hpp"
#include "fwGui/fwMenu.hpp"
#include "fwGui/config.hpp"

namespace fwGui
{
namespace layoutManager
{

/**
 * @brief   Defines the menu bar layout manager for IHM.
 * @class   IMenuBarLayoutManager.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API IMenuBarLayoutManager : public ::fwCore::BaseObject
{
public:
    fwCoreNonInstanciableClassDefinitionsMacro( (IMenuBarLayoutManager)(::fwCore::BaseObject) )

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    typedef std::string RegistryKeyType;

    FWGUI_API const static RegistryKeyType REGISTRAR_KEY;

    /// Constructor. Do nothing.
    FWGUI_API IMenuBarLayoutManager();

    /// Destructor. Do nothing.
    FWGUI_API virtual ~IMenuBarLayoutManager();

    /**
     * @brief Returns the vector of fwMenu managed by this layout.
     */
    FWGUI_API virtual std::vector< ::fwGui::fwMenu::sptr > getMenus();

    /**
     * @brief Configure the menus before creation.
     */
    FWGUI_API virtual void initialize( ConfigurationType configuration);

    /**
     * @brief Instantiate menu with parent menuBar.
     * @pre LayoutManager must be initialized before.
     * @pre parent menuBar must be instanced.
     */
    FWGUI_API virtual void createLayout( ::fwGui::fwMenuBar::sptr parent ) = 0;

    /**
     * @brief Destroy local menus.
     * @pre services using this menus must be stopped before.
     */
    FWGUI_API virtual void destroyLayout() = 0;


    /**
     * @brief Set the menu visibility.
     */
    FWGUI_API virtual void menuIsVisible(::fwGui::fwMenu::sptr, bool isVisible) = 0;

    /**
     * @brief Set the menu enable or not.
     */
    FWGUI_API virtual void menuIsEnabled(::fwGui::fwMenu::sptr, bool isEnabled) = 0;


protected:

    /**
     * @brief Helper to destroy local sub views.
     */
    FWGUI_API virtual void destroyMenus();

    /// All sub containers managed by this layout.
    std::vector< ::fwGui::fwMenu::sptr > m_menus;

    /// Save menu name from configuration.
    std::vector< std::string > m_menuNames;

};

} // namespace layoutManager
} // namespace fwGui

#endif /*_FWGUI_LAYOUTMANAGER_IMENUBARLAYOUTMANAGER_HPP_*/


