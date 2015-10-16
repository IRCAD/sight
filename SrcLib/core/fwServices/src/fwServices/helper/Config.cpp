/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <utility>
#include <string>
#include <vector>

#include <boost/regex.hpp>
#include <boost/foreach.hpp>

#include <fwCom/HasSignals.hpp>
#include <fwCom/HasSlots.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwTools/Object.hpp>

#include "fwServices/helper/SigSlotConnection.hpp"
#include "fwServices/helper/Config.hpp"


namespace fwServices
{

namespace helper
{


void Config::createConnections(
    ::fwRuntime::ConfigurationElement::csptr connectionCfg,
    ::fwServices::helper::SigSlotConnection::sptr connections,
    const SPTR(::fwTools::Object)& obj)
{
    typedef std::pair< std::string, ::fwCom::Signals::SignalKeyType > SignalInfoType;
    typedef std::pair< std::string, ::fwCom::Slots::SlotKeyType > SlotInfoType;
    typedef std::vector< SlotInfoType > SlotInfoContainerType;

    SignalInfoType signalInfo;
    SlotInfoContainerType slotInfos;

    ::boost::regex re("(.*)/(.*)");
    ::boost::smatch match;
    std::string src, uid, key;

    BOOST_FOREACH(::fwRuntime::ConfigurationElement::csptr elem,  connectionCfg->getElements())
    {
        src = elem->getValue();
        if( ::boost::regex_match(src, match, re) )
        {
            OSLM_ASSERT("Wrong value for attribute src: "<<src, match.size() >= 3);
            uid.assign(match[1].first, match[1].second);
            key.assign(match[2].first, match[2].second);

            OSLM_ASSERT(src << " configuration is not correct for "<< elem->getName(),
                        !uid.empty() && !key.empty());

            if (elem->getName() == "signal")
            {
                SLM_ASSERT("There must be only one signal by connection",
                           signalInfo.first.empty() && signalInfo.second.empty());
                signalInfo = std::make_pair(uid, key);
            }
            else if (elem->getName() == "slot")
            {
                slotInfos.push_back( std::make_pair(uid, key) );
            }
        }
        else
        {
            SLM_ASSERT("Object uid is not defined, object used to retrieve signal must be present.", obj);
            uid = obj->getID();
            key = src;
            SLM_ASSERT("Element must be a signal or must be written as <fwID/Key>", elem->getName() == "signal");
            SLM_ASSERT("There must be only one signal by connection",
                       signalInfo.first.empty() && signalInfo.second.empty());
            signalInfo = std::make_pair(uid, key);
        }
    }

    ::fwTools::Object::sptr sigSource    = ::fwTools::fwID::getObject(signalInfo.first);
    ::fwCom::HasSignals::sptr hasSignals = std::dynamic_pointer_cast< ::fwCom::HasSignals >(sigSource);

    SLM_ASSERT("Signal source not found" << signalInfo.first, sigSource);
    SLM_ASSERT("invalid signal source " << signalInfo.first, hasSignals);

    BOOST_FOREACH(SlotInfoType slotInfo,  slotInfos)
    {
        ::fwTools::Object::sptr obj = ::fwTools::fwID::getObject(slotInfo.first);
        SLM_ASSERT("Failed to retrieve object '" + slotInfo.first + "'", obj);
        ::fwCom::HasSlots::sptr hasSlots = std::dynamic_pointer_cast< ::fwCom::HasSlots >(obj);
        SLM_ASSERT("invalid slot owner " << slotInfo.first, hasSlots);

        connections->connect(hasSignals, signalInfo.second, hasSlots, slotInfo.second);
    }
}

} // namespace helper
} // namespace fwServices

