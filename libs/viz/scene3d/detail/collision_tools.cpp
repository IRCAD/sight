/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include "viz/scene3d/detail/collision_tools.hpp"

#include "viz/scene3d/factory/r2vb_renderable.hpp"
#include "viz/scene3d/layer.hpp"
#include "viz/scene3d/r2vb_renderable.hpp"

#include <cmath>
#include <functional>

namespace sight::viz::scene3d::detail
{

collision_tools::collision_tools(Ogre::SceneManager& _scene_mgr, std::uint32_t _query_mask) :
    mSceneMgr(_scene_mgr)
{
    mRaySceneQuery = mSceneMgr.createRayQuery(Ogre::Ray(), _query_mask);
    if(nullptr == mRaySceneQuery)
    {
        // LOG_ERROR << "Failed to create Ogre::RaySceneQuery instance" << ENDLOG;
        return;
    }

    mRaySceneQuery->setSortByDistance(true);
}

collision_tools::~collision_tools()
{
    delete mRaySceneQuery;
}

//------------------------------------------------------------------------------

bool collision_tools::collidesWithEntity(
    const Ogre::Vector3& _from_point,
    const Ogre::Vector3& _to_point,
    const Ogre::uint32 _query_mask,
    const float _collision_radius,
    const float _ray_height_level
) const
{
    Ogre::Vector3 from_point_adj(_from_point.x, _from_point.y + _ray_height_level, _from_point.z);
    Ogre::Vector3 to_point_adj(_to_point.x, _to_point.y + _ray_height_level, _to_point.z);
    Ogre::Vector3 normal = to_point_adj - from_point_adj;
    float dist_to_dest   = normal.normalise();

    std::tuple<bool, Ogre::Vector3, Ogre::MovableObject*,
               float> res = raycastFromPoint(from_point_adj, normal, _query_mask);

    if(std::get<0>(res))
    {
        float dist_to_coll = 0.0F;
        dist_to_coll  = std::get<3>(res);
        dist_to_coll -= _collision_radius;
        return dist_to_coll <= dist_to_dest;
    }

    return false;
}

//------------------------------------------------------------------------------

std::tuple<bool, Ogre::Vector3, Ogre::MovableObject*, float> collision_tools::raycastFromCamera(
    Ogre::RenderWindow* _rw,
    Ogre::Camera* _camera,
    const Ogre::Vector2& _mousecoords,
    const Ogre::uint32 _query_mask
) const
{
    // Create the ray to test
    Ogre::Real tx = _mousecoords.x / static_cast<Ogre::Real>(_rw->getWidth());
    Ogre::Real ty = _mousecoords.y / static_cast<Ogre::Real>(_rw->getHeight());
    Ogre::Ray ray = _camera->getCameraToViewportRay(tx, ty);

    return raycast(ray, _query_mask);
}

//------------------------------------------------------------------------------

std::tuple<bool, Ogre::Vector3, Ogre::MovableObject*, float> collision_tools::raycastFromPoint(
    const Ogre::Vector3& _point,
    const Ogre::Vector3& _normal,
    const Ogre::uint32 _query_mask
) const
{
    // create the ray to test
    static Ogre::Ray ray;
    ray.setOrigin(_point);
    ray.setDirection(_normal);

    return raycast(ray, _query_mask);
}

//------------------------------------------------------------------------------

std::tuple<bool, Ogre::Vector3, Ogre::MovableObject*, float> collision_tools::raycast(
    const Ogre::Ray& _ray,
    const Ogre::uint32 _query_mask
) const
{
    Ogre::Vector3 result {0.0, 0.0, 0.0};
    Ogre::MovableObject* target = nullptr;
    float closest_distance      = NAN;

    // Check we are initialised.
    if(mRaySceneQuery != nullptr)
    {
        // Create a query object.
        mRaySceneQuery->setRay(_ray);
        mRaySceneQuery->setSortByDistance(true);
        mRaySceneQuery->setQueryMask(_query_mask);
        // Execute the query, returns a vector of hits.
        if(mRaySceneQuery->execute().empty())
        {
            // Raycast did not hit an objects bounding box.
            return std::make_tuple(false, result, target, closest_distance);
        }
    }
    else
    {
        return std::make_tuple(false, result, target, closest_distance);
    }

    // At this point we have raycast to a series of different objects bounding boxes.
    // We need to test these different objects to see which is the first polygon hit.
    // There are some minor optimizations (distance based) that mean we wont have to
    // check all of the objects most of the time, but the worst case scenario is that
    // we need to test every triangle of every object.
    closest_distance = -1.0F;
    Ogre::Vector3 closest_result;
    Ogre::RaySceneQueryResult& query_result = mRaySceneQuery->getLastResults();
    for(auto& qr_idx : query_result)
    {
        // Stop checking if we have found a raycast hit that is closer
        // than all remaining entities.
        if((closest_distance >= 0.0F)
           && (closest_distance < qr_idx.distance))
        {
            break;
        }

        // Get the entity to check.
        Ogre::MovableObject* const entity = qr_idx.movable;

        const bool is_entity = qr_idx.movable->getMovableType() == "Entity";
        const bool is_r2_vb  = qr_idx.movable->getMovableType() == factory::r2vb_renderable::FACTORY_TYPE_NAME;

        // Only check this result if its a hit against an entity.
        if((entity != nullptr) && (is_entity || is_r2_vb))
        {
            if(!entity->isVisible())
            {
                continue;
            }

            const Ogre::Vector3 position       = entity->getParentNode()->_getDerivedPosition();
            const Ogre::Quaternion orientation = entity->getParentNode()->_getDerivedOrientation();
            const Ogre::Vector3 scale          = entity->getParentNode()->_getDerivedScale();

            const Ogre::MeshPtr mesh = is_entity
                                       ? static_cast<Ogre::Entity*>(entity)->getMesh()
                                       : static_cast<viz::scene3d::r2vb_renderable*>(entity)->getMesh();

            std::vector<Ogre::Vector3> vertices;
            bool added_shared = false;

            for(const Ogre::SubMesh* const submesh : mesh->getSubMeshes())
            {
                // Store vertices one time if data are shared, else the vector is refilled each time.
                if((!submesh->useSharedVertices) || !added_shared)
                {
                    if(submesh->useSharedVertices)
                    {
                        added_shared = true;
                        vertices.resize(mesh->sharedVertexData->vertexCount);
                    }
                    else
                    {
                        vertices.clear();
                        vertices.resize(submesh->vertexData->vertexCount);
                    }

                    const Ogre::VertexData* vertex_data =
                        submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;
                    const Ogre::VertexElement* const pos_elem =
                        vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
                    const Ogre::HardwareVertexBufferSharedPtr vertex_buffer =
                        vertex_data->vertexBufferBinding->getBuffer(
                            pos_elem->getSource()
                        );

                    auto* vertex =
                        static_cast<unsigned char*>(vertex_buffer->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
                    float* p_real = nullptr;

                    for(std::size_t j = 0 ;
                        j < vertex_data->vertexCount ;
                        ++j, vertex += vertex_buffer->getVertexSize())
                    {
                        pos_elem->baseVertexPointerToElement(vertex, &p_real);
                        Ogre::Vector3 pt(p_real[0], p_real[1], p_real[2]);
                        vertices[j] = (orientation * (pt * scale)) + position;
                    }

                    vertex_buffer->unlock();
                }

                // Retrieve the index buffer to loop over vertices.
                std::vector<Ogre::uint32> indices;
                {
                    const Ogre::IndexData* const index_data       = submesh->indexData;
                    const Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;

                    // Billboars do not define an entity, they do not contain any index buffer.
                    if(ibuf != nullptr)
                    {
                        const bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

                        const auto* const p_long =
                            static_cast<const Ogre::uint32*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
                        const auto* const p_short = reinterpret_cast<const std::uint16_t*>(p_long);

                        indices.resize(index_data->indexCount);

                        // Iterate over index buffer.
                        if(use32bitindexes)
                        {
                            for(std::size_t k = 0 ; k < index_data->indexCount ; ++k)
                            {
                                indices[k] = p_long[k];
                            }
                        }
                        else
                        {
                            for(std::size_t k = 0 ; k < index_data->indexCount ; ++k)
                            {
                                indices[k] = static_cast<Ogre::uint32>(p_short[k]);
                            }
                        }

                        ibuf->unlock();
                    }
                }

                // Retrieve the material to check the culling mode.
                const Ogre::MaterialPtr material = Ogre::MaterialManager::getSingletonPtr()->getByName(
                    submesh->getMaterialName()
                );
                bool positive_side = true;
                bool negative_side = true;
                if(material != nullptr)
                {
                    positive_side = false;
                    negative_side = false;
                    const auto* tech = material->getBestTechnique();
                    SIGHT_ASSERT("'" + material->getName() + "' does not have any suitable technique.", tech);
                    const auto& passes = tech->getPasses();
                    SIGHT_ASSERT(
                        "'" + material->getName() + "' technique '" + tech->getName() + "' does not define any passes.",
                        !passes.empty()
                    );
                    for(const Ogre::Pass* pass : passes)
                    {
                        const Ogre::CullingMode culling_mode = pass->getCullingMode();
                        switch(culling_mode)
                        {
                            case Ogre::CULL_NONE:
                                positive_side = true;
                                negative_side = true;
                                break;

                            case Ogre::CULL_CLOCKWISE:
                                positive_side = true;
                                break;

                            case Ogre::CULL_ANTICLOCKWISE:
                                negative_side = true;
                                break;

                            default:
                                SIGHT_ERROR("Unsupported culling mode");
                                break;
                        }
                    }
                }

                // Check intersections depending on the render operation type.
                bool new_closest_found = false;
                switch(submesh->operationType)
                {
                    // Points list is used for billboard.
                    case Ogre::RenderOperation::OT_POINT_LIST:
                    {
                        // We do not have faces, we simply check all vertices and compute the distance from the picked
                        // point
                        const Ogre::Camera* const camera = mSceneMgr.getCamera(
                            viz::scene3d::layer::s_DEFAULT_CAMERA_NAME
                        );
                        const Ogre::Matrix4 view_matrix = camera->getViewMatrix();
                        const Ogre::Matrix4 proj_matrix = camera->getProjectionMatrixWithRSDepth();

                        const Ogre::Matrix4 view_proj_matrix = proj_matrix * view_matrix;

                        const Ogre::Vector3 res_point_wvp = view_proj_matrix * _ray.getPoint(qr_idx.distance);
                        const Ogre::Vector2 res_point_ss  = (res_point_wvp.xy() / 2.F) + 0.5F;

                        static const Ogre::Real s_TOLERANCE = 0.02F;

                        if(!indices.empty())
                        {
                            for(unsigned int indice : indices)
                            {
                                const Ogre::Vector3 point_wvp = view_proj_matrix * vertices[indice];
                                const Ogre::Vector2 point_ss  = (point_wvp.xy() / 2.F) + 0.5F;

                                if(((closest_distance < 0.0F) || (qr_idx.distance < closest_distance))
                                   && res_point_ss.distance(point_ss) < s_TOLERANCE)
                                {
                                    new_closest_found = true;
                                    closest_distance  = vertices[indice].distance(_ray.getOrigin());
                                }
                            }
                        }
                        else
                        {
                            for(auto& vertice : vertices)
                            {
                                const Ogre::Vector3 point_wvp = view_proj_matrix * vertice;
                                const Ogre::Vector2 point_ss  = (point_wvp.xy() / 2.F) + 0.5F;

                                if(((closest_distance < 0.0F) || (qr_idx.distance < closest_distance))
                                   && res_point_ss.distance(point_ss) < s_TOLERANCE)
                                {
                                    new_closest_found = true;
                                    closest_distance  = vertice.distance(_ray.getOrigin());
                                }
                            }
                        }

                        break;
                    }

                    // Lines list is used to represent a quad.
                    case Ogre::RenderOperation::OT_LINE_LIST:
                        if(!indices.empty())
                        {
                            for(std::size_t i = 0 ; i < indices.size() ; i += 4)
                            {
                                const auto first_hit = intersect(
                                    _ray,
                                    vertices[indices[i]],
                                    vertices[indices[i + 1]],
                                    vertices[indices[i + 2]],
                                    closest_distance,
                                    positive_side,
                                    negative_side
                                );
                                if(first_hit.first)
                                {
                                    new_closest_found = true;
                                    closest_distance  = first_hit.second;
                                }

                                const auto second_hit = intersect(
                                    _ray,
                                    vertices[indices[i + 2]],
                                    vertices[indices[i + 3]],
                                    vertices[indices[i]],
                                    closest_distance,
                                    positive_side,
                                    negative_side
                                );
                                if(second_hit.first)
                                {
                                    new_closest_found = true;
                                    closest_distance  = second_hit.second;
                                }
                            }
                        }
                        else
                        {
                            for(std::size_t i = 0 ; i < vertices.size() ; i += 4)
                            {
                                const auto first_hit = intersect(
                                    _ray,
                                    vertices[i],
                                    vertices[i + 1],
                                    vertices[i + 2],
                                    closest_distance,
                                    positive_side,
                                    negative_side
                                );
                                if(first_hit.first)
                                {
                                    new_closest_found = true;
                                    closest_distance  = first_hit.second;
                                }

                                const auto second_hit = intersect(
                                    _ray,
                                    vertices[i + 2],
                                    vertices[i + 3],
                                    vertices[i],
                                    closest_distance,
                                    positive_side,
                                    negative_side
                                );
                                if(second_hit.first)
                                {
                                    new_closest_found = true;
                                    closest_distance  = second_hit.second;
                                }
                            }
                        }

                        break;

                    // Triangles list is simply a list of triangles.
                    case Ogre::RenderOperation::OT_TRIANGLE_LIST:
                        if(!indices.empty())
                        {
                            for(std::size_t i = 0 ; i < indices.size() ; i += 3)
                            {
                                const auto first_hit = intersect(
                                    _ray,
                                    vertices[indices[i]],
                                    vertices[indices[i + 1]],
                                    vertices[indices[i + 2]],
                                    closest_distance,
                                    positive_side,
                                    negative_side
                                );
                                if(first_hit.first)
                                {
                                    new_closest_found = true;
                                    closest_distance  = first_hit.second;
                                }
                            }
                        }
                        else
                        {
                            for(std::size_t i = 0 ; i < vertices.size() ; i += 3)
                            {
                                const auto first_hit = intersect(
                                    _ray,
                                    vertices[i],
                                    vertices[i + 1],
                                    vertices[i + 2],
                                    closest_distance,
                                    positive_side,
                                    negative_side
                                );
                                if(first_hit.first)
                                {
                                    new_closest_found = true;
                                    closest_distance  = first_hit.second;
                                }
                            }
                        }

                        break;

                    default:
                        SIGHT_ERROR("Unsupported operation type");
                        break;
                }

                if(new_closest_found)
                {
                    target         = entity;
                    closest_result = _ray.getPoint(closest_distance);
                }
            }
        }
    }

    const bool collision_success = (closest_distance >= 0.0F);
    return std::make_tuple(collision_success, closest_result, target, closest_distance);
}

//------------------------------------------------------------------------------

std::pair<bool, float> collision_tools::intersect(
    const Ogre::Ray& _ray,
    const Ogre::Vector3& _a,
    const Ogre::Vector3& _b,
    const Ogre::Vector3& _c,
    const float _closest_distance,
    bool _positive_side,
    bool _negative_side
)
{
    const std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(_ray, _a, _b, _c, _positive_side, _negative_side);
    if(hit.first)
    {
        if((_closest_distance < 0.0F) || (hit.second < _closest_distance))
        {
            return {true, hit.second};
        }
    }

    return {false, hit.second};
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d::detail
