/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWTEST_TEST_TU_SERIESDBTEST_HPP_
#define _FWTEST_TEST_TU_SERIESDBTEST_HPP_

#include <cppunit/extensions/HelperMacros.h>

namespace fwTest
{
namespace ut
{

/**
 * @brief Test many methods to create image.
 */
class SeriesDBTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( SeriesDBTest );
    CPPUNIT_TEST( generationTest );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();


    void generationTest();



};

} //namespace ut
} //namespace fwTest

#endif //_FWTEST_TEST_TU_SERIESDBTEST_HPP_
