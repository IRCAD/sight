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

#include "io/dimse/config.hpp"
#include "io/dimse/exceptions/Base.hpp"

namespace sight::io::dimse
{
namespace exceptions
{

/**
 * @brief RequestFailure Exceptions
 */
class IO_DIMSE_CLASS_API RequestFailure : public sight::io::dimse::exceptions::Base
{
public:
    /**
     * @brief Constructor
     * @param[in] message Exception message
     */
    IO_DIMSE_API RequestFailure(const std::string& message);

    /// Destructor
    IO_DIMSE_API virtual ~RequestFailure() noexcept;

};

} //exceptions
} //fwPacsIO
