/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <fwCom/Slots.hxx>

#include <fwGuiQt/dialog/ProgressDialog.hpp>

#include <fwServices/macros.hpp>

namespace ioPacs
{

static const ::fwCom::Slots::SlotKeyType s_START_PROGRESS_SLOT  = "startProgress";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_PROGRESS_SLOT = "updateProgress";
static const ::fwCom::Slots::SlotKeyType s_STOP_PROGRESS_SLOT   = "stopProgress";

fwServicesRegisterMacro( ::fwServices::IController, ::ioPacs::SProgressBarController)

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

void SProgressBarController::configuring()
{
}

//------------------------------------------------------------------------------

void SProgressBarController::starting()
{
}

//------------------------------------------------------------------------------

void SProgressBarController::updating()
{
}

//------------------------------------------------------------------------------

void SProgressBarController::stopping()
{
}

//------------------------------------------------------------------------------

void SProgressBarController::startProgress(std::string _id)
{
    ::fwCore::mt::ScopedLock lock(m_mutex);
    m_progressDialogs[_id] = ::fwGui::dialog::ProgressDialog::New();
}

//------------------------------------------------------------------------------

void SProgressBarController::updateProgress(std::string _id, float _percentage, std::string _message)
{
    ::fwCore::mt::ScopedLock lock(m_mutex);
    if(m_progressDialogs.find(_id) != m_progressDialogs.end())
    {
        (*m_progressDialogs[_id])(_percentage, _message);
        m_progressDialogs[_id]->setMessage(_message);
    }
    else
    {
        SLM_WARN("Trying to update a progress bar which is not started !");
    }
}

//------------------------------------------------------------------------------

void SProgressBarController::stopProgress(std::string _id)
{
    ::fwCore::mt::ScopedLock lock(m_mutex);
    m_progressDialogs.erase(_id);
}

//------------------------------------------------------------------------------

} // namespace ioPacs.
