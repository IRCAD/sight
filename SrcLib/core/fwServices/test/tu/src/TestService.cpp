/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "TestService.hpp"

#include <fwServices/macros.hpp>

#include <fwData/Object.hpp>

fwServicesRegisterMacro( ::fwServices::ut::TestService, ::fwServices::ut::TestServiceImplementation, ::fwData::Object );
fwServicesRegisterMacro( ::fwServices::ut::TestService, ::fwServices::ut::TestServiceImplementation2 );
fwServicesRegisterMacro( ::fwServices::ut::TestService, ::fwServices::ut::TestSrvAutoconnect );

namespace fwServices
{
namespace ut
{

const ::fwCom::Signals::SignalKeyType TestServiceImplementation::s_MSG_SENT_SIG = "msgSent";
const ::fwCom::Slots::SlotKeyType TestServiceImplementation::s_UPDATE2_SLOT     = "update2";

const ::fwCom::Signals::SignalKeyType TestSrvAutoconnect::s_SIG_1 = "signal1";
const ::fwCom::Slots::SlotKeyType TestSrvAutoconnect::s_SLOT_1    = "slot1";

unsigned int TestService::s_START_COUNTER  = 0;
unsigned int TestService::s_UPDATE_COUNTER = 0;

//------------------------------------------------------------------------------

void TestService::starting()
{
    m_startOrder = s_START_COUNTER++;
}

//------------------------------------------------------------------------------

void TestService::updating()
{
    m_updateOrder = s_UPDATE_COUNTER++;
}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace fwServices
