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
 * @file ui/__/layout/line.cpp
 * @brief This file defines the implementation of the class for managing a line layout geometry.
 *
 */

#include "ui/__/layout/line.hpp"

#include <core/base.hpp>

namespace sight::ui::layout
{

//-----------------------------------------------------------------------------

const line::registry_key_t line::REGISTRY_KEY = "sight::ui::layout::line";

//-----------------------------------------------------------------------------

void line::initialize(const ui::config_t& _configuration)
{
    const auto orientation_cfg = _configuration.get<std::string>("orientation.<xmlattr>.value", "");

    SIGHT_FATAL_IF("missing orientation configuration", orientation_cfg.empty());
    SIGHT_ASSERT(
        "Wrong value '" + orientation_cfg + "' for 'orientation' attribute (require vertical or horizontal)",
        orientation_cfg == "vertical" || orientation_cfg == "horizontal"
    );
    m_orientation = (orientation_cfg == "vertical") ? vertical : horizontal;

    m_views.clear();
    for(auto view : _configuration)
    {
        if(view.first == "spacer")
        {
            view_info vi;
            vi.m_is_spacer = true;
            m_views.push_back(vi);
        }
        else if(view.first == "view")
        {
            view_info vi;
            if(const auto view_cfg = view.second.get_child_optional("<xmlattr>"); view_cfg.has_value())
            {
                vi.m_proportion = view_cfg->get<int>("proportion", vi.m_proportion);

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

                vi.m_spacing = view_cfg->get<int>("spacing", vi.m_spacing);

                vi.m_min_size.first  = view_cfg->get<int>("minWidth", vi.m_min_size.first);
                vi.m_min_size.second = view_cfg->get<int>("minHeight", vi.m_min_size.second);
                vi.m_max_size.first  = view_cfg->get<int>("maxWidth", vi.m_max_size.first);
                vi.m_max_size.second = view_cfg->get<int>("maxHeight", vi.m_max_size.second);

                vi.m_visible        = view_cfg->get<bool>("visible", vi.m_visible);
                vi.m_use_scroll_bar = view_cfg->get<bool>("useScrollBar", vi.m_use_scroll_bar);
                vi.m_tool_tip       = view_cfg->get<std::string>("toolTip", vi.m_tool_tip);
                vi.m_qss_key        = view_cfg->get<std::string>("QSSClass", "");

                if(auto caption = view_cfg->get_optional<std::string>("caption"); caption.has_value())
                {
                    vi.m_caption.first  = true;
                    vi.m_caption.second = caption.value();
                }

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
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::layout
