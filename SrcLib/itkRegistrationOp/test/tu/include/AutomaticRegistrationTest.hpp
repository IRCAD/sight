/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include <cppunit/extensions/HelperMacros.h>

namespace itkRegistrationOp
{
namespace ut
{

class AutomaticRegistrationTest : public CPPUNIT_NS::TestFixture
{

CPPUNIT_TEST_SUITE( AutomaticRegistrationTest );
CPPUNIT_TEST( identityTest );
CPPUNIT_TEST( rigidTransformTest );
CPPUNIT_TEST( translateTransformTest );
CPPUNIT_TEST( rotationTransformTest );
CPPUNIT_TEST( multiresolutionRotationTransformTest );
CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

    void identityTest();
    void rigidTransformTest();
    void translateTransformTest();
    void rotationTransformTest();
    void multiresolutionRotationTransformTest();

};

} //namespace ut
} //namespace itkRegistrationOp
