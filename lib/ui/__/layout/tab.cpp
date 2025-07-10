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

#include "ui/__/layout/tab.hpp"

#include "ui/__/detail/parser.hpp"

#include <boost/range/iterator_range_core.hpp>
namespace sight::ui::layout
{

//-----------------------------------------------------------------------------

const tab::registry_key_t tab::REGISTRY_KEY = "sight::ui::layout::tab";

//-----------------------------------------------------------------------------

void tab::initialize(const ui::config_t& _configuration)
{
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

            vi.m_min_size = detail::parse_min_size(*view_cfg);

            vi.m_is_select      = view_cfg->get<bool>("selected", vi.m_is_select);
            vi.m_use_scroll_bar = view_cfg->get<bool>("useScrollBar", vi.m_use_scroll_bar);

            vi.m_caption = view_cfg->get<std::string>("caption", "");
            vi.m_qss_key = view_cfg->get<std::string>("QSSClass", "");

            vi.m_background_color = detail::parse_background_color(*view_cfg);
        }

        m_views.push_back(vi);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::layout
