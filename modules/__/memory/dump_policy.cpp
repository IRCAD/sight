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

#include "dump_policy.hpp"

#include <core/base.hpp>
#include <core/memory/buffer_manager.hpp>
#include <core/memory/policy/base.hpp>

#include <service/macros.hpp>

#include <vector>

namespace sight::module::memory
{

dump_policy::dump_policy()
= default;

dump_policy::~dump_policy()
= default;

//------------------------------------------------------------------------------

void dump_policy::configuring()
{
    const auto& config = this->get_config();

    m_policy = config.get<std::string>("config.policy", m_policy);

    if(const auto& params = config.get_child_optional("config.params"); params.has_value())
    {
        for(const auto& iter : params.value())
        {
            m_policy_params.emplace_back(iter.first, iter.second.get_value<std::string>());
        }
    }
}

//------------------------------------------------------------------------------

void dump_policy::starting()
{
    core::memory::buffer_manager::sptr manager = core::memory::buffer_manager::get();
    if(manager)
    {
        auto policy = core::memory::policy::registry::get()->create(m_policy);

        if(policy)
        {
            for(const auto& param : m_policy_params)
            {
                const bool success = policy->set_param(param.first, param.second);
                SIGHT_ERROR_IF(
                    "[" << m_policy << "] Unable to set '"
                    << param.first << "' parameter to " << param.second,
                    !success
                );
                SIGHT_INFO_IF("Set '" << param.first << "' policy parameter to " << param.second, success);
            }

            core::mt::write_lock lock(manager->get_mutex());
            manager->set_dump_policy(policy);
            SIGHT_INFO("Set dump policy to : " << m_policy);
        }

        SIGHT_ERROR_IF("Unable to instantiate '" << m_policy << "' dump policy", !policy);
    }
}

//------------------------------------------------------------------------------

void dump_policy::stopping()
{
}

//------------------------------------------------------------------------------

void dump_policy::updating()
{
}

//------------------------------------------------------------------------------

} // namespace sight::module::memory
