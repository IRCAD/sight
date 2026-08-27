/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include "base.hpp"
#include "error.hpp"
#include "information.hpp"
#include "instruction.hpp"
#include "warning.hpp"

#include <core/com/signal.hpp>
#include <core/com/signals.hpp>

namespace sight::core::notification
{

namespace detail
{

template<typename C>
using notification_type_t = std::remove_cv_t<std::remove_reference_t<C> >;

//------------------------------------------------------------------------------

template<typename C>
constexpr bool is_text_notification()
{
    return std::is_same_v<notification_type_t<C>, sight::core::notification::information>
           || std::is_same_v<notification_type_t<C>, sight::core::notification::instruction>
           || std::is_same_v<notification_type_t<C>, sight::core::notification::warning>
           || std::is_same_v<notification_type_t<C>, sight::core::notification::error>;
}

//------------------------------------------------------------------------------

/// Default title used by @ref has_notifications::instruct, @ref has_notifications::inform,
/// @ref has_notifications::warn and @ref has_notifications::fail when no explicit title is needed.
template<typename C>
requires(is_text_notification<C>())
constexpr const char* default_title()
{
    if constexpr(std::is_same_v<notification_type_t<C>, sight::core::notification::instruction>)
    {
        return "Instruction";
    }
    else if constexpr(std::is_same_v<notification_type_t<C>, sight::core::notification::information>)
    {
        return "Information";
    }
    else if constexpr(std::is_same_v<notification_type_t<C>, sight::core::notification::warning>)
    {
        return "Warning";
    }
    else
    {
        return "Error";
    }
}

} // namespace detail

/**
 * @brief Interface for objects that create textual notifications.
 *
 * Classes deriving from has_notifications gain the @ref instruct, @ref inform, @ref warn and @ref fail helpers to
 * create and emit textual notifications (respectively instruction, information, warning and error). These helpers
 * take the notification text as their first argument and automatically set the notification's title to its type
 * ("Instruction", "Information", "Warning" or "Error"); use @ref notify directly if a custom title is required.
 * Each emitted notification is broadcast through the signals::NOTIFICATION_CREATED signal, so that a consumer
 * (e.g. sight::module::ui::qt::notifier) can display it.
 */
class SIGHT_CORE_CLASS_API has_notifications
{
public:

    /// Signals emitted by has_notifications.
    struct signals
    {
        /// Type of signal emitted when a notification is created, carrying the created notification.
        using notification_created_t = sight::core::com::signal<void (sight::core::notification::base::sptr)>;

        /// Key of the signal emitted when a notification is created. @see signals::notification_created_t
        static inline const sight::core::com::signals::key_t NOTIFICATION_CREATED = "notification_created";
    };

    /// Destructor
    SIGHT_CORE_API virtual ~has_notifications() noexcept = default;

protected:

    /**
     * @brief Constructor. Register m_notification_created_sig to the given signals map.
     * @param _signals map of signals of the child class. For services, pass the protected variable base::m_signals.
     */
    SIGHT_CORE_API explicit has_notifications(sight::core::com::signals& _signals) noexcept;

    /**
     * @brief Create a notification of the given type with the provided arguments.
     *
     * @tparam C The type of notification to create. Must be a subclass of sight::core::notification::base and a text
     *         notification (information, instruction, warning, or error).
     * @tparam A The types of the arguments to forward to the notification's constructor.
     * @param _args The arguments to forward to the notification's constructor.
     * @return A shared pointer to the created notification.
     */
    template<typename C, typename ... A>
    /* *INDENT-OFF* */
    requires(detail::is_text_notification<C>())
    /* *INDENT-ON* */
    [[nodiscard]] static constexpr C::sptr make_notification(A&& ... _args)
    {
        return std::make_shared<C>(std::forward<A>(_args) ...);
    }

