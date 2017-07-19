/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioPacs/SProgressBarController.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>
#include <fwData/Composite.hpp>
#include <fwGuiQt/container/QtContainer.hpp>
#include <fwGuiQt/dialog/ProgressDialog.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <boost/foreach.hpp>

namespace ioPacs
{

fwServicesRegisterMacro( ::fwServices::IController, ::ioPacs::SProgressBarController, ::fwData::Composite );

static const ::fwCom::Slots::SlotKeyType s_START_PROGRESS_SLOT  = "startProgress";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_PROGRESS_SLOT = "updateProgress";
static const ::fwCom::Slots::SlotKeyType s_STOP_PROGRESS_SLOT   = "stopProgress";

//------------------------------------------------------------------------------

SProgressBarController::SProgressBarController() noexcept
{
    newSlot(s_START_PROGRESS_SLOT, &SProgressBarController::startProgress, this);
    newSlot(s_UPDATE_PROGRESS_SLOT, &SProgressBarController::updateProgress, this);
    newSlot(s_STOP_PROGRESS_SLOT, &SProgressBarController::stopProgress, this);
}
//------------------------------------------------------------------------------

SProgressBarController::~SProgressBarController() noexcept
{
}

//------------------------------------------------------------------------------

void SProgressBarController::info(std::ostream& _sstream )
{
    _sstream << "SProgressBarController::info";
}

//------------------------------------------------------------------------------

void SProgressBarController::starting()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SProgressBarController::stopping()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SProgressBarController::configuring()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SProgressBarController::updating()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SProgressBarController::startProgress(std::string id)
{
    ::fwCore::mt::ScopedLock lock(m_mutex);
    m_progressDialogs[id] = ::fwGui::dialog::ProgressDialog::New();
}

//------------------------------------------------------------------------------

void SProgressBarController::updateProgress(std::string id, float percentage, std::string message)
{
    ::fwCore::mt::ScopedLock lock(m_mutex);
    if(m_progressDialogs.find(id) != m_progressDialogs.end())
    {
        (*m_progressDialogs[id])(percentage, message);
        m_progressDialogs[id]->setMessage(message);
    }
    else
    {
        SLM_WARN("Trying to update a progress bar which is not started !");
    }

}

//------------------------------------------------------------------------------

void SProgressBarController::stopProgress(std::string id)
{
    ::fwCore::mt::ScopedLock lock(m_mutex);
    m_progressDialogs.erase(id);
}

//------------------------------------------------------------------------------

} // namespace ioPacs
