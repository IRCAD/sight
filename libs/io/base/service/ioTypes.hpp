/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "io/base/config.hpp"

#include <filesystem>
#include <string>
#include <vector>

namespace sight::io::base::service
{

//! IOPathType defines different types of paths used by service readers/writers
typedef enum
{
    FILE             = (1 << 0), /*!< io service can manage file and cannot manage a list of file*/
    FILES            = (1 << 1), /*!< io service can manage a list of file ()*/
    FOLDER           = (1 << 2), /*!< io service can manage folder*/
    TYPE_NOT_DEFINED = (1 << 3)  /*!< io service not used path parameter to works */
} IOPathType;

/// List of paths managed by io services
typedef std::vector<std::filesystem::path> LocationsType;

IO_BASE_API extern const std::string s_DATA_KEY;

} // namespace sight::io::base::service
