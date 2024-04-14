/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include "helper_test.hpp"

#include <core/os/temp_path.hpp>

#include <data/camera.hpp>

#include <navigation/openvslam/helper.hpp>

#include <openvslam/camera/perspective.h>
#include <openvslam/system.h>

#include <spdlog/spdlog.h>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::navigation::openvslam::ut::helper_test);

namespace sight::navigation::openvslam::ut
{

//-----------------------------------------------------------------------------

void helper_test::setUp()
{
}

//-----------------------------------------------------------------------------

void helper_test::tearDown()
{
}

//-----------------------------------------------------------------------------

void helper_test::to_sight()
{
    const ::openvslam::camera::perspective o_v_slam_cam =
        ::openvslam::camera::perspective(
            "Dummy SLAM camera",
            ::openvslam::camera::setup_type_t::Monocular,
            ::openvslam::camera::color_order_t::RGB,
            1920,
            1080,
            60,
            400,
            401,
            200,
            201,
            0.0,
            0.1,
            0.2,
            0.3,
            0.4
        );

    const auto cam = navigation::openvslam::helper::to_sight(o_v_slam_cam);

    compare_cam(*cam, o_v_slam_cam, true);
}

//-----------------------------------------------------------------------------

void helper_test::from_sight()
{
    //Create a dummy data::camera;
    data::camera cam;

    cam.set_camera_id("Dummy Test Camera");
    cam.set_width(1920);
    cam.set_width(1080);
    cam.set_fx(400);
    cam.set_fy(401);
    cam.set_cx(200);
    cam.set_cy(201);
    cam.set_distortion_coefficient(0.0, 0.1, 0.2, 0.3, 0.4);
    cam.set_maximum_frame_rate(60.0);

    const auto o_v_slam_cam = navigation::openvslam::helper::from_sight(cam);

    // Compare values.
    compare_cam(cam, o_v_slam_cam, false);
}

//-----------------------------------------------------------------------------

void helper_test::create_config()
{
    //Create a dummy data::camera;
    data::camera cam;

    cam.set_camera_id("Dummy Test Camera");
    cam.set_width(1920);
    cam.set_width(1080);
    cam.set_fx(400);
    cam.set_fy(401);
    cam.set_cx(200);
    cam.set_cy(201);
    cam.set_distortion_coefficient(0.0, 0.1, 0.2, 0.3, 0.4);
    cam.set_maximum_frame_rate(60.0);

    navigation::openvslam::orb_params orb_param;
    orb_param.max_num_key_pts = 8000;
    orb_param.scale_factor    = 1.2F;
    orb_param.ini_fast_thr    = 2;
    orb_param.min_fast_thr    = 1;
    orb_param.num_levels      = 8;

    const auto config = navigation::openvslam::helper::create_monocular_config(cam, orb_param);

    // We know that the camera is perspective.
    auto* camera = dynamic_cast< ::openvslam::camera::perspective*>(config->camera_);

    ::openvslam::feature::orb_params orb_param_from_config = config->orb_params_;

    compare_cam(cam, *camera, true);

    CPPUNIT_ASSERT_EQUAL(orb_param_from_config.max_num_keypts_, orb_param.max_num_key_pts);
    CPPUNIT_ASSERT_EQUAL(orb_param_from_config.num_levels_, orb_param.num_levels);
    CPPUNIT_ASSERT_EQUAL(orb_param_from_config.ini_fast_thr_, orb_param.ini_fast_thr);
    CPPUNIT_ASSERT_EQUAL(orb_param_from_config.min_fast_thr, orb_param.min_fast_thr);
    CPPUNIT_ASSERT_EQUAL(orb_param_from_config.scale_factor_, orb_param.scale_factor);
}

//-----------------------------------------------------------------------------

void helper_test::write_read_config()
{
    //Create a dummy data::camera;
    data::camera cam;

    cam.set_camera_id("Dummy Test Camera");
    cam.set_width(1920);
    cam.set_width(1080);
    cam.set_fx(400);
    cam.set_fy(401);
    cam.set_cx(200);
    cam.set_cy(201);
    cam.set_distortion_coefficient(0.0, 0.1, 0.2, 0.3, 0.4);
    cam.set_maximum_frame_rate(60.0);

    navigation::openvslam::orb_params orb_param;
    orb_param.max_num_key_pts = 8000;
    orb_param.scale_factor    = 1.2F;
    orb_param.ini_fast_thr    = 2;
    orb_param.min_fast_thr    = 1;
    orb_param.num_levels      = 8;

    navigation::openvslam::init_params init_params;
    init_params.reprojection_err_thr     = 10.F;
    init_params.scaling_factor           = 2.F;
    init_params.parallax_deg_thr         = 3.F;
    init_params.num_ba_iterations        = 101;
    init_params.num_ransac_iterations    = 9;
    init_params.min_num_triangulated_pts = 45;

    const auto config = navigation::openvslam::helper::create_monocular_config(cam, orb_param, init_params);

    core::os::temp_file tmp_file;

    CPPUNIT_ASSERT_NO_THROW(
        navigation::openvslam::helper::write_openvslam_config(
            config->yaml_node_,
            tmp_file
        )
    );

    const auto config2 = navigation::openvslam::helper::read_openvslam_config(tmp_file);
    CPPUNIT_ASSERT(config2 != nullptr);

    const auto orb = config2->orb_params_;
    auto* camera   = dynamic_cast< ::openvslam::camera::perspective*>(config2->camera_);

    CPPUNIT_ASSERT_EQUAL(orb_param.max_num_key_pts, orb.max_num_keypts_);
    CPPUNIT_ASSERT_EQUAL(orb_param.num_levels, orb.num_levels_);
    CPPUNIT_ASSERT_EQUAL(orb_param.ini_fast_thr, orb.ini_fast_thr_);
    CPPUNIT_ASSERT_EQUAL(orb_param.min_fast_thr, orb.min_fast_thr);
    CPPUNIT_ASSERT_EQUAL(orb_param.scale_factor, orb.scale_factor_);

    compare_cam(cam, *camera, false);

    // Since config doesn't export initializer parameters we need to parse the node to check if parameters are correct.

    const auto node = config2->yaml_node_;

    CPPUNIT_ASSERT_DOUBLES_EQUAL(
        static_cast<double>(init_params.reprojection_err_thr),
        node["Initializer.reprojection_error_threshold"].as<double>(),
        10e-7
    );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(
        static_cast<double>(init_params.scaling_factor),
        node["Initializer.scaling_factor"].as<double>(),
        10e-7
    );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(
        static_cast<double>(init_params.parallax_deg_thr),
        node["Initializer.parallax_deg_threshold"].as<double>(),
        10e-7
    );
    CPPUNIT_ASSERT_EQUAL(init_params.num_ba_iterations, node["Initializer.num_ba_iterations"].as<unsigned int>());
    CPPUNIT_ASSERT_EQUAL(
        init_params.num_ransac_iterations,
        node["Initializer.num_ransac_iterations"].as<unsigned int>()
    );
    CPPUNIT_ASSERT_EQUAL(
        init_params.min_num_triangulated_pts,
        node["Initializer.num_min_triangulated_pts"].as<unsigned int>()
    );
}

//-----------------------------------------------------------------------------

void helper_test::compare_cam(
    const data::camera& _sight_cam,
    const ::openvslam::camera::perspective& _ovs_cam,
    bool _sight_expected
)
{
    if(_sight_expected)
    {
        CPPUNIT_ASSERT_EQUAL(_sight_cam.get_camera_id(), _ovs_cam.name_);
        CPPUNIT_ASSERT_EQUAL(_sight_cam.get_width(), static_cast<std::size_t>(_ovs_cam.cols_));
        CPPUNIT_ASSERT_EQUAL(_sight_cam.get_height(), static_cast<std::size_t>(_ovs_cam.rows_));
        CPPUNIT_ASSERT_EQUAL(_sight_cam.get_fx(), _ovs_cam.fx_);
        CPPUNIT_ASSERT_EQUAL(_sight_cam.get_fy(), _ovs_cam.fy_);
        CPPUNIT_ASSERT_EQUAL(_sight_cam.get_cx(), _ovs_cam.cx_);
        CPPUNIT_ASSERT_EQUAL(_sight_cam.get_cy(), _ovs_cam.cy_);

        const auto dist = _sight_cam.get_distortion_coefficient();

        CPPUNIT_ASSERT_DOUBLES_EQUAL(dist[0], _ovs_cam.k1_, 10e-8);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(dist[1], _ovs_cam.k2_, 10e-8);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(dist[2], _ovs_cam.p1_, 10e-8);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(dist[3], _ovs_cam.p2_, 10e-8);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(dist[4], _ovs_cam.k3_, 10e-8);
    }
    else
    {
        CPPUNIT_ASSERT_EQUAL(_ovs_cam.name_, _sight_cam.get_camera_id());
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(_ovs_cam.cols_), _sight_cam.get_width());
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(_ovs_cam.rows_), _sight_cam.get_height());
        CPPUNIT_ASSERT_EQUAL(_ovs_cam.fx_, _sight_cam.get_fx());
        CPPUNIT_ASSERT_EQUAL(_ovs_cam.fy_, _sight_cam.get_fy());
        CPPUNIT_ASSERT_EQUAL(_ovs_cam.cx_, _sight_cam.get_cx());
        CPPUNIT_ASSERT_EQUAL(_ovs_cam.cy_, _sight_cam.get_cy());

        const auto dist = _sight_cam.get_distortion_coefficient();

        CPPUNIT_ASSERT_DOUBLES_EQUAL(_ovs_cam.k1_, dist[0], 10e-8);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(_ovs_cam.k2_, dist[1], 10e-8);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(_ovs_cam.p1_, dist[2], 10e-8);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(_ovs_cam.p2_, dist[3], 10e-8);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(_ovs_cam.k3_, dist[4], 10e-8);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::navigation::openvslam::ut
