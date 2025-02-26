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

#pragma once

#include <sight/app/config.hpp>

#include <app/parser/generic.hpp>

#include <core/object.hpp>

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
class SIGHT_APP_CLASS_API color final : public generic
{
public:

    SIGHT_DECLARE_SERVICE(color, app::parser::generic);
};

} // namespace sight::app::parser
