/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CVIO_UT_FRAMETLTEST_HPP__
#define __CVIO_UT_FRAMETLTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace cvIO
{
namespace ut
{
/**
 * @brief Tests the cvIO::FrameTL functions
 */
class FrameTLTest : public CPPUNIT_NS::TestFixture
{
public:
    CPPUNIT_TEST_SUITE( FrameTLTest );
    CPPUNIT_TEST(moveToCv);
    CPPUNIT_TEST(copyFromCv);
    CPPUNIT_TEST(copyToCv);
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void moveToCv();
    void copyFromCv();
    void copyToCv();
};

} // namespace ut

}// namespace cvIO

#endif //__CVIO_UT_FRAMETLTEST_HPP__
