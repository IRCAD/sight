/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

/******************************************************************************************
   MOC - Minimal Ogre Collision v 1.0
   The MIT License

   Copyright (c) 2008, 2009 MouseVolcano (Thomas Gradl, Karolina Sefyrin), Esa Kylli

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

    /** return mesh properties
     * @param mesh pointer to Ogre Mesh
     * @param position world position of the entity using the mesh
     * @param orientation world orientation of the entity using the mesh
     * @param scale world scale of the entity using the mesh
     * @return tuple containing <list of points, list of triangles, list of quads>
     */
    std::tuple<std::vector<Ogre::Vector3>, std::vector<Ogre::uint32>, std::vector< Ogre::uint32> >
    getMeshInformation(const Ogre::MeshPtr mesh,
                       const Ogre::Vector3& position,
                       const Ogre::Quaternion& orient,
                       const Ogre::Vector3& scale);

};

}
