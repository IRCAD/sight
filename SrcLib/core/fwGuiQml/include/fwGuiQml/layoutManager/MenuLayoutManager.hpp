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

#include "fwGuiQml/config.hpp"
#include "fwGuiQml/container/QmlMenuContainer.hpp"

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
class FWGUIQML_CLASS_API MenuLayoutManager : public ::fwGui::layoutManager::IMenuLayoutManager
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (MenuLayoutManager)(::fwGui::layoutManager::IMenuLayoutManager),
                                            (()),
                                            ::fwGui::factory::New< MenuLayoutManager > );

    FWGUIQML_API MenuLayoutManager(::fwGui::GuiBaseObject::Key key);

    FWGUIQML_API virtual ~MenuLayoutManager();

    /**
     * @brief Instantiate actions with parent menu.
     * @pre LayoutManager must be initialized before.
     * @pre parent menu must be instanced.
     */
    FWGUIQML_API virtual void createLayout( ::fwGui::container::fwMenu::sptr parent ) override;

    /**
     * @brief Destroy local menus.
     * @pre services using this actions must be stopped before.
     */
    FWGUIQML_API virtual void destroyLayout() override;

    /**
     * @brief Set the action visibility.
     */
    FWGUIQML_API virtual void menuItemSetVisible(::fwGui::container::fwMenuItem::sptr menuItem,
                                                 bool isVisible) override;

    /**
     * @brief Set the action enable or not.
     */
    FWGUIQML_API virtual void menuItemSetEnabled(::fwGui::container::fwMenuItem::sptr menuItem,
                                                 bool isEnabled) override;

    /**
     * @brief Set the action checked or not.
     */
    FWGUIQML_API virtual void menuItemSetChecked(::fwGui::container::fwMenuItem::sptr, bool isChecked) override;

protected:
    ::fwGuiQml::container::QmlMenuContainer::sptr m_parent;

};

} // namespace layoutManager
} // namespace fwGui
