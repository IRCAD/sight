/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CVIO_UT_TYPETEST_HPP__
#define __CVIO_UT_TYPETEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace cvIO
{
namespace ut
{
/**
 * @brief Tests the cvIO::Image functions
 */
class TypeTest : public CPPUNIT_NS::TestFixture
{
public:
    CPPUNIT_TEST_SUITE( TypeTest );
    CPPUNIT_TEST(toCv);
    CPPUNIT_TEST(fromCv);
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void toCv();
    void fromCv();
};

} // namespace ut

}// namespace cvIO

#endif //__CVIO_UT_TYPETEST_HPP__
