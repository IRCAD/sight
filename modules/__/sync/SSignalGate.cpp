/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/Object.hpp>

#include <future>
#include <regex>

namespace sight::module::sync
{

// Public signal
const core::com::signals::key_t SSignalGate::ALL_RECEIVED_SIG = "allReceived";

//-----------------------------------------------------------------------------

SSignalGate::SSignalGate()
{
    new_signal<AllReceivedSignalType>(ALL_RECEIVED_SIG);
}

//-----------------------------------------------------------------------------

SSignalGate::~SSignalGate()
= default;

//-----------------------------------------------------------------------------

void SSignalGate::configuring()
{
}

//------------------------------------------------------------------------------

void SSignalGate::starting()
{
    service::config_t config = this->getConfiguration();

    const std::regex re("(.*)/(.*)");
    std::smatch match;

    auto signalsCfg = config.equal_range("signal");
    for(auto itCfg = signalsCfg.first ; itCfg != signalsCfg.second ; ++itCfg)
    {
        const std::string& signal = itCfg->second.get_value<std::string>();
        if(std::regex_match(signal, match, re))
        {
            SIGHT_ASSERT("Wrong value for attribute src: " + signal, match.size() >= 3);

            std::string uid;
            std::string signalKey;
            uid.assign(match[1].first, match[1].second);
            signalKey.assign(match[2].first, match[2].second);

            if(core::tools::id::exist(uid))
            {
                core::tools::object::sptr obj              = core::tools::id::get_object(uid);
                core::com::has_signals::sptr signalsHolder = std::dynamic_pointer_cast<core::com::has_signals>(obj);
                SIGHT_ASSERT("Object with id " << uid << " is not a has_slots", signalsHolder);

                const std::size_t index = m_flags.size();
                m_flags.push_back(false);

                // Create a slot to our callback with a bound index to identify it
                std::function<void()> task([this, index](auto&& ...){received(index);});
                auto slot = core::com::new_slot(task);
                slot->set_worker(this->worker());

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

void SSignalGate::received(std::size_t _index)
{
    SIGHT_DEBUG("'" << this->get_id() << "' received a signal at position : " << _index);
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

        SIGHT_DEBUG("'" << this->get_id() << "' received all signals, sending 'allReceived' now.");
        auto sig = this->signal<AllReceivedSignalType>(ALL_RECEIVED_SIG);
        sig->async_emit();
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::sync
