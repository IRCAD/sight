/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include <sstream>

#include "fwCore/log/SpyLogger.hpp"
#include "fwCore/log/ScopedMessage.hpp"

namespace fwCore
{
namespace log
{

// Msg Format :
// [ENTERING SCOPE] "Timed{'file':m_file, 'line':m_line, 'entermessage':enterMessage}"
// [LEAVING SCOPE] "Timed{'file':m_file, 'line':m_line, 'leavemessage':m_leave, 'elapsed':elapsedTime}"

//-----------------------------------------------------------------------------

ScopedMessage::ScopedMessage( const char * _file, int _line, std::string enterMessage, std::string leaveMessage)
    : m_file(_file), m_line(_line), m_leave(leaveMessage.empty() ? enterMessage : leaveMessage)
{
    std::stringstream base;
    base << "Timed{'file':'"<< m_file <<"','line':"<< m_line <<",";
    m_baseMsg = base.str();

    std::stringstream oslStr;
    oslStr << "[ENTERING SCOPE] "<<  m_baseMsg << "'entermessage':'" <<  enterMessage << "'}";
    ::fwCore::log::SpyLogger::getSpyLogger().trace(oslStr.str(), m_file, m_line);
    m_timer.start();
}

//-----------------------------------------------------------------------------

ScopedMessage::~ScopedMessage()
{
    std::stringstream oslStr;
    oslStr << "[LEAVING SCOPE] "<<  m_baseMsg << "'leavemessage':'" << m_leave <<"','elapsed':"<<
        m_timer.getElapsedTimeInMilliSec() << "}";
    SpyLogger::getSpyLogger().trace(oslStr.str(), m_file, m_line);
}

} // namespace log
} // namespace fwCore

