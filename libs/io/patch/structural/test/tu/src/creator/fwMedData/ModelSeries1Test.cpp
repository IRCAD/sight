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

#include "creator/data/ModelSeries1Test.hpp"

#include "atoms/String.hpp"

#include "io/atoms/patch/helper/functions.hpp"
#include "io/atoms/patch/helper/Object.hpp"
#include "io/patch/structural/creator/data/ModelSeries1.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::sight::io::patch::structural::ut::creatordata::ModelSeries1Test );

namespace sight::io::patch::structural
{
namespace ut
{
namespace creator
{
namespace data
{

//------------------------------------------------------------------------------

void ModelSeries1Test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ModelSeries1Test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ModelSeries1Test::createTest()
{
    using namespace io::patch::structural::creator::fwMedData;
    ModelSeries1::sptr modelSeriesCreator = ModelSeries1::New();

    CPPUNIT_ASSERT(modelSeriesCreator);
    const std::string& classname = modelSeriesCreator->getObjectClassname();
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal", std::string("::sight::data::ModelSeries") == classname );
    const std::string& objectVersion = modelSeriesCreator->getObjectVersion();
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal", std::string("1") == objectVersion );

    sight::atoms::Object::sptr modelSeries = modelSeriesCreator->create();

    const std::string& modelSeriesClassname = io::atoms::patch::helper::getClassname(modelSeries);
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal",
                           std::string("::sight::data::ModelSeries") == modelSeriesClassname );
    const std::string& modelSeriesObjectVersion = io::atoms::patch::helper::getVersion(modelSeries);
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal", std::string("1") == modelSeriesObjectVersion );

    CPPUNIT_ASSERT(modelSeries->getAttribute("reconstruction_db"));
}

//------------------------------------------------------------------------------

void ModelSeries1Test::copyTest()
{
    using namespace io::patch::structural::creator::fwMedData;
    ModelSeries1 modelSeriesCreator;

    const std::string& classname = modelSeriesCreator.getObjectClassname();
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal", std::string("::sight::data::ModelSeries") == classname );
    const std::string& objectVersion = modelSeriesCreator.getObjectVersion();
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal", std::string("1") == objectVersion );

    ModelSeries1* modelSeriesCreator2 = new ModelSeries1(modelSeriesCreator);

    CPPUNIT_ASSERT(modelSeriesCreator2);
    const std::string& classnameModelSeriesCreator2 = modelSeriesCreator2->getObjectClassname();
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal", std::string(
                               "::sight::data::ModelSeries") == classnameModelSeriesCreator2 );

    const std::string& objectVersionModelSeriesCreator2 = modelSeriesCreator2->getObjectVersion();
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal", std::string("1") == objectVersionModelSeriesCreator2 );

    delete(modelSeriesCreator2);
}

//------------------------------------------------------------------------------

} //namespace data
} //namespace creator
} //namespace ut
} //namespace sight::io::patch::structural
