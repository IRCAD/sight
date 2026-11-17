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

#include <chrono>
#include <filesystem>
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace sight::core::notification
{

/**
 * @brief Base class for textual notifications.
 */
class SIGHT_CORE_CLASS_API message : public base
{
public:

    SIGHT_DECLARE_CLASS(message, base);
    SIGHT_ALLOW_SHARED_FROM_THIS();

    /**
     * @brief Everything a textual notification can be built with, meant to be filled with designated
     * initializers so that only the relevant fields have to be named:
     * @code{.cpp}
       this->warn({.text = "Left sensor out of range", .id = "left_sensor_out_of_range"});
       @endcode
     */
    struct params final
    {
        // Every field has a default member initializer, so that a designated initializer list can name only
        // the relevant ones without warning.

        /// Notification title. Defaults to the notification's type when built through has_notifications.
        std::string title {};

        /// Notification text.
        std::string text {};

        /// Key this service knows the notification by, resolved into an id by has_notifications' helpers,
        /// @see has_notifications::configure_notifications(), @see base::id().
        std::string key {};

        /// Notification icon path. Empty means no icon, while nullopt selects the notification type's default icon.
        std::optional<std::filesystem::path> icon {std::nullopt};

        /// Channel used by the notification's consumer (e.g. sight::module::ui::qt::notifier) to group,
        /// replace-in-place, and close related notifications. Uses the title when left empty.
        std::string channel {};

        /// Whether the notification can be canceled.
        bool cancelable {false};

        /// Callback to be called on cancel request. Not named cancel_hook, which is the type's own name here.
        cancel_callback_t cancel_callback {nullptr};

        /// Overrides the display duration used by the notification's consumer (e.g.
        /// sight::module::ui::qt::notification_zone), 0 meaning no timeout at all. Left empty to use the
        /// consumer's default for this type.
        std::optional<std::chrono::milliseconds> duration {};

        /// Overrides whether a sound is played by the notification's consumer. Left empty to use the
        /// consumer's default for this type.
        std::optional<bool> sound {};
    };

    /// Builds a textual notification with only a title and a text. @see message(params) for the other fields.
    SIGHT_CORE_API explicit message(std::string _title = {}, std::string _text = {});

    /// Builds a fully specified textual notification. @see params
    SIGHT_CORE_API explicit message(params _params);

    SIGHT_CORE_API ~message() override = default;

    message(message&)                      = delete;
    message& operator=(const message&)     = delete;
    message(message&&) noexcept            = delete;
    message& operator=(message&&) noexcept = delete;

    /// Change callback type for change hooks, fired by set_title()/set_text().
    using change_hook = std::function<void ()>;

    /// Change callback sequence type.
    using change_hook_seq = std::vector<change_hook>;

    [[nodiscard]] SIGHT_CORE_API const std::optional<std::filesystem::path>& icon() const noexcept;
    [[nodiscard]] SIGHT_CORE_API const std::string& title() const;
    [[nodiscard]] SIGHT_CORE_API const std::string& text() const;
    [[nodiscard]] SIGHT_CORE_API const std::string& channel() const noexcept;
    [[nodiscard]] SIGHT_CORE_API const std::optional<std::chrono::milliseconds>& duration() const noexcept;
    [[nodiscard]] SIGHT_CORE_API const std::optional<bool>& sound() const noexcept;

    /**
     * @brief Update the notification's title and fire all registered change hooks.
     * Allows a service that kept the sptr returned by e.g. notify()/warn()/fail() to update the text
     * displayed by the notification's consumer (e.g. sight::module::ui::qt::notification_zone) after it was
     * created.
     */
    SIGHT_CORE_API void set_title(std::string _title);

    /// Update the notification's text and fire all registered change hooks. @see set_title
    SIGHT_CORE_API void set_text(std::string _text);

    /**
     * @brief Register a callback invoked (synchronously, on the caller's thread) whenever set_title()/
     * set_text() change the notification's displayed content.
     *
     * @param _callback the callback to add.
     */
    SIGHT_CORE_API void add_change_hook(change_hook _callback);

protected:

    /// Returns the parameters with the notification type's default icon, unless the caller named one.
    [[nodiscard]] SIGHT_CORE_API static params with_default_icon(params _params, std::filesystem::path _icon);

private:

    /// Resolve a notification icon once while constructing the notification.
    static std::optional<std::filesystem::path> resolve_icon(std::optional<std::filesystem::path> _icon);

    /// Call every registered change hook. Must not be called while holding the mutex.
    void notify_change() const;

    std::optional<std::filesystem::path> m_icon;
    std::string m_title;
    std::string m_text;
    std::string m_channel;
    std::optional<std::chrono::milliseconds> m_duration;
    std::optional<bool> m_sound;

    /// Container of change callbacks.
    change_hook_seq m_change_hooks;
};

} //namespace sight::core::notification
