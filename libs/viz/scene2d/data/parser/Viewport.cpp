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

#include "viz/scene2d/data/parser/Viewport.hpp"
#include "viz/scene2d/data/Viewport.hpp"

#include <core/runtime/Convert.hpp>

#include <service/macros.hpp>

SIGHT_REGISTER_SERVICE(
    sight::service::IXMLParser,
    sight::viz::scene2d::data::parser::Viewport,
    sight::viz::scene2d::data::Viewport
);

namespace sight::viz::scene2d::data::parser
{

//------------------------------------------------------------------------------

void Viewport::createConfig(core::tools::Object::sptr _obj)
{
    const data::Viewport::sptr viewport = data::Viewport::dynamicCast(_obj);
    SIGHT_ASSERT("Viewport does not exist.", viewport);

    const auto configTree = core::runtime::Convert::toPropertyTree(m_cfg);
    const auto config     = configTree.get_child_optional("object.config.<xmlattr>");
    if(config.has_value())
    {
        const std::string id = config->get("id", "");
        const float x        = config->get<float>("x", 0.F);
        const float y        = config->get<float>("y", 0.F);
        const float width    = config->get<float>("width", 1.F);
        const float height   = config->get<float>("height", 1.F);

        viewport->setX(x);
        viewport->setY(y);
        viewport->setWidth(width);
        viewport->setHeight(height);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene2d::data::parser
