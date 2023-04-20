/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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
 * @file ui/base/layoutManager/OverlayLayoutManager.cpp
 * @brief This file defines the implementation of the class for managing an overlay layout.
 *
 */

#include "ui/base/layoutManager/OverlayLayoutManager.hpp"

namespace sight::ui::base::layoutManager
{

//-----------------------------------------------------------------------------

static OverlayLayoutManager::View::Coord parseSize(const std::string& s)
{
    SIGHT_ASSERT("The provided size shouldn't be empty", !s.empty());
    bool relative = (s.back() == '%');
    int value     = static_cast<int>(std::strtol(s.c_str(), nullptr, 10));
    return {relative, value};
}

//------------------------------------------------------------------------------

void OverlayLayoutManager::initialize(const ui::base::config_t& configuration)
{
    m_views.clear();
    for(auto view : configuration)
    {
        if(view.first == "<xmlattr>")
        {
            continue;
        }

        SIGHT_ASSERT(
            "View type '" + view.first + "' isn't supported; the only supported value is 'view'",
            view.first == "view"
        );
        View vi;
        if(const auto viewCfg = view.second.get_child_optional("<xmlattr>"); viewCfg.has_value())
        {
            vi.x         = parseSize(viewCfg->get("x", "0"));
            vi.y         = parseSize(viewCfg->get("y", "0"));
            vi.width     = parseSize(viewCfg->get("width", "0"));
            vi.height    = parseSize(viewCfg->get("height", "0"));
            vi.minWidth  = viewCfg->get("minWidth", 0);
            vi.minHeight = viewCfg->get("minHeight", 0);
            vi.visible   = viewCfg->get("visible", true);
        }

        m_views.push_back(vi);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::base::layoutManager
