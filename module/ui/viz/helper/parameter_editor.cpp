/************************************************************************
 *
 * Copyright (C) 2017-2025 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "module/ui/viz/helper/parameter_editor.hpp"

#include <data/array.hpp>
#include <data/color.hpp>
#include <data/dvec2.hpp>
#include <data/dvec3.hpp>
#include <data/dvec4.hpp>
#include <data/integer.hpp>
#include <data/ivec2.hpp>
#include <data/ivec3.hpp>
#include <data/ivec4.hpp>
#include <data/real.hpp>

#include <cmath>

namespace sight::module::ui::viz::helper
{

//-----------------------------------------------------------------------------

template<typename T>
std::pair<T, T> get_range(T _value)
{
    static const T s_RANGE = 10;
    T max;
    T min;

    // For _value > 1, we use [-_value * range;+_value *range]
    if(_value > 1)
    {
        max = +_value * s_RANGE;
        min = -_value * s_RANGE;
    }
    else if(_value < -1)
    // For _value < 0, we use [_value * range;-_value *range]
    {
        max = -_value * s_RANGE;
        min = +_value * s_RANGE;
    }
    else if(_value > 0 || _value < 0)
    // For -1 < value < 0 < _value < 1, we use [_value / range;+_value * range]
    {
        max = _value * s_RANGE;
        min = _value / s_RANGE;
    }
    else
    // For _value == 0, we use [0; 1]
    {
        max = 0;
        min = 1;
    }

    return std::make_pair(min, max);
}

//-----------------------------------------------------------------------------

service::config_t parameter_editor::create_config(
    const sight::viz::scene3d::parameter_adaptor::csptr& _adaptor
)
{
    service::config_t param_config;

    /// Getting associated object infos
    const auto shader_obj = _adaptor->inout(sight::viz::scene3d::parameter_adaptor::PARAMETER_INOUT).lock();

    const auto& obj_type = shader_obj->get_classname();

    const auto set_config =
        [&](auto& _data)
        {
            const auto minmax = get_range(_data);
            param_config.add("<xmlattr>.name", _adaptor->get_param_name());
            param_config.add("<xmlattr>.key", _adaptor->get_param_name());
            param_config.add("<xmlattr>.min", minmax.first);
            param_config.add("<xmlattr>.max", minmax.second);
        };

    if(obj_type == "sight::data::boolean" || obj_type == "sight::data::color")
    {
        param_config.add("<xmlattr>.name", _adaptor->get_param_name());
        param_config.add("<xmlattr>.key", _adaptor->get_param_name());
    }
    else if(obj_type == "sight::data::real")
    {
        auto float_value = std::dynamic_pointer_cast<data::real>(shader_obj.get_shared());
        set_config(float_value->value());
        param_config.add("<xmlattr>.widget", "slider");
    }
    else if(obj_type == "sight::data::integer")
    {
        auto int_value = std::dynamic_pointer_cast<data::integer>(shader_obj.get_shared());
        set_config(int_value->value());
        param_config.add("<xmlattr>.widget", "slider");
    }
    else if(obj_type == "sight::data::ivec2")
    {
        auto data = std::dynamic_pointer_cast<data::ivec2>(shader_obj.get_shared());
        set_config(data->value()[0]);
    }
    else if(obj_type == "sight::data::ivec3")
    {
        auto data = std::dynamic_pointer_cast<data::ivec3>(shader_obj.get_shared());
        set_config(data->value()[0]);
    }
    else if(obj_type == "sight::data::ivec4")
    {
        auto data = std::dynamic_pointer_cast<data::ivec4>(shader_obj.get_shared());
        set_config(data->value()[0]);
    }
    else if(obj_type == "sight::data::dvec2")
    {
        auto data = std::dynamic_pointer_cast<data::dvec2>(shader_obj.get_shared());
        set_config(data->value()[0]);
    }
    else if(obj_type == "sight::data::dvec3")
    {
        auto data = std::dynamic_pointer_cast<data::dvec3>(shader_obj.get_shared());
        set_config(data->value()[0]);
    }
    else if(obj_type == "sight::data::dvec4")
    {
        auto data = std::dynamic_pointer_cast<data::dvec4>(shader_obj.get_shared());
        set_config(data->value()[0]);
    }
    else
    {
        SIGHT_ERROR("No editor found for the object of type " << obj_type);
    }

    param_config.add("<xmlattr>.reset", "false");

    return param_config;
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::viz::helper
