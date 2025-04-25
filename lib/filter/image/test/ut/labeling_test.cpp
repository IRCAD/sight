/************************************************************************
 *
 * Copyright (C) 2018-2025 IRCAD France
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
#include <data/helper/medical_image.hpp>
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

void labeling_test::compute_centroids()
{
    // Initial image creation
    data::image::sptr img = std::make_shared<data::image>();

    const core::type type                    = core::type::UINT8;
    const data::image::size_t img_size       = {256, 256, 256};
    const data::image::spacing_t img_spacing = {1., 1., 1.};
    const data::image::origin_t img_origin   = {0., 0., 0.};
    uint8_t val                              = 0;

    img->set_spacing(img_spacing);
    img->set_origin(img_origin);
    img->resize(img_size, type, data::image::pixel_format_t::gray_scale);

    const auto dump_lock = img->dump_lock();

    // Setup image with 0 values
    for(unsigned int x = 0 ; x < img_size[0] ; ++x)
    {
        for(unsigned int y = 0 ; y < img_size[1] ; ++y)
        {
            for(unsigned int z = 0 ; z < img_size[2] ; ++z)
            {
                img->at<std::uint8_t>(x, y, z) = val;
            }
        }
    }

    // Add pre-defined features into the images
    std::vector<std::vector<unsigned int> > feature_origin;
    std::vector<std::vector<unsigned int> > feature_size;

    // feature 1
    feature_origin.emplace_back(3, 96);
    feature_size.emplace_back(3, 64);

    // feature 2
    feature_origin.emplace_back(3, 16);
    feature_size.emplace_back(3, 16);

    // feature 3
    feature_origin.emplace_back(3, 255);
    feature_size.emplace_back(3, 1);

    // Setup the image with the pre-defined features
    for(unsigned int f = 0 ; f < feature_origin.size() ; ++f)
    {
        for(unsigned int x = feature_origin[f][0] ; x < feature_origin[f][0] + feature_size[f][0] ; ++x)
        {
            for(unsigned int y = feature_origin[f][1] ; y < feature_origin[f][1] + feature_size[f][1] ; ++y)
            {
                for(unsigned int z = feature_origin[f][2] ; z < feature_origin[f][2] + feature_size[f][2] ; ++z)
                {
                    val                            = static_cast<uint8_t>(f + 1);
                    img->at<std::uint8_t>(x, y, z) = val;
                }
            }
        }
    }

    std::vector<std::vector<std::size_t> > point_list_labels;
    std::vector<data::point_list::sptr> point_list_centroids;

    // Call the ITK operator
    filter::image::compute_centroids(img, point_list_centroids, point_list_labels);

    data::point_list::sptr landmarks = data::helper::medical_image::get_landmarks(*img);

    // Check that we can get the landmarks
    CPPUNIT_ASSERT(landmarks);
    // Ensure that we have as many landmarks as we defined features
    CPPUNIT_ASSERT_EQUAL(landmarks->get_points().size(), feature_origin.size());

    // Check that the landmarks are placed at the centroids of the defined features
    for(unsigned int f = 0 ; f < feature_origin.size() ; ++f)
    {
        data::point::sptr p = landmarks->get_points().at(f);

        CPPUNIT_ASSERT_EQUAL(feature_origin[f][0] + (feature_size[f][0] - 1.0) / 2.0, (*p)[0]);
        CPPUNIT_ASSERT_EQUAL(feature_origin[f][1] + (feature_size[f][1] - 1.0) / 2.0, (*p)[1]);
        CPPUNIT_ASSERT_EQUAL(feature_origin[f][2] + (feature_size[f][2] - 1.0) / 2.0, (*p)[2]);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::filter::image::ut
