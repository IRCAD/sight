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

#include "Helper.hpp"

#include <fstream>

namespace sight::navigation::openvslam
{

//-----------------------------------------------------------------------------

::openvslam::camera::perspective Helper::fromSight(const data::Camera::csptr _sightCam)
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
    const ::openvslam::camera::perspective oVSlamCamera =
        ::openvslam::camera::perspective(name, cameraType, colorType, cols, rows,
                                         fps, _sightCam->getFx(), _sightCam->getFy(),
                                         _sightCam->getCx(),
                                         _sightCam->getCy(), dist[0], dist[1], dist[2], dist[3], dist[4]);

    return oVSlamCamera;
}

//-----------------------------------------------------------------------------

data::Camera::sptr Helper::toSight(const ::openvslam::camera::perspective _oVSlamCam)
{
    data::Camera::sptr cam = data::Camera::New();

    cam->setCameraID( _oVSlamCam.name_);
    cam->setWidth(_oVSlamCam.cols_);
    cam->setHeight(_oVSlamCam.rows_);

    cam->setFx(_oVSlamCam.fx_);
    cam->setFy(_oVSlamCam.fy_);
    cam->setCx(_oVSlamCam.cx_);
    cam->setCy(_oVSlamCam.cy_);

    std::array<double, 5> dist;

    dist[0] = _oVSlamCam.k1_; //k1
    dist[1] = _oVSlamCam.k2_; //k2
    dist[2] = _oVSlamCam.p1_; //p1
    dist[3] = _oVSlamCam.p2_; //p2
    dist[4] = _oVSlamCam.k3_; //k3

    cam->setDistortionCoefficient(dist);

    cam->setMaximumFrameRate(static_cast<float>(_oVSlamCam.fps_));

    return cam;
}

//-----------------------------------------------------------------------------

std::shared_ptr<::openvslam::config> Helper::createMonocularConfig(const data::Camera::csptr _sightCam,
                                                                   const OrbParams& _orbParams,
                                                                   const InitParams& _initParams)
{
    //Create a YAML node for other parameters.
    ::YAML::Node node;
    node["Camera.name"] = _sightCam->getCameraID();
    // Only Monocular
    node["Camera.setup"] = "monocular";
    // In sight only handles perspective modeles.
    node["Camera.model"] = "perspective";
    node["Camera.fx"]    = _sightCam->getFx();
    node["Camera.fy"]    = _sightCam->getFy();
    node["Camera.cx"]    = _sightCam->getCx();
    node["Camera.cy"]    = _sightCam->getCy();

    node["Camera.k1"] = _sightCam->getDistortionCoefficient()[0];
    node["Camera.k2"] = _sightCam->getDistortionCoefficient()[1];
    node["Camera.p1"] = _sightCam->getDistortionCoefficient()[2];
    node["Camera.p2"] = _sightCam->getDistortionCoefficient()[3];
    node["Camera.k3"] = _sightCam->getDistortionCoefficient()[4];

    node["Camera.fps"]  = _sightCam->getMaximumFrameRate();
    node["Camera.cols"] = _sightCam->getWidth();
    node["Camera.rows"] = _sightCam->getHeight();
    // Values can be RGB, BGR or GRAY.
    //TODO: maybe use _sightCam->getPixelFormatName() and translate result to RGB-BGR or GRAY.
    node["Camera.color_order"] = "RGB";

    // Features (ORB):
    node["Feature.max_num_keypoints"]  = _orbParams.maxNumKeyPts;
    node["Feature.scale_factor"]       = _orbParams.scaleFactor;
    node["Feature.num_levels"]         = _orbParams.numLevels;
    node["Feature.ini_fast_threshold"] = _orbParams.iniFastThr;
    node["Feature.min_fast_threshold"] = _orbParams.minFastThr;

    // Initializer parameters:
    node["Initializer.num_ransac_iterations"]        = _initParams.numRansacIterations;
    node["Initializer.num_min_triangulated_pts"]     = _initParams.minNumTriangulatedPts;
    node["Initializer.parallax_deg_threshold"]       = _initParams.parallaxDegThr;
    node["Initializer.reprojection_error_threshold"] = _initParams.reprojErrThr;
    node["Initializer.num_ba_iterations"]            = _initParams.numBAIterations;
    node["Initializer.scaling_factor"]               = _initParams.scalingFactor;

    // Create the config with YAML node (constructor was added on our version of openvslam).
    std::shared_ptr< ::openvslam::config > conf = std::make_shared< ::openvslam::config >(node);
    return conf;
}

//-----------------------------------------------------------------------------

void Helper::writeOpenvslamConfig(const std::shared_ptr< ::openvslam::config > config, const std::string& _filepath)
{
    writeOpenvslamConfig(config->yaml_node_, _filepath);
}

//-----------------------------------------------------------------------------

void Helper::writeOpenvslamConfig(const YAML::Node& _node, const std::string& _filepath)
{
    std::ofstream fout(_filepath);
    fout << _node;
}

//-----------------------------------------------------------------------------

std::shared_ptr< ::openvslam::config > Helper::readOpenvslamConfig(const std::string& _filepath)
{
    std::shared_ptr< ::openvslam::config > conf;
    try
    {
        conf = std::make_shared< ::openvslam::config>(_filepath);

    }
    catch (std::exception& e)
    {
        SLM_ERROR("Something went wrong when tying to load '" + _filepath + "'. Error: " + e.what());
        return nullptr;
    }

    return conf;

}

//-----------------------------------------------------------------------------

} //namespace sight::navigation::openvslam
