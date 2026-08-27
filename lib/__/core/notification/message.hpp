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
     * @brief Builds a textual notification.
     *
     * @param _title Notification title.
     * @param _text Notification text.
     * @param _icon Notification icon path.
     * @param _channel Channel used by the notification's consumer (e.g. sight::module::ui::qt::notifier) to group,
     *        replace-in-place, and close related notifications. Uses the title when left empty.
     * @param _cancelable Whether the notification can be canceled.
     * @param _cancel_hook Callback to be called on cancel request.
     * @param _duration Overrides the display duration used by the notification's consumer (e.g.
     *        sight::module::ui::qt::notification_zone). Left empty to use the consumer's default for this type.
     * @param _sound Overrides whether a sound is played by the notification's consumer. Left empty to use the
     *        consumer's default for this type.
     */
    SIGHT_CORE_API explicit message(
        std::string _title                                 = {},
        std::string _text                                  = {},
        std::filesystem::path _icon                        = {},
        std::string _channel                               = {},
        bool _cancelable                                   = false,
        cancel_hook _cancel_hook                           = nullptr,
        std::optional<std::chrono::milliseconds> _duration = std::nullopt,
        std::optional<bool> _sound                         = std::nullopt
    );

    SIGHT_CORE_API ~message() override = default;

    message(message&)                      = delete;
    message& operator=(const message&)     = delete;
    message(message&&) noexcept            = delete;
    message& operator=(message&&) noexcept = delete;

    /// Change callback type for change hooks, fired by set_title()/set_text().
    using change_hook = std::function<void ()>;

    /// Change callback sequence type.
    using change_hook_seq = std::vector<change_hook>;

    [[nodiscard]] SIGHT_CORE_API const std::filesystem::path& icon() const noexcept;
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

private:

    /// Call every registered change hook. Must not be called while holding the mutex.
    void notify_change() const;

    std::filesystem::path m_icon;
    std::string m_title;
    std::string m_text;
    std::string m_channel;
    std::optional<std::chrono::milliseconds> m_duration;
    std::optional<bool> m_sound;

    /// Container of change callbacks.
    change_hook_seq m_change_hooks;
};

} //namespace sight::core::notification
