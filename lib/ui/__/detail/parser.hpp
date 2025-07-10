/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#pragma once

#include "ui/__/object.hpp"

#include <core/ptree.hpp>

namespace sight::ui::detail
{

//------------------------------------------------------------------------------

[[nodiscard]] inline std::string parse_background_color(const ui::config_t& _config)
{
    auto assert_color = []([[maybe_unused]] const auto& _color)
                        {
                            SIGHT_ASSERT(
                                "Color string should start with '#' and followed by 6 or 8 "
                                "hexadecimal digits. Given color: " << _color,
                                _color[0] == '#'
                                && (_color.length() == 7 || _color.length() == 9)
                            );
                        };
    if(auto hexa_color = _config.get_optional<std::string>("<xmlattr>.background_color"); hexa_color.has_value())
    {
        assert_color(*hexa_color);
        return *hexa_color;
    }
    else if(hexa_color = _config.get_optional<std::string>("<xmlattr>.backgroundColor"); hexa_color.has_value())
    {
        FW_DEPRECATED_MSG("'backgroundColor' is deprecated, use 'background_color' instead", "26.0");
        assert_color(*hexa_color);
        return *hexa_color;
    }

    return {};
}

//------------------------------------------------------------------------------

[[nodiscard]] inline std::pair<int, int> parse_min_size(const ui::config_t& _config)
{
    return {
        core::ptree::get_and_deprecate<int>(_config, "min_width", "minWidth", "26.0", -1),
        core::ptree::get_and_deprecate<int>(_config, "min_height", "minHeight", "26.0", -1)
    };
}

//------------------------------------------------------------------------------

[[nodiscard]] inline std::pair<int, int> parse_max_size(const ui::config_t& _config)
{
    static const auto DEFAULT = std::numeric_limits<int>::max();
    return {
        core::ptree::get_and_deprecate<int>(_config, "max_width", "maxWidth", "26.0", DEFAULT),
        core::ptree::get_and_deprecate<int>(_config, "max_height", "maxHeight", "26.0", DEFAULT)
    };
}

//------------------------------------------------------------------------------

} // namespace sight::ui::detail
