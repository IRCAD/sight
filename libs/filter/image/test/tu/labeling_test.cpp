/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

#include "labeling_test.hpp"

#include <data/array.hpp>
#include <data/helper/MedicalImage.hpp>
#include <data/image.hpp>
#include <data/point_list.hpp>

#include <filter/image/labeling.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::filter::image::ut::labeling_test);

namespace sight::filter::image::ut
{

//------------------------------------------------------------------------------

void labeling_test::setUp()
{
}

//------------------------------------------------------------------------------

void labeling_test::tearDown()
{
}

//------------------------------------------------------------------------------

void labeling_test::computeCentroids()
{
    // Initial image creation
    data::image::sptr img = std::make_shared<data::image>();

    const core::type type                 = core::type::UINT8;
    const data::image::Size imgSize       = {256, 256, 256};
    const data::image::Spacing imgSpacing = {1., 1., 1.};
    const data::image::Origin imgOrigin   = {0., 0., 0.};
    uint8_t val                           = 0;

    img->setSpacing(imgSpacing);
    img->setOrigin(imgOrigin);
    img->resize(imgSize, type, data::image::PixelFormat::GRAY_SCALE);

    const auto dumpLock = img->dump_lock();

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

    // feature 1
    featureOrigin.emplace_back(3, 96);
    featureSize.emplace_back(3, 64);

    // feature 2
    featureOrigin.emplace_back(3, 16);
    featureSize.emplace_back(3, 16);

    // feature 3
    featureOrigin.emplace_back(3, 255);
    featureSize.emplace_back(3, 1);

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

    std::vector<std::vector<std::size_t> > pointListLabels;
    std::vector<data::point_list::sptr> pointListCentroids;

    // Call the ITK operator
    filter::image::computeCentroids(img, pointListCentroids, pointListLabels);

    data::point_list::sptr landmarks = data::helper::MedicalImage::getLandmarks(*img);

    // Check that we can get the landmarks
    CPPUNIT_ASSERT(landmarks);
    // Ensure that we have as many landmarks as we defined features
    CPPUNIT_ASSERT_EQUAL(landmarks->getPoints().size(), featureOrigin.size());

    // Check that the landmarks are placed at the centroids of the defined features
    for(unsigned int f = 0 ; f < featureOrigin.size() ; ++f)
    {
        data::point::sptr p = landmarks->getPoints().at(f);

        CPPUNIT_ASSERT_EQUAL(featureOrigin[f][0] + (featureSize[f][0] - 1.0) / 2.0, p->getCoord()[0]);
        CPPUNIT_ASSERT_EQUAL(featureOrigin[f][1] + (featureSize[f][1] - 1.0) / 2.0, p->getCoord()[1]);
        CPPUNIT_ASSERT_EQUAL(featureOrigin[f][2] + (featureSize[f][2] - 1.0) / 2.0, p->getCoord()[2]);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::filter::image::ut
