/************************************************************************
 *
 * Copyright (C) 2014-2026 IRCAD France
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

#include <core/tools/random/generator.hpp>

#include <data/camera.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::data::camera")
{
    using sight::core::tools::random::safe_rand;

//------------------------------------------------------------------------------

    static sight::data::camera::sptr init_camera()
    {
        const double cx                                            = safe_rand() % 10000 / 100.;
        const double cy                                            = safe_rand() % 10000 / 100.;
        const double fx                                            = safe_rand() % 10000 / 100.;
        const double fy                                            = safe_rand() % 10000 / 100.;
        const double skew                                          = safe_rand() % 10000 / 100.;
        const double k1                                            = safe_rand() % 10000 / 100.;
        const double k2                                            = safe_rand() % 10000 / 100.;
        const double p1                                            = safe_rand() % 10000 / 100.;
        const double p2                                            = safe_rand() % 10000 / 100.;
        const double k3                                            = safe_rand() % 10000 / 100.;
        const bool is_calibrated                                   = true;
        const std::string desc                                     = "My camera";
        const std::string cam_id                                   = "CAM_461384568";
        const std::size_t width                                    = 800;
        const std::size_t height                                   = 800;
        const sight::data::camera::source_t camerasource           = sight::data::camera::device;
        const float maximumframerate                               = 30.F;
        const enum sight::data::camera::pixel_format_t pixelformat = sight::data::camera::rgba32;
        const std::string videofile                                = "/tmp/video.mp4";
        const std::string streamurl                                = "rtsp://192.168.0.1/h264.sdp";
        const double scale                                         = 0.9996;
        const double error                                         = 0.023;

        sight::data::camera::sptr camera = std::make_shared<sight::data::camera>();
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
        camera->set_calibration_error(error);

        return camera;
    }

//------------------------------------------------------------------------------

    TEST_CASE("param")
    {
        const double cx                                            = safe_rand() % 10000 / 100.;
        const double cy                                            = safe_rand() % 10000 / 100.;
        const double fx                                            = safe_rand() % 10000 / 100.;
        const double fy                                            = safe_rand() % 10000 / 100.;
        const double skew                                          = safe_rand() % 10000 / 100.;
        const double k1                                            = safe_rand() % 10000 / 100.;
        const double k2                                            = safe_rand() % 10000 / 100.;
        const double p1                                            = safe_rand() % 10000 / 100.;
        const double p2                                            = safe_rand() % 10000 / 100.;
        const double k3                                            = safe_rand() % 10000 / 100.;
        const bool is_calibrated                                   = true;
        const std::string desc                                     = "My camera";
        const std::string cam_id                                   = "CAM_461384568";
        const std::size_t width                                    = 800;
        const std::size_t height                                   = 800;
        const sight::data::camera::source_t camerasource           = sight::data::camera::device;
        const float maximumframerate                               = 30.F;
        const enum sight::data::camera::pixel_format_t pixelformat = sight::data::camera::rgba32;
        const std::string videofile                                = "/tmp/video.mp4";
        const std::string streamurl                                = "rtsp://192.168.0.1/h264.sdp";
        const double scale                                         = 0.9796;
        const double error                                         = 0.123;

        sight::data::camera::sptr camera = std::make_shared<sight::data::camera>();
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
        camera->set_calibration_error(error);

        CHECK_EQ(cx, camera->get_cx());
        CHECK_EQ(cy, camera->get_cy());
        CHECK_EQ(fx, camera->get_fx());
        CHECK_EQ(fy, camera->get_fy());
        CHECK_EQ(skew, camera->get_skew());

        sight::data::camera::dist_array_t dist = camera->get_distortion_coefficient();
        CHECK_EQ(k1, dist[0]);
        CHECK_EQ(k2, dist[1]);
        CHECK_EQ(p1, dist[2]);
        CHECK_EQ(p2, dist[3]);
        CHECK_EQ(k3, dist[4]);

        CHECK_EQ(is_calibrated, camera->get_is_calibrated());
        CHECK_EQ(desc, camera->get_description());
        CHECK_EQ(cam_id, camera->get_camera_id());
        CHECK_EQ(width, camera->get_width());
        CHECK_EQ(height, camera->get_height());
        CHECK_EQ(camerasource, camera->get_camera_source());
        CHECK_EQ(maximumframerate, camera->get_maximum_frame_rate());
        CHECK_EQ(pixelformat, camera->pixel_format());
        CHECK_EQ(videofile, camera->get_video_file().string());
        CHECK_EQ(streamurl, camera->get_stream_url());
        CHECK_EQ(scale, camera->get_scale());
        CHECK_EQ(error, camera->calibration_error());
    }

//------------------------------------------------------------------------------

    TEST_CASE("shallow_copy")
    {
        sight::data::camera::sptr camera  = init_camera();
        sight::data::camera::sptr camera2 = std::make_shared<sight::data::camera>();

        CHECK(*camera != *camera2);

        camera2->shallow_copy(camera);

        CHECK(*camera == *camera2);

        CHECK_EQ(camera->get_cx(), camera2->get_cx());
        CHECK_EQ(camera->get_cy(), camera2->get_cy());
        CHECK_EQ(camera->get_fx(), camera2->get_fx());
        CHECK_EQ(camera->get_fy(), camera2->get_fy());
        CHECK_EQ(camera->get_skew(), camera2->get_skew());

        sight::data::camera::dist_array_t dist  = camera->get_distortion_coefficient();
        sight::data::camera::dist_array_t dist2 = camera2->get_distortion_coefficient();
        CHECK_EQ(dist[0], dist2[0]);
        CHECK_EQ(dist[1], dist2[1]);
        CHECK_EQ(dist[2], dist2[2]);
        CHECK_EQ(dist[3], dist2[3]);
        CHECK_EQ(dist[4], dist2[4]);

        CHECK_EQ(camera->get_is_calibrated(), camera2->get_is_calibrated());
        CHECK_EQ(camera->get_description(), camera2->get_description());
        CHECK_EQ(camera->get_camera_id(), camera2->get_camera_id());
        CHECK_EQ(camera->get_width(), camera2->get_width());
        CHECK_EQ(camera->get_height(), camera2->get_height());
        CHECK_EQ(camera->get_camera_source(), camera2->get_camera_source());
        CHECK_EQ(camera->get_maximum_frame_rate(), camera2->get_maximum_frame_rate());
        CHECK_EQ(camera->pixel_format(), camera2->pixel_format());
        CHECK_EQ(camera->get_video_file(), camera2->get_video_file());
        CHECK_EQ(camera->get_stream_url(), camera2->get_stream_url());
        CHECK_EQ(camera->get_scale(), camera2->get_scale());
        CHECK_EQ(camera->calibration_error(), camera2->calibration_error());
    }

//------------------------------------------------------------------------------

    TEST_CASE("deep_copy")
    {
        sight::data::camera::sptr camera = init_camera();
        sight::data::camera::sptr camera2;
        camera2 = sight::data::object::copy(camera);
        CHECK(*camera == *camera2);
    }

//------------------------------------------------------------------------------

    TEST_CASE("equality")
    {
        auto camera1 = std::make_shared<sight::data::camera>();
        auto camera2 = std::make_shared<sight::data::camera>();

        CHECK(*camera1 == *camera2);
        CHECK(!(*camera1 != *camera2));

        // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(op) \
            camera1->op; \
            CHECK_MESSAGE( \
                *camera1 != *camera2, \
                "The cameras must be different when applying " #op " to the first" \
            ); \
            CHECK_MESSAGE( \
                !(*camera1 == *camera2), \
                "The cameras must be different when applying " #op " to the first" \
            ); \
            camera2->op; \
            CHECK_MESSAGE( \
                *camera1 == *camera2, \
                "The cameras must be equal when applying " #op " to both" \
            ); \
            CHECK_MESSAGE( \
                !(*camera1 != *camera2), \
                "The cameras must be equal when applying " #op " to both" \
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
        TEST(set_pixel_format(sight::data::camera::ayuv444_premultiplied));
        TEST(set_video_file("17"));
        TEST(set_stream_url("18"));
        TEST(set_scale(19));
        TEST(set_is_calibrated(true));
        TEST(set_camera_source(sight::data::camera::device));
        TEST(set_calibration_error(0.569));

    #undef TEST
    }

//------------------------------------------------------------------------------

    TEST_CASE("get_pixel_format")
    {
        // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(value) CHECK_EQ(sight::data::camera::value, sight::data::camera::pixel_format(#value));
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
        CHECK_EQ(sight::data::camera::invalid, sight::data::camera::pixel_format("NOT_A_PIXELFORMAT"));
    }

//------------------------------------------------------------------------------

    TEST_CASE("get_pixel_format_name")
    {
        // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(value) CHECK_EQ( \
                std::string(#value), \
                sight::data::camera::get_pixel_format_name(sight::data::camera::value) \
);
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
} // TEST_SUITE("sight::data::camera")
