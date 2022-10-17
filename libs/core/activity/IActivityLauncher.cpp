/************************************************************************
 *
 * Copyright (C) 2019-2022 IRCAD France
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

#include "activity/IActivityLauncher.hpp"

#include "activity/IActivityValidator.hpp"
#include "activity/IValidator.hpp"

#include <core/runtime/runtime.hpp>
#include <core/tools/dateAndTime.hpp>
#include <core/tools/UUID.hpp>

#include <data/Composite.hpp>
#include <data/mt/locked_ptr.hpp>
#include <data/String.hpp>

namespace sight::activity
{

//-----------------------------------------------------------------------------

IActivityLauncher::IActivityLauncher()
= default;

//-----------------------------------------------------------------------------

IActivityLauncher::~IActivityLauncher()
= default;

//------------------------------------------------------------------------------

void IActivityLauncher::parseConfiguration(const ConfigurationType& config, const InOutMapType& inouts)
{
    m_mainActivityId = config.get<std::string>("mainActivity.<xmlattr>.id", "");
    SIGHT_DEBUG_IF("main activity 'id' is not defined", m_mainActivityId.empty());

    const auto inoutsCfg = config.equal_range("inout");
    for(auto itCfg = inoutsCfg.first ; itCfg != inoutsCfg.second ; ++itCfg)
    {
        const auto key = itCfg->second.get<std::string>("<xmlattr>.key");
        SIGHT_ASSERT("Missing 'key' tag.", !key.empty());

        const auto uid = itCfg->second.get<std::string>("<xmlattr>.uid");
        SIGHT_ASSERT("Missing 'uid' tag.", !uid.empty());

        const bool optional = itCfg->second.get<bool>("<xmlattr>.optional", false);
        const auto it       = inouts.find({key, std::nullopt});
        SIGHT_ASSERT("Inout '" + key + "' is not found.", it != inouts.end());
        auto obj = it->second.lock();
        ParameterType param;
        param.replace = key;
        if(optional)
        {
            param.by = uid;
        }
        else
        {
            SIGHT_ASSERT(std::string("Object key '") + key + "'with uid '" + uid + "' does not exists.", obj);
            param.by = obj->getID();
        }

        m_parameters.push_back(param);
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

std::pair<bool, std::string> IActivityLauncher::validateActivity(
    const data::Activity::csptr& activity
)
{
    bool isValid = true;
    std::string message;
    // Applies validator on activity to check the data
    activity::extension::ActivityInfo info;
    info = activity::extension::Activity::getDefault()->getInfo(activity->getActivityConfigId());

    // load activity module
    core::runtime::startModule(info.bundleId);

    for(const std::string& validatorImpl : info.validatorsImpl)
    {
        /// Process activity validator
        activity::IValidator::sptr validator = activity::validator::factory::New(validatorImpl);

        activity::IActivityValidator::sptr activityValidator =
            activity::IActivityValidator::dynamicCast(validator);
        SIGHT_ASSERT("Validator '" + validatorImpl + "' instantiation failed", activityValidator);

        activity::IValidator::ValidationType validation = activityValidator->validate(activity);
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

data::Activity::sptr IActivityLauncher::createMainActivity() const
{
    activity::extension::ActivityInfo info;
    info = activity::extension::Activity::getDefault()->getInfo(m_mainActivityId);

    auto activity = data::Activity::New();
    if(!info.requirements.empty())
    {
        data::Composite::sptr data = activity->getData();
        for(const auto& req : info.requirements)
        {
            if((req.minOccurs == 0 && req.maxOccurs == 0) || req.create)
            {
                (*data)[req.name] = data::factory::New(req.type);
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
