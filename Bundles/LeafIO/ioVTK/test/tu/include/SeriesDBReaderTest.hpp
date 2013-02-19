/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _IOVTK_TEST_TU_SERIESDBREADERTEST_HPP_
#define _IOVTK_TEST_TU_SERIESDBREADERTEST_HPP_

#include <fwMedData/SeriesDB.hpp>

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

#endif //_IOVTK_TEST_TU_SERIESDBREADERTEST_HPP_
