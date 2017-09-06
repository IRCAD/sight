/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "gui/editor/SJobBar.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwGui/dialog/ProgressDialog.hpp>

#include <fwServices/macros.hpp>

#include <fwThread/Worker.hxx>

namespace gui
{
namespace editor
{

fwServicesRegisterMacro( ::gui::editor::IDialogEditor, ::gui::editor::SJobBar, ::fwData::Object );

static const ::fwCom::Slots::SlotKeyType SHOW_JOB_SLOT      = "showJob";
static const ::fwCom::Signals::SignalKeyType STARTED_SIGNAL = "started";
static const ::fwCom::Signals::SignalKeyType ENDED_SIGNAL   = "ended";

//-----------------------------------------------------------------------------

SJobBar::SJobBar() noexcept
{
    newSlot( SHOW_JOB_SLOT, &SJobBar::showJob, this );

    m_sigStarted = newSignal< StartedSignalType >( STARTED_SIGNAL );
    m_sigEnded   = newSignal< EndedSignalType >( ENDED_SIGNAL );
}

//-----------------------------------------------------------------------------

SJobBar::~SJobBar() noexcept
{
}

//-----------------------------------------------------------------------------

void SJobBar::starting()
{
}

//-----------------------------------------------------------------------------

void SJobBar::stopping()
{
}

//-----------------------------------------------------------------------------

void SJobBar::info(std::ostream &_sstream )
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

void SJobBar::showJob( ::fwJobs::IJob::sptr iJob )
{
    ::fwGui::dialog::ProgressDialog::sptr progressDialog = ::fwGui::dialog::ProgressDialog::New();
    progressDialog->setTitle(iJob->getName());

    if(!iJob->isCancelable())
    {
        progressDialog->hideCancelButton();
    }

    iJob->addDoneWorkHook( [ = ](::fwJobs::IJob& job, std::uint64_t oldDoneWork)
            {
                std::string msg = (job.getLogs().empty()) ? "" : job.getLogs().back();
                (*progressDialog)( float(job.getDoneWorkUnits())/job.getTotalWorkUnits(), msg );
            });

    iJob->addStateHook( [ = ](::fwJobs::IJob::State state)
            {
                if(state == ::fwJobs::IJob::CANCELED || state == ::fwJobs::IJob::FINISHED )
                {
                    m_sigEnded->emit();
                    m_associatedWorker->postTask< void >( [ = ]
                    {
                        m_progressDialogs.erase(progressDialog);
                    });
                }
                else if( state == ::fwJobs::IJob::RUNNING)
                {
                    m_sigStarted->emit();
                }
            });

    ::fwJobs::IJob::wptr wIJob = iJob;
    progressDialog->setCancelCallback( [ = ]
            {
                ::fwJobs::IJob::sptr job = wIJob.lock();
                if(job)
                {
                    job->cancel();
                }
            });

    m_progressDialogs.insert(progressDialog);
}

//-----------------------------------------------------------------------------

} // namespace editor
} // namespace gui

