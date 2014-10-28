/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <string>

#include "fwMedDataTools/functions.hpp"

#include "FunctionsTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMedDataTools::ut::FunctionsTest );

namespace fwMedDataTools
{

namespace ut
{

void FunctionsTest::setUp()
{
}

void FunctionsTest::tearDown()
{
}

void FunctionsTest::generatePatientIdTest()
{
    const std::string id = ::fwMedDataTools::generatePatientId();
    CPPUNIT_ASSERT(64 >= id.length());
    CPPUNIT_ASSERT(0 < id.length());
}

void FunctionsTest::generateStudyInstanceUidTest()
{
    const std::string id = ::fwMedDataTools::generateStudyInstanceUid();
    CPPUNIT_ASSERT(16 == id.length());
}

} // namespace ut

} // namespace fwMedDataTools

