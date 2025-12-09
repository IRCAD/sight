/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "core/progress/aggregator.hpp"

#include <core/spy_log.hpp>
#include <core/thread/worker.hpp>
#include <core/thread/worker.hxx>

#include <algorithm>

namespace sight::core::progress
{

//------------------------------------------------------------------------------

aggregator::aggregator(const std::string& _name) :
    monitor(_name)
{
    this->run();
}

//------------------------------------------------------------------------------

aggregator::~aggregator()
{
    this->finish();
}

//------------------------------------------------------------------------------

void aggregator::add(const core::progress::monitor::sptr& _progress, double _weight)
{
    SIGHT_ASSERT("progress shall not be null", _progress);

    SIGHT_ASSERT("progress shall not be added to itself", this != _progress.get());

    if(!_progress)
    {
        return;
    }

    core::mt::read_to_write_lock lock(m_mutex);
    SIGHT_ERROR_IF("A progress weight < 0.01 can lead to undetermined behavior", _weight < 0.01);

    const auto norm_value = std::uint64_t(_weight * 100);

    if(m_state == waiting || m_state == running)
    {
        m_progress_info[_progress.get()] = progress_info(*_progress);
        auto& progress_info = m_progress_info[_progress.get()];

        this->set_total_work_units_upgrade_lock(
            m_total_work_units + (progress_info.total_work != 0U ? norm_value : 0),
            lock
        );
        lock.lock();
        // done_work call after set_total_work_unitsUpgradeLock, because
        // done_work value can be thresholded by set_total_work_unitsUpgradeLock
        // call
        progress_info.last_value = std::uint64_t(progress_info.progress() * double(norm_value));
        {
            core::mt::upgrade_to_write_lock write_lock(lock);
            m_progress_seq.push_back(_progress);
        }
        // take care : done_work unlocks 'lock'
        this->done_work(progress_info.last_value, lock);

        // TODO : add a way to disconnect on aggregator destruction
        _progress->add_done_work_hook(
            [norm_value, &progress_info, weak_this = this->weak_from_this()](monitor& _sub_progress, std::uint64_t)
            {
                if(auto shared_this = weak_this.lock())
                {
                    auto shared = dynamic_pointer_cast<aggregator>(shared_this);
                    core::mt::read_to_write_lock sublock(shared->m_mutex);

                    auto old_info = progress_info;
                    progress_info = aggregator::progress_info(_sub_progress);

                    progress_info.last_value = std::uint64_t(progress_info.progress() * double(norm_value));

                    auto done_work = shared->m_done_work_units + progress_info.last_value;
                    // minimize numerical uncertainty by substracting in a second time :
                    done_work -= old_info.last_value;

                    shared->done_work(done_work, sublock);
                }
            });

        _progress->add_total_work_units_hook(
            [norm_value, weak_this = this->weak_from_this()](monitor& _sub_progress,
                                                             std::uint64_t _old_total_work_units)
            {
                if(auto shared_this = weak_this.lock())
                {
                    auto shared = dynamic_pointer_cast<aggregator>(shared_this);
                    core::mt::read_to_write_lock sublock(shared->m_mutex);

                    auto work_units           = shared->m_total_work_units;
                    auto new_total_work_units = _sub_progress.get_total_work_units();

                    if(_old_total_work_units != new_total_work_units)
                    {
                        if((_old_total_work_units != 0U) && 0 == new_total_work_units)
                        {
                            work_units -= norm_value;
                        }
                        else if(0 == _old_total_work_units && (new_total_work_units != 0U))
                        {
                            work_units += norm_value;
                        }
                    }

                    shared->set_total_work_units_upgrade_lock(work_units, sublock);
                }
            });

        this->add_cancel_hook_no_lock(
            [_progress]()
            {
                _progress->cancel();
            });

        auto i_progress_name = _progress->name();
        i_progress_name = i_progress_name.empty() ? "" : std::string("[") + i_progress_name + "] ";
        _progress->add_log_hook(
            [i_progress_name, weak_this = this->weak_from_this()](monitor& /* progress */, const std::string& _message)
            {
                if(auto shared_this = weak_this.lock())
                {
                    auto shared = std::dynamic_pointer_cast<aggregator>(shared_this);
                    shared->log(i_progress_name + _message);
                }
            });

        auto i_progress_logs = _progress->get_logs();
        std::ranges::for_each(
            i_progress_logs,
            [&](const logs::value_type& _message)
            {
                this->log_no_lock(i_progress_name + _message);
            });
    }
    else
    {
        SIGHT_THROW_EXCEPTION(
            core::exception("A progress can't be added when the aggregator is not in waiting or running state")
        );
    }
}

} //namespace sight::core::progress
