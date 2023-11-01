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

#include "signal_gate.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/object.hpp>

#include <future>
#include <regex>

namespace sight::module::sync
{

// Public signal
const core::com::signals::key_t signal_gate::ALL_RECEIVED_SIG = "all_received";

//-----------------------------------------------------------------------------

signal_gate::signal_gate()
{
    new_signal<all_received_signal_t>(ALL_RECEIVED_SIG);
}

//-----------------------------------------------------------------------------

signal_gate::~signal_gate()
= default;

//-----------------------------------------------------------------------------

void signal_gate::configuring()
{
}

//------------------------------------------------------------------------------

void signal_gate::starting()
{
    service::config_t config = this->get_config();

    const std::regex re("(.*)/(.*)");
    std::smatch match;

    auto signals_cfg = config.equal_range("signal");
    for(auto it_cfg = signals_cfg.first ; it_cfg != signals_cfg.second ; ++it_cfg)
    {
        const std::string& signal = it_cfg->second.get_value<std::string>();
        if(std::regex_match(signal, match, re))
        {
            SIGHT_ASSERT("Wrong value for attribute src: " + signal, match.size() >= 3);

            std::string uid;
            std::string signal_key;
            uid.assign(match[1].first, match[1].second);
            signal_key.assign(match[2].first, match[2].second);

            if(core::tools::id::exist(uid))
            {
                core::tools::object::sptr obj               = core::tools::id::get_object(uid);
                core::com::has_signals::sptr signals_holder = std::dynamic_pointer_cast<core::com::has_signals>(obj);
                SIGHT_ASSERT("Object with id " << uid << " is not a has_slots", signals_holder);

                const std::size_t index = m_flags.size();
                m_flags.push_back(false);

                // Create a slot to our callback with a bound index to identify it
                std::function<void()> task([this, index](auto&& ...){received(index);});
                auto slot = core::com::new_slot(task);
                slot->set_worker(this->worker());

                // Connect the configured signal to this slot
                auto sig = signals_holder->signal(signal_key);
                SIGHT_ASSERT("Object with id " + uid + " does not have a signalKey '" + signal_key + "'", sig);

                sig->connect(slot);

                m_waiting_slots.push_back(slot);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void signal_gate::stopping()
{
    m_waiting_slots.clear();
    m_flags.clear();
}

//-----------------------------------------------------------------------------

void signal_gate::updating()
{
}

//------------------------------------------------------------------------------

void signal_gate::received(std::size_t _index)
{
    SIGHT_DEBUG("'" << this->get_id() << "' received a signal at position : " << _index);
    SIGHT_ASSERT("Could not find a signal at index " << _index, _index < m_flags.size());

    m_flags[_index] = true;

    bool all_received = true;
    for(bool received : m_flags)
    {
        all_received &= received;
    }

    if(all_received)
    {
        // Reset all flags before sending the signal
        std::fill(m_flags.begin(), m_flags.end(), false);

        SIGHT_DEBUG("'" << this->get_id() << "' received all signals, sending 'allReceived' now.");
        auto sig = this->signal<all_received_signal_t>(ALL_RECEIVED_SIG);
        sig->async_emit();
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::sync
