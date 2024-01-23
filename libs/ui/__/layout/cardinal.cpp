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
 * @brief This file defines the implementation of the class for managing a cardinal layout geometry.
 */

#include "ui/__/layout/cardinal.hpp"

#include <core/base.hpp>

#include <boost/lexical_cast.hpp>

namespace sight::ui::layout
{

//-----------------------------------------------------------------------------

const cardinal::registry_key_t cardinal::REGISTRY_KEY = "sight::ui::layout::cardinal";
//-----------------------------------------------------------------------------

const std::map<std::string, cardinal::align> cardinal::STRING_TO_ALIGN = {
    {"center", center},
    {"right", right},
    {"left", left},
    {"bottom", bottom},
    {"top", top},
};

//-----------------------------------------------------------------------------

void cardinal::initialize(const ui::config_t& _configuration)
{
    m_views.clear();

    const auto views_cfg = _configuration.equal_range("view");
    for(const auto& view : boost::make_iterator_range(views_cfg))
    {
        view_info vi;
        if(const auto view_cfg = view.second.get_child_optional("<xmlattr>"); view_cfg.has_value())
        {
            if(const auto align = view_cfg->get_optional<std::string>("align"); align.has_value())
            {
                SIGHT_ASSERT("Align " << *align << " unknown", STRING_TO_ALIGN.find(*align) != STRING_TO_ALIGN.end());
                vi.m_align = STRING_TO_ALIGN.find(*align)->second;
            }

            vi.m_min_size.first  = view_cfg->get<int>("minWidth", vi.m_min_size.first);
            vi.m_min_size.second = view_cfg->get<int>("minHeight", vi.m_min_size.second);
            vi.m_max_size.first  = view_cfg->get<int>("maxWidth", vi.m_max_size.first);
            vi.m_max_size.second = view_cfg->get<int>("maxHeight", vi.m_max_size.second);

            vi.m_is_resizable   = view_cfg->get<bool>("resizable", vi.m_is_resizable);
            vi.m_position       = view_cfg->get<int>("position", vi.m_position);
            vi.m_layer          = view_cfg->get<int>("layer", vi.m_layer);
            vi.m_row            = view_cfg->get<int>("row", vi.m_row);
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

//-----------------------------------------------------------------------------

} // namespace sight::ui::layout
