/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#pragma once

#include "fwGuiQt/config.hpp"
#include "fwGuiQt/container/QtMenuContainer.hpp"

#include <fwCore/base.hpp>

#include <fwGui/container/fwMenu.hpp>
#include <fwGui/layoutManager/IMenuLayoutManager.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

namespace fwGui
{
namespace layoutManager
{

/**
 * @brief   Defines the menu layout manager for IHM.
 */
class FWGUIQT_CLASS_API MenuLayoutManager : public ::fwGui::layoutManager::IMenuLayoutManager
{
public:
    fwCoreClassMacro(MenuLayoutManager, ::fwGui::layoutManager::IMenuLayoutManager,
                     ::fwGui::factory::New< MenuLayoutManager >);

    FWGUIQT_API MenuLayoutManager(::fwGui::GuiBaseObject::Key key);

    FWGUIQT_API virtual ~MenuLayoutManager();

    /**
     * @brief Instantiate actions with parent menu.
     * @pre LayoutManager must be initialized before.
     * @pre parent menu must be instanced.
     */
    FWGUIQT_API virtual void createLayout( ::fwGui::container::fwMenu::sptr parent ) override;

    /**
     * @brief Destroy local menus.
     * @pre services using this actions must be stopped before.
     */
    FWGUIQT_API virtual void destroyLayout() override;

    /**
     * @brief Set the action visibility.
     */
    FWGUIQT_API virtual void menuItemSetVisible(::fwGui::container::fwMenuItem::sptr menuItem, bool isVisible) override;

    /**
     * @brief Set the action enable or not.
     */
    FWGUIQT_API virtual void menuItemSetEnabled(::fwGui::container::fwMenuItem::sptr menuItem, bool isEnabled) override;

    /**
     * @brief Set the action checked or not.
     */
    FWGUIQT_API virtual void menuItemSetChecked(::fwGui::container::fwMenuItem::sptr, bool isChecked) override;

protected:
    ::fwGuiQt::container::QtMenuContainer::sptr m_parent;

};

} // namespace layoutManager
} // namespace fwGui
