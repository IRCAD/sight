/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
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

#include <arData/Camera.hpp>

#include <openvslam/camera/perspective.h>
#include <openvslam/config.h>
#include <openvslam/feature/orb_params.h>

namespace openvslamIO
{
/**
 * @brief The Helper class
 */
class OPENVSLAMIO_CLASS_API Helper
{
public:
    /**
     * @brief fromSight
     * @param _sightCam
     * @return
     */
    static ::openvslam::camera::perspective* fromSight(const ::arData::Camera::csptr _sightCam);

    /**
     * @brief toSight
     * @param _oVSlamCam
     * @return
     */
    static ::arData::Camera::sptr toSight(const ::openvslam::camera::perspective* _oVSlamCam);

    /**
     * @brief createConfig
     * @param _sightCam
     * @param _orbParams
     * @return
     */
    static std::shared_ptr< ::openvslam::config > createConfig(const ::arData::Camera::csptr _sightCam,
                                                               const ::openvslam::feature::orb_params& _orbParams);
    /**
     * @brief createConfig
     * @param _oVSlamCam
     * @param _orbParams
     * @return
     */
    static std::shared_ptr< ::openvslam::config > createConfig(::openvslam::camera::base* _oVSlamCam,
                                                               const ::openvslam::feature::orb_params& _orbParams);

};

} // namespace openvslamIO
