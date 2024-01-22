/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "core/exception.hpp"

#include "data/config.hpp"

#include <exception>
#include <string>

namespace sight::data
{

/**
 * @brief   Implements data exception class.
 */
class DATA_CLASS_API exception : public core::exception
{
public:

    /**
     * @brief       Constructor
     *
     * @param[in]   _message a string containing the failure message
     */
    DATA_API exception(const std::string& _message) noexcept;

    /**
     * @brief   Destructor : do nothing.
     */
    DATA_API ~exception() noexcept override = default;
};

} // namespace sight::data
