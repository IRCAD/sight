/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ITKREGISTRATIONOP_UT_FASTREGISTRATIONTEST_HPP__
#define __ITKREGISTRATIONOP_UT_FASTREGISTRATIONTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace itkRegistrationOp
{
namespace ut
{

class FastRegistrationTest : public CPPUNIT_NS::TestFixture
{

CPPUNIT_TEST_SUITE( FastRegistrationTest );
CPPUNIT_TEST( identityTest );
CPPUNIT_TEST( translateTransformTest );
CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

    void identityTest();
    void translateTransformTest();
};

} //namespace ut
} //namespace itkRegistrationOp

#endif // __ITKREGISTRATIONOP_UT_FASTREGISTRATIONTEST_HPP__
