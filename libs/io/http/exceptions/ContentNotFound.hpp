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
#include "io/http/exceptions/Base.hpp"

namespace sight::io::http::exceptions
{

/**
 * @brief Implements exception for HTTP content not found errors.
 */
class IO_HTTP_CLASS_API ContentNotFound : public sight::io::http::exceptions::Base
{
public:

    /**
     * @name Constructor/Destructor.
     * @{ */
    IO_HTTP_API ContentNotFound(const std::string& _message);
    ~ContentNotFound() noexcept override;
    /**  @} */
};

} // namespace sight::io::http::exceptions
