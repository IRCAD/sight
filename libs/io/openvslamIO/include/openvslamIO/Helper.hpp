/************************************************************************
 *
 * Copyright (C) 2019-2021 IRCAD France
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

#include "openvslamIO/config.hpp"
#include "openvslamIO/OpenvslamConfig.hpp"

#include <data/Camera.hpp>

#include <openvslam/camera/perspective.h>
#include <openvslam/config.h>

namespace openvslamIO
{
/**
 * @brief The Helper class
 */
class OPENVSLAMIO_CLASS_API Helper
{
public:
    /**
     * @brief converts sight camera data to ::openvslam::camera::perspective.
     * @param _sightCam pointer to a sight camera.
     * @return ::openvslam::camera::perspective* pointer to a openvslam perspective camera.
     */
    static ::openvslam::camera::perspective fromSight(const data::Camera::csptr _sightCam);

    /**
     * @brief converts openvslam  perpective camera to sight camera
     * @param _oVSlamCam is a pointer to an openvlsam perspective camera.
     * @return an data::Camera::sptr.
     */
    static data::Camera::sptr toSight(const ::openvslam::camera::perspective _oVSlamCam);

    /**
     * @brief create an openvslam monocular config file from camera, orb parameters and initializer parameters (both
     * optionnals).
     * @param _sightCam sight camera
     * @param _orbParams orb parameters (optionnal).
     * @param _orbParams initialize parameters (optionnal).
     * @return shared_ptr of ::openvslam::config, to initialize openvslam system.
     */
    static std::shared_ptr< ::openvslam::config > createMonocularConfig(const data::Camera::csptr _sightCam,
                                                                        const ::openvslamIO::OrbParams& _orbParams
                                                                            = ::openvslamIO::OrbParams(),
                                                                        const ::openvslamIO::InitParams& _initParams
                                                                            = ::openvslamIO::InitParams());

    static void writeOpenvslamConfig(const std::shared_ptr< ::openvslam::config > config, const std::string& _filepath);

    /**
     * @brief write
     * @param _node
     * @param _filepath
     */
    static void writeOpenvslamConfig(const ::YAML::Node& _node, const std::string& _filepath);

    /**
     * @brief readOpenvslamConfig
     * @param _filepath
     * @return
     */
    static std::shared_ptr< ::openvslam::config > readOpenvslamConfig(const std::string& _filepath);
};

} // namespace openvslamIO
