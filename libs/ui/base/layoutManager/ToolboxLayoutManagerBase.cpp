/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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
 * @file ui/base/layoutManager/ToolboxLayoutManagerBase.cpp
 * @brief This file defines the implementation of the class for managing a line layout geometry.
 *
 */

#include "ui/base/layoutManager/ToolboxLayoutManagerBase.hpp"

#include <boost/range/iterator_range_core.hpp>

namespace sight::ui::base::layoutManager
{

//-----------------------------------------------------------------------------

const ToolboxLayoutManagerBase::RegistryKeyType ToolboxLayoutManagerBase::REGISTRY_KEY =
    "sight::ui::base::ToolboxLayoutManager";

//-----------------------------------------------------------------------------

ToolboxLayoutManagerBase::ToolboxLayoutManagerBase()
= default;

//-----------------------------------------------------------------------------

ToolboxLayoutManagerBase::~ToolboxLayoutManagerBase()
= default;

//-----------------------------------------------------------------------------

void ToolboxLayoutManagerBase::initialize(const ui::base::config_t& configuration)
{
    m_views.clear();
    for(const auto& view : boost::make_iterator_range(configuration.equal_range("view")))
    {
        ViewInfo vi;
        if(const auto viewCfg = view.second.get_child_optional("<xmlattr>"); viewCfg.has_value())
        {
            if(const auto border = viewCfg->get_optional<int>("border"); border.has_value())
            {
                vi.m_border = border.value();
            }
            else
            {
                vi.m_leftBorder   = viewCfg->get<int>("leftBorder", vi.m_leftBorder);
                vi.m_topBorder    = viewCfg->get<int>("topBorder", vi.m_topBorder);
                vi.m_rightBorder  = viewCfg->get<int>("rightBorder", vi.m_rightBorder);
                vi.m_bottomBorder = viewCfg->get<int>("bottomBorder", vi.m_bottomBorder);
            }

            vi.m_minSize.first  = viewCfg->get<int>("minWidth", vi.m_minSize.first);
            vi.m_minSize.second = viewCfg->get<int>("minHeight", vi.m_minSize.second);
            vi.m_maxSize.first  = viewCfg->get<int>("maxWidth", vi.m_maxSize.first);
            vi.m_maxSize.second = viewCfg->get<int>("maxHeight", vi.m_maxSize.second);

            vi.m_visible      = viewCfg->get<bool>("visible", vi.m_visible);
            vi.m_useScrollBar = viewCfg->get<bool>("useScrollBar", vi.m_useScrollBar);
            vi.m_expanded     = viewCfg->get<bool>("expanded", vi.m_expanded);

            vi.m_caption = viewCfg->get<std::string>("caption", "");

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

} // namespace sight::ui::base::layoutManager
