/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#ifndef __FWGUIQT_LAYOUTMANAGER_MENUBARLAYOUTMANAGER_HPP__
#define __FWGUIQT_LAYOUTMANAGER_MENUBARLAYOUTMANAGER_HPP__

#include "fwGuiQt/config.hpp"
#include "fwGuiQt/container/QtMenuBarContainer.hpp"

#include <fwCore/base.hpp>

#include <fwGui/container/fwMenu.hpp>
#include <fwGui/container/fwMenuBar.hpp>
#include <fwGui/layoutManager/IMenuBarLayoutManager.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

namespace fwGui
{
namespace layoutManager
{

/**
 * @brief   Defines the menu bar layout manager for IHM.
 */
class FWGUIQT_CLASS_API MenuBarLayoutManager : public ::fwGui::layoutManager::IMenuBarLayoutManager
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (MenuBarLayoutManager)(::fwGui::layoutManager::IMenuBarLayoutManager),
                                            (()),
                                            ::fwGui::factory::New< MenuBarLayoutManager > );

    FWGUIQT_API MenuBarLayoutManager(::fwGui::GuiBaseObject::Key key);

    FWGUIQT_API virtual ~MenuBarLayoutManager();

    /**
     * @brief Instantiate menu with parent menuBar.
     * @pre LayoutManager must be initialized before.
     * @pre parent menuBar must be instanced.
     */
    FWGUIQT_API virtual void createLayout( ::fwGui::container::fwMenuBar::sptr parent ) override;

    /**
     * @brief Destroy local menus.
     * @pre services using this menus must be stopped before.
     */
    FWGUIQT_API virtual void destroyLayout() override;

    /**
     * @brief Set the menu visibility.
     */
    FWGUIQT_API virtual void menuIsVisible(::fwGui::container::fwMenu::sptr fwMenu, bool isVisible) override;

    /**
     * @brief Set the menu enable or not.
     */
    FWGUIQT_API virtual void menuIsEnabled(::fwGui::container::fwMenu::sptr fwMenu, bool isEnabled) override;

protected:
    ::fwGuiQt::container::QtMenuBarContainer::sptr m_parent;
};

} // namespace layoutManager
} // namespace fwGui

#endif /*__FWGUIQT_LAYOUTMANAGER_MENUBARLAYOUTMANAGER_HPP__*/

