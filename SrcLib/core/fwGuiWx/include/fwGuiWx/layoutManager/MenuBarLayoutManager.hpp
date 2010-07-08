/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIWX_LAYOUTMANAGER_MENUBARLAYOUTMANAGER_HPP_
#define _FWGUIWX_LAYOUTMANAGER_MENUBARLAYOUTMANAGER_HPP_

#include <fwCore/base.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include <fwGui/fwMenuBar.hpp>
#include <fwGui/fwMenu.hpp>
#include <fwGui/layoutManager/IMenuBarLayoutManager.hpp>

#include "fwGuiWx/container/WxMenuBarContainer.hpp"
#include "fwGuiWx/config.hpp"

namespace fwGui
{
namespace layoutManager
{

/**
 * @brief   Defines the menu bar layout manager for IHM.
 * @class   MenuBarLayoutManager.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUIWX_CLASS_API MenuBarLayoutManager : public ::fwGui::layoutManager::IMenuBarLayoutManager
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (MenuBarLayoutManager)(::fwGui::layoutManager::IMenuBarLayoutManager) ,(()), new MenuBarLayoutManager);

    /// Constructor. Do nothing.
    FWGUIWX_API MenuBarLayoutManager();

    /// Destructor. Do nothing.
    FWGUIWX_API virtual ~MenuBarLayoutManager();

    /**
     * @brief Instantiate menu with parent menuBar.
     * @pre LayoutManager must be initialized before.
     * @pre parent menuBar must be instanced.
     */
    FWGUIWX_API virtual void createLayout( ::fwGui::fwMenuBar::sptr parent );

    /**
     * @brief Destroy local menus.
     * @pre services using this menus must be stopped before.
     */
    FWGUIWX_API virtual void destroyLayout();


    /**
     * @brief Set the menu visibility.
     */
    FWGUIWX_API virtual void menuIsVisible(::fwGui::fwMenu::sptr fwMenu, bool isVisible);

    /**
     * @brief Set the menu enable or not.
     */
    FWGUIWX_API virtual void menuIsEnabled(::fwGui::fwMenu::sptr fwMenu, bool isEnabled);

protected:
    ::fwGuiWx::container::WxMenuBarContainer::sptr m_parent;
};

} // namespace layoutManager
} // namespace fwGui

#endif /*_FWGUIWX_LAYOUTMANAGER_MENUBARLAYOUTMANAGER_HPP_*/


