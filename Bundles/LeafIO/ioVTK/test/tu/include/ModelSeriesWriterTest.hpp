/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOVTK_UT_MODELSERIESWRITERTEST_HPP__
#define __IOVTK_UT_MODELSERIESWRITERTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace ioVTK
{
namespace ut
{

/**
 * @brief Test the reader and the writer of Mesh/Image using VTK.
 */
class ModelSeriesWriterTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( ModelSeriesWriterTest );
CPPUNIT_TEST( testWriteMeshes );
CPPUNIT_TEST( testWriteReconstructions );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void testWriteMeshes();
    void testWriteReconstructions();
};

} //namespace ut
} //namespace ioVTK

#endif //__IOVTK_UT_MODELSERIESWRITERTEST_HPP__

