/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "core/notification/monitor.hpp"

#include "core/spy_log.hpp"

namespace sight::core::notification
{

monitor::monitor(std::string _name, bool _cancelable, cancel_hook _cancel_hook) :
    base(_cancelable, std::move(_cancel_hook)),
    m_name(std::move(_name))
{
    new_signal<signals::done_work_t>(signals::DONE_WORK);
    new_signal<signals::log_t>(signals::LOGGED);
}

//------------------------------------------------------------------------------

const std::string& monitor::name() const
{
    return m_name;
}

//------------------------------------------------------------------------------

std::uint64_t monitor::get_done_work_units() const
{
    core::mt::read_lock lock(mutex());
    return m_done_work_units;
}

//------------------------------------------------------------------------------

std::uint64_t monitor::get_total_work_units() const
{
    core::mt::read_lock lock(mutex());
    return m_total_work_units;
}

//------------------------------------------------------------------------------

void monitor::add_done_work_hook(done_work_hook _callback)
{
    core::mt::write_lock lock(mutex());
    this->add_done_work_hook_no_lock(_callback);
}

//------------------------------------------------------------------------------

void monitor::add_total_work_units_hook(total_work_units_hook _callback)
{
    core::mt::write_lock lock(mutex());
    this->add_total_work_units_hook_no_lock(_callback);
}

//------------------------------------------------------------------------------

void monitor::add_log_hook(log_hook _callback)
{
    core::mt::write_lock lock(mutex());
    this->add_log_hook_no_lock(_callback);
}

//------------------------------------------------------------------------------

void monitor::log(const std::string& _message)
{
    core::mt::write_lock lock(mutex());
    this->log_no_lock(_message);
}

//------------------------------------------------------------------------------

void monitor::log_no_lock(const std::string& _message)
{
    m_logs.push_back(_message);

    this->async_emit(signals::LOGGED, _message);

    std::ranges::for_each(
        m_log_hooks,

        [&](const log_hook_seq::value_type& _f)
        {
            _f(*this, _message);
        });
}

//------------------------------------------------------------------------------

void monitor::add_done_work_hook_no_lock(done_work_hook _callback)
{
    const auto state = base::get_state_no_lock();
    if(state == waiting || state == running)
    {
        m_done_work_hooks.push_back(_callback);
    }
}

//------------------------------------------------------------------------------

void monitor::add_total_work_units_hook_no_lock(total_work_units_hook _callback)
{
    const auto state = base::get_state_no_lock();
    if(state == waiting || state == running)
    {
        m_total_work_units_hooks.push_back(_callback);
    }
}

//------------------------------------------------------------------------------

void monitor::add_log_hook_no_lock(log_hook _callback)
{
    m_log_hooks.push_back(_callback);
}

//------------------------------------------------------------------------------

void monitor::add_done_work(std::uint64_t _units)
{
    core::mt::read_to_write_lock lock(mutex());
    const auto state = base::get_state_no_lock();
    if(state != canceled)
    {
        SIGHT_ASSERT(
            "Can not report done work while the monitor is not running.",
            state == running || state == canceling
        );
        this->done_work(m_done_work_units + _units, lock);
    }
}

//------------------------------------------------------------------------------

void monitor::done_work(std::uint64_t _units)
{
    core::mt::read_to_write_lock lock(mutex());
    const auto state = base::get_state_no_lock();
    if(state != canceled)
    {
        SIGHT_ASSERT(
            "Can not report done work while the monitor is not running.",
            state == running || state == canceling
        );
        this->done_work(_units, lock);
    }
}

//------------------------------------------------------------------------------

void monitor::done_work(std::uint64_t _units, core::mt::read_to_write_lock& _lock)
{
    auto old_done_work = m_done_work_units;
    SIGHT_ERROR_IF("Done work units should never decrease", old_done_work > _units);

    decltype(m_done_work_hooks) done_work_hooks;

    if(m_done_work_units == _units)
    {
        _lock.unlock();
        return;
    }

    done_work_hooks = m_done_work_hooks;

    {
        core::mt::upgrade_to_write_lock write_lock(_lock);
        m_done_work_units = _units;
    }

    this->async_emit(signals::DONE_WORK, m_done_work_units, m_total_work_units);

    _lock.unlock();

    std::ranges::for_each(
        done_work_hooks,

        [&](const done_work_hook_seq::value_type& _f)
        {
            _f(*this, old_done_work);
        });
}

//------------------------------------------------------------------------------

void monitor::set_total_work_units(std::uint64_t _units)
{
    core::mt::read_to_write_lock lock(mutex());
    this->set_total_work_units_upgrade_lock(_units, lock);
}

//------------------------------------------------------------------------------

void monitor::set_total_work_units_upgrade_lock(std::uint64_t _units, core::mt::read_to_write_lock& _lock)
{
    SIGHT_ASSERT(
        "Total work units should only be modified before running a monitor",
        base::get_state_no_lock() == waiting or (base::get_state_no_lock() == running and m_done_work_units == 0)
    );

    auto old_total_work_units = m_total_work_units;
    decltype(m_total_work_units_hooks) total_work_units_hook;

    total_work_units_hook = m_total_work_units_hooks;

    if(m_done_work_units > _units)
    {
        this->done_work(_units, _lock);
        _lock.lock();
    }

    {
        core::mt::upgrade_to_write_lock write_lock(_lock);
        m_total_work_units = _units;
    }

    _lock.unlock();
    std::ranges::for_each(
        total_work_units_hook,

        [&](const total_work_units_hook_seq::value_type& _f)
        {
            _f(*this, old_total_work_units);
        });
}

//------------------------------------------------------------------------------

monitor::logs monitor::get_logs() const
{
    core::mt::read_lock lock(mutex());
    return m_logs;
}

//------------------------------------------------------------------------------

void monitor::done()
{
    this->done_work(m_total_work_units);
}

//------------------------------------------------------------------------------

std::uint64_t monitor::get_done_work_units_no_lock() const
{
    return m_done_work_units;
}

//------------------------------------------------------------------------------

std::uint64_t monitor::get_total_work_units_no_lock() const
{
    return m_total_work_units;
}

//------------------------------------------------------------------------------

} //namespace sight::core::notification
