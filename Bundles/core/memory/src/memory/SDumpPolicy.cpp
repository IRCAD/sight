/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "memory/SDumpPolicy.hpp"

#include <fwCore/base.hpp>

#include <fwMemory/BufferManager.hpp>
#include <fwMemory/IPolicy.hpp>

#include <fwServices/macros.hpp>

#include <vector>

namespace memory
{

fwServicesRegisterMacro( ::fwServices::IController, ::memory::SDumpPolicy, ::fwData::Object );

SDumpPolicy::SDumpPolicy()
{
}

SDumpPolicy::~SDumpPolicy()
{
}

//------------------------------------------------------------------------------

void SDumpPolicy::configuring()
{
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
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

            ::fwRuntime::ConfigurationElement::Container::const_iterator iter;

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
    ::fwMemory::BufferManager::sptr manager = ::fwMemory::BufferManager::getDefault();
    if (manager)
    {
        ::fwMemory::IPolicy::sptr policy = ::fwMemory::policy::registry::get()->create(m_policy);

        if (policy)
        {
            bool success;
            for(const ParametersType::value_type& param :  m_policyParams)
            {
                success = policy->setParam(param.first, param.second);
                OSLM_ERROR_IF( "[" << m_policy << "] Unable to set '"
                                   << param.first << "' parameter to " << param.second,
                               !success);
                OSLM_INFO_IF("Set '" << param.first << "' policy parameter to " << param.second, success);
            }
            ::fwCore::mt::WriteLock lock( manager->getMutex() );
            manager->setDumpPolicy(policy);
            OSLM_INFO("Set dump policy to : " << m_policy);
        }
        OSLM_ERROR_IF("Unable to instantiate '" << m_policy << "' dump policy", !policy);
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

} // namespace memory

