/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include <cppunit/extensions/HelperMacros.h>

namespace arData
{
namespace ut
{

class MarkerMapTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( MarkerMapTest );
CPPUNIT_TEST(getterSetter);
CPPUNIT_TEST(shallowCopyTest);
CPPUNIT_TEST(deepCopyTest);
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void getterSetter();
    void shallowCopyTest();
    void deepCopyTest();
};

} //namespace ut
} //namespace arData
