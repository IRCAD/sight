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

#include "openvslamIO/Helper.hpp"

namespace openvslamIO
{

//-----------------------------------------------------------------------------

openvslam::camera::perspective* Helper::fromSight(const arData::Camera::csptr _sightCam)
{
    const auto name = _sightCam->getCameraID();
    const auto dist = _sightCam->getDistortionCoefficient();

    // Handle only Monocular-RGB camera for now.
    const ::openvslam::camera::setup_type_t cameraType = ::openvslam::camera::setup_type_t::Monocular;
    const ::openvslam::camera::color_order_t colorType = ::openvslam::camera::color_order_t::RGB;

    const unsigned int cols = static_cast<unsigned int>(_sightCam->getWidth());
    const unsigned int rows = static_cast<unsigned int>(_sightCam->getHeight());
    const double fps        = static_cast<double>(_sightCam->getMaximumFrameRate());

    // Create a perspective camera (equirectangular and fisheye needs additional information).
    ::openvslam::camera::perspective* oVSlamCamera =
        new ::openvslam::camera::perspective(name, cameraType, colorType, cols, rows,
                                             fps, _sightCam->getFx(), _sightCam->getFy(),
                                             _sightCam->getCx(),
                                             _sightCam->getCy(), dist[0], dist[1], dist[2], dist[3], dist[4]);

    return oVSlamCamera;
}

//-----------------------------------------------------------------------------

arData::Camera::sptr Helper::toSight(const openvslam::camera::perspective* _oVSlamCam)
{
    ::arData::Camera::sptr cam = ::arData::Camera::New();

    cam->setCameraID( _oVSlamCam->name_);
    cam->setWidth(_oVSlamCam->cols_);
    cam->setHeight(_oVSlamCam->rows_);

    cam->setFx(_oVSlamCam->fx_);
    cam->setFy(_oVSlamCam->fy_);
    cam->setCx(_oVSlamCam->cx_);
    cam->setCy(_oVSlamCam->cy_);

    std::array<double, 5> dist;

    dist[0] = _oVSlamCam->k1_; //k1
    dist[1] = _oVSlamCam->k2_; //k2
    dist[2] = _oVSlamCam->p1_; //p1
    dist[3] = _oVSlamCam->p2_; //p2
    dist[4] = _oVSlamCam->k3_; //k3

    cam->setDistortionCoefficient(dist);

    cam->setMaximumFrameRate(static_cast<float>(_oVSlamCam->fps_));

    return cam;
}

//-----------------------------------------------------------------------------

std::shared_ptr< ::openvslam::config > Helper::createConfig(const arData::Camera::csptr _sightCam,
                                                            const openvslam::feature::orb_params& _orbParams)
{
    // First convert into openvslam camera
    const auto slamCam = fromSight(_sightCam);
    // Call the createConfig function.
    const auto conf = createConfig(slamCam, _orbParams);

    return conf;

}

//-----------------------------------------------------------------------------

std::shared_ptr< ::openvslam::config > Helper::createConfig( openvslam::camera::base* _oVSlamCam,
                                                             const openvslam::feature::orb_params& _orbParams)
{
    std::shared_ptr< ::openvslam::config > conf = std::make_shared< ::openvslam::config>(_oVSlamCam, _orbParams);

    return conf;
}

//-----------------------------------------------------------------------------

} //namespace openvslamIO
