/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwStructuralPatch/creator/fwMedData/Study1.hpp"
#include "fwAtomsPatch/helper/functions.hpp"
#include "fwAtomsPatch/helper/Object.hpp"
#include "fwAtoms/String.hpp"

#include "creator/fwMedData/Study1Test.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwStructuralPatch::ut::creator::fwMedData::Study1Test );

namespace fwStructuralPatch
{
namespace ut
{
namespace creator
{
namespace fwMedData
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
    using namespace ::fwStructuralPatch::creator::fwMedData;
    Study1::sptr studyCreator = Study1::New();

    CPPUNIT_ASSERT(studyCreator);
    const std::string& classname = studyCreator->getObjectClassname();
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal" , std::string("::fwMedData::Study") == classname );
    const std::string& objectVersion =  studyCreator->getObjectVersion();
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal" , std::string("1") == objectVersion );

    ::fwAtoms::Object::sptr study = studyCreator->create();

    const std::string& studyClassname = ::fwAtomsPatch::helper::getClassname(study);
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal" , std::string("::fwMedData::Study") == studyClassname );
    const std::string& studyObjectVersion = ::fwAtomsPatch::helper::getVersion(study);
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal" , std::string("1") == studyObjectVersion );

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
    using namespace ::fwStructuralPatch::creator::fwMedData;
    Study1 studyCreator;

    const std::string& classname = studyCreator.getObjectClassname();
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal" , std::string("::fwMedData::Study") == classname );
    const std::string& objectVersion =  studyCreator.getObjectVersion();
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal" , std::string("1") == objectVersion );

    Study1* studyCreator2 = new Study1(studyCreator);

    CPPUNIT_ASSERT(studyCreator2);
    const std::string& classnamestudyCreator2 = studyCreator2->getObjectClassname();
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal" , std::string("::fwMedData::Study") == classnamestudyCreator2 );

    const std::string& objectVersionstudyCreator2 =  studyCreator2->getObjectVersion();
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal" , std::string("1") == objectVersionstudyCreator2 );

    delete(studyCreator2);
}

//------------------------------------------------------------------------------

} //namespace fwMedData
} //namespace creator
} //namespace ut
} //namespace fwStructuralPatch
