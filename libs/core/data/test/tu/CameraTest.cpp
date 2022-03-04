/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include <core/tools/NumericRoundCast.hxx>
#include <core/tools/random/Generator.hpp>

#include <data/Camera.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::CameraTest);

namespace sight::data
{

namespace ut
{

using core::tools::random::safeRand;

//------------------------------------------------------------------------------

void CameraTest::setUp()
{
}

//------------------------------------------------------------------------------

void CameraTest::tearDown()
{
}

//------------------------------------------------------------------------------

data::Camera::sptr initCamera()
{
    const double CX                             = safeRand() % 10000 / 100;
    const double CY                             = safeRand() % 10000 / 100;
    const double FX                             = safeRand() % 10000 / 100;
    const double FY                             = safeRand() % 10000 / 100;
    const double SKEW                           = safeRand() % 10000 / 100;
    const double K1                             = safeRand() % 10000 / 100;
    const double K2                             = safeRand() % 10000 / 100;
    const double P1                             = safeRand() % 10000 / 100;
    const double P2                             = safeRand() % 10000 / 100;
    const double K3                             = safeRand() % 10000 / 100;
    const bool IS_CALIBRATED                    = true;
    const std::string DESC                      = "My camera";
    const std::string CAM_ID                    = "CAM_461384568";
    const std::size_t WIDTH                     = 800;
    const std::size_t HEIGHT                    = 800;
    const data::Camera::SourceType CAMERASOURCE = data::Camera::DEVICE;
    const std::string CAMERAID                  = "/dev/video0";
    const float MAXIMUMFRAMERATE                = 30.f;
    const data::Camera::PixelFormat PIXELFORMAT = data::Camera::RGBA32;
    const std::string VIDEOFILE                 = "/tmp/video.mp4";
    const std::string STREAMURL                 = "rtsp://192.168.0.1/h264.sdp";
    const double SCALE                          = 0.9996;

    data::Camera::sptr camera = data::Camera::New();
    camera->setCx(CX);
    camera->setCy(CY);
    camera->setFx(FX);
    camera->setFy(FY);
    camera->setSkew(SKEW);
    camera->setDistortionCoefficient(K1, K2, P1, P2, K3);
    camera->setIsCalibrated(IS_CALIBRATED);
    camera->setDescription(DESC);
    camera->setCameraID(CAM_ID);
    camera->setWidth(WIDTH);
    camera->setHeight(HEIGHT);
    camera->setCameraSource(CAMERASOURCE);
    camera->setMaximumFrameRate(MAXIMUMFRAMERATE);
    camera->setPixelFormat(PIXELFORMAT);
    camera->setVideoFile(VIDEOFILE);
    camera->setStreamUrl(STREAMURL);
    camera->setScale(SCALE);

    return camera;
}

//------------------------------------------------------------------------------

void CameraTest::paramTest()
{
    const double CX                             = safeRand() % 10000 / 100;
    const double CY                             = safeRand() % 10000 / 100;
    const double FX                             = safeRand() % 10000 / 100;
    const double FY                             = safeRand() % 10000 / 100;
    const double SKEW                           = safeRand() % 10000 / 100;
    const double K1                             = safeRand() % 10000 / 100;
    const double K2                             = safeRand() % 10000 / 100;
    const double P1                             = safeRand() % 10000 / 100;
    const double P2                             = safeRand() % 10000 / 100;
    const double K3                             = safeRand() % 10000 / 100;
    const bool IS_CALIBRATED                    = true;
    const std::string DESC                      = "My camera";
    const std::string CAM_ID                    = "CAM_461384568";
    const std::size_t WIDTH                     = 800;
    const std::size_t HEIGHT                    = 800;
    const data::Camera::SourceType CAMERASOURCE = data::Camera::DEVICE;
    const std::string CAMERAID                  = "/dev/video0";
    const float MAXIMUMFRAMERATE                = 30.f;
    const data::Camera::PixelFormat PIXELFORMAT = data::Camera::RGBA32;
    const std::string VIDEOFILE                 = "/tmp/video.mp4";
    const std::string STREAMURL                 = "rtsp://192.168.0.1/h264.sdp";
    const double SCALE                          = 0.9796;

    data::Camera::sptr camera = data::Camera::New();
    camera->setCx(CX);
    camera->setCy(CY);
    camera->setFx(FX);
    camera->setFy(FY);
    camera->setSkew(SKEW);
    camera->setDistortionCoefficient(K1, K2, P1, P2, K3);
    camera->setIsCalibrated(IS_CALIBRATED);
    camera->setDescription(DESC);
    camera->setCameraID(CAM_ID);

    camera->setWidth(WIDTH);
    camera->setHeight(HEIGHT);
    camera->setCameraSource(CAMERASOURCE);
    camera->setMaximumFrameRate(MAXIMUMFRAMERATE);
    camera->setPixelFormat(PIXELFORMAT);
    camera->setVideoFile(VIDEOFILE);
    camera->setStreamUrl(STREAMURL);
    camera->setScale(SCALE);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(CX, camera->getCx(), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(CY, camera->getCy(), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(FX, camera->getFx(), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(FY, camera->getFy(), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(SKEW, camera->getSkew(), 0.00001);

    data::Camera::DistArrayType dist = camera->getDistortionCoefficient();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(K1, dist[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(K2, dist[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(P1, dist[2], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(P2, dist[3], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(K3, dist[4], 0.00001);

    CPPUNIT_ASSERT_EQUAL(IS_CALIBRATED, camera->getIsCalibrated());
    CPPUNIT_ASSERT_EQUAL(DESC, camera->getDescription());
    CPPUNIT_ASSERT_EQUAL(CAM_ID, camera->getCameraID());
    CPPUNIT_ASSERT_EQUAL(WIDTH, camera->getWidth());
    CPPUNIT_ASSERT_EQUAL(HEIGHT, camera->getHeight());
    CPPUNIT_ASSERT_EQUAL(CAMERASOURCE, camera->getCameraSource());
    CPPUNIT_ASSERT_EQUAL(MAXIMUMFRAMERATE, camera->getMaximumFrameRate());
    CPPUNIT_ASSERT_EQUAL(PIXELFORMAT, camera->getPixelFormat());
    CPPUNIT_ASSERT_EQUAL(VIDEOFILE, camera->getVideoFile().string());
    CPPUNIT_ASSERT_EQUAL(STREAMURL, camera->getStreamUrl());
    CPPUNIT_ASSERT_EQUAL(SCALE, camera->getScale());
}

//------------------------------------------------------------------------------

void CameraTest::shallowCopyTest()
{
    data::Camera::sptr camera  = initCamera();
    data::Camera::sptr camera2 = data::Camera::New();

    CPPUNIT_ASSERT(*camera != *camera2);

    camera2->shallowCopy(camera);

    CPPUNIT_ASSERT(*camera == *camera2);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(camera->getCx(), camera2->getCx(), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(camera->getCy(), camera2->getCy(), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(camera->getFx(), camera2->getFx(), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(camera->getFy(), camera2->getFy(), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(camera->getSkew(), camera2->getSkew(), 0.00001);

    data::Camera::DistArrayType dist  = camera->getDistortionCoefficient();
    data::Camera::DistArrayType dist2 = camera2->getDistortionCoefficient();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(dist[0], dist2[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(dist[1], dist2[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(dist[2], dist2[2], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(dist[3], dist2[3], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(dist[4], dist2[4], 0.00001);

    CPPUNIT_ASSERT_EQUAL(camera->getIsCalibrated(), camera2->getIsCalibrated());
    CPPUNIT_ASSERT_EQUAL(camera->getDescription(), camera2->getDescription());
    CPPUNIT_ASSERT_EQUAL(camera->getCameraID(), camera2->getCameraID());
    CPPUNIT_ASSERT_EQUAL(camera->getWidth(), camera2->getWidth());
    CPPUNIT_ASSERT_EQUAL(camera->getHeight(), camera2->getHeight());
    CPPUNIT_ASSERT_EQUAL(camera->getCameraSource(), camera2->getCameraSource());
    CPPUNIT_ASSERT_EQUAL(camera->getMaximumFrameRate(), camera2->getMaximumFrameRate());
    CPPUNIT_ASSERT_EQUAL(camera->getPixelFormat(), camera2->getPixelFormat());
    CPPUNIT_ASSERT_EQUAL(camera->getVideoFile(), camera2->getVideoFile());
    CPPUNIT_ASSERT_EQUAL(camera->getStreamUrl(), camera2->getStreamUrl());
    CPPUNIT_ASSERT_EQUAL(camera->getScale(), camera2->getScale());
}

//------------------------------------------------------------------------------

void CameraTest::deepCopyTest()
{
    data::Camera::sptr camera = initCamera();
    data::Camera::sptr camera2;
    camera2 = data::Object::copy(camera);
    CPPUNIT_ASSERT(*camera == *camera2);
}

} //namespace ut

} //namespace sight::data
