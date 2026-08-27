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

#include "base.hpp"

#include <algorithm>
#include <utility>

namespace sight::core::notification
{

//------------------------------------------------------------------------------

base::base(bool _cancelable, cancel_hook _cancel_hook) :
    has_signals(),
    m_cancelable(_cancelable)
{
    if(_cancel_hook)
    {
        add_cancel_hook_no_lock(std::move(_cancel_hook));
    }

    new_signal<signals::void_t>(signals::STARTED);
    new_signal<signals::void_t>(signals::FINISHED);
    new_signal<signals::void_t>(signals::CANCEL_REQUESTED);
    new_signal<signals::void_t>(signals::CANCELED);
}

//------------------------------------------------------------------------------

base::~base()
{
    std::ranges::for_each(
        m_state_hooks,

        [&](const state_hook_seq::value_type& _f)
        {
            _f(get_state_no_lock());
        });
}

//------------------------------------------------------------------------------

base::state_t base::state() const
{
    const auto lock = core::mt::read_lock(m_mutex);
    return get_state_no_lock();
}

//------------------------------------------------------------------------------

bool base::is_finished() const
{
    const auto lock = core::mt::read_lock(m_mutex);
    return m_state == finished || m_state == canceled;
}

//------------------------------------------------------------------------------

bool base::cancel_requested() const
{
    const auto lock = core::mt::read_lock(m_mutex);
    return m_cancel_requested;
}

//------------------------------------------------------------------------------

cancel_request_callback_t base::cancel_requested_callback() const
{
    return [this]() -> bool
           {
               const auto lock = core::mt::read_lock(m_mutex);
               return m_cancel_requested;
           };
}

//------------------------------------------------------------------------------

void base::add_cancel_hook(cancel_hook _callback)
{
    const auto lock = core::mt::write_lock(m_mutex);
    this->add_cancel_hook_no_lock(std::move(_callback));
}

//------------------------------------------------------------------------------

void base::add_state_hook(state_hook _callback)
{
    const auto lock = core::mt::write_lock(m_mutex);
    this->add_state_hook_no_lock(std::move(_callback));
}

//------------------------------------------------------------------------------

void base::cancel()
{
    auto lock = core::mt::read_to_write_lock(m_mutex);

    if(!m_cancelable)
    {
        return;
    }

    if(m_state == waiting || m_state == running)
    {
        const auto next_state = (m_state == waiting) ? canceled : canceling;

        {
            const auto write_lock = core::mt::upgrade_to_write_lock(lock);
            m_cancel_requested = true;
            this->set_state_no_lock(canceling);
        }

        // Unlock mutex for state/cancel callbacks sanity: they may call back into this object (e.g.
        // state()/is_finished()), and the mutex is not recursive.
        lock.unlock();
        this->notify_state(canceling);

        for(const auto& cancel : m_cancel_hooks)
        {
            cancel();
        }

        lock.lock();

        SIGHT_ASSERT(
            "State shall be only CANCELING or CANCELED, not " << static_cast<unsigned int>(m_state),
            m_state == canceled || m_state == canceling
        );

        const bool reached_next_state = (m_state == canceling);
        if(reached_next_state)
        {
            const auto write_lock = core::mt::upgrade_to_write_lock(lock);
            this->set_state_no_lock(next_state);
        }

        lock.unlock();

        if(reached_next_state)
        {
            this->notify_state(next_state);
        }
    }
}

//------------------------------------------------------------------------------

void base::finish()
{
    core::mt::write_lock lock(m_mutex);
    if(const auto new_state = finish_no_lock(); new_state)
    {
        lock.unlock();
        this->notify_state(*new_state);
    }
}

//------------------------------------------------------------------------------

void base::run()
{
    core::mt::write_lock lock(m_mutex);
    if(m_state == waiting)
    {
        set_state_no_lock(running);
        lock.unlock();
        this->notify_state(running);
    }
}

//------------------------------------------------------------------------------

void base::set_cancelable(bool _cancel)
{
    core::mt::write_lock lock(m_mutex);
    m_cancelable = _cancel;
}

//------------------------------------------------------------------------------

bool base::cancelable() const
{
    core::mt::read_lock lock(m_mutex);
    return m_cancelable;
}

//------------------------------------------------------------------------------

base::state_t base::get_state_no_lock() const
{
    return m_state;
}

//------------------------------------------------------------------------------

void base::set_state(base::state_t _state)
{
    core::mt::write_lock lock(m_mutex);
    set_state_no_lock(_state);
    lock.unlock();
    this->notify_state(_state);
}

//------------------------------------------------------------------------------

void base::set_state_no_lock(base::state_t _state)
{
    m_state = _state;
}

//------------------------------------------------------------------------------

void base::notify_state(base::state_t _state)
{
    switch(_state)
    {
        case waiting:
            break;

        case running:
            this->async_emit(signals::STARTED);
            break;

        case canceling:
            this->async_emit(signals::CANCEL_REQUESTED);
            break;

        case canceled:
            this->async_emit(signals::CANCELED);
            break;

        case finished:
            this->async_emit(signals::FINISHED);
            break;

        default:
            SIGHT_ASSERT("You shall not pass !", 0);
    }

    // Copy the hooks under lock, then run them outside of it: a hook may call back into this object
    // (e.g. state()/is_finished()), and the mutex is not recursive.
    const auto hooks = [this]
                       {
                           const auto lock = core::mt::read_lock(m_mutex);
                           return m_state_hooks;
                       }();

    std::ranges::for_each(
        hooks,

        [&](const state_hook_seq::value_type& _f)
        {
            _f(_state);
        });
}

//------------------------------------------------------------------------------

void base::add_cancel_hook_no_lock(base::cancel_hook _callback)
{
    if(m_state == waiting || m_state == running)
    {
        m_cancel_hooks.push_back(std::move(_callback));
    }
}

//------------------------------------------------------------------------------

void base::add_state_hook_no_lock(base::state_hook _callback)
{
    m_state_hooks.push_back(std::move(_callback));
}

//------------------------------------------------------------------------------

std::optional<base::state_t> base::finish_no_lock()
{
    if(m_state != finished && m_state != canceled)
    {
        const auto next_state = (m_state == canceling) ? canceled : finished;
        this->set_state_no_lock(next_state);
        return next_state;
    }

    return std::nullopt;
}

//------------------------------------------------------------------------------

} //namespace sight::core::notification
