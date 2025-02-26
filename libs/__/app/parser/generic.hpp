/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

#include <core/object.hpp>

#include <service/object_parser.hpp>

namespace sight::app::parser
{

/**
 * @brief   Specific service for the construction of a generic data and its associated services from an XML-based
 * description.
 *
    <!-- Bool -->
    <object uid="..." type="sight::data::boolean" >
        <value>true</value>
    </object>

    <!-- Integer value -->
    <object uid="..." type="sight::data::integer" >
        <value>23</value>
    </object>

    <!-- Real value -->
    <object uid="..." type="sight::data::real" >
        <value>23.4</value>
    </object>

    <!-- Multiple integer or double value (should all be the same type)-->
    <object uid="..." type="sight::data::dvec2" >
        <value>23;24</value>
    </object>

    <!-- Color -->
    <object uid="..." type="sight::data::color" >
        <value>#FFFFFF</value>
    </object>

    <!-- String -->
    <object uid="..." type="sight::data::string" >
        <value>option_N</value>
    </object>
 */
class SIGHT_APP_CLASS_API generic : public service::object_parser
{
public:

    SIGHT_DECLARE_CLASS(generic, service::object_parser);

    /// Constructor : does nothing.
    generic() = default;

    /// Destructor : does nothing.
    ~generic() override = default;

    SIGHT_APP_API void parse(
        const service::config_t& _cfg,
        core::object::sptr _obj,
        objects_t&
    ) override;
};

} // namespace sight::app::parser
