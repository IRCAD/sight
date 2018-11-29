/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
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

#include "CameraTest.hpp"

#include <cvIO/Camera.hpp>

#include <arData/Camera.hpp>

#include <opencv2/core.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION( ::cvIO::ut::CameraTest );

namespace cvIO
{
namespace ut
{

//-----------------------------------------------------------------------------

void CameraTest::setUp()
{

}

//-----------------------------------------------------------------------------

void CameraTest::tearDown()
{

}

//-----------------------------------------------------------------------------

void CameraTest::copyToCv()
{
    ::arData::Camera::sptr sightCam = ::arData::Camera::New();

    sightCam->setCx(666.);
    sightCam->setCy(777.);
    sightCam->setFx(6.);
    sightCam->setFy(7.);
    sightCam->setWidth(15360);
    sightCam->setHeight(8640);
    sightCam->setDistortionCoefficient(2., 3., 5., 8., 13.);

    ::cv::Mat intrinsic, distCoeffs;
    ::cv::Size imgSize;

    std::tie(intrinsic, imgSize, distCoeffs) = ::cvIO::Camera::copyToCv(sightCam);

    CPPUNIT_ASSERT_EQUAL(3, intrinsic.size[0]);
    CPPUNIT_ASSERT_EQUAL(3, intrinsic.size[1]);

    for(std::uint8_t i = 0; i < 3; ++i)
    {
        for(std::uint8_t j = 0; j < 3; ++j)
        {
            if(i == 0 && j == 0)
            {
                CPPUNIT_ASSERT_EQUAL(sightCam->getFx(), intrinsic.at<double>(i, j));
            }
            else if(i == 1 && j == 1)
            {
                CPPUNIT_ASSERT_EQUAL(sightCam->getFy(), intrinsic.at<double>(i, j));
            }
            else if(i == 0 && j == 2)
            {
                CPPUNIT_ASSERT_EQUAL(sightCam->getCx(), intrinsic.at<double>(i, j));
            }
            else if(i == 1 && j == 2)
            {
                CPPUNIT_ASSERT_EQUAL(sightCam->getCy(), intrinsic.at<double>(i, j));
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

    CPPUNIT_ASSERT_EQUAL(sightCam->getWidth(), static_cast<size_t>(imgSize.width));
    CPPUNIT_ASSERT_EQUAL(sightCam->getHeight(), static_cast<size_t>(imgSize.height));

    CPPUNIT_ASSERT_EQUAL(5, distCoeffs.size[0]);

    for(std::uint8_t i = 0; i < 5; ++i)
    {
        CPPUNIT_ASSERT_EQUAL(sightCam->getDistortionCoefficient()[i], distCoeffs.at<double>(i));
    }
}

//-----------------------------------------------------------------------------

} // namespace ut
} // namespace cvIO
