/************************************************************************
 *
 * Copyright (C) 2019-2022 IRCAD France
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

#include <core/tools/System.hpp>

#include <data/Camera.hpp>

#include <navigation/openvslam/Helper.hpp>

#include <openvslam/camera/perspective.h>
#include <openvslam/system.h>

#include <spdlog/spdlog.h>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::navigation::openvslam::ut::HelperTest);

namespace sight::navigation::openvslam::ut
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
    const ::openvslam::camera::perspective oVSlamCam =
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

    const auto cam = navigation::openvslam::Helper::toSight(oVSlamCam);

    compareCam(*cam, oVSlamCam, true);
}

//-----------------------------------------------------------------------------

void HelperTest::fromSight()
{
    //Create a dummy data::Camera;
    data::Camera cam;

    cam.setCameraID("Dummy Test Camera");
    cam.setWidth(1920);
    cam.setWidth(1080);
    cam.setFx(400);
    cam.setFy(401);
    cam.setCx(200);
    cam.setCy(201);
    cam.setDistortionCoefficient(0.0, 0.1, 0.2, 0.3, 0.4);
    cam.setMaximumFrameRate(60.0);

    const auto oVSlamCam = navigation::openvslam::Helper::fromSight(cam);

    // Compare values.
    compareCam(cam, oVSlamCam, false);
}

//-----------------------------------------------------------------------------

void HelperTest::createConfig()
{
    //Create a dummy data::Camera;
    data::Camera cam;

    cam.setCameraID("Dummy Test Camera");
    cam.setWidth(1920);
    cam.setWidth(1080);
    cam.setFx(400);
    cam.setFy(401);
    cam.setCx(200);
    cam.setCy(201);
    cam.setDistortionCoefficient(0.0, 0.1, 0.2, 0.3, 0.4);
    cam.setMaximumFrameRate(60.0);

    navigation::openvslam::OrbParams orbParam;
    orbParam.maxNumKeyPts = 8000;
    orbParam.scaleFactor  = 1.2F;
    orbParam.iniFastThr   = 2;
    orbParam.minFastThr   = 1;
    orbParam.numLevels    = 8;

    const auto config = navigation::openvslam::Helper::createMonocularConfig(cam, orbParam);

    // We know that the camera is perspective.
    auto* camera = dynamic_cast< ::openvslam::camera::perspective*>(config->camera_);

    ::openvslam::feature::orb_params orbParamFromConfig = config->orb_params_;

    compareCam(cam, *camera, true);

    CPPUNIT_ASSERT_EQUAL(orbParamFromConfig.max_num_keypts_, orbParam.maxNumKeyPts);
    CPPUNIT_ASSERT_EQUAL(orbParamFromConfig.num_levels_, orbParam.numLevels);
    CPPUNIT_ASSERT_EQUAL(orbParamFromConfig.ini_fast_thr_, orbParam.iniFastThr);
    CPPUNIT_ASSERT_EQUAL(orbParamFromConfig.min_fast_thr, orbParam.minFastThr);
    CPPUNIT_ASSERT_EQUAL(orbParamFromConfig.scale_factor_, orbParam.scaleFactor);
}

//-----------------------------------------------------------------------------

void HelperTest::writeReadConfig()
{
    //Create a dummy data::Camera;
    data::Camera cam;

    cam.setCameraID("Dummy Test Camera");
    cam.setWidth(1920);
    cam.setWidth(1080);
    cam.setFx(400);
    cam.setFy(401);
    cam.setCx(200);
    cam.setCy(201);
    cam.setDistortionCoefficient(0.0, 0.1, 0.2, 0.3, 0.4);
    cam.setMaximumFrameRate(60.0);

    navigation::openvslam::OrbParams orbParam;
    orbParam.maxNumKeyPts = 8000;
    orbParam.scaleFactor  = 1.2F;
    orbParam.iniFastThr   = 2;
    orbParam.minFastThr   = 1;
    orbParam.numLevels    = 8;

    navigation::openvslam::InitParams initParams;
    initParams.reprojectionErrThr    = 10.F;
    initParams.scalingFactor         = 2.F;
    initParams.parallaxDegThr        = 3.F;
    initParams.numBAIterations       = 101;
    initParams.numRansacIterations   = 9;
    initParams.minNumTriangulatedPts = 45;

    const auto config = navigation::openvslam::Helper::createMonocularConfig(cam, orbParam, initParams);

    const std::filesystem::path tmp = core::tools::System::getTemporaryFolder();

    CPPUNIT_ASSERT_NO_THROW(
        navigation::openvslam::Helper::writeOpenvslamConfig(
            config->yaml_node_,
            tmp.string()
            + "/test.yaml"
        )
    );

    const auto config2 = navigation::openvslam::Helper::readOpenvslamConfig(tmp.string() + "/test.yaml");
    CPPUNIT_ASSERT(config2 != nullptr);

    const auto orb = config2->orb_params_;
    auto* camera   = dynamic_cast< ::openvslam::camera::perspective*>(config2->camera_);

    CPPUNIT_ASSERT_EQUAL(orbParam.maxNumKeyPts, orb.max_num_keypts_);
    CPPUNIT_ASSERT_EQUAL(orbParam.numLevels, orb.num_levels_);
    CPPUNIT_ASSERT_EQUAL(orbParam.iniFastThr, orb.ini_fast_thr_);
    CPPUNIT_ASSERT_EQUAL(orbParam.minFastThr, orb.min_fast_thr);
    CPPUNIT_ASSERT_EQUAL(orbParam.scaleFactor, orb.scale_factor_);

    compareCam(cam, *camera, false);

    // Since config doesn't export initializer parameters we need to parse the node to check if parameters are correct.

    const auto node = config2->yaml_node_;

    CPPUNIT_ASSERT_DOUBLES_EQUAL(
        static_cast<double>(initParams.reprojectionErrThr),
        node["Initializer.reprojection_error_threshold"].as<double>(),
        10e-7
    );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(
        static_cast<double>(initParams.scalingFactor),
        node["Initializer.scaling_factor"].as<double>(),
        10e-7
    );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(
        static_cast<double>(initParams.parallaxDegThr),
        node["Initializer.parallax_deg_threshold"].as<double>(),
        10e-7
    );
    CPPUNIT_ASSERT_EQUAL(initParams.numBAIterations, node["Initializer.num_ba_iterations"].as<unsigned int>());
    CPPUNIT_ASSERT_EQUAL(initParams.numRansacIterations, node["Initializer.num_ransac_iterations"].as<unsigned int>());
    CPPUNIT_ASSERT_EQUAL(
        initParams.minNumTriangulatedPts,
        node["Initializer.num_min_triangulated_pts"].as<unsigned int>()
    );
}

//-----------------------------------------------------------------------------

void HelperTest::compareCam(
    const data::Camera& _sightCam,
    const ::openvslam::camera::perspective& _ovsCam,
    bool _sightExpected
)
{
    if(_sightExpected)
    {
        CPPUNIT_ASSERT_EQUAL(_sightCam.getCameraID(), _ovsCam.name_);
        CPPUNIT_ASSERT_EQUAL(_sightCam.getWidth(), static_cast<std::size_t>(_ovsCam.cols_));
        CPPUNIT_ASSERT_EQUAL(_sightCam.getHeight(), static_cast<std::size_t>(_ovsCam.rows_));
        CPPUNIT_ASSERT_EQUAL(_sightCam.getFx(), _ovsCam.fx_);
        CPPUNIT_ASSERT_EQUAL(_sightCam.getFy(), _ovsCam.fy_);
        CPPUNIT_ASSERT_EQUAL(_sightCam.getCx(), _ovsCam.cx_);
        CPPUNIT_ASSERT_EQUAL(_sightCam.getCy(), _ovsCam.cy_);

        const auto dist = _sightCam.getDistortionCoefficient();

        CPPUNIT_ASSERT_DOUBLES_EQUAL(dist[0], _ovsCam.k1_, 10e-8);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(dist[1], _ovsCam.k2_, 10e-8);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(dist[2], _ovsCam.p1_, 10e-8);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(dist[3], _ovsCam.p2_, 10e-8);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(dist[4], _ovsCam.k3_, 10e-8);
    }
    else
    {
        CPPUNIT_ASSERT_EQUAL(_ovsCam.name_, _sightCam.getCameraID());
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(_ovsCam.cols_), _sightCam.getWidth());
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(_ovsCam.rows_), _sightCam.getHeight());
        CPPUNIT_ASSERT_EQUAL(_ovsCam.fx_, _sightCam.getFx());
        CPPUNIT_ASSERT_EQUAL(_ovsCam.fy_, _sightCam.getFy());
        CPPUNIT_ASSERT_EQUAL(_ovsCam.cx_, _sightCam.getCx());
        CPPUNIT_ASSERT_EQUAL(_ovsCam.cy_, _sightCam.getCy());

        const auto dist = _sightCam.getDistortionCoefficient();

        CPPUNIT_ASSERT_DOUBLES_EQUAL(_ovsCam.k1_, dist[0], 10e-8);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(_ovsCam.k2_, dist[1], 10e-8);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(_ovsCam.p1_, dist[2], 10e-8);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(_ovsCam.p2_, dist[3], 10e-8);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(_ovsCam.k3_, dist[4], 10e-8);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::navigation::openvslam::ut
