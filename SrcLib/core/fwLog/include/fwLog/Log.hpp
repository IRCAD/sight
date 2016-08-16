/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWLOG_LOG_HPP__
#define __FWLOG_LOG_HPP__

#include "fwLog/config.hpp"

#include <fwCore/BaseObject.hpp>

#include <string>

namespace fwLog
{

/**
 * @brief Log
 * @brief Log class used to retrieve information to the user
 */
class FWLOG_CLASS_API Log : public ::fwCore::BaseObject
{
public:

    typedef enum
    {
        INFORMATION = 0,
        WARNING     = 5,
        CRITICAL    = 10
    } LevelType;

    /// Constructor
    FWLOG_API Log(LevelType level, const std::string& message);

    /// Destructor
    FWLOG_API ~Log();

    /// Set Log level
    void setLevel(LevelType level)
    {
        m_level = level;
    }

    /// Get Log level
    LevelType getLevel() const
    {
        return m_level;
    }

    /// Set Log message
    void setMessage(std::string message)
    {
        m_message = message;
    }

    // Get Log message
    const std::string getMessage() const
    {
        return m_message;
    }

protected:

    /// Log level
    LevelType m_level;

    /// Log message
    std::string m_message;


}; // class Logger

} // namespace fwLog

#endif // __FWLOG_LOG_HPP__
