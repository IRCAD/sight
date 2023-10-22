/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "camera_test.hpp"

#include <data/camera.hpp>

#include <io/opencv/camera.hpp>

#include <opencv2/core.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::opencv::ut::camera_test);

namespace sight::io::opencv::ut
{

//-----------------------------------------------------------------------------

void camera_test::setUp()
{
}

//-----------------------------------------------------------------------------

void camera_test::tearDown()
{
}

//-----------------------------------------------------------------------------

void camera_test::copyToCv()
{
    data::camera::sptr sight_cam = std::make_shared<data::camera>();

    sight_cam->setCx(666.);
    sight_cam->setCy(777.);
    sight_cam->setFx(6.);
    sight_cam->setFy(7.);
    sight_cam->setWidth(15360);
    sight_cam->setHeight(8640);
    sight_cam->setDistortionCoefficient(2., 3., 5., 8., 13.);

    cv::Mat intrinsic;
    cv::Mat dist_coeffs;
    cv::Size img_size;

    std::tie(intrinsic, img_size, dist_coeffs) = io::opencv::camera::copyToCv(sight_cam);

    CPPUNIT_ASSERT_EQUAL(3, intrinsic.size[0]);
    CPPUNIT_ASSERT_EQUAL(3, intrinsic.size[1]);

    for(std::uint8_t i = 0 ; i < 3 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 3 ; ++j)
        {
            if(i == 0 && j == 0)
            {
                CPPUNIT_ASSERT_EQUAL(sight_cam->getFx(), intrinsic.at<double>(i, j));
            }
            else if(i == 1 && j == 1)
            {
                CPPUNIT_ASSERT_EQUAL(sight_cam->getFy(), intrinsic.at<double>(i, j));
            }
            else if(i == 0 && j == 2)
            {
                CPPUNIT_ASSERT_EQUAL(sight_cam->getCx(), intrinsic.at<double>(i, j));
            }
            else if(i == 1 && j == 2)
            {
                CPPUNIT_ASSERT_EQUAL(sight_cam->getCy(), intrinsic.at<double>(i, j));
            }
            else if(i == 2 && j == 2)
            {
                CPPUNIT_ASSERT_EQUAL(1., intrinsic.at<double>(i, j));
            }
            else
            {
                CPPUNIT_ASSERT_EQUAL(0., intrinsic.at<double>(i, j));
            }
        }
    }

    CPPUNIT_ASSERT_EQUAL(sight_cam->getWidth(), static_cast<std::size_t>(img_size.width));
    CPPUNIT_ASSERT_EQUAL(sight_cam->getHeight(), static_cast<std::size_t>(img_size.height));

    CPPUNIT_ASSERT_EQUAL(5, dist_coeffs.size[0]);

    for(std::uint8_t i = 0 ; i < 5 ; ++i)
    {
        CPPUNIT_ASSERT_EQUAL(sight_cam->getDistortionCoefficient()[i], dist_coeffs.at<double>(i));
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::io::opencv::ut
