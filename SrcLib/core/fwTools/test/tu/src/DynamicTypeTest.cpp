/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/DynamicType.hpp>

#include "DynamicTypeTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwTools::ut::DynamicTypeTest );

namespace fwTools
{
namespace ut
{

void DynamicTypeTest::setUp()
{
    // Set up context before running a test.
}

void DynamicTypeTest::tearDown()
{
    // Clean up after the test run.
}

void DynamicTypeTest::typeTest()
{
    const std::string TYPE = "double";
    ::fwTools::DynamicType type1 = ::fwTools::makeDynamicType<int>();
    ::fwTools::DynamicType type2 = ::fwTools::DynamicType();
    type2.setType<double>();

    CPPUNIT_ASSERT(type1.isType<int>());
    CPPUNIT_ASSERT(type2.isType<double>());
    CPPUNIT_ASSERT_EQUAL(type2.string(), TYPE );
}

} // namespace ut
} // namespace fwTools

