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

#include "HelperTest.hpp"

#include <openvslamIO/Helper.hpp>

#include <arData/Camera.hpp>

#include <fwTools/System.hpp>

#include <openvslam/camera/perspective.h>
#include <openvslam/system.h>

#include <spdlog/spdlog.h>

CPPUNIT_TEST_SUITE_REGISTRATION( ::openvslamIO::ut::HelperTest );

namespace openvslamIO
{
namespace ut
{

//-----------------------------------------------------------------------------

void HelperTest::setUp()
{

}

//-----------------------------------------------------------------------------

void HelperTest::tearDown()
{

}

//-----------------------------------------------------------------------------

void HelperTest::toSight()
{
    ::openvslam::camera::perspective* oVSlamCam =
        new ::openvslam::camera::perspective("Dummy SLAM camera", ::openvslam::camera::setup_type_t::Monocular,
                                             ::openvslam::camera::color_order_t::RGB,
                                             1920, 1080, 60,
                                             400, 401, 200, 201, 0.0, 0.1, 0.2, 0.3, 0.4);

    const ::arData::Camera::sptr cam = ::openvslamIO::Helper::toSight(oVSlamCam);

    CPPUNIT_ASSERT_EQUAL(cam->getCameraID(), oVSlamCam->name_);
    CPPUNIT_ASSERT_EQUAL(cam->getWidth(), static_cast<size_t>(oVSlamCam->cols_));
    CPPUNIT_ASSERT_EQUAL(cam->getHeight(), static_cast<size_t>(oVSlamCam->rows_));
    CPPUNIT_ASSERT_EQUAL(cam->getFx(), oVSlamCam->fx_);
    CPPUNIT_ASSERT_EQUAL(cam->getFy(), oVSlamCam->fy_);
    CPPUNIT_ASSERT_EQUAL(cam->getCx(), oVSlamCam->cx_);
    CPPUNIT_ASSERT_EQUAL(cam->getCy(), oVSlamCam->cy_);

    const auto dist = cam->getDistortionCoefficient();

    CPPUNIT_ASSERT_DOUBLES_EQUAL(dist[0], oVSlamCam->k1_, 10e-8);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(dist[1], oVSlamCam->k2_, 10e-8);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(dist[2], oVSlamCam->p1_, 10e-8);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(dist[3], oVSlamCam->p2_, 10e-8);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(dist[4], oVSlamCam->k3_, 10e-8);

    delete oVSlamCam;
}

//-----------------------------------------------------------------------------

void HelperTest::fromSight()
{
    //Create a dummy ::arData::Camera;
    ::arData::Camera::sptr cam = ::arData::Camera::New();

    cam->setCameraID("Dummy Test Camera");
    cam->setWidth(1920);
    cam->setWidth(1080);
    cam->setFx(400);
    cam->setFy(401);
    cam->setCx(200);
    cam->setCy(201);
    cam->setDistortionCoefficient(0.0, 0.1, 0.2, 0.3, 0.4);
    cam->setMaximumFrameRate(60.0);

    const auto oVSlamCam = ::openvslamIO::Helper::fromSight(cam);

    // Compare values.

    CPPUNIT_ASSERT_EQUAL(oVSlamCam->name_, cam->getCameraID());
    CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(oVSlamCam->cols_), cam->getWidth());
    CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(oVSlamCam->rows_), cam->getHeight());
    CPPUNIT_ASSERT_EQUAL(oVSlamCam->fx_, cam->getFx());
    CPPUNIT_ASSERT_EQUAL(oVSlamCam->fy_, cam->getFy());
    CPPUNIT_ASSERT_EQUAL(oVSlamCam->cx_, cam->getCx());
    CPPUNIT_ASSERT_EQUAL(oVSlamCam->cy_, cam->getCy());

    const auto dist = cam->getDistortionCoefficient();

