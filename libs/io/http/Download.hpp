/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include <filesystem>
#include <string>

namespace sight::io::http
{

/**
 * @brief Downloads and writes a file from a distant server to local file location using libcurl
 * (only tested with http(s) protocol).
 *
 * @param _url : complete url or distant file (ex: https://mycloud.com/file.txt) (only tested with http(s) protocol).
 * @param _file_location : complete path where the distant file should be downloaded to, (ex: /home/download/file.txt)
 * the file should not already exists, the function doesn't handle overwriting, test should be done before @see throw
 * section.
 * @param _verbose: to enable verbosity of libCURL (VERY verbose), disabled by default.
 * @throw core::exception:
 * - _url or _file_location is empty
 * - _file_location already exists (tests should be made BEFORE calling this function)
 * - _file_location parent folder isn't writable
 * - downloading fails (libcurl error code will be printed in the exception message)
 * - generic errors of libcurl (initialization or writing file).
 */
IO_HTTP_API void downloadFile(
    const std::string& _url,
    const std::filesystem::path& _file_location,
    bool _verbose = false
);

}
