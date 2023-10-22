/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "io/http/config.hpp"

#include <core/exception.hpp>

namespace sight::io::http::exceptions
{

/**
 * @brief Base class of fwNetworkIO Exception
 */
class IO_HTTP_CLASS_API Base : public core::exception
{
public:

    /**
     * @brief Constructor
     * @param[in] message Exception message
     */
    IO_HTTP_API Base(const std::string& _message);

    /// Destructor
    ~Base() noexcept override;
};

} // namespace sight::io::http::exceptions
