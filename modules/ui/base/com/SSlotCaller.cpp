/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "SSlotCaller.hpp"

#include <core/base.hpp>
#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>

#include <service/macros.hpp>

#include <ui/base/dialog/MessageDialog.hpp>

#include <regex>

namespace sight::module::ui::base
{

namespace com
{

static const service::IService::KeyType s_SLOT_KEY        = "slot";
static const service::IService::KeyType s_SLOTS_KEY       = "slots";
static const service::IService::KeyType s_SYNCED_CALL_KEY = "sync";

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
    this->actionServiceStarting();
}

//-----------------------------------------------------------------------------

void SSlotCaller::stopping()
{
    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void SSlotCaller::info(std::ostream& _sstream)
{
    _sstream << "Starter Action" << std::endl;
}

//-----------------------------------------------------------------------------

void SSlotCaller::updating()
{
    if(this->confirmAction())
    {
        for(const SlotInfoType& info : m_slotInfos)
        {
            const HasSlotIDType& HasSlotId               = info.first;
            const core::com::Slots::SlotKeyType& slotKey = info.second;

            if(core::tools::fwID::exist(HasSlotId))
            {
                const core::tools::Object::sptr obj       = core::tools::fwID::getObject(HasSlotId);
                const core::com::HasSlots::csptr hasSlots = std::dynamic_pointer_cast<core::com::HasSlots>(obj);
                SIGHT_ASSERT("Object with id " << HasSlotId << " is not a HasSlots", hasSlots);

                const core::com::SlotBase::csptr slot = hasSlots->slot(slotKey);

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
    this->initialize();

    ConfigurationType syncCfg = m_configuration->findConfigurationElement(s_SYNCED_CALL_KEY);

    m_synchronized = syncCfg && syncCfg->getValue() == "true";

    SIGHT_ASSERT(
        "Missing slots configuration element in " << this->getID(),
        m_configuration->hasConfigurationElement(s_SLOTS_KEY)
    );
    ConfigurationType cfg = m_configuration->findConfigurationElement(s_SLOTS_KEY);

    core::runtime::ConfigurationElementContainer slotCfgs = cfg->findAllConfigurationElement(s_SLOT_KEY);

    std::regex re("(.*)/(.*)");
    std::smatch match;
    std::string src, uid, key;

    for(ConfigurationType elem : slotCfgs.getElements())
    {
        src = elem->getValue();
        if(std::regex_match(src, match, re))
        {
            SIGHT_ASSERT("Wrong value for attribute src: " << src, match.size() >= 3);
            uid.assign(match[1].first, match[1].second);
            key.assign(match[2].first, match[2].second);

            SIGHT_ASSERT("Missing hasSlotsId attribute", !uid.empty());
            SIGHT_ASSERT("Missing slotKey attribute", !key.empty());

            m_slotInfos.push_back(std::make_pair(uid, key));
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace com

} // namespace sight::module::ui::base
