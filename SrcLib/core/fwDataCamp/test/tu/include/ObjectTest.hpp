/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATACAMP_UT_OBJECTTEST_HPP__
#define __FWDATACAMP_UT_OBJECTTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwDataCamp
{
namespace ut
{

class ObjectTest :  public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( ObjectTest );
    CPPUNIT_TEST( callFunctionTest );
    CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp();
    void tearDown();

    void callFunctionTest();

};

}  // namespace ut
}  // namespace fwDataCamp

#endif // __FWDATACAMP_UT_OBJECTTEST_HPP__

