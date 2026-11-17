/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "app/extension/parameters.hpp"

#include <core/runtime/profile.hpp>
#include <core/runtime/runtime.hpp>

#include <algorithm>
#include <cctype>
#include <iterator>
#include <optional>
#include <ranges>
#include <utility>
#include <vector>

namespace sight::app::extension
{

namespace
{

struct cli_match
{
    std::string field;
    std::optional<std::string> value;
};

} // namespace
//-----------------------------------------------------------------------------

static std::vector<std::string> split_option_names(const std::string& _option)
{
    std::vector<std::string> names;
    std::size_t begin = 0;

    while(begin <= _option.size())
    {
        const std::size_t end = _option.find(',', begin);
        std::string name      = _option.substr(begin, end == std::string::npos ? end : end - begin);

        name.erase(
            name.begin(),
            std::ranges::find_if(name, [](unsigned char _character){return !std::isspace(_character);})
        );
        name.erase(
            std::ranges::find_if(
                std::views::reverse(name),
                [](unsigned char _character){return !std::isspace(_character);}).base(),
            name.end()
        );
        name.erase(0, name.find_first_not_of('-'));

        if(!name.empty())
        {
            names.push_back(std::move(name));
        }

        if(end == std::string::npos)
        {
            break;
        }

        begin = end + 1;
    }

    return names;
}

//-----------------------------------------------------------------------------

static bool is_option_token(const std::string& _token)
{
    return _token.size() > 1 && _token[0] == '-' && (_token[1] == '-' || std::isdigit(
                                                         static_cast<unsigned char>(_token[1])
                                                     ) == 0);
}

//-----------------------------------------------------------------------------

static std::optional<cli_match> find_cli_match(
    const std::string& _argument,
    const field_adaptor_t& _cli_parameters
)
{
    for(const auto& [field, option] : _cli_parameters)
    {
        for(const auto& name : split_option_names(option))
        {
            const std::string long_name  = "--" + name;
            const std::string short_name = "-" + name;

            if(_argument == long_name || _argument == short_name)
            {
                return cli_match {.field = field, .value = std::nullopt};
            }

            if(_argument.starts_with(long_name + "="))
            {
                return cli_match {.field = field, .value = _argument.substr(long_name.size() + 1)};
            }

            if(_argument.starts_with(short_name + "="))
            {
                return cli_match {.field = field, .value = _argument.substr(short_name.size() + 1)};
            }
        }
    }

    return std::nullopt;
}

//-----------------------------------------------------------------------------

static parameters::cli_values_t parse_cli_parameters(
    const field_adaptor_t& _cli_parameters,
    const core::runtime::profile::params_container& _args
)
{
    parameters::cli_values_t values;

    for(std::size_t i = 0 ; i < _args.size() ; ++i)
    {
        if(!is_option_token(_args[i]))
        {
            continue;
        }

        const auto match = find_cli_match(_args[i], _cli_parameters);
        if(!match)
        {
            if(_args[i] != "--")
            {
                SIGHT_THROW("Unknown application command line option: " << _args[i]);
            }

            continue;
        }

        auto& field_values = values[match->field];
        if(match->value)
        {
            field_values.push_back(*match->value);
        }
        else
        {
            while(i + 1 < _args.size() && !is_option_token(_args[i + 1]))
            {
                field_values.push_back(_args[++i]);
            }

            if(field_values.empty())
            {
                field_values.emplace_back();
            }
        }
    }

    return values;
}

parameters::sptr parameters::s_app_config_parameters = std::make_shared<parameters>();

//-----------------------------------------------------------------------------

parameters::sptr parameters::get_default()
{
    return s_app_config_parameters;
}

//-----------------------------------------------------------------------------

void parameters::parse_plugin_infos()
{
    auto extensions = core::runtime::get_all_extensions_for_point("sight::app::extension::parameters");

    for(const std::shared_ptr<core::runtime::extension>& ext : extensions)
    {
        const auto& config = ext->get_config();

        const auto extension_id = config.get<std::string>("id");

        field_adaptor_t parameters;
        field_adaptor_t cli_parameters;

        if(const auto parameters_cfg = config.get_child_optional("parameters"); parameters_cfg.has_value())
        {
            for(const auto& param : boost::make_iterator_range(parameters_cfg->equal_range("param")))
            {
                const auto name  = param.second.get<std::string>("<xmlattr>.name");
                const auto value = param.second.get<std::string>("<xmlattr>.value");

                parameters[name] = value;
            }
        }

        if(const auto cli_cfg = config.get_child_optional("cli"); cli_cfg.has_value())
        {
            for(const auto& param : boost::make_iterator_range(cli_cfg->equal_range("param")))
            {
                const auto name  = param.second.get<std::string>("<xmlattr>.name");
                const auto value = param.second.get<std::string>("<xmlattr>.option");

                cli_parameters[name] = value;
            }
        }

        core::mt::write_lock lock(m_registry_mutex);
#ifdef _DEBUG
        auto iter     = m_reg.find(extension_id);
        auto cli_iter = m_cli_reg.find(extension_id);
#endif
        SIGHT_ASSERT(
            "The id " << extension_id
            << " already exists in the application configuration parameter registry",
            iter == m_reg.end()
        );
        SIGHT_ASSERT(
            "The id " << extension_id
            << " already exists in the application CLI parameter registry",
            cli_iter == m_cli_reg.end()
        );
        m_reg[extension_id]     = parameters;
        m_cli_reg[extension_id] = cli_parameters;
    }
}

//-----------------------------------------------------------------------------

void parameters::clear_registry()
{
    core::mt::write_lock lock(m_registry_mutex);
    m_reg.clear();
    m_cli_reg.clear();
}

//-----------------------------------------------------------------------------

field_adaptor_t parameters::get_parameters(const std::string& _extension_id) const
{
    core::mt::read_lock lock(m_registry_mutex);
    const auto id = core::runtime::filter_id(_extension_id);
    auto iter     = m_reg.find(id);
    SIGHT_ASSERT(
        "The id " << _extension_id << " is not found in the application configuration parameter registry",
        iter != m_reg.end()
    );

    field_adaptor_t fields = iter->second;
    if(const auto profile = core::runtime::get_current_profile(); profile != nullptr)
    {
        const auto cli_iter = m_cli_reg.find(id);
        if(cli_iter != m_cli_reg.end())
        {
            const auto args       = profile->get_params();
            const auto cli_values = parse_cli_parameters(cli_iter->second, args);

            for(const auto& [field, values] : cli_values)
            {
                if(!values.empty())
                {
                    std::string value = values.front();
                    for(auto value_it = std::next(values.begin()) ; value_it != values.end() ; ++value_it)
                    {
                        value += ';';
                        value += *value_it;
                    }

                    fields[field] = std::move(value);
                }
            }
        }
    }

    return fields;
}

//-----------------------------------------------------------------------------

const field_adaptor_t& parameters::get_cli_parameters(const std::string& _extension_id) const
{
    core::mt::read_lock lock(m_registry_mutex);
    auto iter = m_cli_reg.find(core::runtime::filter_id(_extension_id));
    SIGHT_ASSERT(
        "The id " << _extension_id << " is not found in the application CLI parameter registry",
        iter != m_cli_reg.end()
    );
    return iter->second;
}

//-----------------------------------------------------------------------------

parameters::cli_values_t parameters::get_cli_values(const std::string& _extension_id) const
{
    core::mt::read_lock lock(m_registry_mutex);
    auto iter = m_cli_reg.find(core::runtime::filter_id(_extension_id));
    SIGHT_ASSERT(
        "The id " << _extension_id << " is not found in the application CLI parameter registry",
        iter != m_cli_reg.end()
    );

    cli_values_t values;
    if(const auto profile = core::runtime::get_current_profile(); profile != nullptr)
    {
        const auto args = profile->get_params();
        values = parse_cli_parameters(iter->second, args);
    }

    return values;
}

//-----------------------------------------------------------------------------

} // namespace sight::app::extension
