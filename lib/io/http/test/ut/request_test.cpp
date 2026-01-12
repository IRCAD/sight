/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include <io/http/request.hpp>

#include <utest/exception.hpp>

#include <doctest/doctest.h>

#include <QNetworkRequest>

TEST_SUITE("sight::io::http::request")
{
    TEST_CASE("request")
    {
        const std::string dummy_url("http://localhost:8080");
        auto request = sight::io::http::request::New(dummy_url);

        const std::string key("Content-Type");
        const std::string value("application/json");
        request->add_header(key, value);

        sight::io::http::request::headers_t headers = request->get_headers();
        CHECK(headers.find(key) != headers.end());
        CHECK_EQ(headers[key], value);

        sight::io::http::request::headers_t new_headers;
        const std::string new_value("application/dicom");
        new_headers.insert(std::pair<std::string, std::string>(key, new_value));

        request->set_headers(new_headers);
        headers = request->get_headers();

        CHECK_EQ(headers, new_headers);
        CHECK(headers.find(key) != headers.end());
        CHECK_EQ(headers[key], new_value);

        const std::string new_dummy_url("http://localhost:8080/dummy");
        request->set_url(new_dummy_url);
        const std::string& url = request->get_url();
        CHECK_EQ(new_dummy_url, url);
    }
} // TEST_SUITE
