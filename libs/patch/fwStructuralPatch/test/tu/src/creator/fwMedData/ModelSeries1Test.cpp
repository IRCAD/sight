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

#include "fwStructuralPatch/creator/fwMedData/ModelSeries1.hpp"
#include "fwAtomsPatch/helper/functions.hpp"
#include "fwAtomsPatch/helper/Object.hpp"
#include "fwAtoms/String.hpp"

#include "creator/fwMedData/ModelSeries1Test.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwStructuralPatch::ut::creator::fwMedData::ModelSeries1Test );

namespace fwStructuralPatch
{
namespace ut
{
namespace creator
{
namespace fwMedData
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
    using namespace ::fwStructuralPatch::creator::fwMedData;
    ModelSeries1::sptr modelSeriesCreator = ModelSeries1::New();

    CPPUNIT_ASSERT(modelSeriesCreator);
    const std::string& classname = modelSeriesCreator->getObjectClassname();
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal", std::string("::fwMedData::ModelSeries") == classname );
    const std::string& objectVersion = modelSeriesCreator->getObjectVersion();
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal", std::string("1") == objectVersion );

    ::fwAtoms::Object::sptr modelSeries = modelSeriesCreator->create();

    const std::string& modelSeriesClassname = ::fwAtomsPatch::helper::getClassname(modelSeries);
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal", std::string("::fwMedData::ModelSeries") == modelSeriesClassname );
    const std::string& modelSeriesObjectVersion = ::fwAtomsPatch::helper::getVersion(modelSeries);
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal", std::string("1") == modelSeriesObjectVersion );

    CPPUNIT_ASSERT(modelSeries->getAttribute("reconstruction_db"));
}

//------------------------------------------------------------------------------

void ModelSeries1Test::copyTest()
{
    using namespace ::fwStructuralPatch::creator::fwMedData;
    ModelSeries1 modelSeriesCreator;

    const std::string& classname = modelSeriesCreator.getObjectClassname();
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal", std::string("::fwMedData::ModelSeries") == classname );
    const std::string& objectVersion = modelSeriesCreator.getObjectVersion();
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal", std::string("1") == objectVersion );

    ModelSeries1* modelSeriesCreator2 = new ModelSeries1(modelSeriesCreator);

    CPPUNIT_ASSERT(modelSeriesCreator2);
    const std::string& classnameModelSeriesCreator2 = modelSeriesCreator2->getObjectClassname();
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal", std::string(
                               "::fwMedData::ModelSeries") == classnameModelSeriesCreator2 );

    const std::string& objectVersionModelSeriesCreator2 = modelSeriesCreator2->getObjectVersion();
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal", std::string("1") == objectVersionModelSeriesCreator2 );

    delete(modelSeriesCreator2);
}

//------------------------------------------------------------------------------

} //namespace fwMedData
} //namespace creator
} //namespace ut
} //namespace fwStructuralPatch
