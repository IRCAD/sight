/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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
 * @file ui/__/builder/menubar.cpp
 * @brief This file defines the implementation of the interface class  for the menubar builder.
 */

#include "ui/__/builder/menubar.hpp"

namespace sight::ui::builder
{

const menubar::registry_key_t menubar::REGISTRY_KEY = "::ui::menubar";

//-----------------------------------------------------------------------------

void menubar::initialize(const ui::config_t& _configuration)
{
    if(const auto hexa_color = _configuration.get<std::string>("<xmlattr>.backgroundColor", ""); !hexa_color.empty())
    {
        SIGHT_ASSERT(
            "Color string should start with '#' and followed by 6 or 8 "
            "hexadecimal digits. Given color: " << hexa_color,
            hexa_color[0] == '#'
            && (hexa_color.length() == 7 || hexa_color.length() == 9)
        );
        m_backgroundColor = hexa_color;
    }
}

//-----------------------------------------------------------------------------

ui::container::menubar::sptr menubar::getMenuBar()
{
    return this->m_menuBar;
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::builder
