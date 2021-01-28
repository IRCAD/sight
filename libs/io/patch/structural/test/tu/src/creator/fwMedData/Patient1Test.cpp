/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "creator/data/Patient1Test.hpp"

#include "atoms/String.hpp"

#include "io/atoms/patch/helper/functions.hpp"
#include "io/atoms/patch/helper/Object.hpp"
#include "io/patch/structural/creator/data/Patient1.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::sight::io::patch::structural::ut::creatordata::Patient1Test );

namespace sight::io::patch::structural
{
namespace ut
{
namespace creator
{
namespace data
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
    using namespace io::patch::structural::creator::fwMedData;
    Patient1::sptr patientCreator = Patient1::New();

    CPPUNIT_ASSERT(patientCreator);
    const std::string& classname = patientCreator->getObjectClassname();
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal", std::string("::sight::data::Patient") == classname );
    const std::string& objectVersion = patientCreator->getObjectVersion();
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal", std::string("1") == objectVersion );

    sight::atoms::Object::sptr patient = patientCreator->create();

    const std::string& patientClassname = io::atoms::patch::helper::getClassname(patient);
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal", std::string("::sight::data::Patient") == patientClassname );
    const std::string& patientObjectVersion = io::atoms::patch::helper::getVersion(patient);
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal", std::string("1") == patientObjectVersion );

    CPPUNIT_ASSERT(patient->getAttribute("name"));
    CPPUNIT_ASSERT(patient->getAttribute("patient_id"));
    CPPUNIT_ASSERT(patient->getAttribute("birth_date"));
    CPPUNIT_ASSERT(patient->getAttribute("sex"));

}

//------------------------------------------------------------------------------

void Patient1Test::copyTest()
{
    using namespace io::patch::structural::creator::fwMedData;
    Patient1 patientCreator;

    const std::string& classname = patientCreator.getObjectClassname();
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal", std::string("::sight::data::Patient") == classname );
    const std::string& objectVersion = patientCreator.getObjectVersion();
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal", std::string("1") == objectVersion );

    Patient1* patientCreator2 = new Patient1(patientCreator);

    CPPUNIT_ASSERT(patientCreator2);
    const std::string& classnamePatientCreator2 = patientCreator2->getObjectClassname();
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal",
                           std::string("::sight::data::Patient") == classnamePatientCreator2 );

    const std::string& objectVersionPatientCreator2 = patientCreator2->getObjectVersion();
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal", std::string("1") == objectVersionPatientCreator2 );

    delete(patientCreator2);
}

//------------------------------------------------------------------------------

} //namespace data
} //namespace creator
} //namespace ut
} //namespace sight::io::patch::structural
