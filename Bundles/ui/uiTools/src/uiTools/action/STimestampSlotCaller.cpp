/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiTools/action/STimestampSlotCaller.hpp"

#include <fwCom/Slots.hxx>
#include <fwCom/Slot.hxx>

#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwCore/HiResClock.hpp>

#include <arData/Camera.hpp>
#include <arData/CameraSeries.hpp>

#include <boost/regex.hpp>

namespace uiTools
{
namespace action
{

fwServicesRegisterMacro(::fwGui::IActionSrv, ::uiTools::action::STimestampSlotCaller, ::fwData::Object);


//-----------------------------------------------------------------------------

STimestampSlotCaller::STimestampSlotCaller()
{

}

//-----------------------------------------------------------------------------

STimestampSlotCaller::~STimestampSlotCaller()
{
}

//-----------------------------------------------------------------------------

void STimestampSlotCaller::configuring()
{
    this->initialize();

    ConfigurationType cfg = m_configuration->findConfigurationElement("slots");

    ::fwRuntime::ConfigurationElementContainer slotCfgs = cfg->findAllConfigurationElement("slot");

    ::boost::regex re("(.*)/(.*)");
    ::boost::smatch match;
    std::string src, uid, key;

    for(ConfigurationType elem :  slotCfgs.getElements())
    {
        src = elem->getValue();
        if( ::boost::regex_match(src, match, re) )
        {
            OSLM_ASSERT("Wrong value for attribute src: "<<src, match.size() >= 3);
            uid.assign(match[1].first, match[1].second);
            key.assign(match[2].first, match[2].second);

            SLM_ASSERT("Missing hasSlotsId attribute", !uid.empty());
            SLM_ASSERT("Missing slotKey attribute", !key.empty());

            m_slotInfos.push_back( std::make_pair(uid, key) );
        }
    }
}

//-----------------------------------------------------------------------------

void STimestampSlotCaller::starting()
{
    this->actionServiceStarting();
}

//-----------------------------------------------------------------------------

void STimestampSlotCaller::updating()
{
    ::fwCore::HiResClock::HiResClockType timestamp = ::fwCore::HiResClock::getTimeInMilliSec();

    for(SlotInfoType info : m_slotInfos)
    {
        HasSlotIDType HasSlotId = info.first;
        ::fwCom::Slots::SlotKeyType slotKey = info.second;

        if (::fwTools::fwID::exist(HasSlotId))
        {
            ::fwTools::Object::sptr obj      = ::fwTools::fwID::getObject(HasSlotId);
            ::fwCom::HasSlots::sptr hasSlots = ::std::dynamic_pointer_cast< ::fwCom::HasSlots >(obj);
            SLM_ASSERT("Object with id " << HasSlotId << " is not a HasSlots", hasSlots);

            ::fwCom::SlotBase::sptr slot = hasSlots->slot(slotKey);

            slot->asyncRun(timestamp);
        }
    }
}

//-----------------------------------------------------------------------------

void STimestampSlotCaller::stopping()
{
    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

} // namespace action
} // namespace uiTools
