/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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

#include "modules/ui/viz/helper/parameter_editor.hpp"

#include <data/array.hpp>
#include <data/color.hpp>
#include <data/integer.hpp>
#include <data/real.hpp>

#include <cmath>

namespace sight::module::ui::viz::helper
{

//-----------------------------------------------------------------------------

template<typename T>
std::pair<T, T> get_range(T _value)
{
    static const T range = 10;
    T max;
    T min;

    // For _value > 1, we use [-_value * range;+_value *range]
    if(_value > 1)
    {
        max = +_value * range;
        min = -_value * range;
    }
    else if(_value < -1)
    // For _value < 0, we use [_value * range;-_value *range]
    {
        max = -_value * range;
        min = +_value * range;
    }
    else if(_value > 0 || _value < 0)
    // For -1 < value < 0 < _value < 1, we use [_value / range;+_value * range]
    {
        max = _value * range;
        min = _value / range;
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

service::config_t parameter_editor::createConfig(
    const sight::viz::scene3d::parameter_adaptor::csptr& _adaptor,
    const service::base::csptr& _param_srv,
    core::com::helper::sig_slot_connection& _connections
)
{
    service::config_t param_config;

    /// Getting associated object infos
    const auto shader_obj =
        _adaptor->inout(sight::viz::scene3d::parameter_adaptor::s_PARAMETER_INOUT).lock();

    const auto& obj_type = shader_obj->get_classname();

    if(obj_type == "sight::data::boolean")
    {
        _connections.connect(_param_srv, "boolChanged", _adaptor, "setBoolParameter");

        param_config.add("<xmlattr>.type", "bool");
        param_config.add("<xmlattr>.name", _adaptor->getParamName());
        param_config.add("<xmlattr>.key", _adaptor->getParamName());
        param_config.add("<xmlattr>.defaultValue", false);
    }
    else if(obj_type == "sight::data::color")
    {
        _connections.connect(_param_srv, "colorChanged", _adaptor, "setColorParameter");

        auto color_value = std::dynamic_pointer_cast<data::color>(shader_obj.get_shared());

        int r = static_cast<unsigned char>(color_value->red() * 255);
        int g = static_cast<unsigned char>(color_value->green() * 255);
        int b = static_cast<unsigned char>(color_value->blue() * 255);
        int a = static_cast<unsigned char>(color_value->alpha() * 255);

        std::stringstream hex_str;
        hex_str << "#" << std::hex;
        hex_str << ((r < 0x10) ? "0" : "") << r;
        hex_str << ((g < 0x10) ? "0" : "") << g;
        hex_str << ((b < 0x10) ? "0" : "") << b;
        hex_str << ((a < 0x10) ? "0" : "") << a;

        param_config.add("<xmlattr>.type", "color");
        param_config.add("<xmlattr>.name", _adaptor->getParamName());
        param_config.add("<xmlattr>.key", _adaptor->getParamName());
        param_config.add("<xmlattr>.defaultValue", hex_str.str());
    }
    else if(obj_type == "sight::data::real")
    {
        _connections.connect(_param_srv, "doubleChanged", _adaptor, "setDoubleParameter");

        auto float_value         = std::dynamic_pointer_cast<data::real>(shader_obj.get_shared());
        const auto default_value = static_cast<double>(float_value->value());
        const auto minmax        = get_range(default_value);
        const double min         = minmax.first;
        const double max         = minmax.second;

        param_config.add("<xmlattr>.type", "double");
        param_config.add("<xmlattr>.name", _adaptor->getParamName());
        param_config.add("<xmlattr>.key", _adaptor->getParamName());
        param_config.add("<xmlattr>.defaultValue", std::to_string(default_value));
        param_config.add("<xmlattr>.min", min);
        param_config.add("<xmlattr>.max", max);
    }
    else if(obj_type == "sight::data::integer")
    {
        _connections.connect(_param_srv, "intChanged", _adaptor, "setIntParameter");

        auto int_value          = std::dynamic_pointer_cast<data::integer>(shader_obj.get_shared());
        const int default_value = int(int_value->value());
        const auto minmax       = get_range(default_value);
        const int min           = minmax.first;
        const int max           = minmax.second;

        param_config.add("<xmlattr>.type", "int");
        param_config.add("<xmlattr>.name", _adaptor->getParamName());
        param_config.add("<xmlattr>.key", _adaptor->getParamName());
        param_config.add("<xmlattr>.defaultValue", std::to_string(default_value));
        param_config.add("<xmlattr>.min", min);
        param_config.add("<xmlattr>.max", max);
    }
    else if(obj_type == "sight::data::array")
    {
        auto array_object         = std::dynamic_pointer_cast<data::array>(shader_obj.get_shared());
        const auto num_components = array_object->size()[0];
        if(num_components <= 3)
        {
            std::string str_size = std::to_string(num_components);

            if(array_object->getType() == core::type::FLOAT
               || array_object->getType() == core::type::DOUBLE)
            {
                _connections.connect(
                    _param_srv,
                    "double" + str_size + "Changed",
                    _adaptor,
                    "setDouble" + str_size + "Parameter"
                );

                // We can't give a default value for each component to parameters :/
                // For now fill it with the first one
                const auto dump_lock = array_object->dump_lock();

                double default_value = NAN;
                if(array_object->getType() == core::type::FLOAT)
                {
                    default_value = static_cast<double>(array_object->at<float>(0));
                }
                else
                {
                    default_value = array_object->at<double>(0);
                }

                const auto minmax = get_range(default_value);
                const double min  = minmax.first;
                const double max  = minmax.second;

                param_config.add("<xmlattr>.type", "double" + str_size);
                param_config.add("<xmlattr>.name", _adaptor->getParamName());
                param_config.add("<xmlattr>.key", _adaptor->getParamName());
                param_config.add("<xmlattr>.defaultValue", std::to_string(default_value));
                param_config.add("<xmlattr>.min", min);
                param_config.add("<xmlattr>.max", max);
            }
            else if(array_object->getType() == core::type::INT32)
            {
                _connections.connect(
                    _param_srv,
                    "int" + str_size + "Changed",
                    _adaptor,
                    "setInt" + str_size + "Parameter"
                );
                const auto dump_lock = array_object->dump_lock();

                const int default_value = array_object->at<std::int32_t>(0);
                const auto minmax       = get_range(default_value);
                const int min           = minmax.first;
                const int max           = minmax.second;

                param_config.add("<xmlattr>.type", "int" + str_size);
                param_config.add("<xmlattr>.name", _adaptor->getParamName());
                param_config.add("<xmlattr>.key", _adaptor->getParamName());
                param_config.add("<xmlattr>.defaultValue", std::to_string(default_value));
                param_config.add("<xmlattr>.min", min);
                param_config.add("<xmlattr>.max", max);
            }
            else
            {
                SIGHT_ERROR("Array type not handled: " << array_object->getType());
            }
        }
        else
        {
            SIGHT_ERROR("Array size not handled: " << num_components);
        }
    }
    else
    {
        SIGHT_ERROR("No editor found for the object of type " << obj_type);
    }

    return param_config;
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::viz::helper
