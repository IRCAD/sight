/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include <cppunit/extensions/HelperMacros.h>

namespace itkRegistrationOp
{
namespace ut
{

class MIPMatchingRegistrationTest : public CPPUNIT_NS::TestFixture
{

CPPUNIT_TEST_SUITE( MIPMatchingRegistrationTest );
CPPUNIT_TEST( identityTest );
CPPUNIT_TEST( translateTransformTest );
CPPUNIT_TEST( translateTransformWithScalesTest );
CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

    void identityTest();
    void translateTransformTest();
    void translateTransformWithScalesTest();
};

} //namespace ut
} //namespace itkRegistrationOp

