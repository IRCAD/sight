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

#include "fwStructuralPatch/creator/fwMedData/Series1.hpp"
#include "fwAtomsPatch/helper/functions.hpp"
#include "fwAtomsPatch/helper/Object.hpp"
#include "fwAtoms/String.hpp"

#include "creator/fwMedData/Series1Test.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwStructuralPatch::ut::creator::fwMedData::Series1Test );

namespace fwStructuralPatch
{
namespace ut
{
namespace creator
{
namespace fwMedData
{

//------------------------------------------------------------------------------

void Series1Test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void Series1Test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void Series1Test::createTest()
{
    using namespace ::fwStructuralPatch::creator::fwMedData;
    Series1::sptr seriesCreator = Series1::New();

    CPPUNIT_ASSERT(seriesCreator);
    const std::string& classname = seriesCreator->getObjectClassname();
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal", std::string("::fwMedData::Series") == classname );
    const std::string& objectVersion = seriesCreator->getObjectVersion();
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal", std::string("1") == objectVersion );

    ::fwAtoms::Object::sptr series = seriesCreator->create();

    const std::string& seriesClassname = ::fwAtomsPatch::helper::getClassname(series);
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal", std::string("::fwMedData::Series") == seriesClassname );
    const std::string& seriesObjectVersion = ::fwAtomsPatch::helper::getVersion(series);
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal", std::string("1") == seriesObjectVersion );

    CPPUNIT_ASSERT(series->getAttribute("patient"));
    CPPUNIT_ASSERT(series->getAttribute("study"));
    CPPUNIT_ASSERT(series->getAttribute("equipment"));
    CPPUNIT_ASSERT(series->getAttribute("instance_uid"));
    CPPUNIT_ASSERT(series->getAttribute("modality"));
    CPPUNIT_ASSERT(series->getAttribute("date"));
    CPPUNIT_ASSERT(series->getAttribute("time"));
    CPPUNIT_ASSERT(series->getAttribute("performing_physicians_name"));
    CPPUNIT_ASSERT(series->getAttribute("description"));
}

//------------------------------------------------------------------------------

void Series1Test::copyTest()
{
    using namespace ::fwStructuralPatch::creator::fwMedData;
    Series1 seriesCreator;

    const std::string& classname = seriesCreator.getObjectClassname();
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal", std::string("::fwMedData::Series") == classname );
    const std::string& objectVersion = seriesCreator.getObjectVersion();
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal", std::string("1") == objectVersion );

    Series1* seriesCreator2 = new Series1(seriesCreator);

    CPPUNIT_ASSERT(seriesCreator2);
    const std::string& classnameSeriesCreator2 = seriesCreator2->getObjectClassname();
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal", std::string("::fwMedData::Series") == classnameSeriesCreator2 );

    const std::string& objectVersionSeriesCreator2 = seriesCreator2->getObjectVersion();
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal", std::string("1") == objectVersionSeriesCreator2 );

    delete(seriesCreator2);

}

//------------------------------------------------------------------------------

} //namespace fwMedData
} //namespace creator
} //namespace ut
} //namespace fwStructuralPatch
