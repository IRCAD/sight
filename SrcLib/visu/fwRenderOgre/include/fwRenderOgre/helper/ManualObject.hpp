/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "fwRenderOgre/config.hpp"

#include <Ogre.h>

namespace fwRenderOgre
{

namespace helper
{

class ManualObject
{

public:

    /**
     * @brief createCylinder create a cylinder in a ::Ogre::ManualObject along the x axis
     * @param _object the ::Ogre::ManualObject used to store the cylinder
     * @param _material the material used to create the cylinder
     * @param _color the cylinder's color
     * @param _radius the cylinder's radius
     * @param _length the cylinder's length
     * @param _sample the number of samples used to create the cylinder
     */
    FWRENDEROGRE_API static void createCylinder(::Ogre::ManualObject* _object,
                                                const std::string& _material,
                                                const ::Ogre::ColourValue& _color = ::Ogre::ColourValue(1.f, 1.f, 1.f),
                                                float _radius                     = 1.f,
                                                float _length                     = 100.f,
                                                unsigned int _sample              = 64);

    /**
     * @brief createCone create a cone in a ::Ogre::ManualObject along the x axis
     * @param _object the ::Ogre::ManualObject used to store the cone
     * @param _material the material used to create the cone
     * @param _color the cone's color
     * @param _radius the cone's radius
     * @param _length the cone's length
     * @param _sample the number of samples used to create the cone
     */
    FWRENDEROGRE_API static void createCone(::Ogre::ManualObject* _object,
                                            const std::string& _material,
                                            const ::Ogre::ColourValue& _color = ::Ogre::ColourValue(1.f, 1.f, 1.f),
                                            float _radius                     = 1.f,
                                            float _length                     = 100.f,
                                            unsigned int _sample              = 64);

    /**
     * @brief createCube create a cube in a ::Ogre::ManualObject centered in the center of the cube
     * @param _object the ::Ogre::ManualObject used to store the cube
     * @param _material the material used to create the cube
     * @param _color the cube's color
     * @param _length the cube's length
     */
    FWRENDEROGRE_API static void createCube(::Ogre::ManualObject* _object,
                                            const std::string& _material,
                                            const ::Ogre::ColourValue& _color = ::Ogre::ColourValue(1.f, 1.f, 1.f),
                                            float _length                     = 100.f);

    /**
     * @brief createSphere create a sphere in a ::Ogre::ManualObject centered in the center of the sphere
     * @param _object the ::Ogre::ManualObject used to store the sphere
     * @param _material the material used to create the sphere
     * @param _color the sphere's color
     * @param _radius the sphere's radius
     * @param _sample the number of samples used to create the sphere
     */
    FWRENDEROGRE_API static void createSphere(::Ogre::ManualObject* _object,
                                              const std::string& _material,
                                              const ::Ogre::ColourValue& _color = ::Ogre::ColourValue(1.f, 1.f, 1.f),
                                              float _radius                     = 100.f,
                                              unsigned int _sample              = 50);

};

} // namespace helper

} // namespace fwRenderOgre
