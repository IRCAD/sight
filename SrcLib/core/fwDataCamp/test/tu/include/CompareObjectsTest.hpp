/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATACAMP_UT_COMPAREOBJECTSTEST_HPP__
#define __FWDATACAMP_UT_COMPAREOBJECTSTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwDataCamp
{
namespace ut
{

class CompareObjectsTest :  public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( CompareObjectsTest );
CPPUNIT_TEST( compareImageTest );
CPPUNIT_TEST( compareReconstructionTest );
CPPUNIT_TEST( compareBufferTest );
CPPUNIT_TEST( compareEmpty );
CPPUNIT_TEST( exceptionTest );
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp();
    void tearDown();

    void compareImageTest();
    void compareReconstructionTest();
    void compareBufferTest();
    void compareEmpty();
    void exceptionTest();
};

}  // namespace ut
}  // namespace fwDataCamp

#endif // __FWDATACAMP_UT_COMPAREOBJECTSTEST_HPP__

