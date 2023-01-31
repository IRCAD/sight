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

#include <core/tools/random/Generator.hpp>

#include <data/Camera.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::CameraTest);

namespace sight::data::ut
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
    const double CX                             = safeRand() % 10000 / 100.;
    const double CY                             = safeRand() % 10000 / 100.;
    const double FX                             = safeRand() % 10000 / 100.;
    const double FY                             = safeRand() % 10000 / 100.;
    const double SKEW                           = safeRand() % 10000 / 100.;
    const double K1                             = safeRand() % 10000 / 100.;
    const double K2                             = safeRand() % 10000 / 100.;
    const double P1                             = safeRand() % 10000 / 100.;
    const double P2                             = safeRand() % 10000 / 100.;
    const double K3                             = safeRand() % 10000 / 100.;
    const bool IS_CALIBRATED                    = true;
    const std::string DESC                      = "My camera";
    const std::string CAM_ID                    = "CAM_461384568";
    const std::size_t WIDTH                     = 800;
    const std::size_t HEIGHT                    = 800;
    const data::Camera::SourceType CAMERASOURCE = data::Camera::DEVICE;
    const std::string CAMERAID                  = "/dev/video0";
    const float MAXIMUMFRAMERATE                = 30.F;
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
    const double CX                             = safeRand() % 10000 / 100.;
    const double CY                             = safeRand() % 10000 / 100.;
    const double FX                             = safeRand() % 10000 / 100.;
    const double FY                             = safeRand() % 10000 / 100.;
    const double SKEW                           = safeRand() % 10000 / 100.;
    const double K1                             = safeRand() % 10000 / 100.;
    const double K2                             = safeRand() % 10000 / 100.;
    const double P1                             = safeRand() % 10000 / 100.;
    const double P2                             = safeRand() % 10000 / 100.;
    const double K3                             = safeRand() % 10000 / 100.;
    const bool IS_CALIBRATED                    = true;
    const std::string DESC                      = "My camera";
    const std::string CAM_ID                    = "CAM_461384568";
    const std::size_t WIDTH                     = 800;
    const std::size_t HEIGHT                    = 800;
    const data::Camera::SourceType CAMERASOURCE = data::Camera::DEVICE;
    const std::string CAMERAID                  = "/dev/video0";
    const float MAXIMUMFRAMERATE                = 30.F;
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

    CPPUNIT_ASSERT_EQUAL(CX, camera->getCx());
    CPPUNIT_ASSERT_EQUAL(CY, camera->getCy());
    CPPUNIT_ASSERT_EQUAL(FX, camera->getFx());
    CPPUNIT_ASSERT_EQUAL(FY, camera->getFy());
    CPPUNIT_ASSERT_EQUAL(SKEW, camera->getSkew());

    data::Camera::DistArrayType dist = camera->getDistortionCoefficient();
    CPPUNIT_ASSERT_EQUAL(K1, dist[0]);
    CPPUNIT_ASSERT_EQUAL(K2, dist[1]);
    CPPUNIT_ASSERT_EQUAL(P1, dist[2]);
    CPPUNIT_ASSERT_EQUAL(P2, dist[3]);
    CPPUNIT_ASSERT_EQUAL(K3, dist[4]);

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

    CPPUNIT_ASSERT_EQUAL(camera->getCx(), camera2->getCx());
    CPPUNIT_ASSERT_EQUAL(camera->getCy(), camera2->getCy());
    CPPUNIT_ASSERT_EQUAL(camera->getFx(), camera2->getFx());
    CPPUNIT_ASSERT_EQUAL(camera->getFy(), camera2->getFy());
    CPPUNIT_ASSERT_EQUAL(camera->getSkew(), camera2->getSkew());

    data::Camera::DistArrayType dist  = camera->getDistortionCoefficient();
    data::Camera::DistArrayType dist2 = camera2->getDistortionCoefficient();
    CPPUNIT_ASSERT_EQUAL(dist[0], dist2[0]);
    CPPUNIT_ASSERT_EQUAL(dist[1], dist2[1]);
    CPPUNIT_ASSERT_EQUAL(dist[2], dist2[2]);
    CPPUNIT_ASSERT_EQUAL(dist[3], dist2[3]);
    CPPUNIT_ASSERT_EQUAL(dist[4], dist2[4]);

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

//------------------------------------------------------------------------------

