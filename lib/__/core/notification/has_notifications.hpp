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
#include <core/runtime/types.hpp>

#include <map>
#include <string>

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
     * @brief Create a notification of the given type from the given parameters, resolving its key into an id, and
     * emit the notification created signal.
     *
     * @tparam C The type of notification to create. Must be a subclass of sight::core::notification::base and a text
     *         notification (information, instruction, warning, or error).
     * @param _params The parameters to build the notification with, @see message::params.
     * @return A shared pointer to the created notification.
     */
    template<typename C>
    /* *INDENT-OFF* */
    requires(detail::is_text_notification<C>())
    /* *INDENT-ON* */
    // NOLINTNEXTLINE(modernize-use-nodiscard)
    C::sptr notify(message::params _params) const
    {
        // Resolved here rather than in the constructor, which knows nothing of the service configuration.
        auto id = this->notification_id(_params.key);

        const auto notification = make_notification<C>(std::move(_params));
        notification->set_id(std::move(id));
        m_notification_created_sig->async_emit(notification);

        return notification;
    }

    /**
     * @brief Create and emit an instruction notification. Its title is automatically set to "Instruction"; use
     * @ref notify directly if a custom title is needed.
     *
     * @param _text The instruction text. Left empty to close the instruction currently displayed.
     * @return A shared pointer to the created instruction notification.
     */
    // NOLINTNEXTLINE(modernize-use-nodiscard)
    sight::core::notification::instruction::sptr instruct(std::string _text = {}) const
    {
        return this->instruct(message::params {.text = std::move(_text)});
    }

    /// Create and emit an instruction notification. @see instruct(std::string), @see message::params
    // NOLINTNEXTLINE(modernize-use-nodiscard)
    sight::core::notification::instruction::sptr instruct(message::params _params) const
    {
        return this->notify_titled<sight::core::notification::instruction>(std::move(_params));
    }

    /**
     * @brief Create and emit an information notification. Its title is automatically set to "Information"; use
     * @ref notify directly if a custom title is needed.
     *
     * @param _text The information text.
     * @return A shared pointer to the created information notification.
     */
    // NOLINTNEXTLINE(modernize-use-nodiscard)
    sight::core::notification::information::sptr inform(std::string _text = {}) const
    {
        return this->inform(message::params {.text = std::move(_text)});
    }

    /// Create and emit an information notification. @see inform(std::string), @see message::params
    // NOLINTNEXTLINE(modernize-use-nodiscard)
    sight::core::notification::information::sptr inform(message::params _params) const
    {
        return this->notify_titled<sight::core::notification::information>(std::move(_params));
    }

    /**
     * @brief Create and emit a warning notification. Its title is automatically set to "Warning"; use @ref notify
     * directly if a custom title is needed.
     *
     * @param _text The warning text.
     * @return A shared pointer to the created warning notification.
     */
    // NOLINTNEXTLINE(modernize-use-nodiscard)
    sight::core::notification::warning::sptr warn(std::string _text = {}) const
    {
        return this->warn(message::params {.text = std::move(_text)});
    }

    /// Create and emit a warning notification. @see warn(std::string), @see message::params
    // NOLINTNEXTLINE(modernize-use-nodiscard)
    sight::core::notification::warning::sptr warn(message::params _params) const
    {
        return this->notify_titled<sight::core::notification::warning>(std::move(_params));
    }

    /**
     * @brief Create and emit an error notification. Its title is automatically set to "Error"; use @ref notify
     * directly if a custom title is needed.
     *
     * @param _text The error text.
     * @return A shared pointer to the created error notification.
     */
    // NOLINTNEXTLINE(modernize-use-nodiscard)
    sight::core::notification::error::sptr fail(std::string _text = {}) const
    {
        return this->fail(message::params {.text = std::move(_text)});
    }

    /// Create and emit an error notification. @see fail(std::string), @see message::params
    // NOLINTNEXTLINE(modernize-use-nodiscard)
    sight::core::notification::error::sptr fail(message::params _params) const
    {
        return this->notify_titled<sight::core::notification::error>(std::move(_params));
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

    /**
     * @brief Parse the optional `<notification key="..." id="..."/>` elements of a service configuration, which
     * map a key, known by the emitting service, to an id, known by the notification's consumer (e.g.
     * sight::module::ui::notification_merger):
     * @code{.xml}
       <service uid="left_sensor_srv" type="...">
           <notification key="out_of_range" id="left_sensor_out_of_range"/>
       </service>
       @endcode
     *
     * Since has_notifications is a mixin with no access to the service configuration, implementations must call
     * this from their own configuring().
     *
     * @param _config The service configuration to parse.
     */
    SIGHT_CORE_API void configure_notifications(const sight::core::runtime::config_t& _config);

private:

    /**
     * @brief Get the id mapped to the given key by the XML configuration, @see configure_notifications(),
     * @see message::params::key. Called when emitting, so that services never handle the id themselves.
     *
     * @param _key The key used by this service to designate a kind of notification.
     * @return The mapped id, empty if the key is not mapped, which is the normal case for an application that
     *         does not compose this notification.
     */
    [[nodiscard]] SIGHT_CORE_API std::string notification_id(const std::string& _key) const;

    /// Emit a notification of the given type, defaulting its title to that type when left empty.
    template<typename C>
    /* *INDENT-OFF* */
    requires(detail::is_text_notification<C>())
    /* *INDENT-ON* */
    [[nodiscard]] C::sptr notify_titled(message::params _params) const
    {
        if(_params.title.empty())
        {
            _params.title = detail::default_title<C>();
        }

        return this->notify<C>(std::move(_params));
    }

    // Since we can't inherits from has_signals without having multi-inheritance problems,
    // we declare the notification created signal here.
    const signals::notification_created_t::sptr m_notification_created_sig {
        std::make_shared<signals::notification_created_t>()
    };

    /// Key -> id map filled by configure_notifications(). @see notification_id()
    std::map<std::string, std::string> m_notification_ids;
};

//------------------------------------------------------------------------------

} // namespace sight::core::notification
