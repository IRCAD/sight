/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCORE_LOG_SCOPEDMESSAGE_HPP_
#define _FWCORE_LOG_SCOPEDMESSAGE_HPP_

#include <string>

#include "fwCore/BaseObject.hpp"
#include "fwCore/HiResTimer.hpp"
#include "fwCore/config.hpp"

namespace fwCore
{
namespace log
{

class SpyLogger;

/**
 * @brief Scoped messages.
 *
 * Generate a scoped trace message with an enter and a leave message, and the
 * time elapsed between the two messages.
 *
 * 
 */
class FWCORE_CLASS_API ScopedMessage: public ::fwCore::BaseObject
{

public:

    FWCORE_API ScopedMessage( const char * _file, int _line, std::string enterMessage, std::string leaveMessage = "");

    void use(){}; // avoid compiler warnings like "variable X never used"

    FWCORE_API virtual ~ScopedMessage();

protected:

    const char * m_file;
    int          m_line;
    std::string  m_baseMsg;
    std::string  m_leave;

    ::fwCore::HiResTimer  m_timer;
};

} // namespace log
} // namespace fwCore

#endif // _FWCORE_LOG_SCOPEDMESSAGE_HPP_
