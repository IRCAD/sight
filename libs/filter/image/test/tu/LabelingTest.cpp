/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

#include "LabelingTest.hpp"

#include <data/Array.hpp>
#include <data/fieldHelper/Image.hpp>
#include <data/fieldHelper/MedicalImageHelpers.hpp>
#include <data/Image.hpp>
#include <data/PointList.hpp>

#include <filter/image/Labeling.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::filter::image::ut::LabelingTest);

namespace sight::filter::image
{

namespace ut
{

//------------------------------------------------------------------------------

void LabelingTest::setUp()
{
}

//------------------------------------------------------------------------------

void LabelingTest::tearDown()
{
}

//------------------------------------------------------------------------------

void LabelingTest::computeCentroids()
{
    // Initial image creation
    data::Image::sptr img = data::Image::New();

    const core::tools::Type type          = core::tools::Type::s_UINT8;
    const data::Image::Size imgSize       = {256, 256, 256};
    const data::Image::Spacing imgSpacing = {1., 1., 1.};
    const data::Image::Origin imgOrigin   = {0., 0., 0.};
    uint8_t val                           = 0;

    img->setSpacing(imgSpacing);
    img->setOrigin(imgOrigin);
    img->resize(imgSize, type, data::Image::PixelFormat::GRAY_SCALE);

    const auto dumpLock = img->lock();

    // Setup image with 0 values
    for(unsigned int x = 0 ; x < imgSize[0] ; ++x)
    {
        for(unsigned int y = 0 ; y < imgSize[1] ; ++y)
        {
            for(unsigned int z = 0 ; z < imgSize[2] ; ++z)
            {
                img->at<std::uint8_t>(x, y, z) = val;
            }
        }
    }

    // Add pre-defined features into the images
    std::vector<std::vector<unsigned int> > featureOrigin;
    std::vector<std::vector<unsigned int> > featureSize;
    val = 255;

    // feature 1
    featureOrigin.push_back(std::vector<unsigned int>(3, 96));
    featureSize.push_back(std::vector<unsigned int>(3, 64));

    // feature 2
    featureOrigin.push_back(std::vector<unsigned int>(3, 16));
    featureSize.push_back(std::vector<unsigned int>(3, 16));

    // feature 3
    featureOrigin.push_back(std::vector<unsigned int>(3, 255));
    featureSize.push_back(std::vector<unsigned int>(3, 1));

    // Setup the image with the pre-defined features
    for(unsigned int f = 0 ; f < featureOrigin.size() ; ++f)
    {
        for(unsigned int x = featureOrigin[f][0] ; x < featureOrigin[f][0] + featureSize[f][0] ; ++x)
        {
            for(unsigned int y = featureOrigin[f][1] ; y < featureOrigin[f][1] + featureSize[f][1] ; ++y)
            {
                for(unsigned int z = featureOrigin[f][2] ; z < featureOrigin[f][2] + featureSize[f][2] ; ++z)
                {
                    val                            = static_cast<uint8_t>(f + 1);
                    img->at<std::uint8_t>(x, y, z) = val;
                }
            }
        }
    }

    std::vector<std::vector<size_t> > pointListLabels;
    std::vector<data::PointList::sptr> pointListCentroids;

    // Call the ITK operator
    filter::image::computeCentroids(img, pointListCentroids, pointListLabels);

    data::fieldHelper::MedicalImageHelpers::checkLandmarks(img);
    data::PointList::sptr landmarks =
        img->getField<data::PointList>(data::fieldHelper::Image::m_imageLandmarksId);

    // Check that we can get the landmarks
    CPPUNIT_ASSERT(landmarks);
    // Ensure that we have as many landmarks as we defined features
    CPPUNIT_ASSERT_EQUAL(landmarks->getPoints().size(), featureOrigin.size());

    // Check that the landmarks are placed at the centroids of the defined features
    for(unsigned int f = 0 ; f < featureOrigin.size() ; ++f)
    {
        data::Point::sptr p = landmarks->getPoints().at(f);

        CPPUNIT_ASSERT_EQUAL(featureOrigin[f][0] + (featureSize[f][0] - 1.0) / 2.0, p->getCoord()[0]);
        CPPUNIT_ASSERT_EQUAL(featureOrigin[f][1] + (featureSize[f][1] - 1.0) / 2.0, p->getCoord()[1]);
        CPPUNIT_ASSERT_EQUAL(featureOrigin[f][2] + (featureSize[f][2] - 1.0) / 2.0, p->getCoord()[2]);
    }
}

//------------------------------------------------------------------------------

} //namespace ut.

} //namespace sight::filter::image.
