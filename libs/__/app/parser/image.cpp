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

#include "app/parser/image.hpp"

#include <data/image.hpp>
#include <data/tools/color.hpp>

namespace sight::app::parser
{

//------------------------------------------------------------------------------

void image::createConfig(core::tools::object::sptr _obj)
{
    const auto image = std::dynamic_pointer_cast<data::image>(_obj);
    SIGHT_ASSERT("image does not exist.", image);

    if(m_cfg.count("color") != 0U)
    {
        const auto color_str = m_cfg.get<std::string>("color");

        std::array<std::uint8_t, 4> color {};
        data::tools::color::hexaStringToRGBA(color_str, color);

        // Initialize with a dummy 4x4 black image
        image->setSpacing({1, 1, 1});
        image->setOrigin({0, 0, 0});
        image->resize({4, 4, 1}, core::type::UINT8, data::image::RGBA);

        const auto dump_lock = image->dump_lock();
        auto itr             = image->begin<sight::data::iterator::rgba>();
        const auto itr_end   = image->end<sight::data::iterator::rgba>();

        for( ; itr != itr_end ; ++itr)
        {
            itr->r = color[0];
            itr->g = color[1];
            itr->b = color[2];
            itr->a = color[3];
        }
    }
    else if(m_cfg.count("gray") != 0U)
    {
        const auto value = m_cfg.get<std::uint8_t>("gray", 0);

        // Initialize with a dummy 4x4 black image
        image->setSpacing({1, 1, 1});
        image->setOrigin({0, 0, 0});
        image->resize({4, 4, 1}, core::type::UINT8, data::image::GRAY_SCALE);

        const auto dump_lock = image->dump_lock();
        auto itr             = image->begin<std::uint8_t>();
        const auto itr_end   = image->end<std::uint8_t>();

        for( ; itr != itr_end ; ++itr)
        {
            *itr = value;
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::app::parser
