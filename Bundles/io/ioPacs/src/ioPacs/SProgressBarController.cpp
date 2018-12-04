/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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
