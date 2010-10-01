/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIQT_LAYOUTMANAGER_MENUBARLAYOUTMANAGER_HPP_
#define _FWGUIQT_LAYOUTMANAGER_MENUBARLAYOUTMANAGER_HPP_

#include <fwCore/base.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include <fwGui/container/fwMenuBar.hpp>
#include <fwGui/container/fwMenu.hpp>
#include <fwGui/layoutManager/IMenuBarLayoutManager.hpp>

#include "fwGuiQt/container/QtMenuBarContainer.hpp"
#include "fwGuiQt/config.hpp"

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
class FWGUIQT_CLASS_API MenuBarLayoutManager : public ::fwGui::layoutManager::IMenuBarLayoutManager
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (MenuBarLayoutManager)(::fwGui::layoutManager::IMenuBarLayoutManager) ,(()), new MenuBarLayoutManager);

    /// Constructor. Do nothing.
    FWGUIQT_API MenuBarLayoutManager();

    /// Destructor. Do nothing.
    FWGUIQT_API virtual ~MenuBarLayoutManager();

    /**
     * @brief Instantiate menu with parent menuBar.
     * @pre LayoutManager must be initialized before.
     * @pre parent menuBar must be instanced.
     */
    FWGUIQT_API virtual void createLayout( ::fwGui::container::fwMenuBar::sptr parent );

    /**
     * @brief Destroy local menus.
     * @pre services using this menus must be stopped before.
     */
    FWGUIQT_API virtual void destroyLayout();


    /**
     * @brief Set the menu visibility.
     */
    FWGUIQT_API virtual void menuIsVisible(::fwGui::container::fwMenu::sptr fwMenu, bool isVisible);

    /**
     * @brief Set the menu enable or not.
     */
    FWGUIQT_API virtual void menuIsEnabled(::fwGui::container::fwMenu::sptr fwMenu, bool isEnabled);

protected:
    ::fwGuiQt::container::QtMenuBarContainer::sptr m_parent;
};

} // namespace layoutManager
} // namespace fwGui

#endif /*_FWGUIQT_LAYOUTMANAGER_MENUBARLAYOUTMANAGER_HPP_*/


