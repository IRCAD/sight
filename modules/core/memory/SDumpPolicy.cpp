/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "SDumpPolicy.hpp"

#include <core/base.hpp>
#include <core/memory/BufferManager.hpp>
#include <core/memory/IPolicy.hpp>

#include <service/macros.hpp>

#include <vector>

namespace sight::module::memory
{

SDumpPolicy::SDumpPolicy()
= default;

SDumpPolicy::~SDumpPolicy()
= default;

//------------------------------------------------------------------------------

void SDumpPolicy::configuring()
{
    const auto& config = this->getConfiguration();

    m_policy = config.get<std::string>("config.policy", m_policy);

    if(const auto& params = config.get_child_optional("config.params"); params.has_value())
    {
        for(const auto& iter : params.value())
        {
            m_policyParams.push_back(ParametersType::value_type(iter.first, iter.second.get_value<std::string>()));
        }
    }
}

//------------------------------------------------------------------------------

void SDumpPolicy::starting()
{
    core::memory::BufferManager::sptr manager = core::memory::BufferManager::getDefault();
    if(manager)
    {
        core::memory::IPolicy::sptr policy = core::memory::policy::registry::get()->create(m_policy);

        if(policy)
        {
            for(const ParametersType::value_type& param : m_policyParams)
            {
                const bool success = policy->setParam(param.first, param.second);
                SIGHT_ERROR_IF(
                    "[" << m_policy << "] Unable to set '"
                    << param.first << "' parameter to " << param.second,
                    !success
                );
                SIGHT_INFO_IF("Set '" << param.first << "' policy parameter to " << param.second, success);
            }

            core::mt::WriteLock lock(manager->getMutex());
            manager->setDumpPolicy(policy);
            SIGHT_INFO("Set dump policy to : " << m_policy);
        }

        SIGHT_ERROR_IF("Unable to instantiate '" << m_policy << "' dump policy", !policy);
    }
}

//------------------------------------------------------------------------------

void SDumpPolicy::stopping()
{
}

//------------------------------------------------------------------------------

void SDumpPolicy::updating()
{
}

//------------------------------------------------------------------------------

} // namespace sight::module::memory
