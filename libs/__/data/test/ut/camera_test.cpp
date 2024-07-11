/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
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
    const double cx                                   = safe_rand() % 10000 / 100.;
    const double cy                                   = safe_rand() % 10000 / 100.;
    const double fx                                   = safe_rand() % 10000 / 100.;
    const double fy                                   = safe_rand() % 10000 / 100.;
    const double skew                                 = safe_rand() % 10000 / 100.;
    const double k1                                   = safe_rand() % 10000 / 100.;
    const double k2                                   = safe_rand() % 10000 / 100.;
    const double p1                                   = safe_rand() % 10000 / 100.;
    const double p2                                   = safe_rand() % 10000 / 100.;
    const double k3                                   = safe_rand() % 10000 / 100.;
    const bool is_calibrated                          = true;
    const std::string desc                            = "My camera";
    const std::string cam_id                          = "CAM_461384568";
    const std::size_t width                           = 800;
    const std::size_t height                          = 800;
    const data::camera::source_t camerasource         = data::camera::device;
    const float maximumframerate                      = 30.F;
    const enum data::camera::pixel_format pixelformat = data::camera::rgba32;
    const std::string videofile                       = "/tmp/video.mp4";
    const std::string streamurl                       = "rtsp://192.168.0.1/h264.sdp";
    const double scale                                = 0.9996;

    data::camera::sptr camera = std::make_shared<data::camera>();
    camera->set_cx(cx);
    camera->set_cy(cy);
    camera->set_fx(fx);
    camera->set_fy(fy);
    camera->set_skew(skew);
    camera->set_distortion_coefficient(k1, k2, p1, p2, k3);
    camera->set_is_calibrated(is_calibrated);
    camera->set_description(desc);
    camera->set_camera_id(cam_id);
    camera->set_width(width);
    camera->set_height(height);
    camera->set_camera_source(camerasource);
    camera->set_maximum_frame_rate(maximumframerate);
    camera->set_pixel_format(pixelformat);
    camera->set_video_file(videofile);
    camera->set_stream_url(streamurl);
    camera->set_scale(scale);

    return camera;
}

//------------------------------------------------------------------------------

void camera_test::param_test()
{
    const double cx                                   = safe_rand() % 10000 / 100.;
    const double cy                                   = safe_rand() % 10000 / 100.;
    const double fx                                   = safe_rand() % 10000 / 100.;
    const double fy                                   = safe_rand() % 10000 / 100.;
    const double skew                                 = safe_rand() % 10000 / 100.;
    const double k1                                   = safe_rand() % 10000 / 100.;
    const double k2                                   = safe_rand() % 10000 / 100.;
    const double p1                                   = safe_rand() % 10000 / 100.;
    const double p2                                   = safe_rand() % 10000 / 100.;
    const double k3                                   = safe_rand() % 10000 / 100.;
    const bool is_calibrated                          = true;
    const std::string desc                            = "My camera";
    const std::string cam_id                          = "CAM_461384568";
    const std::size_t width                           = 800;
    const std::size_t height                          = 800;
    const data::camera::source_t camerasource         = data::camera::device;
    const float maximumframerate                      = 30.F;
    const enum data::camera::pixel_format pixelformat = data::camera::rgba32;
    const std::string videofile                       = "/tmp/video.mp4";
    const std::string streamurl                       = "rtsp://192.168.0.1/h264.sdp";
    const double scale                                = 0.9796;

    data::camera::sptr camera = std::make_shared<data::camera>();
    camera->set_cx(cx);
    camera->set_cy(cy);
    camera->set_fx(fx);
    camera->set_fy(fy);
    camera->set_skew(skew);
    camera->set_distortion_coefficient(k1, k2, p1, p2, k3);
    camera->set_is_calibrated(is_calibrated);
    camera->set_description(desc);
    camera->set_camera_id(cam_id);

    camera->set_width(width);
    camera->set_height(height);
    camera->set_camera_source(camerasource);
    camera->set_maximum_frame_rate(maximumframerate);
    camera->set_pixel_format(pixelformat);
    camera->set_video_file(videofile);
    camera->set_stream_url(streamurl);
    camera->set_scale(scale);

    CPPUNIT_ASSERT_EQUAL(cx, camera->get_cx());
    CPPUNIT_ASSERT_EQUAL(cy, camera->get_cy());
    CPPUNIT_ASSERT_EQUAL(fx, camera->get_fx());
    CPPUNIT_ASSERT_EQUAL(fy, camera->get_fy());
    CPPUNIT_ASSERT_EQUAL(skew, camera->get_skew());

    data::camera::dist_array_t dist = camera->get_distortion_coefficient();
    CPPUNIT_ASSERT_EQUAL(k1, dist[0]);
    CPPUNIT_ASSERT_EQUAL(k2, dist[1]);
    CPPUNIT_ASSERT_EQUAL(p1, dist[2]);
    CPPUNIT_ASSERT_EQUAL(p2, dist[3]);
    CPPUNIT_ASSERT_EQUAL(k3, dist[4]);

    CPPUNIT_ASSERT_EQUAL(is_calibrated, camera->get_is_calibrated());
    CPPUNIT_ASSERT_EQUAL(desc, camera->get_description());
    CPPUNIT_ASSERT_EQUAL(cam_id, camera->get_camera_id());
    CPPUNIT_ASSERT_EQUAL(width, camera->get_width());
    CPPUNIT_ASSERT_EQUAL(height, camera->get_height());
    CPPUNIT_ASSERT_EQUAL(camerasource, camera->get_camera_source());
    CPPUNIT_ASSERT_EQUAL(maximumframerate, camera->get_maximum_frame_rate());
    CPPUNIT_ASSERT_EQUAL(pixelformat, camera->pixel_format());
    CPPUNIT_ASSERT_EQUAL(videofile, camera->get_video_file().string());
    CPPUNIT_ASSERT_EQUAL(streamurl, camera->get_stream_url());
    CPPUNIT_ASSERT_EQUAL(scale, camera->get_scale());
}

