/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include "PoCDialog/LoggerDialogs.hpp"

#include <fwGui/dialog/LoggerDialog.hpp>

#include <fwQml/QmlEngine.hpp>

LoggerDialogs::LoggerDialogs()
{
    Q_EMIT errorChanged();
}

//------------------------------------------------------------------------------

LoggerDialogs::~LoggerDialogs()
{
}

//------------------------------------------------------------------------------

void LoggerDialogs::open()
{
    ::fwLog::Logger::sptr logger = ::fwLog::Logger::New();

    // Fill Logger
    for (int i = 0; i < m_errorLevel.size(); ++i)
    {
        const auto& level   = m_errorLevel.at(i).toStdString();
        const auto& message = m_errorMessage.at(i).toStdString();
        if (level == "Critical")
        {
            logger->critical(message);
        }
        else if (level == "Warning")
        {
            logger->warning(message);
        }
        else if (level == "Information")
        {
            logger->information(message);
        }
    }
    ::fwGui::dialog::LoggerDialog dialogFile(m_title.toStdString(), m_message.toStdString(), logger);

    dialogFile.show();
    this->m_hasError = false;
    Q_EMIT errorChanged();
}
