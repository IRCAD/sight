/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "gui/action/SBooleanSlotCaller.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwCore/base.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwRuntime/Extension.hpp>
#include <fwRuntime/helper.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/fwID.hpp>

#include <boost/regex.hpp>

namespace gui
{
namespace action
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::gui::action::SBooleanSlotCaller );

//-----------------------------------------------------------------------------

SBooleanSlotCaller::SBooleanSlotCaller() noexcept
{
}

//-----------------------------------------------------------------------------

SBooleanSlotCaller::~SBooleanSlotCaller() noexcept
{
}

//-----------------------------------------------------------------------------

void SBooleanSlotCaller::updating()
{
    for(SlotInfoType info :  m_slotInfos)
    {
        HasSlotIDType HasSlotId = info.first;
        ::fwCom::Slots::SlotKeyType slotKey = info.second;

        if (::fwTools::fwID::exist(HasSlotId))
        {
            ::fwTools::Object::sptr obj      = ::fwTools::fwID::getObject(HasSlotId);
            ::fwCom::HasSlots::sptr hasSlots = std::dynamic_pointer_cast< ::fwCom::HasSlots >(obj);
            SLM_ASSERT("Object with id " << HasSlotId << " is not a HasSlots", hasSlots);

            ::fwCom::SlotBase::sptr slot = hasSlots->slot(slotKey);

            slot->asyncRun(this->getIsActive());
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace action
} // namespace gui
