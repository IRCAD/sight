/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include "app/parser/transfer_function.hpp"

#include <data/color.hpp>
#include <data/transfer_function.hpp>

namespace sight::app::parser
{

// ------------------------------------------------------------------------------

void transfer_function::parse(
    const service::config_t& _cfg,
    core::object::sptr _obj,
    objects_t&
    /*_sub_objects*/
)
{
    data::transfer_function::sptr tf = std::dynamic_pointer_cast<data::transfer_function>(_obj);
    SIGHT_ASSERT("transfer_function not instanced", tf);

    if(const auto color_cfg = _cfg.get_child_optional("colors"); color_cfg.has_value())
    {
        const bool is_default = color_cfg->get("<xmlattr>.default", false);
        if(is_default)
        {
            data::transfer_function::sptr default_tf = data::transfer_function::create_default_tf();
            tf->deep_copy(default_tf);
        }
        else
        {
            const auto steps_config = color_cfg->equal_range("step");

            auto tf_data           = tf->pieces().emplace_back(std::make_shared<data::transfer_function_piece>());
            const std::string name = _cfg.get<std::string>("name", "");
            if(!name.empty())
            {
                tf->set_name(name);
            }

            for(auto it_step_cfg = steps_config.first ; it_step_cfg != steps_config.second ; ++it_step_cfg)
            {
                const auto value     = it_step_cfg->second.get<double>("<xmlattr>.value");
                const auto str_color = it_step_cfg->second.get<std::string>("<xmlattr>.color");

                data::color::sptr new_color = std::make_shared<data::color>();
                new_color->from_string(str_color);

                const data::transfer_function::color_t color(new_color->red(), new_color->green(),
                                                             new_color->blue(), new_color->alpha());
                (*tf_data)[value] = color;
            }

            tf_data->set_window_min_max(tf_data->min_max());

            const bool is_clamped = color_cfg->get<bool>("<xmlattr>.isClamped", true);
            tf_data->set_clamped(is_clamped);

            const bool resample_to_max_texture_size = color_cfg->get<bool>(
                "<xmlattr>.resample_to_max_texture_size",
                true
            );
            tf->set_resample_to_max_texture_size(resample_to_max_texture_size);

            tf->fit_window();
        }
    }
    else
    {
        data::transfer_function::sptr default_tf = data::transfer_function::create_default_tf();
        tf->deep_copy(default_tf);
    }
}

// ------------------------------------------------------------------------------

} // namespace sight::app::parser
