/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "app/parser/matrix4.hpp"

#include "service/macros.hpp"

#include <data/matrix4.hpp>

#include <cmath>

namespace sight::app::parser
{

//------------------------------------------------------------------------------

void matrix4::parse(
    const service::config_t& _cfg,
    core::object::sptr _obj,
    objects_t&
    /*_sub_objects*/
)
{
    data::matrix4::sptr matrix = std::dynamic_pointer_cast<data::matrix4>(_obj);
    SIGHT_ASSERT("matrix not instanced", matrix);

    for(const auto& elem : _cfg)
    {
        if(elem.first == "matrix")
        {
            const auto input = elem.second.get_value<std::string>();
            std::istringstream input_string(input);

            double coef = NAN;
            for(unsigned int k = 0 ; k < 16 ; k++)
            {
                input_string >> coef;
                (*matrix)[k] = coef;
            }
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::app::parser
