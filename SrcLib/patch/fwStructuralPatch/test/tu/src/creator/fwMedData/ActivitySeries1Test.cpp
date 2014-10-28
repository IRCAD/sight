/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwStructuralPatch/creator/fwMedData/ActivitySeries1.hpp"
#include "fwAtomsPatch/helper/functions.hpp"
#include "fwAtomsPatch/helper/Object.hpp"
#include "fwAtoms/String.hpp"

#include "creator/fwMedData/ActivitySeries1Test.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwStructuralPatch::ut::creator::fwMedData::ActivitySeries1Test );

namespace fwStructuralPatch
{
namespace ut
{
namespace creator
{
namespace fwMedData
{

//------------------------------------------------------------------------------

void ActivitySeries1Test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ActivitySeries1Test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ActivitySeries1Test::createTest()
{
    using namespace ::fwStructuralPatch::creator::fwMedData;
    ActivitySeries1::sptr actSeriesCreator = ActivitySeries1::New();

    CPPUNIT_ASSERT(actSeriesCreator);
    const std::string& classname = actSeriesCreator->getObjectClassname();
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal" , std::string("::fwMedData::ActivitySeries") == classname );
    const std::string& objectVersion =  actSeriesCreator->getObjectVersion();
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal" , std::string("1") == objectVersion );

    ::fwAtoms::Object::sptr actSeries = actSeriesCreator->create();

    const std::string& actSeriesClassname = ::fwAtomsPatch::helper::getClassname(actSeries);
    CPPUNIT_ASSERT_MESSAGE(
            "Classname must be equal" , std::string("::fwMedData::ActivitySeries") == actSeriesClassname );
    const std::string& actSeriesObjectVersion = ::fwAtomsPatch::helper::getVersion(actSeries);
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal" , std::string("1") == actSeriesObjectVersion );

    CPPUNIT_ASSERT(actSeries->getAttribute("data"));
    CPPUNIT_ASSERT(actSeries->getAttribute("activity_config_id"));
}

//------------------------------------------------------------------------------

void ActivitySeries1Test::copyTest()
{
    using namespace ::fwStructuralPatch::creator::fwMedData;
    ActivitySeries1 actSeriesCreator;

    const std::string& classname = actSeriesCreator.getObjectClassname();
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal" , std::string("::fwMedData::ActivitySeries") == classname );
    const std::string& objectVersion =  actSeriesCreator.getObjectVersion();
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal" , std::string("1") == objectVersion );

    ActivitySeries1* actSeriesCreator2 = new ActivitySeries1(actSeriesCreator);

    CPPUNIT_ASSERT(actSeriesCreator2);
    const std::string& classnameActivitySeriesCreator2 = actSeriesCreator2->getObjectClassname();
    CPPUNIT_ASSERT_MESSAGE(
            "Classname must be equal", std::string("::fwMedData::ActivitySeries") == classnameActivitySeriesCreator2 );

    const std::string& objectVersionActivitySeriesCreator2 =  actSeriesCreator2->getObjectVersion();
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal" , std::string("1") == objectVersionActivitySeriesCreator2 );

    delete(actSeriesCreator2);
}

//------------------------------------------------------------------------------

} //namespace fwMedData
} //namespace creator
} //namespace ut
} //namespace fwStructuralPatch
