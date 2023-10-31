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

#include "helper.hpp"

#include <fstream>

/**
 * Do not mark `equirectangular` as incorrect.
 * cspell:ignore equirectangular
 */
namespace sight::navigation::openvslam
{

//-----------------------------------------------------------------------------

::openvslam::camera::perspective helper::from_sight(const data::camera& _sight_cam)
{
    const auto name = _sight_cam.get_camera_id();
    const auto dist = _sight_cam.get_distortion_coefficient();

    // Handle only Monocular-RGB camera for now.
    const ::openvslam::camera::setup_type_t camera_type = ::openvslam::camera::setup_type_t::Monocular;
    const ::openvslam::camera::color_order_t color_type = ::openvslam::camera::color_order_t::RGB;

    const auto cols = static_cast<unsigned int>(_sight_cam.get_width());
    const auto rows = static_cast<unsigned int>(_sight_cam.get_height());
    const auto fps  = static_cast<double>(_sight_cam.get_maximum_frame_rate());
    // Create a perspective camera (equirectangular and fisheye needs additional information).
    const ::openvslam::camera::perspective o_v_slam_camera =
        ::openvslam::camera::perspective(
            name,
            camera_type,
            color_type,
            cols,
            rows,
            fps,
            _sight_cam.get_fx(),
            _sight_cam.get_fy(),
            _sight_cam.get_cx(),
            _sight_cam.get_cy(),
            dist[0],
            dist[1],
            dist[2],
            dist[3],
            dist[4]
        );

    return o_v_slam_camera;
}

//-----------------------------------------------------------------------------

data::camera::sptr helper::to_sight(const ::openvslam::camera::perspective _o_v_slam_cam)
{
    data::camera::sptr cam = std::make_shared<data::camera>();

    cam->set_camera_id(_o_v_slam_cam.name_);
    cam->set_width(_o_v_slam_cam.cols_);
    cam->set_height(_o_v_slam_cam.rows_);

    cam->set_fx(_o_v_slam_cam.fx_);
    cam->set_fy(_o_v_slam_cam.fy_);
    cam->set_cx(_o_v_slam_cam.cx_);
    cam->set_cy(_o_v_slam_cam.cy_);

    std::array<double, 5> dist {};

    dist[0] = _o_v_slam_cam.k1_; //k1
    dist[1] = _o_v_slam_cam.k2_; //k2
    dist[2] = _o_v_slam_cam.p1_; //p1
    dist[3] = _o_v_slam_cam.p2_; //p2
    dist[4] = _o_v_slam_cam.k3_; //k3

    cam->set_distortion_coefficient(dist);

    cam->set_maximum_frame_rate(static_cast<float>(_o_v_slam_cam.fps_));

    return cam;
}

//-----------------------------------------------------------------------------

std::shared_ptr<::openvslam::config> helper::create_monocular_config(
    const data::camera& _sight_cam,
    const orb_params& _orb_params,
    const init_params& _init_params
)
{
    //Create a YAML node for other parameters.
    ::YAML::Node node;
    node["Camera.name"] = _sight_cam.get_camera_id();
    // Only Monocular
    node["Camera.setup"] = "monocular";
    // In sight only handles perspective modeles.
    node["Camera.model"] = "perspective";
    node["Camera.fx"]    = _sight_cam.get_fx();
    node["Camera.fy"]    = _sight_cam.get_fy();
    node["Camera.cx"]    = _sight_cam.get_cx();
    node["Camera.cy"]    = _sight_cam.get_cy();

    node["Camera.k1"] = _sight_cam.get_distortion_coefficient()[0];
    node["Camera.k2"] = _sight_cam.get_distortion_coefficient()[1];
    node["Camera.p1"] = _sight_cam.get_distortion_coefficient()[2];
    node["Camera.p2"] = _sight_cam.get_distortion_coefficient()[3];
    node["Camera.k3"] = _sight_cam.get_distortion_coefficient()[4];

    node["Camera.fps"]  = _sight_cam.get_maximum_frame_rate();
    node["Camera.cols"] = _sight_cam.get_width();
    node["Camera.rows"] = _sight_cam.get_height();
    // Values can be RGB, BGR or GRAY.
    //TODO: maybe use _sightCam.getPixelFormatName() and translate result to RGB-BGR or GRAY.
    node["Camera.color_order"] = "RGB";

    // Features (ORB):
    node["Feature.max_num_keypoints"]  = _orb_params.max_num_key_pts;
    node["Feature.scale_factor"]       = _orb_params.scale_factor;
    node["Feature.num_levels"]         = _orb_params.num_levels;
    node["Feature.ini_fast_threshold"] = _orb_params.ini_fast_thr;
    node["Feature.min_fast_threshold"] = _orb_params.min_fast_thr;

    // Initializer parameters:
    node["Initializer.num_ransac_iterations"]        = _init_params.num_ransac_iterations;
    node["Initializer.num_min_triangulated_pts"]     = _init_params.min_num_triangulated_pts;
    node["Initializer.parallax_deg_threshold"]       = _init_params.parallax_deg_thr;
    node["Initializer.reprojection_error_threshold"] = _init_params.reprojection_err_thr;
    node["Initializer.num_ba_iterations"]            = _init_params.num_ba_iterations;
    node["Initializer.scaling_factor"]               = _init_params.scaling_factor;

    // Create the config with YAML node (constructor was added on our version of openvslam).
    std::shared_ptr< ::openvslam::config> conf = std::make_shared< ::openvslam::config>(node);
    return conf;
}

//-----------------------------------------------------------------------------

void helper::write_openvslam_config(const std::shared_ptr< ::openvslam::config> _config, const std::string& _filepath)
{
    write_openvslam_config(_config->yaml_node_, _filepath);
}

//-----------------------------------------------------------------------------

void helper::write_openvslam_config(const YAML::Node& _node, const std::string& _filepath)
{
    std::ofstream f_out(_filepath);
    f_out << _node;
}

//-----------------------------------------------------------------------------

std::shared_ptr< ::openvslam::config> helper::read_openvslam_config(const std::string& _filepath)
{
    std::shared_ptr< ::openvslam::config> conf;
    try
    {
        conf = std::make_shared< ::openvslam::config>(_filepath);
    }
    catch(std::exception& e)
    {
        SIGHT_ERROR("Something went wrong when tying to load '" + _filepath + "'. Error: " + e.what());
        return nullptr;
    }

    return conf;
}

//-----------------------------------------------------------------------------

} //namespace sight::navigation::openvslam
