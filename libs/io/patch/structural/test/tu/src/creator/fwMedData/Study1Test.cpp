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

#include "creator/data/Study1Test.hpp"

#include "atoms/String.hpp"

#include "io/atoms/patch/helper/functions.hpp"
#include "io/atoms/patch/helper/Object.hpp"
#include "io/patch/structural/creator/data/Study1.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::sight::io::patch::structural::ut::creatordata::Study1Test );

namespace sight::io::patch::structural
{
namespace ut
{
namespace creator
{
namespace data
{

//------------------------------------------------------------------------------

void Study1Test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void Study1Test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void Study1Test::createTest()
{
    using namespace io::patch::structural::creator::fwMedData;
    Study1::sptr studyCreator = Study1::New();

    CPPUNIT_ASSERT(studyCreator);
    const std::string& classname = studyCreator->getObjectClassname();
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal", std::string("::sight::data::Study") == classname );
    const std::string& objectVersion = studyCreator->getObjectVersion();
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal", std::string("1") == objectVersion );

    sight::atoms::Object::sptr study = studyCreator->create();

    const std::string& studyClassname = io::atoms::patch::helper::getClassname(study);
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal", std::string("::sight::data::Study") == studyClassname );
    const std::string& studyObjectVersion = io::atoms::patch::helper::getVersion(study);
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal", std::string("1") == studyObjectVersion );

    CPPUNIT_ASSERT(study->getAttribute("instance_uid"));
    CPPUNIT_ASSERT(study->getAttribute("date"));
    CPPUNIT_ASSERT(study->getAttribute("time"));
    CPPUNIT_ASSERT(study->getAttribute("referring_physician_name"));
    CPPUNIT_ASSERT(study->getAttribute("description"));
    CPPUNIT_ASSERT(study->getAttribute("patient_age"));

}

//------------------------------------------------------------------------------

void Study1Test::copyTest()
{
    using namespace io::patch::structural::creator::fwMedData;
    Study1 studyCreator;

    const std::string& classname = studyCreator.getObjectClassname();
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal", std::string("::sight::data::Study") == classname );
    const std::string& objectVersion = studyCreator.getObjectVersion();
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal", std::string("1") == objectVersion );

    Study1* studyCreator2 = new Study1(studyCreator);

    CPPUNIT_ASSERT(studyCreator2);
    const std::string& classnamestudyCreator2 = studyCreator2->getObjectClassname();
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal", std::string("::sight::data::Study") == classnamestudyCreator2 );

    const std::string& objectVersionstudyCreator2 = studyCreator2->getObjectVersion();
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal", std::string("1") == objectVersionstudyCreator2 );

    delete(studyCreator2);
}

//------------------------------------------------------------------------------

} //namespace data
} //namespace creator
} //namespace ut
} //namespace sight::io::patch::structural
