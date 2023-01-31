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

#include <core/com/Signal.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>
#include <core/thread/Worker.hxx>

#include <service/macros.hpp>

#include <ui/base/dialog/ProgressDialog.hpp>

namespace sight::module::ui::base
{

static const core::com::Slots::SlotKeyType SHOW_JOB_SLOT      = "showJob";
static const core::com::Signals::SignalKeyType STARTED_SIGNAL = "started";
static const core::com::Signals::SignalKeyType ENDED_SIGNAL   = "ended";

//-----------------------------------------------------------------------------

SJobBar::SJobBar() noexcept
{
    newSlot(SHOW_JOB_SLOT, &SJobBar::showJob, this);

    m_sigStarted = newSignal<StartedSignalType>(STARTED_SIGNAL);
    m_sigEnded   = newSignal<EndedSignalType>(ENDED_SIGNAL);
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

void SJobBar::showJob(core::jobs::IJob::sptr iJob)
{
    auto progressDialog = sight::ui::base::dialog::ProgressDialog::New();
    progressDialog->setTitle(iJob->getName());

    if(!iJob->isCancelable())
    {
        progressDialog->hideCancelButton();
    }

    iJob->addDoneWorkHook(
        [ = ](core::jobs::IJob& job, std::uint64_t)
        {
            std::string msg = (job.getLogs().empty()) ? "" : job.getLogs().back();
            (*progressDialog)(float(job.getDoneWorkUnits()) / float(job.getTotalWorkUnits()), msg);
        });

    iJob->addStateHook(
        [ =, this](core::jobs::IJob::State state)
        {
            if(state == core::jobs::IJob::CANCELED || state == core::jobs::IJob::FINISHED)
            {
                m_sigEnded->emit();
                this->worker()->postTask<void>(
                    [ =, this]
                {
                    m_progressDialogs.erase(progressDialog);
                });
            }
            else if(state == core::jobs::IJob::RUNNING)
            {
                m_sigStarted->emit();
            }
        });

    core::jobs::IJob::wptr wIJob = iJob;
    progressDialog->setCancelCallback(
        [ = ]
        {
            core::jobs::IJob::sptr job = wIJob.lock();
            if(job)
            {
                job->cancel();
            }
        });

    m_progressDialogs.insert(progressDialog);
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::base