    CPPUNIT_ASSERT_DOUBLES_EQUAL(oVSlamCam->k1_, dist[0], 10e-8);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(oVSlamCam->k2_, dist[1], 10e-8);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(oVSlamCam->p1_, dist[2], 10e-8);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(oVSlamCam->p2_, dist[3], 10e-8);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(oVSlamCam->k3_, dist[4],  10e-8);

}

//-----------------------------------------------------------------------------

void HelperTest::createConfig()
{
    //Create a dummy ::arData::Camera;
    ::arData::Camera::sptr cam = ::arData::Camera::New();

    cam->setCameraID("Dummy Test Camera");
    cam->setWidth(1920);
    cam->setWidth(1080);
    cam->setFx(400);
    cam->setFy(401);
    cam->setCx(200);
    cam->setCy(201);
    cam->setDistortionCoefficient(0.0, 0.1, 0.2, 0.3, 0.4);
    cam->setMaximumFrameRate(60.0);

    ::openvslamIO::OrbParams orbParam;
    orbParam.maxNumKeyPts = 8000;
    orbParam.scaleFactor  = 1.2f;
    orbParam.iniFastThr   = 2;
    orbParam.minFastThr   = 1;
    orbParam.numLevels    = 8;

    const auto config = ::openvslamIO::Helper::createMonocularConfig(cam, orbParam);

    // We know that the camera is perspective.
    ::openvslam::camera::perspective* camera = dynamic_cast< ::openvslam::camera::perspective* >(config->camera_);

    ::openvslam::feature::orb_params orbParamFromConfig = config->orb_params_;

    CPPUNIT_ASSERT_EQUAL(cam->getCameraID(), camera->name_);
    CPPUNIT_ASSERT_EQUAL(cam->getWidth(), static_cast<size_t>(camera->cols_));
    CPPUNIT_ASSERT_EQUAL(cam->getHeight(), static_cast<size_t>(camera->rows_));
    CPPUNIT_ASSERT_EQUAL(cam->getFx(), camera->fx_);
    CPPUNIT_ASSERT_EQUAL(cam->getFy(), camera->fy_);
    CPPUNIT_ASSERT_EQUAL(cam->getCx(), camera->cx_);
    CPPUNIT_ASSERT_EQUAL(cam->getCy(), camera->cy_);

    const auto dist = cam->getDistortionCoefficient();

    CPPUNIT_ASSERT_DOUBLES_EQUAL(dist[0], camera->k1_, 10e-8);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(dist[1], camera->k2_, 10e-8);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(dist[2], camera->p1_, 10e-8);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(dist[3], camera->p2_, 10e-8);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(dist[4], camera->k3_, 10e-8);

    CPPUNIT_ASSERT_EQUAL(orbParamFromConfig.max_num_keypts_, orbParam.maxNumKeyPts);
    CPPUNIT_ASSERT_EQUAL(orbParamFromConfig.num_levels_, orbParam.numLevels);
    CPPUNIT_ASSERT_EQUAL(orbParamFromConfig.ini_fast_thr_, orbParam.iniFastThr);
    CPPUNIT_ASSERT_EQUAL(orbParamFromConfig.min_fast_thr, orbParam.minFastThr);
    CPPUNIT_ASSERT_EQUAL(orbParamFromConfig.scale_factor_, orbParam.scaleFactor);

}

//-----------------------------------------------------------------------------

