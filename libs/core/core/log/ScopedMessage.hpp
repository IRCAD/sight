/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#pragma once

#include "core/BaseObject.hpp"
#include "core/config.hpp"
#include "core/HiResTimer.hpp"

#include <string>

namespace sight::core
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
class CORE_CLASS_API ScopedMessage : public core::BaseObject
{
public:

    CORE_API ScopedMessage(const char* _file, int _line, std::string enterMessage, std::string leaveMessage = "");

    //------------------------------------------------------------------------------

    void use()
    {
    } // avoid compiler warnings like "variable X never used"

    CORE_API virtual ~ScopedMessage();

protected:

    const char* m_file;
    int m_line;
    std::string m_baseMsg;
    std::string m_leave;

    core::HiResTimer m_timer;
};

} // namespace log

} // namespace sight::core
