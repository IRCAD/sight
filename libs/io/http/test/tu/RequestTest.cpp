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

#include "RequestTest.hpp"

#include <utest/exception.hpp>

#include <cppunit/Exception.h>

#include <QNetworkRequest>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::http::ut::request_test);

namespace sight::io::http::ut
{

//------------------------------------------------------------------------------

void request_test::setUp()
{
    const std::string dummy_url("http://localhost:8080");
    m_request = sight::io::http::request::New(dummy_url);
}

//------------------------------------------------------------------------------

void request_test::tearDown()
{
}

//------------------------------------------------------------------------------

void request_test::test_request()
{
    const std::string key("Content-Type");
    const std::string value("application/json");
    m_request->add_header(key, value);

    sight::io::http::request::headers_t headers = m_request->get_headers();
    CPPUNIT_ASSERT(headers.find(key) != headers.end());
    CPPUNIT_ASSERT_EQUAL(headers[key], value);

    sight::io::http::request::headers_t new_headers;
    const std::string new_value("application/dicom");
    new_headers.insert(std::pair<std::string, std::string>(key, new_value));

    m_request->set_headers(new_headers);
    headers = m_request->get_headers();

    CPPUNIT_ASSERT(headers == new_headers);
    CPPUNIT_ASSERT(headers.find(key) != headers.end());
    CPPUNIT_ASSERT_EQUAL(headers[key], new_value);

    const std::string new_dummy_url("http://localhost:8080/dummy");
    m_request->set_url(new_dummy_url);
    const std::string& url = m_request->get_url();
    CPPUNIT_ASSERT_EQUAL(new_dummy_url, url);
}

//------------------------------------------------------------------------------

} // namespace sight::io::http::ut
