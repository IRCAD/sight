/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include "camera_test.hpp"

#include <core/tools/random/generator.hpp>

#include <data/camera.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::camera_test);

namespace sight::data::ut
{

using core::tools::random::safe_rand;

//------------------------------------------------------------------------------

void camera_test::setUp()
{
}

//------------------------------------------------------------------------------

void camera_test::tearDown()
{
}

//------------------------------------------------------------------------------

data::camera::sptr init_camera()
{
    const double cx                             = safe_rand() % 10000 / 100.;
    const double cy                             = safe_rand() % 10000 / 100.;
    const double fx                             = safe_rand() % 10000 / 100.;
    const double fy                             = safe_rand() % 10000 / 100.;
    const double skew                           = safe_rand() % 10000 / 100.;
    const double k1                             = safe_rand() % 10000 / 100.;
    const double k2                             = safe_rand() % 10000 / 100.;
    const double p1                             = safe_rand() % 10000 / 100.;
    const double p2                             = safe_rand() % 10000 / 100.;
    const double k3                             = safe_rand() % 10000 / 100.;
    const bool is_calibrated                    = true;
    const std::string desc                      = "My camera";
    const std::string cam_id                    = "CAM_461384568";
    const std::size_t width                     = 800;
    const std::size_t height                    = 800;
    const data::camera::source_t camerasource   = data::camera::DEVICE;
    const std::string cameraid                  = "/dev/video0";
    const float maximumframerate                = 30.F;
    const data::camera::PixelFormat pixelformat = data::camera::RGBA32;
    const std::string videofile                 = "/tmp/video.mp4";
    const std::string streamurl                 = "rtsp://192.168.0.1/h264.sdp";
    const double scale                          = 0.9996;

    data::camera::sptr camera = std::make_shared<data::camera>();
    camera->setCx(cx);
    camera->setCy(cy);
    camera->setFx(fx);
    camera->setFy(fy);
    camera->setSkew(skew);
    camera->setDistortionCoefficient(k1, k2, p1, p2, k3);
    camera->setIsCalibrated(is_calibrated);
    camera->setDescription(desc);
    camera->setCameraID(cam_id);
    camera->setWidth(width);
    camera->setHeight(height);
    camera->setCameraSource(camerasource);
    camera->setMaximumFrameRate(maximumframerate);
    camera->setPixelFormat(pixelformat);
    camera->setVideoFile(videofile);
    camera->setStreamUrl(streamurl);
    camera->setScale(scale);

    return camera;
}

//------------------------------------------------------------------------------

void camera_test::paramTest()
{
    const double cx                             = safe_rand() % 10000 / 100.;
    const double cy                             = safe_rand() % 10000 / 100.;
    const double fx                             = safe_rand() % 10000 / 100.;
    const double fy                             = safe_rand() % 10000 / 100.;
    const double skew                           = safe_rand() % 10000 / 100.;
    const double k1                             = safe_rand() % 10000 / 100.;
    const double k2                             = safe_rand() % 10000 / 100.;
    const double p1                             = safe_rand() % 10000 / 100.;
    const double p2                             = safe_rand() % 10000 / 100.;
    const double k3                             = safe_rand() % 10000 / 100.;
    const bool is_calibrated                    = true;
    const std::string desc                      = "My camera";
    const std::string cam_id                    = "CAM_461384568";
    const std::size_t width                     = 800;
    const std::size_t height                    = 800;
    const data::camera::source_t camerasource   = data::camera::DEVICE;
    const std::string cameraid                  = "/dev/video0";
    const float maximumframerate                = 30.F;
    const data::camera::PixelFormat pixelformat = data::camera::RGBA32;
    const std::string videofile                 = "/tmp/video.mp4";
    const std::string streamurl                 = "rtsp://192.168.0.1/h264.sdp";
    const double scale                          = 0.9796;

    data::camera::sptr camera = std::make_shared<data::camera>();
    camera->setCx(cx);
    camera->setCy(cy);
    camera->setFx(fx);
    camera->setFy(fy);
    camera->setSkew(skew);
    camera->setDistortionCoefficient(k1, k2, p1, p2, k3);
    camera->setIsCalibrated(is_calibrated);
    camera->setDescription(desc);
    camera->setCameraID(cam_id);

    camera->setWidth(width);
    camera->setHeight(height);
    camera->setCameraSource(camerasource);
    camera->setMaximumFrameRate(maximumframerate);
    camera->setPixelFormat(pixelformat);
    camera->setVideoFile(videofile);
    camera->setStreamUrl(streamurl);
    camera->setScale(scale);

    CPPUNIT_ASSERT_EQUAL(cx, camera->getCx());
    CPPUNIT_ASSERT_EQUAL(cy, camera->getCy());
    CPPUNIT_ASSERT_EQUAL(fx, camera->getFx());
    CPPUNIT_ASSERT_EQUAL(fy, camera->getFy());
    CPPUNIT_ASSERT_EQUAL(skew, camera->getSkew());

    data::camera::dist_array_t dist = camera->getDistortionCoefficient();
    CPPUNIT_ASSERT_EQUAL(k1, dist[0]);
    CPPUNIT_ASSERT_EQUAL(k2, dist[1]);
    CPPUNIT_ASSERT_EQUAL(p1, dist[2]);
    CPPUNIT_ASSERT_EQUAL(p2, dist[3]);
    CPPUNIT_ASSERT_EQUAL(k3, dist[4]);

    CPPUNIT_ASSERT_EQUAL(is_calibrated, camera->getIsCalibrated());
    CPPUNIT_ASSERT_EQUAL(desc, camera->getDescription());
    CPPUNIT_ASSERT_EQUAL(cam_id, camera->getCameraID());
    CPPUNIT_ASSERT_EQUAL(width, camera->getWidth());
    CPPUNIT_ASSERT_EQUAL(height, camera->getHeight());
    CPPUNIT_ASSERT_EQUAL(camerasource, camera->getCameraSource());
    CPPUNIT_ASSERT_EQUAL(maximumframerate, camera->getMaximumFrameRate());
    CPPUNIT_ASSERT_EQUAL(pixelformat, camera->getPixelFormat());
    CPPUNIT_ASSERT_EQUAL(videofile, camera->getVideoFile().string());
    CPPUNIT_ASSERT_EQUAL(streamurl, camera->getStreamUrl());
    CPPUNIT_ASSERT_EQUAL(scale, camera->getScale());
}

//------------------------------------------------------------------------------

void camera_test::shallow_copyTest()
{
    data::camera::sptr camera  = init_camera();
    data::camera::sptr camera2 = std::make_shared<data::camera>();

    CPPUNIT_ASSERT(*camera != *camera2);

    camera2->shallow_copy(camera);

    CPPUNIT_ASSERT(*camera == *camera2);

    CPPUNIT_ASSERT_EQUAL(camera->getCx(), camera2->getCx());
    CPPUNIT_ASSERT_EQUAL(camera->getCy(), camera2->getCy());
    CPPUNIT_ASSERT_EQUAL(camera->getFx(), camera2->getFx());
    CPPUNIT_ASSERT_EQUAL(camera->getFy(), camera2->getFy());
    CPPUNIT_ASSERT_EQUAL(camera->getSkew(), camera2->getSkew());

    data::camera::dist_array_t dist  = camera->getDistortionCoefficient();
    data::camera::dist_array_t dist2 = camera2->getDistortionCoefficient();
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

void camera_test::deep_copyTest()
{
    data::camera::sptr camera = init_camera();
    data::camera::sptr camera2;
    camera2 = data::object::copy(camera);
    CPPUNIT_ASSERT(*camera == *camera2);
}

//------------------------------------------------------------------------------

void camera_test::equalityTest()
{
    auto camera1 = std::make_shared<data::camera>();
    auto camera2 = std::make_shared<data::camera>();

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
    TEST(setPixelFormat(data::camera::AYUV444_PREMULTIPLIED));
    TEST(setVideoFile("17"));
    TEST(setStreamUrl("18"));
    TEST(setScale(19));
    TEST(setIsCalibrated(true));
    TEST(setCameraSource(data::camera::DEVICE));

    #undef TEST
}

//------------------------------------------------------------------------------

void camera_test::getPixelFormatTest()
{
    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(value) CPPUNIT_ASSERT_EQUAL(data::camera::value, camera::getPixelFormat(#value));
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
    CPPUNIT_ASSERT_EQUAL(data::camera::INVALID, camera::getPixelFormat("NOT_A_PIXELFORMAT"));
}

//------------------------------------------------------------------------------

void camera_test::getPixelFormatNameTest()
{
    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(value) CPPUNIT_ASSERT_EQUAL(std::string(#value), camera::getPixelFormatName(data::camera::value));
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
