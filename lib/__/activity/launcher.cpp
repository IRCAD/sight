/************************************************************************
 *
 * Copyright (C) 2019-2025 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include "activity/launcher.hpp"

#include "activity/validator/activity.hpp"
#include "activity/validator/base.hpp"

#include <activity/builder/data.hpp>

#include <core/runtime/runtime.hpp>

#include <data/map.hpp>
#include <data/mt/locked_ptr.hpp>

#include <boost/range/iterator_range_core.hpp>

namespace sight::activity
{

//------------------------------------------------------------------------------

void launcher::parse_configuration(const configuration_t& _config, const in_out_map_t& _inouts)
{
    m_main_activity_id = _config.get<std::string>("mainActivity.<xmlattr>.id", "");
    SIGHT_DEBUG_IF("main activity 'id' is not defined", m_main_activity_id.empty());

    if(const auto inouts_cfg = _config.get_child_optional("inout"); inouts_cfg.has_value())
    {
        const auto group = inouts_cfg->get<std::string>("<xmlattr>.group");
        if(group == "data")
        {
            std::size_t i = 0;
            for(const auto& it_cfg : boost::make_iterator_range(inouts_cfg->equal_range("key")))
            {
                const auto key = it_cfg.second.get<std::string>("<xmlattr>.name");
                SIGHT_ASSERT("Missing 'name' tag.", !key.empty());

                const auto uid = it_cfg.second.get<std::string>("<xmlattr>.uid");
                SIGHT_ASSERT("Missing 'uid' tag.", !uid.empty());

                const bool optional = it_cfg.second.get<bool>("<xmlattr>.optional", false);
                const auto& obj_id  = _inouts[i];
                parameter_t param;
                param.replace = key;
                if(optional)
                {
                    param.by = uid;
                }
                else
                {
                    param.by = obj_id;
                    ++i;
                }

                m_parameters.push_back(param);
            }
        }
    }

    configuration_t config_params = _config.get_child("parameters");

    const auto params_cfg = config_params.equal_range("parameter");
    for(auto it_params = params_cfg.first ; it_params != params_cfg.second ; ++it_params)
    {
        const auto replace = it_params->second.get<std::string>("<xmlattr>.replace");
        std::string by     = it_params->second.get<std::string>("<xmlattr>.by", "");
        if(by.empty())
        {
            by = it_params->second.get<std::string>("<xmlattr>.uid");
        }

        SIGHT_ASSERT(
            "'parameter' tag must contain valid 'replace' and 'by' attributes.",
            !replace.empty() && !by.empty()
        );
        parameter_t param;
        param.replace = replace;
        param.by      = by;
        SIGHT_ASSERT("'camp' paths are not managed in the configuration parameters", !param.is_object_path());
        m_parameters.push_back(param);
    }

    for(const auto& it_cfg : boost::make_iterator_range(config_params.equal_range("param")))
    {
        parameter_t param;
        const auto name = it_cfg.second.get<std::string>("<xmlattr>.name", "");
        SIGHT_ASSERT("Missing 'name' attribute in <param>.", !name.empty());

        const auto value = it_cfg.second.get_optional<std::string>("<xmlattr>.value");
        SIGHT_ASSERT("Missing 'value' attribute  in <param>.", value.has_value());

        param.replace = name;
        param.by      = *value;
        m_parameters.push_back(param);
    }

    for(const auto& it_cfg : boost::make_iterator_range(config_params.equal_range("channel")))
    {
        parameter_t param;

        const auto name = it_cfg.second.get<std::string>("<xmlattr>.name", "");
        SIGHT_ASSERT("Missing 'attribute' tag in <param>.", !name.empty());

        const auto uid = it_cfg.second.get<std::string>("<xmlattr>.uid", "");
        SIGHT_ASSERT("Missing 'uid' attribute  in <param>.", !uid.empty());

        param.replace = name;
        param.by      = uid;
        m_parameters.push_back(param);
    }
}

//------------------------------------------------------------------------------

std::pair<bool, std::string> launcher::validate_activity(
    const data::activity::csptr& _activity
)
{
    bool is_valid = true;
    std::string message;
    // Applies validator on activity to check the data
    activity::extension::activity_info info;
    info = activity::extension::activity::get_default()->get_info(_activity->get_activity_config_id());

    // load activity module
    core::runtime::start_module(info.bundle_id);

    for(const std::string& validator_impl : info.validators_impl)
    {
        /// Process activity validator
        auto validator          = activity::validator::factory::make(validator_impl);
        auto activity_validator = std::dynamic_pointer_cast<sight::activity::validator::activity>(validator);
        SIGHT_ASSERT("Validator '" + validator_impl + "' instantiation failed", activity_validator);

        auto validation = activity_validator->validate(_activity);
        if(!validation.first)
        {
            message += "\n" + validation.second;
            is_valid = false;
        }
    }

    if(!is_valid)
    {
        message = std::string("The activity '") + info.title + "' can not be launched:\n" + message;
    }

    return std::make_pair(is_valid, message);
}

//------------------------------------------------------------------------------

data::activity::sptr launcher::create_main_activity() const
{
    activity::extension::activity_info info;
    info = activity::extension::activity::get_default()->get_info(m_main_activity_id);

    auto activity = std::make_shared<data::activity>();
    if(!info.requirements.empty())
    {
        for(const auto& req : info.requirements)
        {
            if((req.min_occurs == 0 && req.max_occurs == 0) || req.create)
            {
                (*activity)[req.name] = sight::activity::detail::data::create(req.type, req.object_config);
            }
            else
            {
                return nullptr;
            }
        }
    }

    activity->set_activity_config_id(info.id);

    return activity;
}

} // namespace sight::activity
