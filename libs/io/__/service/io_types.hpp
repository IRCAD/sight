/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include <sight/io/__/config.hpp>

#include <filesystem>
#include <string>
#include <vector>

namespace sight::io::service
{

//! path_type_t defines different types of paths used by service readers/writers
enum path_type_t
{
    file             = (1 << 0), /*!< io service can manage file and cannot manage a list of file*/
    files            = (1 << 1), /*!< io service can manage a list of file ()*/
    folder           = (1 << 2), /*!< io service can manage folder*/
    type_not_defined = (1 << 3)  /*!< io service not used path parameter to works */
};

/// List of paths managed by io services
using locations_t = std::vector<std::filesystem::path>;

inline static const std::string DATA_KEY             = "data";
inline static const std::string OLD_FILE_KEY         = "file";
inline static const std::string FILES_KEY            = "files";
inline static const std::string FOLDER_KEY           = "folder";
inline static const std::string OLD_RESOURCE_KEY     = "resource";
inline static const std::string RESOURCES_KEY        = "resources";
inline static const std::string WINDOW_TITLE_KEY     = "window_title";
inline static const std::string OLD_WINDOW_TITLE_KEY = "windowTitle";

} // namespace sight::io::service
