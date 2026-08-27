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

#include "core/notification/message.hpp"

#include <utility>

namespace sight::core::notification
{

message::message(
    std::string _title,
    std::string _text,
    std::filesystem::path _icon,
    std::string _channel,
    bool _cancelable,
    cancel_hook _cancel_hook,
    std::optional<std::chrono::milliseconds> _duration,
    std::optional<bool> _sound
) :
    base(_cancelable, std::move(_cancel_hook)),
    m_icon(std::move(_icon)),
    m_title(std::move(_title)),
    m_text(std::move(_text)),
    m_channel(_channel.empty() ? m_title : std::move(_channel)),
    m_duration(_duration),
    m_sound(_sound)
{
}

//------------------------------------------------------------------------------

const std::filesystem::path& message::icon() const noexcept
{
    return m_icon;
}

//------------------------------------------------------------------------------

const std::string& message::title() const
{
    const auto lock = core::mt::read_lock(mutex());
    return m_title;
}

//------------------------------------------------------------------------------

const std::string& message::text() const
{
    const auto lock = core::mt::read_lock(mutex());
    return m_text;
}

//------------------------------------------------------------------------------

const std::string& message::channel() const noexcept
{
    return m_channel;
}

//------------------------------------------------------------------------------

const std::optional<std::chrono::milliseconds>& message::duration() const noexcept
{
    return m_duration;
}

//------------------------------------------------------------------------------

const std::optional<bool>& message::sound() const noexcept
{
    return m_sound;
}

//------------------------------------------------------------------------------

void message::set_title(std::string _title)
{
    {
        const auto lock = core::mt::write_lock(mutex());
        m_title = std::move(_title);
    }

    this->notify_change();
}

//------------------------------------------------------------------------------

void message::set_text(std::string _text)
{
    {
        const auto lock = core::mt::write_lock(mutex());
        m_text = std::move(_text);
    }

    this->notify_change();
}

//------------------------------------------------------------------------------

void message::add_change_hook(change_hook _callback)
{
    const auto lock = core::mt::write_lock(mutex());
    m_change_hooks.push_back(std::move(_callback));
}

//------------------------------------------------------------------------------

void message::notify_change() const
{
    // Copy the hooks under lock, then invoke them unlocked, so a hook can safely call back into title()/
    // text() (read lock) without deadlocking - same "unlock before invoking hooks" precedent as base::cancel().
    const change_hook_seq hooks = [this]
                                  {
                                      const auto lock = core::mt::read_lock(mutex());
                                      return m_change_hooks;
                                  }();

    for(const auto& hook : hooks)
    {
        hook();
    }
}

//------------------------------------------------------------------------------

} //namespace sight::core::notification
