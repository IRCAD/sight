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

#include "SJobBar.hpp"

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

SJobBar::SJobBar() noexcept
{
    new_slot(SHOW_JOB_SLOT, &SJobBar::showJob, this);

    m_sigStarted = new_signal<StartedSignalType>(STARTED_SIGNAL);
    m_sigEnded   = new_signal<EndedSignalType>(ENDED_SIGNAL);
}

//-----------------------------------------------------------------------------

SJobBar::~SJobBar() noexcept =
    default;

//-----------------------------------------------------------------------------

void SJobBar::starting()
{
}

//-----------------------------------------------------------------------------

void SJobBar::stopping()
{
}

//-----------------------------------------------------------------------------

void SJobBar::info(std::ostream& _sstream)
{
    _sstream << "Starter editor" << std::endl;
}

//-----------------------------------------------------------------------------

void SJobBar::updating()
{
}

//-----------------------------------------------------------------------------

void SJobBar::configuring()
{
}

//-----------------------------------------------------------------------------

void SJobBar::showJob(core::jobs::base::sptr job)
{
    auto progressDialog = std::make_shared<sight::ui::dialog::progress>();
    progressDialog->setTitle(job->name());

    if(!job->is_cancelable())
    {
        progressDialog->hideCancelButton();
    }

    job->add_done_work_hook(
        [ = ](core::jobs::base& job, std::uint64_t)
        {
            std::string msg = (job.get_logs().empty()) ? "" : job.get_logs().back();
            (*progressDialog)(float(job.get_done_work_units()) / float(job.get_total_work_units()), msg);
        });

    job->add_state_hook(
        [progressDialog, this](core::jobs::base::state state)
        {
            if(state == core::jobs::base::CANCELED || state == core::jobs::base::FINISHED)
            {
                m_sigEnded->emit();
                this->worker()->post_task<void>(
                    [progressDialog, this]
                {
                    m_progressDialogs.erase(progressDialog);
                });
            }
            else if(state == core::jobs::base::RUNNING)
            {
                m_sigStarted->emit();
            }
        });

    core::jobs::base::wptr wIJob = job;
    progressDialog->setCancelCallback(
        [ = ]
        {
            core::jobs::base::sptr job = wIJob.lock();
            if(job)
            {
                job->cancel();
            }
        });

    m_progressDialogs.insert(progressDialog);
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui
