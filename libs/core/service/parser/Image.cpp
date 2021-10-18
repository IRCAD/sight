/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "service/parser/Image.hpp"

#include <core/runtime/ConfigurationElement.hpp>
#include <core/runtime/Convert.hpp>

#include <data/Image.hpp>
#include <data/tools/Color.hpp>

namespace sight::service
{

namespace parser
{

//------------------------------------------------------------------------------

void Image::createConfig(core::tools::Object::sptr _obj)
{
    const auto image = data::Image::dynamicCast(_obj);
    SIGHT_ASSERT("Image does not exist.", image);

    const auto config = core::runtime::Convert::toPropertyTree(m_cfg).get_child("object");
    if(config.count("color"))
    {
        const std::string colorStr = config.get<std::string>("color");

        std::uint8_t color[4];
        data::tools::Color::hexaStringToRGBA(colorStr, color);

        // Initialize with a dummy 4x4 black image
        image->setType(core::tools::Type::s_UINT8);
        image->setNumberOfComponents(4);
        image->setSize2({4, 4, 1});
        image->setSpacing2({1, 1, 1});
        image->setOrigin2({0, 0, 0});
        image->setPixelFormat(data::Image::RGBA);
        image->resize();

        const auto dumpLock = image->lock();
        auto itr            = image->begin<sight::data::iterator::rgba>();
        const auto itrEnd   = image->end<sight::data::iterator::rgba>();

        for( ; itr != itrEnd ; ++itr)
        {
            itr->r = color[0];
            itr->g = color[1];
            itr->b = color[2];
            itr->a = color[3];
        }
    }
}

//------------------------------------------------------------------------------

} //namespace parser

} //namespace sight::service
