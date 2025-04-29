/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include "interactor.hpp"

#include "detail/event_loop.hpp"

#include <core/spy_log.hpp>

// cspell:ignore NOLINTNEXTLINE interactor
namespace sight::io::joystick
{

interactor::~interactor()
{
    if(const auto instance = detail::event_loop::instance(); instance)
    {
        instance->remove_interactor(this, true);
    }
}

//------------------------------------------------------------------------------

void interactor::start_listening_joystick()
{
    const auto instance = detail::event_loop::instance();
    SIGHT_WARN_IF("Event loop is not available", !instance);

    if(instance)
    {
        instance->add_interactor(this);
    }
}

//------------------------------------------------------------------------------

void interactor::stop_listening_joystick() const
{
    const auto instance = detail::event_loop::instance();
    SIGHT_WARN_IF("Event loop is not available", !instance);

    if(instance)
    {
        instance->remove_interactor(this);
    }
}

//------------------------------------------------------------------------------

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
std::int32_t interactor::left_joystick() const
{
    const auto instance = detail::event_loop::instance();
    SIGHT_WARN_IF("Event loop is not available", !instance);

    if(instance)
    {
        return instance->left_joystick();
    }

    return -1;
}

//------------------------------------------------------------------------------

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
std::int32_t interactor::right_joystick() const
{
    const auto instance = detail::event_loop::instance();
    SIGHT_WARN_IF("Event loop is not available", !instance);

    if(instance)
    {
        return instance->right_joystick();
    }

    return -1;
}

//------------------------------------------------------------------------------

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
std::vector<std::shared_ptr<const device> > interactor::devices() const
{
    if(const auto instance = detail::event_loop::instance(); instance)
    {
        return instance->devices();
    }

    return {};
}

//------------------------------------------------------------------------------

void interactor::joystick_axis_motion_event(const axis_motion_event& /*_event*/)
{
}

//------------------------------------------------------------------------------

void interactor::joystick_axis_direction_event(const axis_direction_event& /*_event*/)
{
}

//------------------------------------------------------------------------------

void interactor::joystick_hat_motion_event(const hat_motion_event& /*_event*/)
{
}

//------------------------------------------------------------------------------

void interactor::joystick_ball_motion_event(const ball_motion_event& /*_event*/)
{
}

//------------------------------------------------------------------------------

void interactor::joystick_button_pressed_event(const button_event& /*_event*/)
{
}

//------------------------------------------------------------------------------

void interactor::joystick_button_released_event(const button_event& /*_event*/)
{
}

//------------------------------------------------------------------------------

void interactor::joystick_added_event(const joystick_event& /*_event*/)
{
}

//------------------------------------------------------------------------------

void interactor::joystick_removed_event(const joystick_event& /*_event*/)
{
}

} //namespace sight::io::joystick
