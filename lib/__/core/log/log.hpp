/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include <sight/core/config.hpp>

#include <cstdint>
#include <string>

namespace sight::core::log
{

/**
 * @brief Log
 * @brief Log class used to retrieve information to the user
 */
class SIGHT_CORE_CLASS_API log final
{
public:

    enum class level_t : std::uint8_t
    {
        information = 0,
        warning     = 5,
        critical    = 10
    };

    /// Constructor
    SIGHT_CORE_API log(level_t _level, std::string _message);

    /// Destructor
    SIGHT_CORE_API ~log();

    /// Set Log level
    void set_level(level_t _level)
    {
        m_level = _level;
    }

    /// Get Log level
    [[nodiscard]] level_t get_level() const
    {
        return m_level;
    }

    /// Set Log message
    void set_message(std::string _message)
    {
        m_message = _message;
    }

    // Get Log message
    [[nodiscard]] std::string get_message() const
    {
        return m_message;
    }

private:

    /// Log level
    level_t m_level;

    /// Log message
    std::string m_message;
};

} // namespace sight::core::log