void CameraTest::equalityTest()
{
    auto camera1 = data::Camera::New();
    auto camera2 = data::Camera::New();

    CPPUNIT_ASSERT(*camera1 == *camera2 && !(*camera1 != *camera2));

    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(op) \
    camera1->op; \
    CPPUNIT_ASSERT_MESSAGE( \
        "The cameras must be different when applying " #op " to the first", \
        *camera1 != *camera2 && !(*camera1 == *camera2) \
    ); \
    camera2->op; \
    CPPUNIT_ASSERT_MESSAGE( \
        "The cameras must be equal when applying " #op " to both", \
        *camera1 == *camera2 && !(*camera1 != *camera2) \
    );

    TEST(setCx(1));
    TEST(setCy(2));
    TEST(setFx(3));
    TEST(setFy(4));
    TEST(setSkew(5));
    TEST(setDistortionCoefficient(6, 7, 8, 9, 10));
    TEST(setDescription("11"));
    TEST(setCameraID("12"));
    TEST(setWidth(13));
    TEST(setHeight(14));
    TEST(setMaximumFrameRate(15));
    TEST(setPixelFormat(data::Camera::AYUV444_PREMULTIPLIED));
    TEST(setVideoFile("17"));
    TEST(setStreamUrl("18"));
    TEST(setScale(19));
    TEST(setIsCalibrated(true));
    TEST(setCameraSource(data::Camera::DEVICE));

    #undef TEST
}

//------------------------------------------------------------------------------

void CameraTest::getPixelFormatTest()
{
    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(value) CPPUNIT_ASSERT_EQUAL(data::Camera::value, Camera::getPixelFormat(#value));
    TEST(INVALID);
    TEST(ARGB32);
    TEST(ARGB32_PREMULTIPLIED);
    TEST(RGB32);
    TEST(RGB24);
    TEST(RGB565);
    TEST(RGB555);
    TEST(ARGB8565_PREMULTIPLIED);
    TEST(BGRA32);
    TEST(BGRA32_PREMULTIPLIED);
    TEST(BGR32);
    TEST(BGR24);
    TEST(BGR565);
    TEST(BGR555);
    TEST(BGRA5658_PREMULTIPLIED);
    TEST(AYUV444);
    TEST(AYUV444_PREMULTIPLIED);
    TEST(YUV444);
    TEST(YUV420P);
    TEST(YV12);
    TEST(UYVY);
    TEST(YUYV);
    TEST(NV12);
    TEST(NV21);
    TEST(IMC1);
    TEST(IMC2);
    TEST(IMC3);
    TEST(IMC4);
    TEST(Y8);
    TEST(Y16);
    TEST(JPEG);
    TEST(CAMERARAW);
    TEST(ADOBEDNG);
    TEST(RGBA32);
    TEST(USER);
    #undef TEST
    CPPUNIT_ASSERT_EQUAL(data::Camera::INVALID, Camera::getPixelFormat("NOT_A_PIXELFORMAT"));
}

//------------------------------------------------------------------------------

void CameraTest::getPixelFormatNameTest()
{
    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(value) CPPUNIT_ASSERT_EQUAL(std::string(#value), Camera::getPixelFormatName(data::Camera::value));
    TEST(INVALID);
    TEST(ARGB32);
    TEST(ARGB32_PREMULTIPLIED);
    TEST(RGB32);
    TEST(RGB24);
    TEST(RGB565);
    TEST(RGB555);
    TEST(ARGB8565_PREMULTIPLIED);
    TEST(BGRA32);
    TEST(BGRA32_PREMULTIPLIED);
    TEST(BGR32);
    TEST(BGR24);
    TEST(BGR565);
    TEST(BGR555);
    TEST(BGRA5658_PREMULTIPLIED);
    TEST(AYUV444);
    TEST(AYUV444_PREMULTIPLIED);
    TEST(YUV444);
    TEST(YUV420P);
    TEST(YV12);
    TEST(UYVY);
    TEST(YUYV);
    TEST(NV12);
    TEST(NV21);
    TEST(IMC1);
    TEST(IMC2);
    TEST(IMC3);
    TEST(IMC4);
    TEST(Y8);
    TEST(Y16);
    TEST(JPEG);
    TEST(CAMERARAW);
    TEST(ADOBEDNG);
    TEST(RGBA32);
    TEST(USER);
    #undef TEST
}

} // namespace sight::data::ut
