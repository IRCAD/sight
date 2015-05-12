/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWVTKIO_UT_SERIESDBTEST_HPP__
#define __FWVTKIO_UT_SERIESDBTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

#include <fwData/Mesh.hpp>
#include <fwData/Array.hpp>

namespace fwVtkIO
{
namespace ut
{

/**
 * @brief Test many methods to create mesh.
 */
class SeriesDBTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( SeriesDBTest );
CPPUNIT_TEST( testImportSeriesDB );
CPPUNIT_TEST( testLazyImportSeriesDB );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void testImportSeriesDB();
    void testLazyImportSeriesDB();

};

} // namespace ut
} // namespace fwVtkIO

#endif //__FWVTKIO_UT_SERIESDBTEST_HPP__
