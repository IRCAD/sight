/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "SSignalGate.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <data/Object.hpp>

#include <future>
#include <regex>

namespace sight::module::sync
{

// Public signal
const core::com::Signals::SignalKeyType SSignalGate::s_ALL_RECEIVED_SIG = "allReceived";

// Private slot
static const core::com::Slots::SlotKeyType s_RECEIVE_SLOT = "receive";

//-----------------------------------------------------------------------------

SSignalGate::SSignalGate()
{
    newSignal<AllReceivedSignalType>(s_ALL_RECEIVED_SIG);
}

//-----------------------------------------------------------------------------

SSignalGate::~SSignalGate()
{
}

//-----------------------------------------------------------------------------

void SSignalGate::configuring()
{
}

//------------------------------------------------------------------------------

void SSignalGate::starting()
{
    service::IService::ConfigType config = this->getConfigTree();

    const std::regex re("(.*)/(.*)");
    std::smatch match;

    auto signalsCfg = config.equal_range("signal");
    for(auto itCfg = signalsCfg.first ; itCfg != signalsCfg.second ; ++itCfg)
    {
        const std::string& signal = itCfg->second.get_value<std::string>();
        if(std::regex_match(signal, match, re))
        {
            SIGHT_ASSERT("Wrong value for attribute src: " + signal, match.size() >= 3);

            std::string uid, signalKey;
            uid.assign(match[1].first, match[1].second);
            signalKey.assign(match[2].first, match[2].second);

            if(core::tools::fwID::exist(uid))
            {
                core::tools::Object::sptr obj             = core::tools::fwID::getObject(uid);
                core::com::HasSignals::sptr signalsHolder = std::dynamic_pointer_cast<core::com::HasSignals>(obj);
                SIGHT_ASSERT("Object with id " << uid << " is not a HasSlots", signalsHolder);

                const size_t index = m_flags.size();
                m_flags.push_back(false);

                // Create a slot to our callback with a bound index to identify it
                std::function<void()> task(std::bind(&SSignalGate::received, this, index));
                auto slot = core::com::newSlot(task);
                slot->setWorker(m_associatedWorker);

                // Connect the configured signal to this slot
                auto sig = signalsHolder->signal(signalKey);
                SIGHT_ASSERT("Object with id " + uid + " does not have a signalKey '" + signalKey + "'", sig);

                sig->connect(slot);

                m_waitingSlots.push_back(slot);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void SSignalGate::stopping()
{
    m_waitingSlots.clear();
    m_flags.clear();
}

//-----------------------------------------------------------------------------

void SSignalGate::updating()
{
}

//------------------------------------------------------------------------------

void SSignalGate::received(size_t _index)
{
    SIGHT_DEBUG("'" << this->getID() << "' received a signal at position : " << _index);
    SIGHT_ASSERT("Could not find a signal at index " << _index, _index < m_flags.size());

    m_flags[_index] = true;

    bool allReceived = true;
    for(bool received : m_flags)
    {
        allReceived &= received;
    }

    if(allReceived)
    {
        // Reset all flags before sending the signal
        std::fill(m_flags.begin(), m_flags.end(), false);

        SIGHT_DEBUG("'" << this->getID() << "' received all signals, sending 'allReceived' now.");
        auto sig = this->signal<AllReceivedSignalType>(s_ALL_RECEIVED_SIG);
        sig->asyncEmit();
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::sync
