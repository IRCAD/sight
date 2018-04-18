/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
    ::arData::Camera::sptr f4sCam = ::arData::Camera::New();

    f4sCam->setCx(666.);
    f4sCam->setCy(777.);
    f4sCam->setFx(6.);
    f4sCam->setFy(7.);
    f4sCam->setWidth(15360);
    f4sCam->setHeight(8640);
    f4sCam->setDistortionCoefficient(2., 3., 5., 8., 13.);

    ::cv::Mat intrinsic, distCoeffs;
    ::cv::Size imgSize;

    std::tie(intrinsic, imgSize, distCoeffs) = ::cvIO::Camera::copyToCv(f4sCam);

    CPPUNIT_ASSERT_EQUAL(3, intrinsic.size[0]);
    CPPUNIT_ASSERT_EQUAL(3, intrinsic.size[1]);

    for(std::uint8_t i = 0; i < 3; ++i)
    {
        for(std::uint8_t j = 0; j < 3; ++j)
        {
            if(i == 0 && j == 0)
            {
                CPPUNIT_ASSERT_EQUAL(f4sCam->getFx(), intrinsic.at<double>(i, j));
            }
            else if(i == 1 && j == 1)
            {
                CPPUNIT_ASSERT_EQUAL(f4sCam->getFy(), intrinsic.at<double>(i, j));
            }
            else if(i == 0 && j == 2)
            {
                CPPUNIT_ASSERT_EQUAL(f4sCam->getCx(), intrinsic.at<double>(i, j));
            }
            else if(i == 1 && j == 2)
            {
                CPPUNIT_ASSERT_EQUAL(f4sCam->getCy(), intrinsic.at<double>(i, j));
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

    CPPUNIT_ASSERT_EQUAL(f4sCam->getWidth(), static_cast<size_t>(imgSize.width));
    CPPUNIT_ASSERT_EQUAL(f4sCam->getHeight(), static_cast<size_t>(imgSize.height));

    CPPUNIT_ASSERT_EQUAL(5, distCoeffs.size[0]);

    for(std::uint8_t i = 0; i < 5; ++i)
    {
        CPPUNIT_ASSERT_EQUAL(f4sCam->getDistortionCoefficient()[i], distCoeffs.at<double>(i));
    }
}

//-----------------------------------------------------------------------------

} // namespace ut
} // namespace cvIO
