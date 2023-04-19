/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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
#include <viz/scene3d/WindowManager.hpp>

#include <OgreLogManager.h>
#include <OgreMatrix4.h>
#include <OgreRenderWindow.h>
#include <OgreViewport.h>

#include <limits>
#include <tuple>
#include <vector>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::viz::scene3d::ut::CameraTest);

namespace sight::viz::scene3d::ut
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
    const float n            = 0.1F;
    const float f            = 100;

    // Original matrix
    Ogre::Matrix4 expected(1.6568645238876F, 0.F, -0.0295966863632202F, 0.F,
                           0.F, 3.01612949371338F, 0.0245949625968933F, 0.F,
                           0.F, 0.F, -1.00200200080872F, -0.200200200080872F,
                           0.F, 0.F, -1.F, 0.F);

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
        using tupleType = std::tuple<std::function<void (const data::Camera::sptr, double)>, double, unsigned int,
                                     unsigned int, float>;
        const std::vector<tupleType> permutation {
            tupleType(&sight::data::Camera::setCx, cx, 0, 2, -0.0295966863632202F),
            tupleType(&sight::data::Camera::setCy, cy, 1, 2, 0.0245949625968933F),
            tupleType(&sight::data::Camera::setFx, fx, 0, 0, 1.65686452388763F),
            tupleType(&sight::data::Camera::setFy, fy, 1, 1, 3.01612949371338F),

            tupleType(&sight::data::Camera::setCx, 0., 0, 2, 1.00104212760925F),
            tupleType(&sight::data::Camera::setCy, 0., 1, 2, -1.00185346603394F),
            tupleType(&sight::data::Camera::setFx, 0., 0, 0, 0.F),
            tupleType(&sight::data::Camera::setFy, 0., 1, 1, 0.F),

            tupleType(&sight::data::Camera::setCx, std::numeric_limits<float>::min(), 0, 2, 1.00104212760925F),
            tupleType(&sight::data::Camera::setCy, std::numeric_limits<float>::min(), 1, 2, -1.00185346603394F),
            tupleType(&sight::data::Camera::setFx, std::numeric_limits<float>::min(), 0, 0, 1.22445459812703e-41F),
            tupleType(&sight::data::Camera::setFy, std::numeric_limits<float>::min(), 1, 1, 2.17677703448217e-41F),

            tupleType(&sight::data::Camera::setCx, std::numeric_limits<float>::max(), 0, 2, -3.5464549820937e+35F),
            tupleType(&sight::data::Camera::setCy, std::numeric_limits<float>::max(), 1, 2, 6.30736510970334e+35F),
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
        using tupleType = std::tuple<std::function<void (const data::Camera::sptr, std::size_t)>, std::size_t,
                                     unsigned int, unsigned int, float>;
        const std::vector<tupleType> permutation {
            tupleType(&sight::data::Camera::setWidth, width, 0, 2, -3.5464549820937e+35F),
            tupleType(&sight::data::Camera::setHeight, height, 1, 2, 6.30736510970334e+35F),

            tupleType(&sight::data::Camera::setWidth, 1, 0, 2, -std::numeric_limits<float>::infinity()),
            tupleType(&sight::data::Camera::setHeight, 1, 1, 2, std::numeric_limits<float>::infinity()),

            tupleType(
                &sight::data::Camera::setWidth,
                std::numeric_limits<std::size_t>::max(),
                0,
                2,
                -3.68934859483958e+19F
            ),
            tupleType(
                &sight::data::Camera::setHeight,
                std::numeric_limits<std::size_t>::max(),
                1,
                2,
                3.68934859483958e+19F
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
    auto* const root         = viz::scene3d::Utils::getOgreRoot();
    auto* const sceneManager = root->createSceneManager("DefaultSceneManager", "TestSceneManager");

    sight::viz::scene3d::WindowManager::sptr mgr = sight::viz::scene3d::WindowManager::get();
    Ogre::RenderWindow* renderWindow             = mgr->get("test");
    auto* const camera                           = sceneManager->createCamera("TestCamera");
    renderWindow->addViewport(camera, 0);

    const auto width  = static_cast<float>(renderWindow->getWidth());
    const auto height = static_cast<float>(renderWindow->getHeight());

    camera->setNearClipDistance(1);
    camera->setFarClipDistance(10);
    camera->setAutoAspectRatio(true);
    camera->setOrthoWindowWidth(1920);
    camera->setOrthoWindowHeight(1080);

    camera->setProjectionType(Ogre::ProjectionType::PT_PERSPECTIVE);
    {
        // Manually project a point
        const Ogre::Vector4 standardPoint(1.F, 2.F, 3.F, 1.F);
        const Ogre::Vector4 clippedPoint = camera->getProjectionMatrix() * camera->getViewMatrix() * standardPoint;
        const Ogre::Vector3 ndcPoint     = clippedPoint.xyz() / clippedPoint.w;

        // /!\ in openGl, y coordinate begin from the upper left corner, we need to set him from the lower left corner.
        const Ogre::Real fX = (ndcPoint.x + 1.F) * 0.5F;
        const Ogre::Real fY = 1.F - (ndcPoint.y + 1.F) * 0.5F;
        const Ogre::Real fZ = (ndcPoint.z + 1.F) * 0.5F;
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
        const Ogre::Vector4 standardPoint(1.F, 2.F, 3.F, 1.F);
        const Ogre::Vector4 clippedPoint = camera->getProjectionMatrix() * camera->getViewMatrix() * standardPoint;
        const Ogre::Vector3 ndcPoint     = clippedPoint.xyz() / clippedPoint.w;

        // /!\ in openGl, y coordinate begin from the upper left corner, we need to set him from the lower left corner.
        const Ogre::Real fX = (ndcPoint.x + 1.F) * 0.5F;
        const Ogre::Real fY = 1.F - (ndcPoint.y + 1.F) * 0.5F;
        const Ogre::Real fZ = (ndcPoint.z + 1.F) * 0.5F;
        const Ogre::Vector3 viewportPoint(fX, fY, fZ);

        // Unproject the projected point
        const Ogre::Vector3 point            = viewportPoint * Ogre::Vector3(width, height, 1);
        const Ogre::Vector3 unprojectedPoint =
            viz::scene3d::helper::Camera::convertScreenSpaceToViewSpace(*camera, point);

        comparePoint(standardPoint, unprojectedPoint);
    }
    renderWindow->removeViewport(0);
    root->destroySceneManager(sceneManager);
}

//------------------------------------------------------------------------------

void CameraTest::convertWorldSpaceToScreenSpace()
{
    auto* const root         = viz::scene3d::Utils::getOgreRoot();
    auto* const sceneManager = root->createSceneManager("DefaultSceneManager", "TestSceneManager");

    sight::viz::scene3d::WindowManager::sptr mgr = sight::viz::scene3d::WindowManager::get();
    Ogre::RenderWindow* renderWindow             = mgr->get("test");
    auto* const camera                           = sceneManager->createCamera("TestCamera");
    renderWindow->addViewport(camera, 0);

    camera->setNearClipDistance(1);
    camera->setFarClipDistance(10);
    camera->setAutoAspectRatio(true);
    camera->setOrthoWindowWidth(1920);
    camera->setOrthoWindowHeight(1080);

    auto* cameraNode = sceneManager->createSceneNode();
    const Ogre::Quaternion rotateX(Ogre::Degree(65), Ogre::Vector3(1, 0, 0));
    const Ogre::Quaternion rotateY(Ogre::Degree(-176), Ogre::Vector3(0, 1, 0));
    cameraNode->setOrientation(rotateX * rotateY);
    cameraNode->setPosition(-12.F, 5.F, 234.F);

    camera->setProjectionType(Ogre::ProjectionType::PT_PERSPECTIVE);
    {
        const Ogre::Vector3 standardPoint(-4.F, 4.F, 3.F);
        const Ogre::Vector2 projectedPoint = viz::scene3d::helper::Camera::convertWorldSpaceToScreenSpace(
            *camera,
            standardPoint
        );

        const Ogre::Vector2 point(341.4213F, 421.8951F);

        CPPUNIT_ASSERT_DOUBLES_EQUAL(point[0], projectedPoint[0], 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(point[1], projectedPoint[1], 0.0001);
    }

    camera->setProjectionType(Ogre::ProjectionType::PT_ORTHOGRAPHIC);
    {
        const Ogre::Vector3 standardPoint(-4.F, 87.F, 3.F);
        const Ogre::Vector2 projectedPoint = viz::scene3d::helper::Camera::convertWorldSpaceToScreenSpace(
            *camera,
            standardPoint
        );

        const Ogre::Vector2 point(99.4444F, 83.8888F);

        CPPUNIT_ASSERT_DOUBLES_EQUAL(point[0], projectedPoint[0], 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(point[1], projectedPoint[1], 0.0001);
    }
    renderWindow->removeViewport(0);
    root->destroySceneManager(sceneManager);
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d::ut