//------------------------------------------------------------------------------

void camera_test::shallow_copy_test()
{
    data::camera::sptr camera  = init_camera();
    data::camera::sptr camera2 = std::make_shared<data::camera>();

    CPPUNIT_ASSERT(*camera != *camera2);

    camera2->shallow_copy(camera);

    CPPUNIT_ASSERT(*camera == *camera2);

    CPPUNIT_ASSERT_EQUAL(camera->get_cx(), camera2->get_cx());
    CPPUNIT_ASSERT_EQUAL(camera->get_cy(), camera2->get_cy());
    CPPUNIT_ASSERT_EQUAL(camera->get_fx(), camera2->get_fx());
    CPPUNIT_ASSERT_EQUAL(camera->get_fy(), camera2->get_fy());
    CPPUNIT_ASSERT_EQUAL(camera->get_skew(), camera2->get_skew());

    data::camera::dist_array_t dist  = camera->get_distortion_coefficient();
    data::camera::dist_array_t dist2 = camera2->get_distortion_coefficient();
    CPPUNIT_ASSERT_EQUAL(dist[0], dist2[0]);
    CPPUNIT_ASSERT_EQUAL(dist[1], dist2[1]);
    CPPUNIT_ASSERT_EQUAL(dist[2], dist2[2]);
    CPPUNIT_ASSERT_EQUAL(dist[3], dist2[3]);
    CPPUNIT_ASSERT_EQUAL(dist[4], dist2[4]);

    CPPUNIT_ASSERT_EQUAL(camera->get_is_calibrated(), camera2->get_is_calibrated());
    CPPUNIT_ASSERT_EQUAL(camera->get_description(), camera2->get_description());
    CPPUNIT_ASSERT_EQUAL(camera->get_camera_id(), camera2->get_camera_id());
    CPPUNIT_ASSERT_EQUAL(camera->get_width(), camera2->get_width());
    CPPUNIT_ASSERT_EQUAL(camera->get_height(), camera2->get_height());
    CPPUNIT_ASSERT_EQUAL(camera->get_camera_source(), camera2->get_camera_source());
    CPPUNIT_ASSERT_EQUAL(camera->get_maximum_frame_rate(), camera2->get_maximum_frame_rate());
    CPPUNIT_ASSERT_EQUAL(camera->pixel_format(), camera2->pixel_format());
    CPPUNIT_ASSERT_EQUAL(camera->get_video_file(), camera2->get_video_file());
    CPPUNIT_ASSERT_EQUAL(camera->get_stream_url(), camera2->get_stream_url());
    CPPUNIT_ASSERT_EQUAL(camera->get_scale(), camera2->get_scale());
}