void HelperTest::writeReadConfig()
{

    //Create a dummy ::arData::Camera;
    ::arData::Camera::sptr cam = ::arData::Camera::New();

    cam->setCameraID("Dummy Test Camera");
    cam->setWidth(1920);
    cam->setWidth(1080);
    cam->setFx(400);
    cam->setFy(401);
    cam->setCx(200);
    cam->setCy(201);
    cam->setDistortionCoefficient(0.0, 0.1, 0.2, 0.3, 0.4);
    cam->setMaximumFrameRate(60.0);

    ::openvslamIO::OrbParams orbParam;
    orbParam.maxNumKeyPts = 8000;
    orbParam.scaleFactor  = 1.2f;
    orbParam.iniFastThr   = 2;
    orbParam.minFastThr   = 1;
    orbParam.numLevels    = 8;

    ::openvslamIO::InitParams initParams;
    initParams.reprojErrThr          = 10.f;
    initParams.scalingFactor         = 2.f;
    initParams.parallaxDegThr        = 3.f;
    initParams.numBAIterations       = 101;
    initParams.numRansacIterations   = 9;
    initParams.minNumTriangulatedPts = 45;

    const auto config = ::openvslamIO::Helper::createMonocularConfig(cam, orbParam, initParams);

    const ::boost::filesystem::path tmp = ::fwTools::System::getTemporaryFolder();

    CPPUNIT_ASSERT_NO_THROW(::openvslamIO::Helper::writeOpenvslamConfig(config->yaml_node_,
                                                                        tmp.string() + "/test.yaml"));

    const auto config2 = ::openvslamIO::Helper::readOpenvslamConfig(tmp.string() + "/test.yaml");
    CPPUNIT_ASSERT(config2 != nullptr);

    const auto orb = config2->orb_params_;
    ::openvslam::camera::perspective* camera = dynamic_cast< ::openvslam::camera::perspective* >(config2->camera_);

    CPPUNIT_ASSERT_EQUAL(orbParam.maxNumKeyPts, orb.max_num_keypts_);
    CPPUNIT_ASSERT_EQUAL(orbParam.numLevels, orb.num_levels_ );
    CPPUNIT_ASSERT_EQUAL(orbParam.iniFastThr, orb.ini_fast_thr_ );
    CPPUNIT_ASSERT_EQUAL(orbParam.minFastThr, orb.min_fast_thr );
    CPPUNIT_ASSERT_EQUAL(orbParam.scaleFactor, orb.scale_factor_ );

    CPPUNIT_ASSERT_EQUAL(camera->name_, cam->getCameraID());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(camera->cols_), cam->getWidth());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(camera->rows_), cam->getHeight());
    CPPUNIT_ASSERT_EQUAL(camera->fx_, cam->getFx());
    CPPUNIT_ASSERT_EQUAL(camera->fy_, cam->getFy());
    CPPUNIT_ASSERT_EQUAL(camera->cx_, cam->getCx());
    CPPUNIT_ASSERT_EQUAL(camera->cy_, cam->getCy());

    const auto dist = cam->getDistortionCoefficient();

    CPPUNIT_ASSERT_DOUBLES_EQUAL(camera->k1_, dist[0], 10e-8);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(camera->k2_, dist[1], 10e-8);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(camera->p1_, dist[2], 10e-8);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(camera->p2_, dist[3], 10e-8);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(camera->k3_, dist[4], 10e-8);

    // Since config doesn't export initializer parameters we need to parse the node to check if parameters are correct.

    const auto node = config2->yaml_node_;

    CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<double>(initParams.reprojErrThr),
                                 node["Initializer.reprojection_error_threshold"].as<double>(), 10e-7);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<double>(initParams.scalingFactor),
                                 node["Initializer.scaling_factor"].as<double>(),  10e-7);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<double>(initParams.parallaxDegThr),
                                 node["Initializer.parallax_deg_threshold"].as<double>(), 10e-7);
    CPPUNIT_ASSERT_EQUAL(initParams.numBAIterations, node["Initializer.num_ba_iterations"].as<unsigned int>());
    CPPUNIT_ASSERT_EQUAL(initParams.numRansacIterations, node["Initializer.num_ransac_iterations"].as<unsigned int>());
    CPPUNIT_ASSERT_EQUAL(initParams.minNumTriangulatedPts,
                         node["Initializer.num_min_triangulated_pts"].as<unsigned int>());

}

//-----------------------------------------------------------------------------
}
}
