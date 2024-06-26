/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
 * Copyright (C) 2014-2021 IHU Strasbourg
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
// cspell: disable
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
// cspell: enable

#include <sight/viz/scene3d/config.hpp>

#include <Ogre.h>

/**
 * @brief The collision_tools class
 * Copy of MOC under MIT License, allows to do ray casts
 */
#pragma once

namespace sight::viz::scene3d::detail
{

class collision_tools
{
public:

    /// Use for querying along the ray.
    Ogre::RaySceneQuery* m_ray_scene_query {nullptr};

    /// The scene manager where launch the ray.
    Ogre::SceneManager& m_scene_mgr;

    /**
     * @brief Create the ray scene query.
     * @param _scene_mgr The scene manager where launch the ray.
     * @param _query_mask Mask used to query entities with matching flags.
     */
    collision_tools(Ogre::SceneManager& _scene_mgr, std::uint32_t _query_mask);

    /// Delete resources.
    ~collision_tools();

    /**
     * @brief Check for collisions with entities along a cylinder.
     * @param _from_point The ray origin.
     * @param _to_point The ray destination.
     * @param _query_mask Mask used to query entities with matching flags.
     * @param _collision_radius Radius of the cylinder.
     * @param _ray_height_level The height level of the ray.
     * @return True if an entity collide the ray.
     */
    [[nodiscard]] bool collides_with_entity(
        const Ogre::Vector3& _from_point,
        const Ogre::Vector3& _to_point,
        Ogre::uint32 _query_mask,
        float _collision_radius = 2.5F,
        float _ray_height_level = 0.0F
    ) const;

    /**
     * @brief Check for collisions with entities along a ray define by a mouse position.
     * @param _rw The render window used to retrieve width and height.
     * @param _camera The camera use to get the origin of the ray.
     * @param _mousecoords The mouse position used to get the ray destination.
     * @param _query_mask Mask used to query entities with matching flags.
     * @return A tuple containing a boolean to specify if there was an intersection, the position of the intersection,
     * and the intersected object.
     */
    std::tuple<bool, Ogre::Vector3, Ogre::MovableObject*, float> raycast_from_camera(
        Ogre::RenderWindow* _rw,
        Ogre::Camera* _camera,
        const Ogre::Vector2& _mousecoords,
        Ogre::uint32 _query_mask
    ) const;

    /**
     * @brief raycastFromPoint Check for collisions with entities along a ray define by an origin and a direction.
     * @param _point The ray origin.
     * @param _normal The ray direction.
     * @param _query_mask Mask used to query entities with matching flags.
     * @return A tuple containing a boolean to specify if there was an intersection, the position of the intersection,
     * and the intersected object.
     */
    [[nodiscard]] std::tuple<bool, Ogre::Vector3, Ogre::MovableObject*, float> raycast_from_point(
        const Ogre::Vector3& _point,
        const Ogre::Vector3& _normal,
        Ogre::uint32 _query_mask
    ) const;

    /**
     * @brief Check for collisions with entities along a ray.
     * @param _ray The ray used to check collisions.
     * @param _query_mask   Mask used to query entities with matching flags.
     * @return A tuple containing a boolean to specify if there was an intersection, the position of the intersection,
     * and the intersected object.
     */
    [[nodiscard]] std::tuple<bool, Ogre::Vector3, Ogre::MovableObject*, float> raycast(
        const Ogre::Ray& _ray,
        Ogre::uint32 _query_mask
    ) const;

private:

    /**
     * @brief Compute the intersection between a ray and a triangle.
     * @param _ray The ray used to compute the intersection.
     * @param _a The first vertex of the triangle.
     * @param _b The second vertex of the triangle.
     * @param _c The third vertex of the triangle.
     * @param _closest_distance The current closest intersection distance with the ray.
     * @param _positive_side Set to true to intersect with the positive side of the triangle.
     * @param _negative_side Set to true to intersect with the negative side of the triangle.
     * @return A pair containing a boolean to specify if there was an intersection, and the distance of between the ray
     * origin and the intersection.
     */
    static std::pair<bool, float> intersect(
        const Ogre::Ray& _ray,
        const Ogre::Vector3& _a,
        const Ogre::Vector3& _b,
        const Ogre::Vector3& _c,
        float _closest_distance,
        bool _positive_side,
        bool _negative_side
    );
};

} // namespace sight::viz::scene3d::detail
