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

#include "joystick.hpp"

#include "detail/event_loop.hpp"

#include <mutex>

namespace sight::io::joystick
{

/// This structure is used to ensure that SDL is correctly terminated when the program exits, even in case of crashes.
static struct event_loop_keeper
{
    ~event_loop_keeper()
    {
        std::unique_lock lock(mutex);

        // We need to perform special action if shutdown() is not called due to unexpected scalar destruction
        // If we are there, like when CPPUnit test breaks at start, the main thread is already dead (at least on linux).
        if(event_loop)
        {
            event_loop->internal_stop();
        }
    }

    std::mutex mutex;
    std::shared_ptr<detail::event_loop> event_loop;
} s_event_loop_keeper;

//------------------------------------------------------------------------------

void init()
{
    std::unique_lock lock(s_event_loop_keeper.mutex);

    if(!s_event_loop_keeper.event_loop)
    {
        s_event_loop_keeper.event_loop = detail::event_loop::make();
    }
}

//------------------------------------------------------------------------------

void shutdown()
{
    std::unique_lock lock(s_event_loop_keeper.mutex);
    s_event_loop_keeper.event_loop.reset();
}

} // namespace sight::io::joystick
