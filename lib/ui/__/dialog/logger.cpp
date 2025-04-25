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

#include "ui/__/dialog/logger.hpp"

#include <core/thread/worker.hpp>
#include <core/thread/worker.hxx>

#include <functional>

namespace sight::ui::dialog
{

//-----------------------------------------------------------------------------

bool logger::show_logger_dialog(
    const std::string& _title,
    const std::string& _message,
    const core::log::logger::sptr& _logger
)
{
    // The construction itself must be run in the main thread, since it creates widgets.
    return core::thread::get_default_worker()->post_task<bool>(
        [_title, _message, _logger]
        {
            ui::dialog::logger dialog(_title, _message, _logger);
            return dialog.show();
        }).get();
}

//-----------------------------------------------------------------------------

logger::logger()
{
    core::thread::get_default_worker()->post_task<void>(
        [this]
        {
            ui::object::sptr gui_obj = ui::factory::make(logger_base::REGISTRY_KEY);
            m_implementation         = std::dynamic_pointer_cast<ui::dialog::logger_base>(gui_obj);
        }).wait();
}

//-----------------------------------------------------------------------------

logger::logger(const std::string& _title, const std::string& _message, const core::log::logger::sptr& _logger)
{
    core::thread::get_default_worker()->post_task<void>(
        [_title, _message, _logger, this]
        {
            ui::object::sptr gui_obj = ui::factory::make(logger_base::REGISTRY_KEY);
            m_implementation         = std::dynamic_pointer_cast<ui::dialog::logger_base>(gui_obj);
            m_implementation->set_title(_title);
            m_implementation->set_message(_message);
            m_implementation->set_logger(_logger);
        }).wait();
}

//-----------------------------------------------------------------------------

void logger::set_title(const std::string& _title)
{
    m_implementation->set_title(_title);
}

//-----------------------------------------------------------------------------

void logger::set_message(const std::string& _message)
{
    m_implementation->set_message(_message);
}

//-----------------------------------------------------------------------------

void logger::set_logger(const core::log::logger::sptr& _logger)
{
    m_implementation->set_logger(_logger);
}

//-----------------------------------------------------------------------------

bool logger::show()
{
    return core::thread::get_default_worker()->post_task<bool>(
        [this]
        {
            return m_implementation->show();
        }).get();
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::dialog
