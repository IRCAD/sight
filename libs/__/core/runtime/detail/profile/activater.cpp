/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "core/runtime/detail/profile/activater.hpp"

#include "core/runtime/detail/extension_point.hpp"
#include "core/runtime/detail/module.hpp"
#include "core/runtime/extension.hpp"

#include <boost/algorithm/string/replace.hpp>

#include <utility>

namespace sight::core::runtime::detail::profile
{

//------------------------------------------------------------------------------

activater::activater(std::string _identifier, const std::string& /*unused*/) :
    m_identifier(std::move(_identifier))
{
}

//------------------------------------------------------------------------------

void activater::add_parameter(const std::string& _identifier, const std::string& _value)
{
    m_parameters[_identifier] = _value;
}

//------------------------------------------------------------------------------

void activater::add_disable_extension_point(const std::string& _identifier)
{
    m_disable_extension_points.push_back(_identifier);
}

//------------------------------------------------------------------------------

void activater::add_disable_extension(const std::string& _identifier)
{
    m_disable_extensions.push_back(_identifier);
}

//------------------------------------------------------------------------------

void activater::apply()
{
    auto module = std::dynamic_pointer_cast<detail::module>(runtime::get().find_module(m_identifier));

    // TEMP_FB: until I refactor the profile.xml
    if(module == nullptr)
    {
        const auto identifier = boost::algorithm::replace_first_copy(m_identifier, "sight_", "");
        module = std::dynamic_pointer_cast<detail::module>(runtime::get().find_module(identifier));
        SIGHT_FATAL_IF("Unable to activate Module " + identifier + ". Not found.", module == nullptr);
    }

    SIGHT_FATAL_IF("Unable to activate Module " + m_identifier + ". Not found.", module == nullptr);
    // TEMP_FB: useless now since all modules are now enabled by default
    module->set_enable(true);

    // Management of parameter configuration
    for(auto& m_parameter : m_parameters)
    {
        module->add_parameter(m_parameter.first, m_parameter.second);
    }

    // Disable extension point for this module
    for(auto& m_disable_extension_point : m_disable_extension_points)
    {
        if(module->has_extension_point(m_disable_extension_point))
        {
            module->set_enable_extension_point(m_disable_extension_point, false);
        }
        else
        {
            SIGHT_ERROR(
                "Unable to disable Extension Point " << m_disable_extension_point << " defined in the Module " << m_identifier
                << ". Not found."
            );
        }
    }

    // Disable extension for this module
    for(auto& m_disable_extension : m_disable_extensions)
    {
        if(module->has_extension(m_disable_extension))
        {
            module->set_enable_extension(m_disable_extension, false);
        }
        else
        {
            SIGHT_ERROR(
                "Unable to disable Extension " << m_disable_extension << " defined in the Module " << m_identifier
                << ". Not found."
            );
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::core::runtime::detail::profile
