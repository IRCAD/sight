/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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
#include "core/Exception.hpp"

namespace sight::core::runtime
{

/// Exception thrown when the application is exiting
class CORE_CLASS_API ExitException : public core::Exception
{
public:

    inline explicit ExitException(int exit_code) noexcept :
        core::Exception(std::to_string(exit_code)),
        m_exit_code(exit_code)
    {
    }

    ~ExitException() override = default;

    //------------------------------------------------------------------------------

    constexpr int exitCode() const noexcept
    {
        return m_exit_code;
    }

private:

    int m_exit_code {0};
};

} // namespace sight::core::runtime
