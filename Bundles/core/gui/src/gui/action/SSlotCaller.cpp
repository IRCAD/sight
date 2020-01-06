/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "gui/action/SSlotCaller.hpp"

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

#include <regex>

namespace gui
{
namespace action
{

static const ::fwServices::IService::KeyType s_SLOT_KEY        = "slot";
static const ::fwServices::IService::KeyType s_SLOTS_KEY       = "slots";
static const ::fwServices::IService::KeyType s_SYNCED_CALL_KEY = "sync";

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::gui::action::SSlotCaller );

//-----------------------------------------------------------------------------

SSlotCaller::SSlotCaller() noexcept
{
}

//-----------------------------------------------------------------------------

SSlotCaller::~SSlotCaller() noexcept
{
}

//-----------------------------------------------------------------------------

void SSlotCaller::starting()
{
    SLM_TRACE_FUNC();
    this->actionServiceStarting();
}

//-----------------------------------------------------------------------------

void SSlotCaller::stopping()
{
    SLM_TRACE_FUNC();

    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void SSlotCaller::info(std::ostream& _sstream )
{
    _sstream << "Starter Action" << std::endl;
}

//-----------------------------------------------------------------------------

void SSlotCaller::updating()
{
    SLM_TRACE_FUNC();

    if(this->confirmAction())
    {
        for(const SlotInfoType& info :  m_slotInfos)
        {
            const HasSlotIDType& HasSlotId             = info.first;
            const ::fwCom::Slots::SlotKeyType& slotKey = info.second;

            if (::fwTools::fwID::exist(HasSlotId))
            {
                const ::fwTools::Object::sptr obj       = ::fwTools::fwID::getObject(HasSlotId);
                const ::fwCom::HasSlots::csptr hasSlots = std::dynamic_pointer_cast< ::fwCom::HasSlots >(obj);
                SLM_ASSERT("Object with id " << HasSlotId << " is not a HasSlots", hasSlots);

                const ::fwCom::SlotBase::csptr slot = hasSlots->slot(slotKey);

                if(m_synchronized)
                {
                    slot->run();
                }
                else
                {
                    slot->asyncRun();
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

void SSlotCaller::configuring()
{
    SLM_TRACE_FUNC();
    this->initialize();

    ConfigurationType syncCfg = m_configuration->findConfigurationElement(s_SYNCED_CALL_KEY);

    m_synchronized = syncCfg && syncCfg->getValue() == "true";

    OSLM_ASSERT("Missing slots configuration element in " << this->getID(),
                m_configuration->hasConfigurationElement(s_SLOTS_KEY));
    ConfigurationType cfg = m_configuration->findConfigurationElement(s_SLOTS_KEY);

    ::fwRuntime::ConfigurationElementContainer slotCfgs = cfg->findAllConfigurationElement(s_SLOT_KEY);

    std::regex re("(.*)/(.*)");
    std::smatch match;
    std::string src, uid, key;

    for(ConfigurationType elem :   slotCfgs.getElements())
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

} // namespace action
} // namespace gui
