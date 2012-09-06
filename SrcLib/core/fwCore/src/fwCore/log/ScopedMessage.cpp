/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
    : m_file(_file), m_line(_line)
{
    m_leave = leaveMessage.empty() ? enterMessage : leaveMessage;
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
    oslStr << "[LEAVING SCOPE] "<<  m_baseMsg << "'leavemessage':'" << m_leave <<"','elapsed':"<<  m_timer.getElapsedTimeInMilliSec() << "}";
    m_logger->trace(oslStr.str(), m_file, m_line);
}

} // namespace log
} // namespace fwCore

