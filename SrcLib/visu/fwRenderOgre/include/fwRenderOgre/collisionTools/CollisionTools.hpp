/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

/******************************************************************************************
   MOC - Minimal Ogre Collision v 1.0
   The MIT License

   Copyright (c) 2008, 2009 MouseVolcano (Thomas Gradl, Karolina Sefyrin), Esa Kylli
   Copyright (c) 2014-2018 IRCAD France
   Copyright (c) 2014-2018 IHU Strasbourg

   Thanks to Erik Biermann for the help with the Videos, SEO and Webwork

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
******************************************************************************************/

#include "fwRenderOgre/config.hpp"

#include <Ogre.h>

namespace fwRenderOgre
{

/**
 * @brief The CollisionTools class
 * Copy of MOC under MIT License, allows to do ray casts
 */
#pragma once

class FWRENDEROGRE_CLASS_API CollisionTools
{
public:
    Ogre::RaySceneQuery* mRaySceneQuery;
    Ogre::RaySceneQuery* mTSMRaySceneQuery;

    Ogre::SceneManager* mSceneMgr;

    FWRENDEROGRE_API CollisionTools(::Ogre::SceneManager* sceneMgr);
    FWRENDEROGRE_API ~CollisionTools();

    FWRENDEROGRE_API bool collidesWithEntity(const Ogre::Vector3& fromPoint, const Ogre::Vector3& toPoint,
                                             const float collisionRadius = 2.5f, const float rayHeightLevel = 0.0f,
                                             const Ogre::uint32 queryMask = 0xFFFFFFFF);

    FWRENDEROGRE_API std::tuple<bool, Ogre::Vector3, Ogre::MovableObject*, float> raycastFromCamera(
        ::Ogre::RenderWindow* rw, Ogre::Camera* camera, const Ogre::Vector2& mousecoords,
        const Ogre::uint32 queryMask = 0xFFFFFFFF);

    FWRENDEROGRE_API std::tuple<bool, Ogre::Vector3, Ogre::MovableObject*, float> raycastFromPoint(const Ogre::Vector3&,
                                                                                                   const Ogre::Vector3&,
                                                                                                   const Ogre::uint32 queryMask =
                                                                                                       0xFFFFFFFF);

    FWRENDEROGRE_API std::tuple<bool, Ogre::Vector3, Ogre::MovableObject*, float> raycast(const Ogre::Ray&,
                                                                                          const Ogre::uint32 queryMask =
                                                                                              0xFFFFFFFF);

private:

    /**
     * @brief Compute the intersection between a ray and a triangle.
     * @param _ray The ray used to compute the intersection.
     * @param _a The first vertex of the triangle.
     * @param _b The second vertex of the triangle.
     * @param _c The third vertex of the triangle.
     * @param _closestDistance The current closest intersection distance with the ray.
     * @param _positiveSide Set to true to intersect with the positive side of the triangle.
     * @param _negativeSide Set to true to intersect with the negative side of the triangle.
     * @return A pair containing a boolean to specify if there was an intersection, and the distance of between the ray
     * origin and the intersection.
     */
    static std::pair<bool, float> intersect(const ::Ogre::Ray& _ray,
                                            const ::Ogre::Vector3& _a,
                                            const ::Ogre::Vector3& _b,
                                            const ::Ogre::Vector3& _c,
                                            const float _closestDistance,
                                            bool _positiveSide,
                                            bool _negativeSide);

};

}
