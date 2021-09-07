/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "ui/base/dialog/LoggerDialog.hpp"

#include <core/thread/ActiveWorkers.hpp>
#include <core/thread/Worker.hpp>
#include <core/thread/Worker.hxx>

#include <functional>

namespace sight::ui::base
{

namespace dialog
{

//-----------------------------------------------------------------------------

bool LoggerDialog::showLoggerDialog(
    const std::string& title,
    const std::string& message,
    const core::log::Logger::sptr& logger
)
{
    ui::base::dialog::LoggerDialog dialog(title, message, logger);
    return dialog.show();
}

//-----------------------------------------------------------------------------

LoggerDialog::LoggerDialog()
{
    ui::base::GuiBaseObject::sptr guiObj = ui::base::factory::New(ILoggerDialog::REGISTRY_KEY);
    m_implementation = ui::base::dialog::ILoggerDialog::dynamicCast(guiObj);
}

//-----------------------------------------------------------------------------

LoggerDialog::~LoggerDialog()
{
}

//-----------------------------------------------------------------------------

LoggerDialog::LoggerDialog(const std::string& title, const std::string& message, const core::log::Logger::sptr& logger)
{
    ui::base::GuiBaseObject::sptr guiObj = ui::base::factory::New(ILoggerDialog::REGISTRY_KEY);
    m_implementation = ui::base::dialog::ILoggerDialog::dynamicCast(guiObj);
    m_implementation->setTitle(title);
    m_implementation->setMessage(message);
    m_implementation->setLogger(logger);
}

//-----------------------------------------------------------------------------

void LoggerDialog::setTitle(const std::string& title)
{
    m_implementation->setTitle(title);
}

//-----------------------------------------------------------------------------

void LoggerDialog::setMessage(const std::string& message)
{
    m_implementation->setMessage(message);
}

//-----------------------------------------------------------------------------

void LoggerDialog::setLogger(const core::log::Logger::sptr& logger)
{
    m_implementation->setLogger(logger);
}

//-----------------------------------------------------------------------------

bool LoggerDialog::show()
{
    if(m_implementation)
    {
        std::function<bool()> func = std::bind(&ILoggerDialog::show, m_implementation);
        std::shared_future<bool> f = core::thread::ActiveWorkers::getDefaultWorker()->postTask<bool>(func);
        f.wait();

        return f.get();
    }

    return false;
}

//-----------------------------------------------------------------------------

} //namespace dialog

} //namespace sight::ui::base
