/************************************************************************
 *
 * Copyright (C) 2020-2022 IRCAD France
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

#include "service/parser/Color.hpp"

#include <data/Color.hpp>

namespace sight::service::parser
{

//------------------------------------------------------------------------------

void Color::createConfig(core::tools::Object::sptr _obj)
{
    const data::Color::sptr color = data::Color::dynamicCast(_obj);
    SIGHT_ASSERT("Color does not exist.", color);

    const auto hexaColor = m_cfg.get<std::string>("object.value");
    SIGHT_ASSERT(
        "Color string should start with '#' and followed by 6 or 8 "
        "hexadecimal digits. Given color: " << hexaColor,
        hexaColor[0] == '#'
        && (hexaColor.length() == 7 || hexaColor.length() == 9)
    );
    color->setRGBA(hexaColor);
}

//------------------------------------------------------------------------------

} // namespace sight::service::parser
