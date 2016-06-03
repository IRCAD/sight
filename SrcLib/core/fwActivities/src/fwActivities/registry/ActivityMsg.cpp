/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2015-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwActivities/registry/ActivityMsg.hpp"

#include <fwData/Composite.hpp>
#include <fwData/Object.hpp>
#include <fwData/String.hpp>

#include <fwDataCamp/getObject.hpp>

#include <fwTools/UUID.hpp>

#include <boost/regex.hpp>

namespace fwActivities
{
namespace registry
{

//-----------------------------------------------------------------------------

ActivityMsg::ActivityMsg(const ::fwMedData::ActivitySeries::sptr &series,
                         const ::fwActivities::registry::ActivityInfo & info,
                         const ParametersType& parameters)
{
    SLM_ASSERT("ActivitySeries instantiation failed", series);

    const std::string asUID = "AS_UID";

    m_title       = info.title;
    m_appConfigID = info.appConfig.id;
    m_closable    = true;
    m_tabID       = "TABID_" + ::fwTools::UUID::generateUUID();

    if(info.tabInfo.empty())
    {
        m_tabInfo = info.title;
    }
    else
    {
        std::string newTabInfo = info.tabInfo;
        ::boost::regex e("(!(([[:word:]]+\\.?)+[[:word:]]))");
        ::boost::smatch what;
        if(boost::regex_search(newTabInfo, what, e))
        {
            std::string submatch(what[1].first, what[1].second);

            submatch.replace(0, 1, "@");

            ::fwData::Object::sptr obj = ::fwDataCamp::getObject(series->getData(), submatch);
            OSLM_ASSERT("Invalid seshat path : '" << submatch <<"'", obj);

            ::fwData::String::sptr stringParameter = ::fwData::String::dynamicCast(obj);

            std::string tabInfoSeshat;

            if(stringParameter)
            {
                tabInfoSeshat = stringParameter->getValue();
            }
            else
            {
                OSLM_WARN("Seshat path '" << submatch << "' doesn't reference an fwData::String");
            }

            submatch.replace(0, 1, "!");
            ::boost::algorithm::replace_all(newTabInfo, submatch, tabInfoSeshat);

        }
        m_tabInfo = newTabInfo;
    }
    m_iconPath = info.icon;
    m_tooltip  = m_tabInfo;
    m_series   = series;

    namespace ActiReg = ::fwActivities::registry;
    ActiReg::ActivityAppConfig::ActivityAppConfigParamsType params = info.appConfig.parameters;
    params.reserve(params.size() + parameters.size() + 1);
    params.insert(params.end(), parameters.begin(), parameters.end());
    ActiReg::ActivityAppConfigParam asConfigParam;
    asConfigParam.replace = asUID;
    asConfigParam.by      = series->getID();
    params.push_back(asConfigParam);
    for(const ActiReg::ActivityAppConfigParam& param :  params)
    {
        if(!param.isSeshat())
        {
            m_replaceMap[param.replace] = param.by;
        }
        else
        {
            std::string parameterToReplace = param.by;
            if (parameterToReplace.substr(0,1) == "!")
            {
                parameterToReplace.replace(0, 1, "@");
            }

            ::fwData::Object::sptr obj = ::fwDataCamp::getObject(series->getData(), parameterToReplace);
            OSLM_ASSERT("Invalid seshat path : '"<<param.by<<"'", obj);

            ::fwData::String::sptr stringParameter = ::fwData::String::dynamicCast(obj);

            std::string parameterValue = obj->getID();

            if(stringParameter && param.by.substr(0,1) == "!")
            {
                parameterValue = stringParameter->getValue();
            }
            m_replaceMap[param.replace] = parameterValue;
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace registry

} // namespace fwActivities
