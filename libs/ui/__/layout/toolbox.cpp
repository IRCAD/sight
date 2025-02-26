/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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
 * @file ui/__/layout/toolbox.cpp
 * @brief This file defines the implementation of the class for managing a line layout geometry.
 *
 */

#include "ui/__/layout/toolbox.hpp"

#include <boost/range/iterator_range_core.hpp>

namespace sight::ui::layout
{

//-----------------------------------------------------------------------------

const toolbox::registry_key_t toolbox::REGISTRY_KEY = "sight::ui::layout::toolbox";

//-----------------------------------------------------------------------------

toolbox::toolbox()
= default;

//-----------------------------------------------------------------------------

toolbox::~toolbox()
= default;

//-----------------------------------------------------------------------------

void toolbox::initialize(const ui::config_t& _configuration)
{
    m_icon_color = _configuration.get<std::string>("icon_color.<xmlattr>.value", "#000000");
    m_views.clear();
    for(const auto& view : boost::make_iterator_range(_configuration.equal_range("view")))
    {
        view_info vi;
        if(const auto view_cfg = view.second.get_child_optional("<xmlattr>"); view_cfg.has_value())
        {
            if(const auto border = view_cfg->get_optional<int>("border"); border.has_value())
            {
                vi.m_border = border.value();
            }
            else
            {
                vi.m_left_border   = view_cfg->get<int>("leftBorder", vi.m_left_border);
                vi.m_top_border    = view_cfg->get<int>("topBorder", vi.m_top_border);
                vi.m_right_border  = view_cfg->get<int>("rightBorder", vi.m_right_border);
                vi.m_bottom_border = view_cfg->get<int>("bottomBorder", vi.m_bottom_border);
            }

            vi.m_min_size.first  = view_cfg->get<int>("minWidth", vi.m_min_size.first);
            vi.m_min_size.second = view_cfg->get<int>("minHeight", vi.m_min_size.second);
            vi.m_max_size.first  = view_cfg->get<int>("maxWidth", vi.m_max_size.first);
            vi.m_max_size.second = view_cfg->get<int>("maxHeight", vi.m_max_size.second);

            vi.m_visible        = view_cfg->get<bool>("visible", vi.m_visible);
            vi.m_use_scroll_bar = view_cfg->get<bool>("useScrollBar", vi.m_use_scroll_bar);
            vi.m_expanded       = view_cfg->get<bool>("expanded", vi.m_expanded);

            vi.m_caption = view_cfg->get<std::string>("caption", "");
            vi.m_qss_key = view_cfg->get<std::string>("QSSClass", "");

            if(const auto hexa_color = view_cfg->get<std::string>("backgroundColor", ""); !hexa_color.empty())
            {
                SIGHT_ASSERT(
                    "Color string should start with '#' and followed by 6 or 8 "
                    "hexadecimal digits. Given color: " << hexa_color,
                    hexa_color[0] == '#'
                    && (hexa_color.length() == 7 || hexa_color.length() == 9)
                );
                vi.m_background_color = hexa_color;
            }
        }

        m_views.push_back(vi);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::layout
