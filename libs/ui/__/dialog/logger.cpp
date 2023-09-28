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

bool logger::showLoggerDialog(
    const std::string& title,
    const std::string& message,
    const core::log::logger::sptr& logger
)
{
    // The construction itself must be run in the main thread, since it creates widgets.
    return core::thread::get_default_worker()->post_task<bool>(
        [title, message, logger]
        {
            ui::dialog::logger dialog(title, message, logger);
            return dialog.show();
        }).get();
}

//-----------------------------------------------------------------------------

logger::logger()
{
    core::thread::get_default_worker()->post_task<void>(
        [this]
        {
            ui::object::sptr guiObj = ui::factory::make(logger_base::REGISTRY_KEY);
            m_implementation        = std::dynamic_pointer_cast<ui::dialog::logger_base>(guiObj);
        }).wait();
}

//-----------------------------------------------------------------------------

logger::logger(const std::string& title, const std::string& message, const core::log::logger::sptr& logger)
{
    core::thread::get_default_worker()->post_task<void>(
        [title, message, logger, this]
        {
            ui::object::sptr guiObj = ui::factory::make(logger_base::REGISTRY_KEY);
            m_implementation        = std::dynamic_pointer_cast<ui::dialog::logger_base>(guiObj);
            m_implementation->setTitle(title);
            m_implementation->setMessage(message);
            m_implementation->setLogger(logger);
        }).wait();
}

//-----------------------------------------------------------------------------

void logger::setTitle(const std::string& title)
{
    m_implementation->setTitle(title);
}

//-----------------------------------------------------------------------------

void logger::setMessage(const std::string& message)
{
    m_implementation->setMessage(message);
}

//-----------------------------------------------------------------------------

void logger::setLogger(const core::log::logger::sptr& logger)
{
    m_implementation->setLogger(logger);
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
