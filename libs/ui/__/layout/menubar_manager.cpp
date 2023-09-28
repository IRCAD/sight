/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

/**
 * @file ui/__/layout/menubar_manager.cpp
 * @brief This file defines the implementation of the base class for managing a menubar.
 *
 */

#include "ui/__/layout/menubar_manager.hpp"

#include <boost/range/iterator_range_core.hpp>

namespace sight::ui::layout
{

const menubar_manager::RegistryKeyType menubar_manager::REGISTRY_KEY = "sight::ui::layout::menubar";

//-----------------------------------------------------------------------------

void menubar_manager::initialize(const ui::config_t& configuration)
{
    m_menus.clear();
    for(const auto& menu : boost::make_iterator_range(configuration.equal_range("menu")))
    {
        const auto name = menu.second.get<std::string>("<xmlattr>.name");
        m_menuNames.push_back(name);
    }
}

//-----------------------------------------------------------------------------

void menubar_manager::destroyMenus()
{
    for(const ui::container::menu::sptr& menu : m_menus)
    {
        menu->destroyContainer();
    }

    m_menus.clear();
}

//-----------------------------------------------------------------------------

std::vector<ui::container::menu::sptr> menubar_manager::getMenus()
{
    return this->m_menus;
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::layout
