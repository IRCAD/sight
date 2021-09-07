/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#pragma once

#include "core/config.hpp"
#include <core/BaseObject.hpp>

#include <string>

namespace sight::core::log
{

/**
 * @brief Log
 * @brief Log class used to retrieve information to the user
 */
class CORE_CLASS_API Log : public core::BaseObject
{
public:

    typedef enum
    {
        INFORMATION = 0,
        WARNING     = 5,
        CRITICAL    = 10
    } LevelType;

    /// Constructor
    CORE_API Log(LevelType level, const std::string& message);

    /// Destructor
    CORE_API ~Log();

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

} // namespace sight::core::log
