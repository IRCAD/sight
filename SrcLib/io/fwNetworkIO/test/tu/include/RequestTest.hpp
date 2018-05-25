/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include <fwNetworkIO/http/Request.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace fwNetworkIO
{
namespace ut
{

class RequestTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( RequestTest );
CPPUNIT_TEST( testRequest );
CPPUNIT_TEST_SUITE_END();

public:
    // Interface
    // Creates a Request object
    void setUp();
    // Does nothing
    void tearDown();

    // Test functions
    // Tests the Request class API
    void testRequest();

private:
    // Object Request
    ::fwNetworkIO::http::Request::sptr m_request;
};

} // namespace ut
} // namespace fwNetworkIO
