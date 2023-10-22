/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
 * Copyright (C) 2021 IHU Strasbourg
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

#include "viz/scene3d/config.hpp"

#include <Ogre.h>

#include <optional>

namespace sight::viz::scene3d::helper
{

class manual_object
{
public:

    /**
     * @brief createCylinder create a cylinder in a Ogre::ManualObject along the x axis
     * @param _object the Ogre::ManualObject used to store the cylinder
     * @param _material the material used to create the cylinder
     * @param _color the cylinder's color
     * @param _radius the cylinder's radius
     * @param _length the cylinder's length
     * @param _sample the number of samples used to create the cylinder
     */
    VIZ_SCENE3D_API static void createCylinder(
        Ogre::ManualObject* _object,
        const std::string& _material,
        const Ogre::ColourValue& _color = Ogre::ColourValue(1.F, 1.F, 1.F),
        float _radius                   = 1.F,
        float _length                   = 100.F,
        unsigned int _sample            = 64
    );

    /**
     * @brief createCone create a cone in a Ogre::ManualObject along the x axis
     * @param _object the Ogre::ManualObject used to store the cone
     * @param _material the material used to create the cone
     * @param _color the cone's color
     * @param _radius the cone's radius
     * @param _length the cone's length
     * @param _sample the number of samples used to create the cone
     */
    VIZ_SCENE3D_API static void createCone(
        Ogre::ManualObject* _object,
        const std::string& _material,
        const Ogre::ColourValue& _color = Ogre::ColourValue(1.F, 1.F, 1.F),
        float _radius                   = 1.F,
        float _length                   = 100.F,
        unsigned int _sample            = 64
    );

    /**
     * @brief createCube create a cube in a Ogre::ManualObject centered in the center of the cube
     * @param _object the Ogre::ManualObject used to store the cube
     * @param _material the material used to create the cube
     * @param _color the cube's color
     * @param _length the cube's length
     */
    VIZ_SCENE3D_API static void createCube(
        Ogre::ManualObject* _object,
        const std::string& _material,
        const Ogre::ColourValue& _color = Ogre::ColourValue(1.F, 1.F, 1.F),
        float _length                   = 100.F
    );

    /**
     * @brief createSphere create a sphere in a Ogre::ManualObject centered in the center of the sphere
     * @param _object the Ogre::ManualObject used to store the sphere
     * @param _material the material used to create the sphere
     * @param _color the sphere's color
     * @param _radius the sphere's radius
     * @param _sample the number of samples used to create the sphere
     */
    VIZ_SCENE3D_API static void createSphere(
        Ogre::ManualObject* _object,
        const std::string& _material,
        const Ogre::ColourValue& _color = Ogre::ColourValue(1.F, 1.F, 1.F),
        float _radius                   = 100.F,
        unsigned int _sample            = 50
    );

    /**
     * @brief Creates a camera frustum in a Ogre::ManualObject
     * @param _object the Ogre::ManualObject used to store the frustum
     * @param _material the material used to create the frustum
     * @param _frustum Ogre's camera frustum
     */
    VIZ_SCENE3D_API static void createFrustum(
        Ogre::ManualObject* _object,
        const std::string& _material,
        const Ogre::Frustum& _frustum
    );

    /**
     * @brief Draw a dashed line in a Ogre::ManualObject between two points.
     *
     * On the contrary to the create* functions of this class, this function is intended to be used inside
     * a manual_object::begin()/manual_object::end() scope. Thus it can be called many times to create more complex
     * shape made of lines, but which will issue a single draw call.
     *
     * @param _object the Ogre::ManualObject used to store the cone
     * @param _p1 first point
     * @param _p2 second point
     * @param _dashLength length of a dash
     * @param _dashSpacing spacing between two dashes
     * @param _color optional line color, only necessary when using a material that supports vertex color
     */
    VIZ_SCENE3D_API static void drawDashedLine(
        Ogre::ManualObject* _object,
        const Ogre::Vector3& _p1,
        const Ogre::Vector3& _p2,
        const float _dash_length,
        const float _dash_spacing,
        std::optional<Ogre::ColourValue> _color = {});
};

} // namespace sight::viz::scene3d::helper
