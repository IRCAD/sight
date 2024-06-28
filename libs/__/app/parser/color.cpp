/************************************************************************
 *
 * Copyright (C) 2020-2024 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "app/parser/color.hpp"

#include <data/color.hpp>

namespace sight::app::parser
{

//------------------------------------------------------------------------------

void color::create_config(core::object::sptr _obj)
{
    const data::color::sptr color = std::dynamic_pointer_cast<data::color>(_obj);
    SIGHT_ASSERT("color does not exist.", color);

    const auto hexa_color = m_cfg.get<std::string>("value");
    SIGHT_ASSERT(
        "color string should start with '#' and followed by 6 or 8 "
        "hexadecimal digits. Given color: " << hexa_color,
        hexa_color[0] == '#'
        && (hexa_color.length() == 7 || hexa_color.length() == 9)
    );
    color->set_rgba(hexa_color);
}

//------------------------------------------------------------------------------

} // namespace sight::app::parser
