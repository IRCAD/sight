/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ARLCORE_UT_REGISTRATIONTEST_HPP__
#define __ARLCORE_UT_REGISTRATIONTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace ARLcore
{
namespace ut
{

class RegistrationTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( RegistrationTest );
CPPUNIT_TEST( HomographyRegistrationTest );
CPPUNIT_TEST( ProjectiveRegistrationTest );
CPPUNIT_TEST( Registration3D3DTest     );
//CPPUNIT_TEST( ICPRegistrationTest     ); FIXME dataset
CPPUNIT_TEST( threeD3DMatchingTest );
CPPUNIT_TEST( registration3D3DwithoutMatchingTest );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void HomographyRegistrationTest();
    void ProjectiveRegistrationTest();
    void Registration3D3DTest();
    void ICPRegistrationTest();
    void threeD3DMatchingTest();
    void registration3D3DwithoutMatchingTest();



};

} //namespace ut
} //namespace ARLCORE
#endif //__ARLCORE_UT_REGISTRATIONTEST_HPP__

