/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

static const core::com::slots::key_t SHOW_JOB_SLOT    = "showJob";
static const core::com::signals::key_t STARTED_SIGNAL = "started";
static const core::com::signals::key_t ENDED_SIGNAL   = "ended";

//-----------------------------------------------------------------------------

job_bar::job_bar() noexcept
{
    new_slot(SHOW_JOB_SLOT, &job_bar::showJob, this);

    m_sigStarted = new_signal<started_signal_t>(STARTED_SIGNAL);
    m_sigEnded   = new_signal<ended_signal_t>(ENDED_SIGNAL);
}

//-----------------------------------------------------------------------------

job_bar::~job_bar() noexcept =
    default;

//-----------------------------------------------------------------------------

void job_bar::starting()
{
}

//-----------------------------------------------------------------------------

void job_bar::stopping()
{
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

void job_bar::showJob(core::jobs::base::sptr _job)
{
    auto progress_dialog = std::make_shared<sight::ui::dialog::progress>();
    progress_dialog->setTitle(_job->name());

    if(!_job->is_cancelable())
    {
        progress_dialog->hideCancelButton();
    }

    _job->add_done_work_hook(
        [ = ](core::jobs::base& _job, std::uint64_t)
        {
            std::string msg = (_job.get_logs().empty()) ? "" : _job.get_logs().back();
            (*progress_dialog)(float(_job.get_done_work_units()) / float(_job.get_total_work_units()), msg);
        });

    _job->add_state_hook(
        [progress_dialog, this](core::jobs::base::state _state)
        {
            if(_state == core::jobs::base::CANCELED || _state == core::jobs::base::FINISHED)
            {
                m_sigEnded->emit();
                this->worker()->post_task<void>(
                    [progress_dialog, this]
                {
                    m_progressDialogs.erase(progress_dialog);
                });
            }
            else if(_state == core::jobs::base::RUNNING)
            {
                m_sigStarted->emit();
            }
        });

    core::jobs::base::wptr w_i_job = _job;
    progress_dialog->setCancelCallback(
        [ = ]
        {
            core::jobs::base::sptr job = w_i_job.lock();
            if(job)
            {
                job->cancel();
            }
        });

    m_progressDialogs.insert(progress_dialog);
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui
