/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

parameters::sptr parameters::s_appConfigParameters = std::make_shared<parameters>();

//-----------------------------------------------------------------------------

parameters::sptr parameters::getDefault()
{
    return s_appConfigParameters;
}

//-----------------------------------------------------------------------------

void parameters::parse_plugin_infos()
{
    auto extensions = core::runtime::get_all_extensions_for_point("sight::app::extension::parameters");

    for(const std::shared_ptr<core::runtime::extension>& ext : extensions)
    {
        const auto& config = ext->get_config();

        const auto extensionId = config.get<std::string>("id");

        field_adaptor_t parameters;

        if(const auto parametersCfg = config.get_child_optional("parameters"); parametersCfg.has_value())
        {
            for(const auto& param : boost::make_iterator_range(parametersCfg->equal_range("param")))
            {
                const auto name  = param.second.get<std::string>("<xmlattr>.name");
                const auto value = param.second.get<std::string>("<xmlattr>.value");
                parameters[name] = value;
            }
        }

        core::mt::write_lock lock(m_registryMutex);
#ifdef _DEBUG
        auto iter = m_reg.find(extensionId);
#endif
        SIGHT_ASSERT(
            "The id " << extensionId
            << " already exists in the application configuration parameter registry",
            iter == m_reg.end()
        );
        m_reg[extensionId] = parameters;
    }
}

//-----------------------------------------------------------------------------

parameters::parameters()
= default;

//-----------------------------------------------------------------------------

void parameters::clear_registry()
{
    core::mt::write_lock lock(m_registryMutex);
    m_reg.clear();
}

//-----------------------------------------------------------------------------

const field_adaptor_t& parameters::getParameters(const std::string& extensionId) const
{
    core::mt::read_lock lock(m_registryMutex);
    auto iter = m_reg.find(core::runtime::filter_id(extensionId));
    SIGHT_ASSERT(
        "The id " << extensionId << " is not found in the application configuration parameter registry",
        iter != m_reg.end()
    );
    return iter->second;
}

//-----------------------------------------------------------------------------

} // namespace sight::app::extension
