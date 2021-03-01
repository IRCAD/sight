/************************************************************************
 *
 * Copyright (C) 2019-2021 IRCAD France
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

#include <core/runtime/Module.hpp>
#include <core/runtime/operations.hpp>
#include <core/tools/dateAndTime.hpp>
#include <core/tools/UUID.hpp>

#include <data/Composite.hpp>
#include <data/mt/locked_ptr.hpp>
#include <data/reflection/getObject.hpp>
#include <data/String.hpp>

namespace sight::activity
{

//-----------------------------------------------------------------------------

IActivityLauncher::IActivityLauncher()
{
}

//-----------------------------------------------------------------------------

IActivityLauncher::~IActivityLauncher()
{
}

//------------------------------------------------------------------------------

void IActivityLauncher::parseConfiguration(const ConfigurationType& config, const InOutMapType& inouts)
{
    m_mainActivityId = config.get<std::string>("mainActivity.<xmlattr>.id", "");
    SIGHT_DEBUG_IF("main activity 'id' is not defined", m_mainActivityId.empty());

    const auto inoutsCfg = config.equal_range("inout");
    for (auto itCfg = inoutsCfg.first; itCfg != inoutsCfg.second; ++itCfg)
    {
        const std::string key = itCfg->second.get<std::string>("<xmlattr>.key");
        SIGHT_ASSERT("Missing 'key' tag.", !key.empty());

        const std::string uid = itCfg->second.get<std::string>("<xmlattr>.uid");
        SIGHT_ASSERT("Missing 'uid' tag.", !uid.empty());

        const std::string strOptional = itCfg->second.get<std::string>("<xmlattr>.optional", "no");
        const bool optional           = (strOptional == "yes");

        const auto it = inouts.find(key);
        SIGHT_ASSERT("Inout '" + key + "' is not found.", it != inouts.end());
        data::Object::csptr obj = it->second.getShared();
        ParameterType param;
        param.replace = key;
        if(optional)
        {
            param.by = uid;
        }
        else
        {
            SIGHT_ASSERT("Object key '" + key + "'with uid '" + uid + "' does not exists.", obj);
            param.by = obj->getID();
        }
        m_parameters.push_back(param);
    }

    ConfigurationType configParams = config.get_child("parameters");

    const auto paramsCfg = configParams.equal_range("parameter");
    for (auto itParams = paramsCfg.first; itParams != paramsCfg.second; ++itParams)
    {
        const std::string replace = itParams->second.get<std::string>("<xmlattr>.replace");
        std::string by            = itParams->second.get<std::string>("<xmlattr>.by", "");
        if(by.empty())
        {
            by = itParams->second.get<std::string>("<xmlattr>.uid");
        }
        SIGHT_ASSERT("'parameter' tag must contain valid 'replace' and 'by' attributes.",
                     !replace.empty() && !by.empty());
        ParameterType param;
        param.replace = replace;
        param.by      = by;
        SIGHT_ASSERT("'camp' paths are not managed in the configuration parameters", !param.isSeshat());
        m_parameters.push_back(param);
    }
}

//------------------------------------------------------------------------------

std::pair<bool, std::string> IActivityLauncher::validateActivity(
    const data::ActivitySeries::csptr& activitySeries) const
{
    bool isValid = true;
    std::string message;
    // Applies validator on activity series to check the data
    activity::extension::ActivityInfo info;
    info = activity::extension::Activity::getDefault()->getInfo(activitySeries->getActivityConfigId());

    // load activity module
    std::shared_ptr< core::runtime::Module > module = core::runtime::findModule(info.bundleId);
    if (!module->isStarted())
    {
        module->start();
    }

    for (std::string validatorImpl : info.validatorsImpl)
    {
        /// Process activity validator
        activity::IValidator::sptr validator = activity::validator::factory::New(validatorImpl);

        activity::IActivityValidator::sptr activityValidator =
            activity::IActivityValidator::dynamicCast(validator);
        SIGHT_ASSERT("Validator '" + validatorImpl + "' instantiation failed", activityValidator);

        activity::IValidator::ValidationType validation = activityValidator->validate(activitySeries);
        if(!validation.first)
        {
            message += "\n" + validation.second;
            isValid  = false;
        }
    }
    if (!isValid)
    {
        message = "The activity '" + info.title + "' can not be launched:\n" + message;
    }

    return std::make_pair(isValid, message);
}

//------------------------------------------------------------------------------

data::ActivitySeries::sptr IActivityLauncher::createMainActivity() const
{
    activity::extension::ActivityInfo info;
    info = activity::extension::Activity::getDefault()->getInfo(m_mainActivityId);

    data::ActivitySeries::sptr actSeries = data::ActivitySeries::New();
    if (info.requirements.size() > 0)
    {
        data::Composite::sptr data = actSeries->getData();
        for (activity::extension::ActivityRequirement req : info.requirements)
        {
            if ((req.minOccurs == 0 && req.maxOccurs == 0) || req.create)
            {
                (*data)[req.name] = data::factory::New(req.type);
            }
            else
            {
                return nullptr;
            }
        }
    }

    actSeries->setModality("OT");
    actSeries->setInstanceUID("activity." + core::tools::UUID::generateUUID() );

    const ::boost::posix_time::ptime now = ::boost::posix_time::second_clock::local_time();
    actSeries->setDate(core::tools::getDate(now));
    actSeries->setTime(core::tools::getTime(now));
    actSeries->setActivityConfigId(info.id);

    return actSeries;
}

//------------------------------------------------------------------------------

void IActivityLauncher::translateParameters( const data::Object::csptr& sourceObj, const ParametersType& parameters,
                                             ReplaceMapType& replaceMap )
{
    for(const ParametersType::value_type& param :  parameters)
    {
        if(!param.isSeshat())
        {
            replaceMap[param.replace] = param.by;
        }
        else
        {
            std::string parameterToReplace = param.by;
            if (parameterToReplace.substr(0, 1) == "!")
            {
                parameterToReplace.replace(0, 1, "@");
            }

            data::Object::sptr obj = data::reflection::getObject(sourceObj, parameterToReplace);
            SIGHT_ASSERT("Invalid seshat path : '" + param.by + "'", obj);

            data::String::sptr stringParameter = data::String::dynamicCast(obj);

            std::string parameterValue = obj->getID();

            if(stringParameter && param.by.substr(0, 1) == "!")
            {
                parameterValue = stringParameter->getValue();
            }
            replaceMap[param.replace] = parameterValue;
        }
    }
}

//------------------------------------------------------------------------------

void IActivityLauncher::translateParameters( const ParametersType& parameters, ReplaceMapType& replaceMap )
{
    for(const ParametersType::value_type& param :  parameters)
    {
        replaceMap[param.replace] = param.by;
    }
}

} // namespace sight::activity
