/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "fwStructuralPatch/creator/fwMedData/Patient1.hpp"
#include "fwAtomsPatch/helper/functions.hpp"
#include "fwAtomsPatch/helper/Object.hpp"
#include "fwAtoms/String.hpp"

#include "creator/fwMedData/Patient1Test.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwStructuralPatch::ut::creator::fwMedData::Patient1Test );

namespace fwStructuralPatch
{
namespace ut
{
namespace creator
{
namespace fwMedData
{

//------------------------------------------------------------------------------

void Patient1Test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void Patient1Test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void Patient1Test::createTest()
{
    using namespace ::fwStructuralPatch::creator::fwMedData;
    Patient1::sptr patientCreator = Patient1::New();

    CPPUNIT_ASSERT(patientCreator);
    const std::string& classname = patientCreator->getObjectClassname();
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal", std::string("::fwMedData::Patient") == classname );
    const std::string& objectVersion = patientCreator->getObjectVersion();
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal", std::string("1") == objectVersion );

    ::fwAtoms::Object::sptr patient = patientCreator->create();

    const std::string& patientClassname = ::fwAtomsPatch::helper::getClassname(patient);
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal", std::string("::fwMedData::Patient") == patientClassname );
    const std::string& patientObjectVersion = ::fwAtomsPatch::helper::getVersion(patient);
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal", std::string("1") == patientObjectVersion );

    CPPUNIT_ASSERT(patient->getAttribute("name"));
    CPPUNIT_ASSERT(patient->getAttribute("patient_id"));
    CPPUNIT_ASSERT(patient->getAttribute("birth_date"));
    CPPUNIT_ASSERT(patient->getAttribute("sex"));

}

void Patient1Test::copyTest()
{
    using namespace ::fwStructuralPatch::creator::fwMedData;
    Patient1 patientCreator;

    const std::string& classname = patientCreator.getObjectClassname();
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal", std::string("::fwMedData::Patient") == classname );
    const std::string& objectVersion = patientCreator.getObjectVersion();
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal", std::string("1") == objectVersion );

    Patient1* patientCreator2 = new Patient1(patientCreator);

    CPPUNIT_ASSERT(patientCreator2);
    const std::string& classnamePatientCreator2 = patientCreator2->getObjectClassname();
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal", std::string("::fwMedData::Patient") == classnamePatientCreator2 );

    const std::string& objectVersionPatientCreator2 = patientCreator2->getObjectVersion();
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal", std::string("1") == objectVersionPatientCreator2 );

    delete(patientCreator2);
}

//------------------------------------------------------------------------------

} //namespace fwMedData
} //namespace creator
} //namespace ut
} //namespace fwStructuralPatch
