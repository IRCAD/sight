/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include "ui/__/detail/parser.hpp"

namespace sight::ui::builder
{

const menubar::registry_key_t menubar::REGISTRY_KEY = "::ui::menubar";

//-----------------------------------------------------------------------------

void menubar::initialize(const ui::config_t& _configuration)
{
    m_background_color = detail::parse_background_color(_configuration);

    if(const auto qss_class = _configuration.get<std::string>("<xmlattr>.QSSClass", ""); !qss_class.empty())
    {
        m_qss_class = qss_class;
    }
}

//-----------------------------------------------------------------------------

ui::container::menubar::sptr menubar::get_menu_bar()
{
    return this->m_menu_bar;
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::builder
