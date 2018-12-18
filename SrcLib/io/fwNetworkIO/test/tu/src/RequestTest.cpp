/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
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

#include <fwTest/Exception.hpp>

#include <cppunit/Exception.h>

#include <QNetworkRequest>

CPPUNIT_TEST_SUITE_REGISTRATION( ::fwNetworkIO::ut::RequestTest );

namespace fwNetworkIO
{
namespace ut
{

static ::fwTest::Exception e(""); // force link with fwTest

//------------------------------------------------------------------------------

void RequestTest::setUp()
{
    const std::string dummyUrl("http://localhost:8080");
    m_request = ::fwNetworkIO::http::Request::New(dummyUrl);
}

//------------------------------------------------------------------------------

void RequestTest::tearDown()
{
}

//------------------------------------------------------------------------------

void RequestTest::testRequest()
{
    const std::string key("Content-Type");
    const std::string value("application/json");
    m_request->addHeader(key, value);

    ::fwNetworkIO::http::Request::HeadersType headers = m_request->getHeaders();
    CPPUNIT_ASSERT(headers.find(key) != headers.end());
    CPPUNIT_ASSERT_EQUAL(headers[key], value);

    ::fwNetworkIO::http::Request::HeadersType newHeaders;
    const std::string newValue("application/dicom");
    newHeaders.insert(std::pair<std::string, std::string>(key, newValue));

    m_request->setHeaders(newHeaders);
    headers = m_request->getHeaders();

    CPPUNIT_ASSERT(headers == newHeaders);
    CPPUNIT_ASSERT(headers.find(key) != headers.end());
    CPPUNIT_ASSERT_EQUAL(headers[key], newValue);

    const std::string newDummyUrl("http://localhost:8080/dummy");
    m_request->setUrl(newDummyUrl);
    const std::string& url = m_request->getUrl();
    CPPUNIT_ASSERT_EQUAL(newDummyUrl, url);
}

//------------------------------------------------------------------------------

} // namespace ut

} // namespace fwNetworkIO
