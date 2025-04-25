/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/io/dimse/config.hpp>

#include "io/dimse/exceptions/base.hpp"

namespace sight::io::dimse::exceptions
{

/**
 * @brief TagMissing exceptions
 */
class SIGHT_IO_DIMSE_CLASS_API tag_missing : public sight::io::dimse::exceptions::base
{
public:

    /**
     * @brief Constructor
     * @param[in] _message Exception message
     */
    SIGHT_IO_DIMSE_API tag_missing(const std::string& _message);

    /// Destructor
    SIGHT_IO_DIMSE_API ~tag_missing() noexcept override;
};

} // namespace sight::io::dimse::exceptions