    /**
     * @brief Create a notification of the given type with the provided arguments and emit the notification created
     * signal.
     *
     * @tparam C The type of notification to create. Must be a subclass of sight::core::notification::base and a text
     *         notification (information, instruction, warning, or error).
     * @tparam A The types of the arguments to forward to the notification's constructor.
     * @param _args The arguments to forward to the notification's constructor.
     * @return A shared pointer to the created notification.
     */
    template<typename C, typename ... A>
    /* *INDENT-OFF* */
    requires(detail::is_text_notification<C>())
    /* *INDENT-ON* */
    constexpr C::sptr notify(A&& ... _args) const
    {
        const auto notification = make_notification<C>(std::forward<A>(_args) ...);
        m_notification_created_sig->async_emit(notification);

        return notification;
    }

    /**
     * @brief Create and emit an instruction notification. Its title is automatically set to "Instruction"; use
     * @ref notify directly if a custom title is needed.
     *
     * @tparam A The types of the arguments to forward to sight::core::notification::instruction's constructor
     *         (text, icon, channel, cancelable, cancel_hook, duration, sound), see message::message.
     * @param _args The arguments to forward to sight::core::notification::instruction's constructor.
     * @return A shared pointer to the created instruction notification.
     */
    template<typename ... A>
    // NOLINTNEXTLINE(modernize-use-nodiscard)
    sight::core::notification::instruction::sptr instruct(A&& ... _args) const
    {
        return notify<sight::core::notification::instruction>(
            detail::default_title<sight::core::notification::instruction>(),
            std::forward<A>(_args) ...
        );
    }

    /**
     * @brief Create and emit an information notification. Its title is automatically set to "Information"; use
     * @ref notify directly if a custom title is needed.
     *
     * @tparam A The types of the arguments to forward to sight::core::notification::information's constructor
     *         (text, icon, channel, cancelable, cancel_hook, duration, sound), see message::message.
     * @param _args The arguments to forward to sight::core::notification::information's constructor.
     * @return A shared pointer to the created information notification.
     */
    template<typename ... A>
    // NOLINTNEXTLINE(modernize-use-nodiscard)
    sight::core::notification::information::sptr inform(A&& ... _args) const
    {
        return notify<sight::core::notification::information>(
            detail::default_title<sight::core::notification::information>(),
            std::forward<A>(_args) ...
        );
    }

    /**
     * @brief Create and emit a warning notification. Its title is automatically set to "Warning"; use @ref notify
     * directly if a custom title is needed.
     *
     * @tparam A The types of the arguments to forward to sight::core::notification::warning's constructor
     *         (text, icon, channel, cancelable, cancel_hook, duration, sound), see message::message.
     * @param _args The arguments to forward to sight::core::notification::warning's constructor.
     * @return A shared pointer to the created warning notification.
     */
    template<typename ... A>
    // NOLINTNEXTLINE(modernize-use-nodiscard)
    sight::core::notification::warning::sptr warn(A&& ... _args) const
    {
        return notify<sight::core::notification::warning>(
            detail::default_title<sight::core::notification::warning>(),
            std::forward<A>(_args) ...
        );
    }

    /**
     * @brief Create and emit an error notification. Its title is automatically set to "Error"; use @ref notify
     * directly if a custom title is needed.
     *
     * @tparam A The types of the arguments to forward to sight::core::notification::error's constructor
     *         (text, icon, channel, cancelable, cancel_hook, duration, sound), see message::message.
     * @param _args The arguments to forward to sight::core::notification::error's constructor.
     * @return A shared pointer to the created error notification.
     */
    template<typename ... A>
    // NOLINTNEXTLINE(modernize-use-nodiscard)
    sight::core::notification::error::sptr fail(A&& ... _args) const
    {
        return notify<sight::core::notification::error>(
            detail::default_title<sight::core::notification::error>(),
            std::forward<A>(_args) ...
        );
    }

    /**
     * @brief Emit the notification created signal with the given notification.
     *
     * @param _notification The notification to emit
     */
    void emit_notification_created(sight::core::notification::base::sptr _notification) const noexcept
    {
        m_notification_created_sig->async_emit(_notification);
    }

private:

    // Since we can't inherits from has_signals without having multi-inheritance problems,
    // we declare the notification created signal here.
    const signals::notification_created_t::sptr m_notification_created_sig {
        std::make_shared<signals::notification_created_t>()
    };
};

//------------------------------------------------------------------------------

} // namespace sight::core::notification
