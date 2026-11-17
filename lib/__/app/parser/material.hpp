/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <sight/app/config.hpp>

#include <service/object_parser.hpp>

namespace sight::app::parser
{

/**
 * @brief Specific service that initializes a sight::data::material.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <object uid="material" type="sight::data::material">
        <ambient>#808080FF</ambient>
        <diffuse>#FFFFFFFF</diffuse>
        <shading>phong</shading>
        <representation>surface</representation>
        <options>standard</options>
    </object>
 * @endcode
 *
 * The supported values are:
 * - shading: ambient, flat or phong.
 * - representation: surface, point, wireframe or edge.
 * - options: standard, normals, cells_normals or selected.
 *
 * Texture-related properties are intentionally not handled by this parser.
 */
class SIGHT_APP_CLASS_API material : public service::object_parser
{
public:

    SIGHT_DECLARE_SERVICE(material, service::object_parser);

    SIGHT_APP_API void parse(
        const service::config_t& _cfg,
        core::object::sptr _obj,
        objects_t& /*_sub_objects*/
    ) override;
};

} // namespace sight::app::parser
