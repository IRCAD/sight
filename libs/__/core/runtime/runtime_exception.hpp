/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "core/exception.hpp"

#include <exception>
#include <string>

namespace sight::core::runtime
{

/**
 * @brief   Defines the runtime exception class.
 *
 */
struct SIGHT_CORE_CLASS_API runtime_exception : public core::exception
{
    /**
     * @brief       Copy constructor.
     *
     * @param[in]   _exception   an exception instance to copy
     */
    SIGHT_CORE_API runtime_exception(const runtime_exception& _exception) noexcept;

    /**
     * @brief       Constructor.
     *
     * @param[in]   _message a string containing the exception message
     */
    SIGHT_CORE_API runtime_exception(const std::string& _message) noexcept;

    /**
     * @brief   Destructor : does nothing.
     */
    SIGHT_CORE_API ~runtime_exception() noexcept override;
};

} // namespace sight::core::runtime
