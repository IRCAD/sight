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

#include "download.hpp"

#include <core/exception.hpp>
#include <core/spy_log.hpp>

#include <curl/curl.h>

namespace sight::io::http
{

// Private write method used by CURL to write the content of the downloaded file on the disk.
size_t write_data(void* _ptr, size_t _size, size_t _nmemb, FILE* _stream)
{
    size_t written = 0;
    written = fwrite(_ptr, _size, _nmemb, _stream);
    return written;
}

//------------------------------------------------------------------------------

void download_file(const std::string& _url, const std::filesystem::path& _file_location, bool _verbose)
{
    // Basic checks.
    if(_url.empty() || _file_location.empty())
    {
        throw core::exception("Missing inputs (url or file location)");
    }

    // Don't override file if already exists, test should be done before calling the function.
    if(std::filesystem::exists(_file_location))
    {
        throw core::exception("file: '" + _file_location.string() + "' already exists.");
    }

    // Check that we have write permission on the parent folder of _file_location.
    const auto perms = std::filesystem::status(_file_location.parent_path()).permissions();
    if((perms& std::filesystem::perms::owner_write) == std::filesystem::perms::none)
    {
        throw core::exception("Cannot write in '" + _file_location.parent_path().string() + "' directory");
    }

    CURL* curl = nullptr;
    FILE* fp   = nullptr;
    curl = curl_easy_init();
    if(curl != nullptr)
    {
        fp = fopen(_file_location.string().c_str(), "wb");
        curl_easy_setopt(curl, CURLOPT_URL, _url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        if(_verbose)
        {
            // Can be useful to debug (Warning: very very verbose !).
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        }

        const auto res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        const auto closed = fclose(fp);

        if(closed != 0)
        {
            // Remove file created with the fopen.
            std::filesystem::remove(_file_location);
            throw core::exception("Cannot close file '" + _file_location.string() + "'");
        }

        if(res != 0)
        {
            // Remove file created with the fopen.
            std::filesystem::remove(_file_location);
            throw core::exception("CURL error code [" + std::to_string(res) + "] when trying to download file");
        }
    }
    else
    {
        throw core::exception("Cannot initialize CURL.");
    }
}

} // namespace sight::io::http
