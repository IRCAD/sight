/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "progress_bar_controller.hpp"

#include <core/com/slots.hxx>

#include <service/macros.hpp>

#include <ui/qt/dialog/progress.hpp>

namespace sight::module::io::dimse
{

static const core::com::slots::key_t START_PROGRESS_SLOT  = "start_progress";
static const core::com::slots::key_t UPDATE_PROGRESS_SLOT = "update_progress";
static const core::com::slots::key_t STOP_PROGRESS_SLOT   = "stop_progress";

//------------------------------------------------------------------------------

progress_bar_controller::progress_bar_controller() noexcept
{
    new_slot(START_PROGRESS_SLOT, &progress_bar_controller::start_progress, this);
    new_slot(UPDATE_PROGRESS_SLOT, &progress_bar_controller::update_progress, this);
    new_slot(STOP_PROGRESS_SLOT, &progress_bar_controller::stop_progress, this);
}

//------------------------------------------------------------------------------

progress_bar_controller::~progress_bar_controller() noexcept =
    default;

//------------------------------------------------------------------------------

void progress_bar_controller::configuring()
{
}

//------------------------------------------------------------------------------

void progress_bar_controller::starting()
{
}

//------------------------------------------------------------------------------

void progress_bar_controller::updating()
{
}

//------------------------------------------------------------------------------

void progress_bar_controller::stopping()
{
}

//------------------------------------------------------------------------------

void progress_bar_controller::start_progress(std::string _id)
{
    core::mt::scoped_lock lock(m_mutex);
    m_progress_dialogs[_id] = std::make_shared<sight::ui::dialog::progress>();
}

//------------------------------------------------------------------------------

void progress_bar_controller::update_progress(std::string _id, float _percentage, std::string _message)
{
    core::mt::scoped_lock lock(m_mutex);
    if(m_progress_dialogs.find(_id) != m_progress_dialogs.end())
    {
        (*m_progress_dialogs[_id])(_percentage, _message);
        m_progress_dialogs[_id]->set_message(_message);
    }
    else
    {
        SIGHT_WARN("Trying to update a progress bar which is not started !");
    }
}

//------------------------------------------------------------------------------

void progress_bar_controller::stop_progress(std::string _id)
{
    core::mt::scoped_lock lock(m_mutex);
    m_progress_dialogs.erase(_id);
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::dimse.
