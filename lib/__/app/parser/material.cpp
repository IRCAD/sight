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

#include "app/parser/material.hpp"

#include <data/material.hpp>

#include <boost/algorithm/string/case_conv.hpp>

namespace sight::app::parser
{

//-----------------------------------------------------------------------------

void material::parse(
    const service::config_t& _cfg,
    core::object::sptr _obj,
    objects_t& /*_sub_objects*/
)
{
    const auto material = std::dynamic_pointer_cast<data::material>(_obj);
    SIGHT_ASSERT("The passed object must be a data::material", material);

    if(const auto ambient = _cfg.get_optional<std::string>("ambient"); ambient.has_value())
    {
        auto color = std::make_shared<data::color>();
        color->from_string(*ambient);
        material->set_ambient(color);
    }

    if(const auto diffuse = _cfg.get_optional<std::string>("diffuse"); diffuse.has_value())
    {
        auto color = std::make_shared<data::color>();
        color->from_string(*diffuse);
        material->set_diffuse(color);
    }

    if(const auto shading = _cfg.get_optional<std::string>("shading"); shading.has_value())
    {
        const auto shading_mode = boost::to_lower_copy(*shading);
        if(shading_mode == "ambient")
        {
            material->set_shading_mode(data::material::shading_t::ambient);
        }
        else if(shading_mode == "flat")
        {
            material->set_shading_mode(data::material::shading_t::flat);
        }
        else if(shading_mode == "phong")
        {
            material->set_shading_mode(data::material::shading_t::phong);
        }
        else
        {
            SIGHT_ERROR(
                "Value: " + *shading + " is not valid for 'shading'. Accepted values are: ambient/flat/phong. "
                                       "'shading' is reset to default value (phong)."
            );
            material->set_shading_mode(data::material::shading_t::phong);
        }
    }

    if(const auto representation = _cfg.get_optional<std::string>("representation"); representation.has_value())
    {
        material->set_representation_mode(data::material::string_to_representation_mode(*representation));
    }

    if(const auto options = _cfg.get_optional<std::string>("options"); options.has_value())
    {
        material->set_options_mode(data::material::string_to_options_mode(*options));
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::app::parser
