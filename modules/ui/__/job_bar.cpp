/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "job_bar.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>
#include <core/thread/worker.hxx>

#include <service/macros.hpp>

#include <ui/__/dialog/progress.hpp>

namespace sight::module::ui
{

static const core::com::slots::key_t SHOW_JOB_SLOT    = "show_job";
static const core::com::signals::key_t STARTED_SIGNAL = "started";
static const core::com::signals::key_t ENDED_SIGNAL   = "ended";

//-----------------------------------------------------------------------------

job_bar::job_bar() noexcept
{
    new_slot(SHOW_JOB_SLOT, &job_bar::show_job, this);

    m_sig_started = new_signal<started_signal_t>(STARTED_SIGNAL);
    m_sig_ended   = new_signal<ended_signal_t>(ENDED_SIGNAL);
}

//-----------------------------------------------------------------------------

void job_bar::starting()
{
}

//-----------------------------------------------------------------------------

void job_bar::stopping()
{
    m_progress_dialogs.clear();
}

//-----------------------------------------------------------------------------

void job_bar::info(std::ostream& _sstream)
{
    _sstream << "Starter editor" << std::endl;
}

//-----------------------------------------------------------------------------

void job_bar::updating()
{
}

//-----------------------------------------------------------------------------

void job_bar::configuring()
{
}

//-----------------------------------------------------------------------------

void job_bar::show_job(core::jobs::base::sptr _job)
{
    auto progress_dialog = std::make_shared<sight::ui::dialog::progress>();
    progress_dialog->set_title(_job->name());

    std::weak_ptr<sight::ui::dialog::progress> weak_progress_dialog = progress_dialog;

    if(!_job->is_cancelable())
    {
        progress_dialog->hide_cancel_button();
    }

    _job->add_done_work_hook(
        [weak_progress_dialog](core::jobs::base& _job, std::uint64_t)
        {
            if(auto progress_dialog = weak_progress_dialog.lock(); progress_dialog)
            {
                std::string msg = (_job.get_logs().empty()) ? "" : _job.get_logs().back();
                (*progress_dialog)(float(_job.get_done_work_units()) / float(_job.get_total_work_units()), msg);
            }
        });

    _job->add_state_hook(
        [weak_progress_dialog, this](core::jobs::base::state _state)
        {
            if(_state == core::jobs::base::canceled || _state == core::jobs::base::finished)
            {
                m_sig_ended->emit();
                this->worker()->post_task<void>(
                    [weak_progress_dialog, this]
                {
                    if(auto progress_dialog = weak_progress_dialog.lock(); progress_dialog)
                    {
                        m_progress_dialogs.erase(progress_dialog);
                    }
                });
            }
            else if(_state == core::jobs::base::running)
            {
                m_sig_started->emit();
            }
        });

    core::jobs::base::wptr weak_job = _job;
    progress_dialog->set_cancel_callback(
        [weak_job]
        {
            core::jobs::base::sptr job = weak_job.lock();
            if(job)
            {
                job->cancel();
            }
        });

    m_progress_dialogs.insert(progress_dialog);
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui
