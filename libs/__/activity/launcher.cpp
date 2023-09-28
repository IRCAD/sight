/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
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

#include <data/Composite.hpp>
#include <data/mt/locked_ptr.hpp>

#include <boost/range/iterator_range_core.hpp>

namespace sight::activity
{

//------------------------------------------------------------------------------

void launcher::parseConfiguration(const ConfigurationType& config, const InOutMapType& inouts)
{
    m_mainActivityId = config.get<std::string>("mainActivity.<xmlattr>.id", "");
    SIGHT_DEBUG_IF("main activity 'id' is not defined", m_mainActivityId.empty());

    if(const auto inoutsCfg = config.get_child_optional("inout"); inoutsCfg.has_value())
    {
        const auto group = inoutsCfg->get<std::string>("<xmlattr>.group");
        if(group == "data")
        {
            std::size_t i = 0;
            for(const auto& itCfg : boost::make_iterator_range(inoutsCfg->equal_range("key")))
            {
                const auto key = itCfg.second.get<std::string>("<xmlattr>.name");
                SIGHT_ASSERT("Missing 'name' tag.", !key.empty());

                const auto uid = itCfg.second.get<std::string>("<xmlattr>.uid");
                SIGHT_ASSERT("Missing 'uid' tag.", !uid.empty());

                const bool optional = itCfg.second.get<bool>("<xmlattr>.optional", false);
                const auto& objId   = inouts[i++];
                ParameterType param;
                param.replace = key;
                if(optional)
                {
                    param.by = uid;
                }
                else
                {
                    param.by = objId;
                }

                m_parameters.push_back(param);
            }
        }
    }

    ConfigurationType configParams = config.get_child("parameters");

    const auto paramsCfg = configParams.equal_range("parameter");
    for(auto itParams = paramsCfg.first ; itParams != paramsCfg.second ; ++itParams)
    {
        const auto replace = itParams->second.get<std::string>("<xmlattr>.replace");
        std::string by     = itParams->second.get<std::string>("<xmlattr>.by", "");
        if(by.empty())
        {
            by = itParams->second.get<std::string>("<xmlattr>.uid");
        }

        SIGHT_ASSERT(
            "'parameter' tag must contain valid 'replace' and 'by' attributes.",
            !replace.empty() && !by.empty()
        );
        ParameterType param;
        param.replace = replace;
        param.by      = by;
        SIGHT_ASSERT("'camp' paths are not managed in the configuration parameters", !param.isObjectPath());
        m_parameters.push_back(param);
    }
}

//------------------------------------------------------------------------------

std::pair<bool, std::string> launcher::validateActivity(
    const data::Activity::csptr& activity
)
{
    bool isValid = true;
    std::string message;
    // Applies validator on activity to check the data
    activity::extension::ActivityInfo info;
    info = activity::extension::Activity::getDefault()->getInfo(activity->getActivityConfigId());

    // load activity module
    core::runtime::start_module(info.bundleId);

    for(const std::string& validatorImpl : info.validatorsImpl)
    {
        /// Process activity validator
        auto validator         = activity::validator::factory::make(validatorImpl);
        auto activityValidator = std::dynamic_pointer_cast<sight::activity::validator::activity>(validator);
        SIGHT_ASSERT("Validator '" + validatorImpl + "' instantiation failed", activityValidator);

        auto validation = activityValidator->validate(activity);
        if(!validation.first)
        {
            message += "\n" + validation.second;
            isValid  = false;
        }
    }

    if(!isValid)
    {
        message = std::string("The activity '") + info.title + "' can not be launched:\n" + message;
    }

    return std::make_pair(isValid, message);
}

//------------------------------------------------------------------------------

data::Activity::sptr launcher::createMainActivity() const
{
    activity::extension::ActivityInfo info;
    info = activity::extension::Activity::getDefault()->getInfo(m_mainActivityId);

    auto activity = std::make_shared<data::Activity>();
    if(!info.requirements.empty())
    {
        for(const auto& req : info.requirements)
        {
            if((req.minOccurs == 0 && req.maxOccurs == 0) || req.create)
            {
                (*activity)[req.name] = sight::activity::detail::data::create(req.type, req.objectConfig);
            }
            else
            {
                return nullptr;
            }
        }
    }

    activity->setActivityConfigId(info.id);

    return activity;
}

} // namespace sight::activity
