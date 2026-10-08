/************************************************************************
 *
 * Copyright (C) 2024-2026 IRCAD France
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

#include <sight/app/config.hpp>

#include "service/base.hpp"

#include <atomic>

namespace sight::app
{

/**
 * @brief   This interface defines control service API.
 * This class provides an interface for updaters that manage update sequences for services or other updaters.
 */
class SIGHT_APP_CLASS_API updater : public service::base
{
public:

    SIGHT_DECLARE_CLASS(updater, service::base);

    SIGHT_APP_API ~updater() override = default;

    /// Request cancellation of a pending update sequence before stopping this updater.
    SIGHT_APP_API void request_stop() noexcept;

    /// Return whether cancellation was requested.
    [[nodiscard]] SIGHT_APP_API bool stop_requested() const noexcept;

protected:

    /// Clear a previously requested stop when the updater starts again.
    SIGHT_APP_API void reset_stop_request() noexcept;

    /// Type of update element (service or updater).
    enum class type_t : std::uint8_t
    {
        service,
        updater
    };

    struct update_element_t
    {
        std::string uid;
        std::string slot;
        type_t type {type_t::service};
        bool ignore_stopped {false};
    };

    /// Does nothing
    SIGHT_APP_API void configuring(const config_t& _config) final;

    /// Return the list of update elements for this updater.
    [[nodiscard]] const std::vector<update_element_t>& elements() const noexcept
    {
        return m_elements;
    }

    /// Return the parent UID of this updater.
    [[nodiscard]] const std::string& parent() const noexcept
    {
        return m_parent;
    }

    /// Return whether this updater is set to loop.
    [[nodiscard]] bool loop() const noexcept
    {
        return m_loop;
    }

private:

    /// Keep track of received signals
    std::vector<update_element_t> m_elements;

    /// Parent UID of this updater.
    std::string m_parent;

    /// Whether this updater is set to loop.
    bool m_loop {false};

    /// Whether a stop has been requested for this updater. Used by the config manager and especially useful when
    /// the updater does no run on the main thread.
    std::atomic_bool m_stop_requested {false};
};

} // namespace sight::app
