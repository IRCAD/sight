/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/UUID.hpp>

#include <fwData/Boolean.hpp>
#include <fwData/Composite.hpp>
#include <fwData/String.hpp>

#include <fwServices/registry/AppConfig.hpp>
#include <fwServices/ObjectMsg.hpp>

#include <fwAtomConversion/RetreiveObjectVisitor.hpp>

#include <fwActivities/ActivitySeries.hpp>
#include <fwActivities/registry/Activities.hpp>

#include "activities/helper/Activity.hpp"

namespace activities
{
namespace helper
{

//-----------------------------------------------------------------------------

::fwServices::ObjectMsg::sptr buildActivityMsg( ::fwActivities::ActivitySeries::sptr series,
        const ::fwActivities::registry::ActivityInfo & info,
        const ParametersType& parameters)
{
    namespace ActiReg = fwActivities::registry;

    ::fwServices::ObjectMsg::sptr activityMsg = ::fwServices::ObjectMsg::New();
    ::fwData::Composite::sptr replaceMap = ::fwData::Composite::New();
    SLM_ASSERT("ActivitySeries instantiation failed", series);

    const std::string eventID              = "NEW_CONFIGURATION_HELPER";
    const std::string fieldID              = "APPCONFIG";
    const std::string viewConfigFieldID    = "VIEWCONFIGID";
    const std::string closableFieldID      = "CLOSABLE";
    const std::string iconFieldID          = "ICON";
    const std::string tooltipFieldID       = "TOOLTIP";
    const std::string tabIDFieldID         = "TABID";
    const std::string asFieldID            = "ACTIVITYSERIES";
    const std::string asUID                = "AS_UID";
    const std::string genericUID           = "GENERIC_UID";
    const std::string tabID                = "TABID_" + ::fwTools::UUID::generateUUID();

    ::fwData::String::sptr title = ::fwData::String::New();

    title->value() = info.title;
    activityMsg->addEvent( eventID, title );
    title->setField( viewConfigFieldID, ::fwData::String::New(info.appConfig.id) );
    title->setField( closableFieldID, ::fwData::Boolean::New(true));
    title->setField( tabIDFieldID, ::fwData::String::New(tabID));
    title->setField( iconFieldID, ::fwData::String::New(info.icon) );
    title->setField( tooltipFieldID, ::fwData::String::New(info.title) );
    title->setField( asFieldID, series );
    title->setField( fieldID, replaceMap );


    std::string genericUidAdaptor = ::fwServices::registry::AppConfig::getUniqueIdentifier(info.id, true);
    (*replaceMap)[genericUID] = ::fwData::String::New(genericUidAdaptor);

    ActiReg::ActivityAppConfig::ActivityAppConfigParamsType params = info.appConfig.parameters;
    params.reserve(params.size() + parameters.size() + 1);
    params.insert(params.end(), parameters.begin(), parameters.end());
    ActiReg::ActivityAppConfigParam asConfigParam;
    asConfigParam.replace = asUID;
    asConfigParam.by = series->getID();
    params.push_back(asConfigParam);
    BOOST_FOREACH(const ActiReg::ActivityAppConfigParam& param, params)
    {
        if(!param.isSeshat())
        {
            (*replaceMap)[param.replace] = ::fwData::String::New(param.by);
        }
        else
        {
            ::fwData::Object::sptr obj = ::fwAtomConversion::getSubObject(series->getData(), param.by);
            OSLM_ASSERT("Invalid seshat path : '"<<param.by<<"'", obj);
            (*replaceMap)[param.replace] = ::fwData::String::New(obj->getID());
        }
    }
    return activityMsg;
}

//-----------------------------------------------------------------------------

} // namespace helper
} // namespace activities
