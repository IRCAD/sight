/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#pragma once

#include "app/config.hpp"

#include <core/tools/object.hpp>

#include <service/object_parser.hpp>

namespace sight::app::parser
{

/**
 * @brief This service parse an hexa color value and set it to the color data.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <object uid="..." type="data::color" >
        <value>#FFFFFF</value>
    </object>
   @endcode
 *
 * @subsection Configuration Configuration
 * - \b value (optional, hexadecimal, default=#FFFFFF): color sets to the data.
 */
class APP_CLASS_API color final : public service::object_parser
{
public:

    /// Creates default attributes and methods (New(), ::sptr, ...).
    SIGHT_DECLARE_SERVICE(color, service::object_parser);

    /// Does nothing.
    APP_API color() = default;

    /// Does nothing.
    APP_API ~color() override = default;

private:

    /**
     * @brief Parse a color XML configuration.
     * @param _obj color in which apply the configuration.
     */
    void create_config(core::tools::object::sptr _obj) override;
};

} // namespace sight::app::parser
