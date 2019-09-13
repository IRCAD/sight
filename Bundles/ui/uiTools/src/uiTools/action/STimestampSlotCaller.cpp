/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "uiTools/action/STimestampSlotCaller.hpp"

#include <arData/Camera.hpp>
#include <arData/CameraSeries.hpp>

#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwCore/HiResClock.hpp>

#include <fwData/mt/ObjectWriteLock.hpp>

#include <regex>

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

    std::regex re("(.*)/(.*)");
    std::smatch match;
    std::string src, uid, key;

    for(ConfigurationType elem :  slotCfgs.getElements())
    {
        src = elem->getValue();
        if( std::regex_match(src, match, re) )
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
