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

const std::map<std::string, cardinal::Align> cardinal::STRING_TO_ALIGN = {
    {"center", CENTER},
    {"right", RIGHT},
    {"left", LEFT},
    {"bottom", BOTTOM},
    {"top", TOP},
};

//-----------------------------------------------------------------------------

void cardinal::initialize(const ui::config_t& _configuration)
{
    m_views.clear();

    const auto views_cfg = _configuration.equal_range("view");
    for(const auto& view : boost::make_iterator_range(views_cfg))
    {
        ViewInfo vi;
        if(const auto view_cfg = view.second.get_child_optional("<xmlattr>"); view_cfg.has_value())
        {
            if(const auto align = view_cfg->get_optional<std::string>("align"); align.has_value())
            {
                SIGHT_ASSERT("Align " << *align << " unknown", STRING_TO_ALIGN.find(*align) != STRING_TO_ALIGN.end());
                vi.m_align = STRING_TO_ALIGN.find(*align)->second;
            }

            vi.m_minSize.first  = view_cfg->get<int>("minWidth", vi.m_minSize.first);
            vi.m_minSize.second = view_cfg->get<int>("minHeight", vi.m_minSize.second);
            vi.m_maxSize.first  = view_cfg->get<int>("maxWidth", vi.m_maxSize.first);
            vi.m_maxSize.second = view_cfg->get<int>("maxHeight", vi.m_maxSize.second);

            vi.m_isResizable  = view_cfg->get<bool>("resizable", vi.m_isResizable);
            vi.m_position     = view_cfg->get<int>("position", vi.m_position);
            vi.m_layer        = view_cfg->get<int>("layer", vi.m_layer);
            vi.m_row          = view_cfg->get<int>("row", vi.m_row);
            vi.m_visible      = view_cfg->get<bool>("visible", vi.m_visible);
            vi.m_useScrollBar = view_cfg->get<bool>("useScrollBar", vi.m_useScrollBar);
            vi.m_toolTip      = view_cfg->get<std::string>("toolTip", vi.m_toolTip);

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
                vi.m_backgroundColor = hexa_color;
            }
        }

        m_views.push_back(vi);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::layout
