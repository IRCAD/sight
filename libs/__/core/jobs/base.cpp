/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "core/jobs/base.hpp"

#include "core/jobs/exception/waiting.hpp"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/signals.hpp>
#include <core/thread/worker.hpp>
#include <core/thread/worker.hxx>

#include <algorithm>
#include <utility>

namespace sight::core::jobs
{

base::base(std::string _name) :
    m_sig_cancel_requested(std::make_shared<cancel_requested_signal>()),
    m_sig_canceled(std::make_shared<state_signal>()),
    m_sig_started(std::make_shared<state_signal>()),
    m_sig_finished(std::make_shared<state_signal>()),
    m_sig_done_work(std::make_shared<done_work_signal>()),
    m_sig_logged(std::make_shared<log_signal>()),
    m_name(std::move(_name))
{
}

//------------------------------------------------------------------------------

base::~base()
{
    std::for_each(
        m_state_hooks.begin(),
        m_state_hooks.end(),
        [&](const state_hook_seq::value_type& _f)
        {
            _f(m_state);
        });
}

//------------------------------------------------------------------------------

bool base::cancel_requested() const
{
    core::mt::read_lock lock(m_mutex);
    return m_cancel_requested;
}

//------------------------------------------------------------------------------

base::cancel_request_callback base::cancel_requested_callback() const
{
    return [this]() -> bool
           {
               core::mt::read_lock lock(m_mutex);
               return m_cancel_requested;
           };
}

//------------------------------------------------------------------------------

std::uint64_t base::get_done_work_units() const
{
    core::mt::read_lock lock(m_mutex);
    return m_done_work_units;
}

//------------------------------------------------------------------------------

std::uint64_t base::get_total_work_units() const
{
    core::mt::read_lock lock(m_mutex);
    return m_total_work_units;
}

//------------------------------------------------------------------------------

base::shared_future base::cancel()
{
    core::mt::read_to_write_lock lock(m_mutex);
    if(m_cancelable && (m_state == waiting || m_state == running))
    {
        state next_state = (m_state == waiting) ? canceled : canceling;

        {
            core::mt::upgrade_to_write_lock write_lock(lock);
            m_cancel_requested = true;
            this->set_state_no_lock(canceling);
        }

        // unlock mutex for cancel callbacks sanity
        lock.unlock();
        // m_cancelHooks can not be changed when m_state != WAITING or
        // RUNNING, no need to lock m_mutex
        for(const auto& cancel : m_cancel_hooks)
        {
            (cancel) (*this);
        }

        lock.lock();

        core::mt::upgrade_to_write_lock write_lock(lock);

        SIGHT_ASSERT(
            "State shall be only CANCELING or CANCELED, not " << m_state,
            m_state == canceled || m_state == canceling
        );

        if(m_state == canceling)
        {
            this->set_state_no_lock(next_state);

            if(next_state == canceled && !m_run_future.valid())
            {
                // If we use the default constructor, the future is not valid and we will not be able to wait for it
                // Thus we build a dummy future to get a valid one
                m_run_future = std::async([](){});
            }
        }
    }

    // else if (m_state == CANCELING)
    // NOP
    return m_run_future;
}

//------------------------------------------------------------------------------

void base::add_cancel_hook(job_cancel_hook _callback)
{
    core::mt::write_lock lock(m_mutex);
    this->add_cancel_hook_no_lock(_callback);
}

//------------------------------------------------------------------------------

void base::add_simple_cancel_hook(cancel_hook _callback)
{
    this->add_cancel_hook(
        job_cancel_hook(
            [ = ](base& /*job*/)
        {
            _callback();
        })
    );
}

//------------------------------------------------------------------------------

void base::add_done_work_hook(done_work_hook _callback)
{
    core::mt::write_lock lock(m_mutex);
    this->add_done_work_hook_no_lock(_callback);
}

//------------------------------------------------------------------------------

void base::add_total_work_units_hook(total_work_units_hook _callback)
{
    core::mt::write_lock lock(m_mutex);
    this->add_total_work_units_hook_no_lock(_callback);
}

//------------------------------------------------------------------------------

void base::add_log_hook(log_hook _callback)
{
    core::mt::write_lock lock(m_mutex);
    this->add_log_hook_no_lock(_callback);
}

//------------------------------------------------------------------------------

void base::add_state_hook(state_hook _callback)
{
    core::mt::write_lock lock(m_mutex);
    this->add_state_hook_no_lock(_callback);
}

//------------------------------------------------------------------------------

base::state base::get_state() const
{
    core::mt::read_lock lock(m_mutex);
    return m_state;
}

//------------------------------------------------------------------------------

const std::string& base::name() const
{
    return m_name; //Does not need to lock mutex, the name can't be changed
}

//------------------------------------------------------------------------------

base::shared_future base::run()
{
    core::mt::read_to_write_lock lock(m_mutex);

    if(m_state == waiting)
    {
        {
            core::mt::upgrade_to_write_lock write_lock(lock);
            this->set_state_no_lock(running);
        }

        lock.unlock();
        auto future = this->run_impl();
        lock.lock();

        core::mt::upgrade_to_write_lock write_lock(lock);
        m_run_future = future;
    }

    return m_run_future;
}

//------------------------------------------------------------------------------

void base::set_state(base::state _state)
{
    core::mt::write_lock lock(m_mutex);
    this->set_state_no_lock(_state);
}

//------------------------------------------------------------------------------

void base::set_state_no_lock(base::state _state)
{
    m_state = _state;
    switch(_state)
    {
        case waiting:
            break;

        case running:
            m_sig_started->async_emit();
            break;

        case canceling:
            m_sig_cancel_requested->async_emit();
            break;

        case canceled:
            m_sig_canceled->async_emit();
            break;

        case finished:
            m_sig_finished->async_emit();
            break;

        default:
            SIGHT_ASSERT("You shall not pass !", 0);
    }

    std::for_each(
        m_state_hooks.begin(),
        m_state_hooks.end(),
        [&](const state_hook_seq::value_type& _f)
        {
            _f(m_state);
        });
}

//------------------------------------------------------------------------------

void base::finish()
{
    core::mt::write_lock lock(m_mutex);
    this->finish_no_lock();
}

//------------------------------------------------------------------------------

void base::finish_no_lock()
{
    this->set_state_no_lock((m_state == canceling) ? canceled : finished);
}

//------------------------------------------------------------------------------

void base::wait()
{
    decltype(m_run_future) run_future;

    {
        core::mt::read_lock lock(m_mutex);
        run_future = m_run_future;
    }

    // JobTest::APIAndStateTest() crash on macOS if we don't test the validity of the future before calling wait()
    // No std::future_error are raised before the segfault
    if(!run_future.valid())
    {
        SIGHT_THROW_EXCEPTION(core::jobs::exception::waiting("Job has not been started"));
    }

    try
    {
        run_future.wait();
    }
    catch(std::future_error&)
    {
        SIGHT_THROW_EXCEPTION(core::jobs::exception::waiting("Job has not been started"));
    }
}

//------------------------------------------------------------------------------

void base::log(const std::string& _message)
{
    core::mt::write_lock lock(m_mutex);
    this->log_no_lock(_message);
}

//------------------------------------------------------------------------------

void base::log_no_lock(const std::string& _message)
{
    m_logs.push_back(_message);

    m_sig_logged->async_emit(_message);

    std::for_each(
        m_log_hooks.begin(),
        m_log_hooks.end(),
        [&](const log_hook_seq::value_type& _f)
        {
            _f(*this, _message);
        });
}

//------------------------------------------------------------------------------

base::state base::get_state_no_lock() const
{
    return m_state;
}

//------------------------------------------------------------------------------

void base::add_cancel_hook_no_lock(job_cancel_hook _callback)
{
    if(m_state == waiting || m_state == running)
    {
        m_cancel_hooks.push_back(_callback);
    }
}

//------------------------------------------------------------------------------

void base::add_done_work_hook_no_lock(done_work_hook _callback)
{
    if(m_state == waiting || m_state == running)
    {
        m_done_work_hooks.push_back(_callback);
    }
}

//------------------------------------------------------------------------------

void base::add_total_work_units_hook_no_lock(total_work_units_hook _callback)
{
    if(m_state == waiting || m_state == running)
    {
        m_total_work_units_hooks.push_back(_callback);
    }
}

//------------------------------------------------------------------------------

void base::add_log_hook_no_lock(log_hook _callback)
{
    m_log_hooks.push_back(_callback);
}

//------------------------------------------------------------------------------

void base::add_state_hook_no_lock(state_hook _callback)
{
    m_state_hooks.push_back(_callback);
}

//------------------------------------------------------------------------------

void base::done_work(std::uint64_t _units)
{
    SIGHT_ASSERT("Can not report done work while the job is not running.", m_state == running || m_state == canceling);
    core::mt::read_to_write_lock lock(m_mutex);
    this->done_work(_units, lock);
}

//------------------------------------------------------------------------------

void base::done_work(std::uint64_t _units, core::mt::read_to_write_lock& _lock)
{
    auto old_done_work = m_done_work_units;
    decltype(m_done_work_hooks) done_work_hooks;

    if(m_done_work_units == _units)
    {
        return;
    }

    done_work_hooks = m_done_work_hooks;

    {
        core::mt::upgrade_to_write_lock write_lock(_lock);
        m_done_work_units = _units;
    }

    m_sig_done_work->async_emit(m_done_work_units, m_total_work_units);

    _lock.unlock();

    std::for_each(
        done_work_hooks.begin(),
        done_work_hooks.end(),
        [&](const done_work_hook_seq::value_type& _f)
        {
            _f(*this, old_done_work);
        });
}

//------------------------------------------------------------------------------

void base::set_total_work_units(std::uint64_t _units)
{
    core::mt::read_to_write_lock lock(m_mutex);
    this->set_total_work_units_upgrade_lock(_units, lock);
}

//------------------------------------------------------------------------------

void base::set_total_work_units_upgrade_lock(std::uint64_t _units, core::mt::read_to_write_lock& _lock)
{
    auto old_total_work_units = m_total_work_units;
    decltype(m_total_work_units_hooks) total_work_units_hook;

    total_work_units_hook = m_total_work_units_hooks;

    if(m_done_work_units > _units)
    {
        this->done_work(_units, _lock);
    }

    {
        core::mt::upgrade_to_write_lock write_lock(_lock);
        m_total_work_units = _units;
    }

    _lock.unlock();
    std::for_each(
        total_work_units_hook.begin(),
        total_work_units_hook.end(),
        [&](const total_work_units_hook_seq::value_type& _f)
        {
            _f(*this, old_total_work_units);
        });
}

//------------------------------------------------------------------------------

base::logs base::get_logs() const
{
    core::mt::read_lock lock(m_mutex);
    return m_logs;
}

//------------------------------------------------------------------------------

void base::set_cancelable(bool _cancel)
{
    core::mt::write_lock lock(m_mutex);
    m_cancelable = _cancel;
}

//------------------------------------------------------------------------------

bool base::is_cancelable() const
{
    core::mt::read_lock lock(m_mutex);
    return m_cancelable;
}

//------------------------------------------------------------------------------

void base::done()
{
    this->done_work(m_total_work_units);
}

//------------------------------------------------------------------------------

} //namespace sight::core::jobs
