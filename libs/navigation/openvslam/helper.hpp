/************************************************************************
 *
 * Copyright (C) 2019-2024 IRCAD France
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

#pragma once

#include <sight/navigation/openvslam/config.hpp>

#include "navigation/openvslam/openvslam_config.hpp"

#include <data/camera.hpp>

#include <openvslam/camera/perspective.h>
#include <openvslam/config.h>

namespace sight::navigation::openvslam
{

/**
 * @brief The Helper class
 */
class SIGHT_NAVIGATION_OPENVSLAM_CLASS_API helper
{
public:

    /**
     * @brief converts sight camera data to ::openvslam::camera::perspective.
     * @param _sightCam: sight camera reference.
     * @return ::openvslam::camera::perspective* pointer to a openvslam perspective camera.
     */
    static ::openvslam::camera::perspective from_sight(const data::camera& _sight_cam);

    /**
     * @brief converts openvslam  perspective camera to sight camera
     * @param _oVSlamCam is a pointer to an openvlsam perspective camera.
     * @return an data::camera::sptr.
     */
    static data::camera::sptr to_sight(const ::openvslam::camera::perspective _o_v_slam_cam);

    /**
     * @brief create an openvslam monocular config file from camera, orb parameters and initializer parameters (both
     * optional).
     * @param _sightCam sight camera
     * @param _orbParams orb parameters (optional).
     * @param _orbParams initialize parameters (optional).
     * @return shared_ptr of ::openvslam::config, to initialize openvslam system.
     */
    static std::shared_ptr< ::openvslam::config> create_monocular_config(
        const data::camera& _sight_cam,
        const navigation::openvslam::orb_params& _orb_params
        = navigation::openvslam::orb_params(),
        const navigation::openvslam::init_params& _init_params
        = navigation::openvslam::init_params()
    );

    static void write_openvslam_config(
        const std::shared_ptr<::openvslam::config> _config,
        const std::string& _filepath
    );

    /**
     * @brief write
     * @param _node
     * @param _filepath
     */
    static void write_openvslam_config(const ::YAML::Node& _node, const std::string& _filepath);

    /**
     * @brief readOpenvslamConfig
     * @param _filepath
     * @return
     */
    static std::shared_ptr<::openvslam::config> read_openvslam_config(const std::string& _filepath);
};

} // namespace sight::navigation::openvslam
