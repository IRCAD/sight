/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include <fwNetworkIO/helper/Series.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace fwNetworkIO
{
namespace ut
{

class SeriesTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( SeriesTest );
CPPUNIT_TEST( testSeries );
CPPUNIT_TEST_SUITE_END();

public:
    // Interface
    // Creates a JSON
    void setUp();
    // Does nothing
    void tearDown();

    // Test functions
    // Tests the Series helper API
    void testSeries();

private:
    // JSON corresponding to a result of a Orthanc GET /instances/{id}/simplified-tags request.
    QJsonObject m_json;
};

} // namespace ut
} // namespace fwNetworkIO
