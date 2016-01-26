/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOVTK_UT_SERIESDBREADERTEST_HPP__
#define __IOVTK_UT_SERIESDBREADERTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace ioVTK
{
namespace ut
{

/**
 * @brief Test the reader and the writer of Mesh/Image using VTK.
 */
class SeriesDBReaderTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( SeriesDBReaderTest );
CPPUNIT_TEST( testSeriesDBReader );
CPPUNIT_TEST( testMergeSeriesDBReader );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void testSeriesDBReader();
    void testMergeSeriesDBReader();
};

} //namespace ut
} //namespace ioVTK

#endif //__IOVTK_UT_SERIESDBREADERTEST_HPP__
