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

#include "ui/__/layout/tab.hpp"

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
        ViewInfo vi;
        if(const auto view_cfg = view.second.get_child_optional("<xmlattr>"); view_cfg.has_value())
        {
            if(const auto border = view_cfg->get_optional<int>("border"); border.has_value())
            {
                vi.m_border = border.value();
            }
            else
            {
                vi.m_leftBorder   = view_cfg->get<int>("leftBorder", vi.m_leftBorder);
                vi.m_topBorder    = view_cfg->get<int>("topBorder", vi.m_topBorder);
                vi.m_rightBorder  = view_cfg->get<int>("rightBorder", vi.m_rightBorder);
                vi.m_bottomBorder = view_cfg->get<int>("bottomBorder", vi.m_bottomBorder);
            }

            vi.m_minSize.first  = view_cfg->get<int>("minWidth", vi.m_minSize.first);
            vi.m_minSize.second = view_cfg->get<int>("minHeight", vi.m_minSize.second);

            vi.m_isSelect     = view_cfg->get<bool>("selected", vi.m_isSelect);
            vi.m_useScrollBar = view_cfg->get<bool>("useScrollBar", vi.m_useScrollBar);

            vi.m_caption = view_cfg->get<std::string>("caption", "");

            if(const auto hexa_color = view_cfg->get<std::string>("backgroundColor", ""); !hexa_color.empty())
            {
                SIGHT_ASSERT(
                    "Color string should start with '#' and followed by 6 or 8 "
                    "hexadecimal digits. Given color: " << hexa_color,
                    hexa_color[0] == '#'
                    && (hexa_color.length() == 7 || hexa_color.length() == 9)
                );
                vi.m_backgroundColor = hexa_color;
            }
        }

        m_views.push_back(vi);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::layout
