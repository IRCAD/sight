/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "core/config.hpp"
#include "core/exception.hpp"

#include <exception>
#include <string>

namespace sight::core::runtime
{

/**
 * @brief   Defines the runtime exception class.
 *
 */
struct CORE_CLASS_API runtime_exception : public core::exception
{
    /**
     * @brief       Copy constructor.
     *
     * @param[in]   exception   an exception instance to copy
     */
    CORE_API runtime_exception(const runtime_exception& exception) noexcept;

    /**
     * @brief       Constructor.
     *
     * @param[in]   message a string containing the exception message
     */
    CORE_API runtime_exception(const std::string& message) noexcept;

    /**
     * @brief   Destructor : does nothing.
     */
    CORE_API ~runtime_exception() noexcept override;
};

} // namespace sight::core::runtime