//------------------------------------------------------------------------------

void camera_test::deep_copy_test()
{
    data::camera::sptr camera = init_camera();
    data::camera::sptr camera2;
    camera2 = data::object::copy(camera);
    CPPUNIT_ASSERT(*camera == *camera2);
}

//------------------------------------------------------------------------------

void camera_test::equality_test()
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

    TEST(set_cx(1));
    TEST(set_cy(2));
    TEST(set_fx(3));
    TEST(set_fy(4));
    TEST(set_skew(5));
    TEST(set_distortion_coefficient(6, 7, 8, 9, 10));
    TEST(set_description("11"));
    TEST(set_camera_id("12"));
    TEST(set_width(13));
    TEST(set_height(14));
    TEST(set_maximum_frame_rate(15));
    TEST(set_pixel_format(data::camera::ayuv444_premultiplied));
    TEST(set_video_file("17"));
    TEST(set_stream_url("18"));
    TEST(set_scale(19));
    TEST(set_is_calibrated(true));
    TEST(set_camera_source(data::camera::device));

    #undef TEST
}

//------------------------------------------------------------------------------

void camera_test::get_pixel_format_test()
{
    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(value) CPPUNIT_ASSERT_EQUAL(data::camera::value, camera::pixel_format(#value));
    TEST(invalid);
    TEST(argb32);
    TEST(argb32_premultiplied);
    TEST(rgb32);
    TEST(rgb24);
    TEST(rgb565);
    TEST(rgb555);
    TEST(argb8565_premultiplied);
    TEST(bgra32);
    TEST(bgra32_premultiplied);
    TEST(bgr32);
    TEST(bgr24);
    TEST(bgr565);
    TEST(bgr555);
    TEST(bgra5658_premultiplied);
    TEST(ayuv444);
    TEST(ayuv444_premultiplied);
    TEST(yuv444);
    TEST(yuv420_p);
    TEST(yv12);
    TEST(uyvy);
    TEST(yuyv);
    TEST(nv12);
    TEST(nv21);
    TEST(imc1);
    TEST(imc2);
    TEST(imc3);
    TEST(imc4);
    TEST(y8);
    TEST(y16);
    TEST(jpeg);
    TEST(cameraraw);
    TEST(adobedng);
    TEST(rgba32);
    TEST(user);
    #undef TEST
    CPPUNIT_ASSERT_EQUAL(data::camera::invalid, camera::pixel_format("NOT_A_PIXELFORMAT"));
}

//------------------------------------------------------------------------------

void camera_test::get_pixel_format_name_test()
{
    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(value) CPPUNIT_ASSERT_EQUAL(std::string(#value), camera::get_pixel_format_name(data::camera::value));
    TEST(invalid);
    TEST(argb32);
    TEST(argb32_premultiplied);
    TEST(rgb32);
    TEST(rgb24);
    TEST(rgb565);
    TEST(rgb555);
    TEST(argb8565_premultiplied);
    TEST(bgra32);
    TEST(bgra32_premultiplied);
    TEST(bgr32);
    TEST(bgr24);
    TEST(bgr565);
    TEST(bgr555);
    TEST(bgra5658_premultiplied);
    TEST(ayuv444);
    TEST(ayuv444_premultiplied);
    TEST(yuv444);
    TEST(yuv420_p);
    TEST(yv12);
    TEST(uyvy);
    TEST(yuyv);
    TEST(nv12);
    TEST(nv21);
    TEST(imc1);
    TEST(imc2);
    TEST(imc3);
    TEST(imc4);
    TEST(y8);
    TEST(y16);
    TEST(jpeg);
    TEST(cameraraw);
    TEST(adobedng);
    TEST(rgba32);
    TEST(user);
    #undef TEST
}

} // namespace sight::data::ut
