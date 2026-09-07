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

#include <core/runtime/path.hpp>
#include <core/spy_log.hpp>

#include <filesystem>
#include <utility>

namespace sight::core::notification
{

message::message(std::string _title, std::string _text) :
    message(params {.title = std::move(_title), .text = std::move(_text)})
{
}

//------------------------------------------------------------------------------

message::message(params _params) :
    base(_params.cancelable, std::move(_params.cancel_callback)),
    m_icon(resolve_icon(std::move(_params.icon))),
    m_title(std::move(_params.title)),
    m_text(std::move(_params.text)),
    m_channel(_params.channel.empty() ? m_title : std::move(_params.channel)),
    m_duration(_params.duration),
    m_sound(_params.sound)
{
}

//------------------------------------------------------------------------------

const std::optional<std::filesystem::path>& message::icon() const noexcept
{
    return m_icon;
}

//------------------------------------------------------------------------------

message::params message::with_default_icon(params _params, std::filesystem::path _icon)
{
    if(!_params.icon.has_value())
    {
        _params.icon = std::move(_icon);
    }

    return _params;
}

//------------------------------------------------------------------------------

std::optional<std::filesystem::path> message::resolve_icon(std::optional<std::filesystem::path> _icon)
{
    if(!_icon.has_value() || _icon->empty())
    {
        return _icon;
    }

    // Asserts rather than throws: icon paths come from XML, which is written and run in debug, and no longer
    // changes in a packaged application.
    if(_icon->is_absolute())
    {
        SIGHT_ASSERT("Notification icon was not found: " << *_icon, std::filesystem::exists(*_icon));
        return _icon;
    }

    // Same module relative form as toolbar, menu and progress bar icons: "<module id>/<resource path>".
    SIGHT_ASSERT("Notification icon path must name a module and a resource: " << *_icon, _icon->has_parent_path());

    auto path = core::runtime::get_module_resource_file_path(*_icon);

    SIGHT_ASSERT("Notification icon was not found: " << *_icon, !path.empty() && std::filesystem::exists(path));
    return path;
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
