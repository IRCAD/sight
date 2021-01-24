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

#include "creator/data/ImageSeries1Test.hpp"

#include "fwAtoms/String.hpp"

#include "fwAtomsPatch/helper/functions.hpp"
#include "fwAtomsPatch/helper/Object.hpp"

#include "fwStructuralPatch/creator/data/ImageSeries1.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwStructuralPatch::ut::creatordata::ImageSeries1Test );

namespace fwStructuralPatch
{
namespace ut
{
namespace creator
{
namespace sight::data
{

//------------------------------------------------------------------------------

void ImageSeries1Test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ImageSeries1Test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ImageSeries1Test::createTest()
{
    using namespace ::fwStructuralPatch::creator::fwMedData;
    ImageSeries1::sptr imageSeriesCreator = ImageSeries1::New();

    CPPUNIT_ASSERT(imageSeriesCreator);
    const std::string& classname = imageSeriesCreator->getObjectClassname();
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal", std::string("data::ImageSeries") == classname );
    const std::string& objectVersion = imageSeriesCreator->getObjectVersion();
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal", std::string("1") == objectVersion );

    ::fwAtoms::Object::sptr imageSeries = imageSeriesCreator->create();

    const std::string& imageSeriesClassname = ::fwAtomsPatch::helper::getClassname(imageSeries);
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal", std::string("data::ImageSeries") == imageSeriesClassname );
    const std::string& imageSeriesObjectVersion = ::fwAtomsPatch::helper::getVersion(imageSeries);
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal", std::string("1") == imageSeriesObjectVersion );

    CPPUNIT_ASSERT(imageSeries->getAttribute("image"));
}

//------------------------------------------------------------------------------

void ImageSeries1Test::copyTest()
{
    using namespace ::fwStructuralPatch::creator::fwMedData;
    ImageSeries1 imageSeriesCreator;

    const std::string& classname = imageSeriesCreator.getObjectClassname();
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal", std::string("data::ImageSeries") == classname );
    const std::string& objectVersion = imageSeriesCreator.getObjectVersion();
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal", std::string("1") == objectVersion );

    ImageSeries1* imageSeriesCreator2 = new ImageSeries1(imageSeriesCreator);

    CPPUNIT_ASSERT(imageSeriesCreator2);
    const std::string& classnameImageSeriesCreator2 = imageSeriesCreator2->getObjectClassname();
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal", std::string(
                               "data::ImageSeries") == classnameImageSeriesCreator2 );

    const std::string& objectVersionImageSeriesCreator2 = imageSeriesCreator2->getObjectVersion();
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal", std::string("1") == objectVersionImageSeriesCreator2 );

    delete(imageSeriesCreator2);
}

//------------------------------------------------------------------------------

} //namespace sight::data
} //namespace creator
} //namespace ut
} //namespace fwStructuralPatch
