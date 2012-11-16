/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOM_UT_UTILTEST_HPP__
#define __FWCOM_UT_UTILTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwCom
{
namespace ut
{

class UtilTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( UtilTest );

    CPPUNIT_TEST( autoBindTest );
    CPPUNIT_TEST( convertFunctionTypeTest );
    CPPUNIT_TEST( removeLastArgTest );

    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void autoBindTest ();
    void convertFunctionTypeTest ();
    void removeLastArgTest ();

};

} //namespace ut
} //namespace fwCom
#endif //__FWCOM_UT_UTILTEST_HPP__

