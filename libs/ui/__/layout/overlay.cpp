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
 * @file ui/__/layout/overlay.cpp
 * @brief This file defines the implementation of the class for managing an overlay layout.
 *
 */

#include "ui/__/layout/overlay.hpp"

namespace sight::ui::layout
{

//-----------------------------------------------------------------------------

static overlay::view::coord parse_size(const std::string& _s)
{
    SIGHT_ASSERT("The provided size shouldn't be empty", !_s.empty());
    bool relative = (_s.back() == '%');
    int value     = static_cast<int>(std::strtol(_s.c_str(), nullptr, 10));
    bool negative = _s[0] == '-';
    return {relative, negative, value};
}

//------------------------------------------------------------------------------

void overlay::initialize(const ui::config_t& _configuration)
{
    m_views.clear();
    for(auto view : _configuration)
    {
        if(view.first == "<xmlattr>")
        {
            continue;
        }

        SIGHT_ASSERT(
            "view type '" + view.first + "' isn't supported; the only supported value is 'view'",
            view.first == "view"
        );
        overlay::view vi;
        if(const auto view_cfg = view.second.get_child_optional("<xmlattr>"); view_cfg.has_value())
        {
            vi.x          = parse_size(view_cfg->get("x", "0"));
            vi.y          = parse_size(view_cfg->get("y", "0"));
            vi.width      = parse_size(view_cfg->get("width", "0"));
            vi.height     = parse_size(view_cfg->get("height", "0"));
            vi.min_width  = view_cfg->get("minWidth", 0);
            vi.min_height = view_cfg->get("minHeight", 0);
            vi.visible    = view_cfg->get("visible", true);
            vi.opacity    = view_cfg->get("opacity", 0.F);
        }

        m_views.push_back(vi);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::layout
