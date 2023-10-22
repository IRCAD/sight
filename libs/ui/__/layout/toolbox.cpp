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
            vi.m_maxSize.first  = view_cfg->get<int>("maxWidth", vi.m_maxSize.first);
            vi.m_maxSize.second = view_cfg->get<int>("maxHeight", vi.m_maxSize.second);

            vi.m_visible      = view_cfg->get<bool>("visible", vi.m_visible);
            vi.m_useScrollBar = view_cfg->get<bool>("useScrollBar", vi.m_useScrollBar);
            vi.m_expanded     = view_cfg->get<bool>("expanded", vi.m_expanded);

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
