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

const cardinal::RegistryKeyType cardinal::REGISTRY_KEY = "sight::ui::layout::cardinal";
//-----------------------------------------------------------------------------

const std::map<std::string, cardinal::Align> cardinal::STRING_TO_ALIGN = {
    {"center", CENTER},
    {"right", RIGHT},
    {"left", LEFT},
    {"bottom", BOTTOM},
    {"top", TOP},
};

//-----------------------------------------------------------------------------

void cardinal::initialize(const ui::config_t& configuration)
{
    m_views.clear();

    const auto viewsCfg = configuration.equal_range("view");
    for(const auto& view : boost::make_iterator_range(viewsCfg))
    {
        ViewInfo vi;
        if(const auto viewCfg = view.second.get_child_optional("<xmlattr>"); viewCfg.has_value())
        {
            if(const auto align = viewCfg->get_optional<std::string>("align"); align.has_value())
            {
                SIGHT_ASSERT("Align " << *align << " unknown", STRING_TO_ALIGN.find(*align) != STRING_TO_ALIGN.end());
                vi.m_align = STRING_TO_ALIGN.find(*align)->second;
            }

            vi.m_minSize.first  = viewCfg->get<int>("minWidth", vi.m_minSize.first);
            vi.m_minSize.second = viewCfg->get<int>("minHeight", vi.m_minSize.second);
            vi.m_maxSize.first  = viewCfg->get<int>("maxWidth", vi.m_maxSize.first);
            vi.m_maxSize.second = viewCfg->get<int>("maxHeight", vi.m_maxSize.second);

            vi.m_isResizable  = viewCfg->get<bool>("resizable", vi.m_isResizable);
            vi.m_position     = viewCfg->get<int>("position", vi.m_position);
            vi.m_layer        = viewCfg->get<int>("layer", vi.m_layer);
            vi.m_row          = viewCfg->get<int>("row", vi.m_row);
            vi.m_visible      = viewCfg->get<bool>("visible", vi.m_visible);
            vi.m_useScrollBar = viewCfg->get<bool>("useScrollBar", vi.m_useScrollBar);
            vi.m_toolTip      = viewCfg->get<std::string>("toolTip", vi.m_toolTip);

            if(auto caption = viewCfg->get_optional<std::string>("caption"); caption.has_value())
            {
                vi.m_caption.first  = true;
                vi.m_caption.second = caption.value();
            }

            if(const auto hexaColor = viewCfg->get<std::string>("backgroundColor", ""); !hexaColor.empty())
            {
                SIGHT_ASSERT(
                    "Color string should start with '#' and followed by 6 or 8 "
                    "hexadecimal digits. Given color: " << hexaColor,
                    hexaColor[0] == '#'
                    && (hexaColor.length() == 7 || hexaColor.length() == 9)
                );
                vi.m_backgroundColor = hexaColor;
            }
        }

        m_views.push_back(vi);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::layout
