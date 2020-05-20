/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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
