/************************************************************************
 *
 * Copyright (C) 2025-2026 IRCAD France
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

#include <sight/core/config.hpp>

#include "aggregator.hpp"
#include "has_notifications.hpp"
#include "observer.hpp"

#include <core/com/signals.hpp>

namespace sight::core::notification
{

namespace detail
{

template<typename C>
using notification_type_t = std::remove_cv_t<std::remove_reference_t<C> >;

//------------------------------------------------------------------------------

template<typename C>
constexpr bool is_monitor_notification()
{
    return std::is_same_v<notification_type_t<C>, sight::core::notification::observer>
           || std::is_same_v<notification_type_t<C>, sight::core::notification::aggregator>;
}

} // namespace detail

/**
 * @brief   Interface for objects that offer progress feedback with progress monitors.
 * If a service inherits from this class, the configuration manager will automatically assigns it to a worker.
 * Indeed, it is assumed that services that offer progress feedback will need to run in a separate thread.
 */
class SIGHT_CORE_CLASS_API has_monitors : public has_notifications
{
public:

    /// Destructor
    SIGHT_CORE_API ~has_monitors() noexcept override = default;

protected:

    /// Constructor. Create all default slots.
    /// @param _signals map of signals of the child class. For services, pass the protected variable base::m_signals.
    SIGHT_CORE_API explicit has_monitors(sight::core::com::signals& _signals) noexcept;

    /**
     * @brief Create a notification of the given type with the provided arguments.
     *
     * @tparam C The type of notification to create. Must be a subclass of sight::core::notification::base and a text
     *         notification (information, instruction, warning, or error) or a monitor notification (observer or
     * aggregator).
     * @tparam A The types of the arguments to forward to the notification's constructor.
     * @param _args The arguments to forward to the notification's constructor.
     * @return A shared pointer to the created notification.
     */
    template<typename C, typename ... A>
    /* *INDENT-OFF* */
    requires(detail::is_text_notification<C>() || detail::is_monitor_notification<C>())
    /* *INDENT-ON* */
    [[nodiscard]] C::sptr make_notification(A&& ... _args) const
    {
        return std::make_shared<C>(std::forward<A>(_args) ...);
    }

    /**
     * @brief Create a monitor of the given type with the provided arguments and emit the notification_created signal.
     *
     * @tparam C The type of monitor to create. Must be a subclass of sight::core::notification::base and a monitor
     *         notification (observer or aggregator).
     * @tparam A The types of the arguments to forward to the monitor's constructor.
     * @param _args The arguments to forward to the monitor's constructor.
     * @return A shared pointer to the created monitor.
     */
    template<typename C = sight::core::notification::observer, typename ... A>
    /* *INDENT-OFF* */
    requires(detail::is_monitor_notification<C>())
    /* *INDENT-ON* */
    [[nodiscard]] C::sptr observe(A&& ... _args) const
    {
        const auto observer = make_notification<C>(std::forward<A>(_args) ...);
        this->emit_notification_created(observer);
        return observer;
    }

    /**
     * @brief Create an aggregator, add the given monitors to it and emit the notification_created signal.
     *
     * @tparam A The types of the monitors to add. Each must be convertible to
     *         sight::core::notification::weighted_monitor, i.e. a monitor::sptr (using the default weight of 1.)
     *         or an explicit weighted_monitor for a custom weight.
     * @param _name The aggregator's name
     * @param _monitors The monitors (observer or aggregator) to add to the aggregator, optionally weighted
     * @return A shared pointer to the created aggregator
     */
    template<typename ... A>
    [[nodiscard]] aggregator::sptr aggregate(const std::string& _name, A&& ... _monitors) const
    {
        const auto agg = make_notification<aggregator>(_name);

        (
            [&agg](const weighted_monitor& _monitor)
            {
                agg->add(_monitor.progress, _monitor.weight);
            }(std::forward<A>(_monitors)), ...
        );

        this->emit_notification_created(agg);
        return agg;
    }
};

//------------------------------------------------------------------------------

} // namespace sight::core::notification
