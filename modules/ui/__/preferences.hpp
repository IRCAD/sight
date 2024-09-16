/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

#pragma once

#include <core/com/helper/sig_slot_connection.hpp>
#include <core/mt/types.hpp>
#include <core/thread/timer.hpp>

#include <service/controller.hpp>

#include <ui/__/preferences.hpp>

namespace sight::module::ui
{

/**
 * @brief Read and write application user preferences.
 *
 * This service is a listener for a range of data objects given in its configuration. It stores their content in the
 * application preferences file in the user folder. It restores them when the app is launched.
 *
 * For a sake of simplicity, this service is automatically instantiated in every XML configuration for objects
 * declared with src="preference" attribute. So normally, no one should ever need to use it explicitly.
 *
 * @section XML XML Configuration
 *
 * The configuration with several objects is as follows:
 *
 * @code{.xml}
       <service uid="..." type="sight::module::ui::preferences" >
        <inout group="keys">
            <key uid="..." />
            <key uid="..." />
            <key uid="..." />
        </inout>
       </service>
   @endcode
 */
class preferences final : public sight::service::controller
{
public:

    SIGHT_DECLARE_SERVICE(preferences, sight::service::controller);

    /// Destructor. Does nothing
    ~preferences() noexcept final = default;

    /// Saves the configured preferences
    service::connections_t auto_connections() const final;

    /// Does nothing.
    void configuring(const config_t& /*_config*/) final;

    /// Does nothing.
    void starting() final;

    /// Clears timers
    void stopping() final;

    /// Saves the configured preferences
    void updating() final;

private:

    /// Used when we bind widgets to data
    data::ptr_vector<data::object, data::access::inout> m_preferences {this, "keys"};

    /// Allows to delay the write
    std::map<int, sight::core::thread::timer::sptr> m_timers;

    /// Protect the access to the timers map
    core::mt::read_write_mutex m_timers_mutex;

    /// Timer index to ensure we will only write after we stop to receive modifications
    int m_last_timer_id {-1};

    /// Manual connections from object modifications to update slot when we pass maps
    core::com::helper::sig_slot_connection m_map_keys_connections;

    /// Timer for the worker thread
    core::thread::worker::sptr m_timer_worker;
};

//------------------------------------------------------------------------------

} //namespace sight::module::ui
