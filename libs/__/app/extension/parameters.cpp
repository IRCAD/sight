/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <core/runtime/helper.hpp>
#include <core/runtime/runtime.hpp>

#include <data/composite.hpp>
#include <data/string.hpp>

#include <regex>

namespace sight::app::extension
{

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

        if(const auto parameters_cfg = config.get_child_optional("parameters"); parameters_cfg.has_value())
        {
            for(const auto& param : boost::make_iterator_range(parameters_cfg->equal_range("param")))
            {
                const auto name  = param.second.get<std::string>("<xmlattr>.name");
                const auto value = param.second.get<std::string>("<xmlattr>.value");
                parameters[name] = value;
            }
        }

        core::mt::write_lock lock(m_registry_mutex);
#ifdef _DEBUG
        auto iter = m_reg.find(extension_id);
#endif
        SIGHT_ASSERT(
            "The id " << extension_id
            << " already exists in the application configuration parameter registry",
            iter == m_reg.end()
        );
        m_reg[extension_id] = parameters;
    }
}

//-----------------------------------------------------------------------------

void parameters::clear_registry()
{
    core::mt::write_lock lock(m_registry_mutex);
    m_reg.clear();
}

//-----------------------------------------------------------------------------

const field_adaptor_t& parameters::get_parameters(const std::string& _extension_id) const
{
    core::mt::read_lock lock(m_registry_mutex);
    auto iter = m_reg.find(core::runtime::filter_id(_extension_id));
    SIGHT_ASSERT(
        "The id " << _extension_id << " is not found in the application configuration parameter registry",
        iter != m_reg.end()
    );
    return iter->second;
}

//-----------------------------------------------------------------------------

} // namespace sight::app::extension
