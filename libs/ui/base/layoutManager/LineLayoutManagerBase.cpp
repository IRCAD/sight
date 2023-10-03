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
 * @file ui/base/layoutManager/LineLayoutManagerBase.cpp
 * @brief This file defines the implementation of the class for managing a line layout geometry.
 *
 */

#include "ui/base/layoutManager/LineLayoutManagerBase.hpp"

#include <core/base.hpp>

namespace sight::ui::base::layoutManager
{

//-----------------------------------------------------------------------------

const LineLayoutManagerBase::RegistryKeyType LineLayoutManagerBase::REGISTRY_KEY =
    "sight::ui::base::LineLayoutManager";

//-----------------------------------------------------------------------------

void LineLayoutManagerBase::initialize(const ui::base::config_t& configuration)
{
    const auto orientationCfg = configuration.get<std::string>("orientation.<xmlattr>.value", "");

    SIGHT_FATAL_IF("missing orientation configuration", orientationCfg.empty());
    SIGHT_ASSERT(
        "Wrong value '" + orientationCfg + "' for 'orientation' attribute (require vertical or horizontal)",
        orientationCfg == "vertical" || orientationCfg == "horizontal"
    );
    m_orientation = (orientationCfg == "vertical") ? VERTICAL : HORIZONTAL;

    m_views.clear();
    for(auto view : configuration)
    {
        if(view.first == "spacer")
        {
            ViewInfo vi;
            vi.m_isSpacer = true;
            m_views.push_back(vi);
        }
        else if(view.first == "view")
        {
            ViewInfo vi;
            if(const auto viewCfg = view.second.get_child_optional("<xmlattr>"); viewCfg.has_value())
            {
                vi.m_proportion = viewCfg->get<int>("proportion", vi.m_proportion);

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

                vi.m_spacing = viewCfg->get<int>("spacing", vi.m_spacing);

                vi.m_minSize.first  = viewCfg->get<int>("minWidth", vi.m_minSize.first);
                vi.m_minSize.second = viewCfg->get<int>("minHeight", vi.m_minSize.second);
                vi.m_maxSize.first  = viewCfg->get<int>("maxWidth", vi.m_maxSize.first);
                vi.m_maxSize.second = viewCfg->get<int>("maxHeight", vi.m_maxSize.second);

                vi.m_visible      = viewCfg->get<bool>("visible", vi.m_visible);
                vi.m_useScrollBar = viewCfg->get<bool>("useScrollBar", vi.m_useScrollBar);
                vi.m_toolTip      = viewCfg->get<std::string>("toolTip", vi.m_toolTip);
                vi.m_qssKey       = viewCfg->get<std::string>("QSSClass", "");

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
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::base::layoutManager
