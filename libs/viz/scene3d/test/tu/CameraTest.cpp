/************************************************************************
 *
 * Copyright (C) 2018-2022 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "CameraTest.hpp"

#include "data/Camera.hpp"

#include "viz/scene3d/Utils.hpp"

#include <utest/Filter.hpp>

#include <viz/scene3d/helper/Camera.hpp>

#include <OgreLogManager.h>
#include <OgreMatrix4.h>
#include <OgreRenderWindow.h>
#include <OgreViewport.h>

#include <limits>
#include <tuple>
#include <vector>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::viz::scene3d::ut::CameraTest);

namespace sight::viz::scene3d
{

namespace ut
{

//------------------------------------------------------------------------------

void CameraTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void CameraTest::tearDown()
{
}

//------------------------------------------------------------------------------

void compareMatrix(const Ogre::Matrix4& _m1, const Ogre::Matrix4& _m2)
{
    for(unsigned int i = 0 ; i < 4 ; ++i)
    {
        for(unsigned int j = 0 ; j < 4 ; ++j)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(_m1[i][j], _m2[i][j], 0.00000000000001);
        }
    }
}

//------------------------------------------------------------------------------

void comparePoint(const Ogre::Vector4& _p1, const Ogre::Vector3& _p2)
{
    CPPUNIT_ASSERT_DOUBLES_EQUAL(_p1[0], _p2[0], 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(_p1[1], _p2[1], 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(_p1[2], _p2[2], 0.0001);
}

//------------------------------------------------------------------------------

void CameraTest::computeProjectionMatrix()
{
    // Sorry for this incomprehensible test but this is better than 16 loops
    const double cx          = 988.898;
    const double cy          = 553.769;
    const double fx          = 1590.59;
    const double fy          = 1628.71;
    const std::size_t width  = 1920;
    const std::size_t height = 1080;
    const float n            = 0.1f;
    const float f            = 100;

    // Original matrix
    Ogre::Matrix4 expected(1.6568645238876f, 0.f, -0.0295966863632202f, 0.f,
                           0.f, 3.01612949371338f, 0.0245949625968933f, 0.f,
                           0.f, 0.f, -1.00200200080872f, -0.200200200080872f,
                           0.f, 0.f, -1.f, 0.f);

    // Original camera
    data::Camera::sptr camera = data::Camera::New();
    camera->setCx(cx);
    camera->setCy(cy);
    camera->setFx(fx);
    camera->setFy(fy);
    camera->setWidth(width);
    camera->setHeight(height);

    {
        // Function of the camera to set, followed by the new value in the expected matrix
        typedef std::tuple<std::function<void (const data::Camera::sptr, double)>, double, unsigned int,
                           unsigned int, float> tupleType;
        const std::vector<tupleType> permutation {
            tupleType(&sight::data::Camera::setCx, cx, 0, 2, -0.0295966863632202f),
            tupleType(&sight::data::Camera::setCy, cy, 1, 2, 0.0245949625968933f),
            tupleType(&sight::data::Camera::setFx, fx, 0, 0, 1.65686452388763f),
            tupleType(&sight::data::Camera::setFy, fy, 1, 1, 3.01612949371338f),

            tupleType(&sight::data::Camera::setCx, 0., 0, 2, 1.00104212760925f),
            tupleType(&sight::data::Camera::setCy, 0., 1, 2, -1.00185346603394f),
            tupleType(&sight::data::Camera::setFx, 0., 0, 0, 0.f),
            tupleType(&sight::data::Camera::setFy, 0., 1, 1, 0.f),

            tupleType(&sight::data::Camera::setCx, std::numeric_limits<float>::min(), 0, 2, 1.00104212760925f),
            tupleType(&sight::data::Camera::setCy, std::numeric_limits<float>::min(), 1, 2, -1.00185346603394f),
            tupleType(&sight::data::Camera::setFx, std::numeric_limits<float>::min(), 0, 0, 1.22445459812703e-41f),
            tupleType(&sight::data::Camera::setFy, std::numeric_limits<float>::min(), 1, 1, 2.17677703448217e-41f),

            tupleType(&sight::data::Camera::setCx, std::numeric_limits<float>::max(), 0, 2, -3.5464549820937e+35f),
            tupleType(&sight::data::Camera::setCy, std::numeric_limits<float>::max(), 1, 2, 6.30736510970334e+35f),
            tupleType(
                &sight::data::Camera::setFx,
                std::numeric_limits<float>::max(),
                0,
                0,
                std::numeric_limits<float>::infinity()
            ),
            tupleType(
                &sight::data::Camera::setFy,
                std::numeric_limits<float>::max(),
                1,
                1,
                std::numeric_limits<float>::infinity()
            ),
        };

        // Apply permutations and check the expected result
        for(auto tu : permutation)
        {
            auto fun = std::get<0>(tu);

            // Apply function on the camera
            fun(camera, std::get<1>(tu));

            // Change the expected result
            expected[std::get<2>(tu)][std::get<3>(tu)] = std::get<4>(tu);

            Ogre::Matrix4 actual =
                viz::scene3d::helper::Camera::computeProjectionMatrix(
                    *camera,
                    static_cast<float>(camera->getWidth()),
                    static_cast<float>(camera->getHeight()),
                    n,
                    f
                );
            compareMatrix(expected, actual);
        }
    }

    {
        // Function of the camera to set, followed by the new value in the expected matrix
        typedef std::tuple<std::function<void (const data::Camera::sptr, std::size_t)>, std::size_t, unsigned int,
                           unsigned int, float> tupleType;
        const std::vector<tupleType> permutation {
            tupleType(&sight::data::Camera::setWidth, width, 0, 2, -3.5464549820937e+35f),
            tupleType(&sight::data::Camera::setHeight, height, 1, 2, 6.30736510970334e+35f),

            tupleType(&sight::data::Camera::setWidth, 1, 0, 2, -std::numeric_limits<float>::infinity()),
            tupleType(&sight::data::Camera::setHeight, 1, 1, 2, std::numeric_limits<float>::infinity()),

            tupleType(
                &sight::data::Camera::setWidth,
                std::numeric_limits<std::size_t>::max(),
                0,
                2,
                -3.68934859483958e+19f
            ),
            tupleType(
                &sight::data::Camera::setHeight,
                std::numeric_limits<std::size_t>::max(),
                1,
                2,
                3.68934859483958e+19f
            ),
        };

        // Apply permutations and check the expected result
        for(auto tu : permutation)
        {
            auto fun = std::get<0>(tu);

            // Apply function on the camera
            fun(camera, std::get<1>(tu));

            // Change the expected result
            expected[std::get<2>(tu)][std::get<3>(tu)] = std::get<4>(tu);

            Ogre::Matrix4 actual =
                viz::scene3d::helper::Camera::computeProjectionMatrix(
                    *camera,
                    static_cast<float>(camera->getWidth()),
                    static_cast<float>(camera->getHeight()),
                    n,
                    f
                );
            compareMatrix(expected, actual);
        }
    }
}

//------------------------------------------------------------------------------

void CameraTest::convertPixelToWorldSpace()
{
    // On some platform / environment like Ubuntu 21.04 in a dockerized environment,
    // this test will fail because opengl context cannot be acquired correctly
    if(utest::Filter::ignoreUnstableTests())
    {
        return;
    }

    auto* const root         = viz::scene3d::Utils::getOgreRoot();
    auto* const sceneManager = root->createSceneManager("DefaultSceneManager", "TestSceneManager");

    // Use a size > 120 because windows will anyway switch to a larger size
    const unsigned int width  = 200;
    const unsigned int height = 200;
    auto* const renderWindow  = root->createRenderWindow(
        "TestRenderWindow",
        width,
        height,
        false,
        nullptr
    );
    renderWindow->setVisible(false);
    renderWindow->setAutoUpdated(false);
    auto* const camera = sceneManager->createCamera("TestCamera");
    renderWindow->addViewport(camera);

    camera->setNearClipDistance(1);
    camera->setFarClipDistance(10);
    camera->setAutoAspectRatio(true);
    camera->setOrthoWindowWidth(1920);
    camera->setOrthoWindowHeight(1080);

    camera->setProjectionType(Ogre::ProjectionType::PT_PERSPECTIVE);
    {
        // Manually project a point
        const Ogre::Vector4 standardPoint(1.f, 2.f, 3.f, 1.f);
        const Ogre::Vector4 clippedPoint = camera->getProjectionMatrix() * camera->getViewMatrix() * standardPoint;
        const Ogre::Vector3 ndcPoint     = clippedPoint.xyz() / clippedPoint.w;

        // /!\ in openGl, y coordinate begin from the upper left corner, we need to set him from the lower left corner.
        const Ogre::Real fX = (ndcPoint.x + 1.f) * 0.5f;
        const Ogre::Real fY = 1.f - (ndcPoint.y + 1.f) * 0.5f;
        const Ogre::Real fZ = (ndcPoint.z + 1.f) * 0.5f;
        const Ogre::Vector3 viewportPoint(fX, fY, fZ);

        // Unproject the projected point
        const Ogre::Vector3 point            = viewportPoint * Ogre::Vector3(width, height, 1);
        const Ogre::Vector3 unprojectedPoint =
            viz::scene3d::helper::Camera::convertScreenSpaceToViewSpace(*camera, point);

        comparePoint(standardPoint, unprojectedPoint);
    }

    camera->setProjectionType(Ogre::ProjectionType::PT_ORTHOGRAPHIC);
    {
        // Manually project a point
        const Ogre::Vector4 standardPoint(1.f, 2.f, 3.f, 1.f);
        const Ogre::Vector4 clippedPoint = camera->getProjectionMatrix() * camera->getViewMatrix() * standardPoint;
        const Ogre::Vector3 ndcPoint     = clippedPoint.xyz() / clippedPoint.w;

        // /!\ in openGl, y coordinate begin from the upper left corner, we need to set him from the lower left corner.
        const Ogre::Real fX = (ndcPoint.x + 1.f) * 0.5f;
        const Ogre::Real fY = 1.f - (ndcPoint.y + 1.f) * 0.5f;
        const Ogre::Real fZ = (ndcPoint.z + 1.f) * 0.5f;
        const Ogre::Vector3 viewportPoint(fX, fY, fZ);

        // Unproject the projected point
        const Ogre::Vector3 point            = viewportPoint * Ogre::Vector3(width, height, 1);
        const Ogre::Vector3 unprojectedPoint =
            viz::scene3d::helper::Camera::convertScreenSpaceToViewSpace(*camera, point);

        comparePoint(standardPoint, unprojectedPoint);
    }
    root->destroySceneManager(sceneManager);
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace sight::viz::scene3d
