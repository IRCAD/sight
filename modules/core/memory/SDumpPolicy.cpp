/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include <services/macros.hpp>

#include <vector>

namespace sight::modules::memory
{

fwServicesRegisterMacro( ::sight::services::IController, sight::modules::memory::SDumpPolicy, ::sight::data::Object )

SDumpPolicy::SDumpPolicy()
{
}

SDumpPolicy::~SDumpPolicy()
{
}

//------------------------------------------------------------------------------

void SDumpPolicy::configuring()
{
    typedef core::runtime::ConfigurationElement::sptr ConfigurationType;
    std::vector < ConfigurationType > config = m_configuration->find("config");

    if (!config.empty())
    {
        std::vector < ConfigurationType > policy        = config.at(0)->find("policy");
        std::vector < ConfigurationType > paramsElement = config.at(0)->find("params");

        m_policyParams.clear();
        m_policy = "";

        if (!policy.empty())
        {
            m_policy = policy.at(0)->getValue();
        }

        if(!paramsElement.empty())
        {
            const ConfigurationType& params = paramsElement.at(0);

            core::runtime::ConfigurationElement::Container::const_iterator iter;

            for (iter = params->begin(); iter != params->end(); ++iter)
            {
                m_policyParams.push_back( ParametersType::value_type((*iter)->getName(), (*iter)->getValue()));
            }
        }
    }
}

//------------------------------------------------------------------------------

void SDumpPolicy::starting()
{
    core::memory::BufferManager::sptr manager = core::memory::BufferManager::getDefault();
    if (manager)
    {
        core::memory::IPolicy::sptr policy = core::memory::policy::registry::get()->create(m_policy);

        if (policy)
        {
            bool success;
            for(const ParametersType::value_type& param :  m_policyParams)
            {
                success = policy->setParam(param.first, param.second);
                SLM_ERROR_IF( "[" << m_policy << "] Unable to set '"
                                  << param.first << "' parameter to " << param.second,
                              !success);
                SLM_INFO_IF("Set '" << param.first << "' policy parameter to " << param.second, success);
            }
            core::mt::WriteLock lock( manager->getMutex() );
            manager->setDumpPolicy(policy);
            SLM_INFO("Set dump policy to : " << m_policy);
        }
        SLM_ERROR_IF("Unable to instantiate '" << m_policy << "' dump policy", !policy);
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

void SDumpPolicy::swapping()
{
    this->stopping();
    this->starting();
}

} // namespace sight::modules::memory
