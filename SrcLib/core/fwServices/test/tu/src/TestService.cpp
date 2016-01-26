/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "TestService.hpp"

#include <fwData/Object.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>



fwServicesRegisterMacro( ::fwServices::ut::TestService, ::fwServices::ut::TestServiceImplementation, ::fwData::Object );

namespace fwServices
{
namespace ut
{

const ::fwCom::Signals::SignalKeyType TestServiceImplementation::s_MSG_SENT_SIG = "msgSent";
const ::fwCom::Slots::SlotKeyType TestServiceImplementation::s_RECEIVE_MSG_SLOT = "receiveMsg";

} // namespace ut
} // namespace fwServices
