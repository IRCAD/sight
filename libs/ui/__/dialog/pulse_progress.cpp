/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "ui/__/dialog/pulse_progress.hpp"

#include <core/thread/worker.hpp>

namespace sight::ui::dialog
{

//-----------------------------------------------------------------------------

pulse_progress::pulse_progress(
    const std::string& _title,
    ui::dialog::pulse_progress_base::Stuff _stuff,
    const std::string& _msg,
    ui::dialog::pulse_progress_base::MilliSecond _frequence_refresh
)
{
    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            ui::object::sptr gui_obj = ui::factory::make(pulse_progress_base::REGISTRY_KEY);
            m_implementation         = std::dynamic_pointer_cast<ui::dialog::pulse_progress_base>(gui_obj);
            if(m_implementation)
            {
                m_implementation->set_stuff(_stuff);
                m_implementation->set_title(_title);
                m_implementation->set_message(_msg);
                m_implementation->set_frequence(_frequence_refresh);
            }
            else
            {
                this->set_stuff(_stuff);
            }
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void pulse_progress::set_title(const std::string& _title)
{
    if(m_implementation)
    {
        m_implementation->set_title(_title);
    }
}

//-----------------------------------------------------------------------------

void pulse_progress::set_message(const std::string& _msg)
{
    if(m_implementation)
    {
        m_implementation->set_message(_msg);
    }
}

//-----------------------------------------------------------------------------

void pulse_progress::show()
{
    if(m_implementation)
    {
        core::thread::get_default_worker()->post_task<void>(
            [this](auto&& ...){m_implementation->show();}).wait();
    }
    else
    {
        m_stuff();
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::dialog
