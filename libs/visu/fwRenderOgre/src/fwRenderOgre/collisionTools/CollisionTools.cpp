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

#include "fwRenderOgre/collisionTools/CollisionTools.hpp"

#include "fwRenderOgre/factory/R2VBRenderable.hpp"
#include "fwRenderOgre/Layer.hpp"
#include "fwRenderOgre/R2VBRenderable.hpp"

#include <functional>

namespace fwRenderOgre
{

CollisionTools::CollisionTools(::Ogre::SceneManager* _sceneMgr, std::uint32_t _queryMask)
{
    mSceneMgr = _sceneMgr;

    mRaySceneQuery = mSceneMgr->createRayQuery(::Ogre::Ray(), _queryMask);
    if (nullptr == mRaySceneQuery)
    {
        // LOG_ERROR << "Failed to create Ogre::RaySceneQuery instance" << ENDLOG;
        return;
    }
    mRaySceneQuery->setSortByDistance(true);
}

CollisionTools::~CollisionTools()
{
    if (mRaySceneQuery != nullptr)
    {
        delete mRaySceneQuery;
    }
}

//------------------------------------------------------------------------------

bool CollisionTools::collidesWithEntity(const Ogre::Vector3& _fromPoint,
                                        const Ogre::Vector3& _toPoint,
                                        const Ogre::uint32 _queryMask,
                                        const float _collisionRadius,
                                        const float _rayHeightLevel)
{
    Ogre::Vector3 fromPointAdj(_fromPoint.x, _fromPoint.y + _rayHeightLevel, _fromPoint.z);
    Ogre::Vector3 toPointAdj(_toPoint.x, _toPoint.y + _rayHeightLevel, _toPoint.z);
    Ogre::Vector3 normal = toPointAdj - fromPointAdj;
    float distToDest     = normal.normalise();
    float distToColl     = 0.0f;

    std::tuple<bool, Ogre::Vector3, Ogre::MovableObject*,
               float> res = raycastFromPoint(fromPointAdj, normal, _queryMask);

    if (std::get<0>(res))
    {
        distToColl  = std::get<3>(res);
        distToColl -= _collisionRadius;
        return (distToColl <= distToDest);
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------

std::tuple<bool, Ogre::Vector3, Ogre::MovableObject*, float> CollisionTools::raycastFromCamera(
    ::Ogre::RenderWindow* _rw,
    Ogre::Camera* _camera,
    const Ogre::Vector2& _mousecoords,
    const Ogre::uint32 _queryMask)
{
    // Create the ray to test
    Ogre::Real tx = _mousecoords.x / static_cast< ::Ogre::Real>(_rw->getWidth());
    Ogre::Real ty = _mousecoords.y / static_cast< ::Ogre::Real>(_rw->getHeight());
    Ogre::Ray ray = _camera->getCameraToViewportRay(tx, ty);

    return raycast(ray, _queryMask);
}

//------------------------------------------------------------------------------

std::tuple<bool, Ogre::Vector3, Ogre::MovableObject*, float> CollisionTools::raycastFromPoint(
    const Ogre::Vector3& _point,
    const Ogre::Vector3& _normal,
    const Ogre::uint32 _queryMask)
{
    // create the ray to test
    static Ogre::Ray ray;
    ray.setOrigin(_point);
    ray.setDirection(_normal);

    return raycast(ray, _queryMask);
}

//------------------------------------------------------------------------------

std::tuple<bool, Ogre::Vector3, Ogre::MovableObject*, float> CollisionTools::raycast(const Ogre::Ray& _ray,
                                                                                     const Ogre::uint32 _queryMask)
{
    Ogre::Vector3 result;
    Ogre::MovableObject* target = nullptr;
    float closestDistance;

    // Check we are initialised.
    if (mRaySceneQuery != nullptr)
    {
        // Create a query object.
        mRaySceneQuery->setRay(_ray);
        mRaySceneQuery->setSortByDistance(true);
        mRaySceneQuery->setQueryMask(_queryMask);
        // Execute the query, returns a vector of hits.
        if (mRaySceneQuery->execute().size() <= 0)
        {
            // Raycast did not hit an objects bounding box.
            return std::make_tuple(false, result, target, closestDistance);
        }
    }
    else
    {
        return std::make_tuple(false, result, target, closestDistance);
    }

    // At this point we have raycast to a series of different objects bounding boxes.
    // We need to test these different objects to see which is the first polygon hit.
    // There are some minor optimizations (distance based) that mean we wont have to
    // check all of the objects most of the time, but the worst case scenario is that
    // we need to test every triangle of every object.
    closestDistance = -1.0f;
    Ogre::Vector3 closestResult;
    Ogre::RaySceneQueryResult& queryResult = mRaySceneQuery->getLastResults();
    for (size_t qrIdx = 0; qrIdx < queryResult.size(); qrIdx++)
    {
        // Stop checking if we have found a raycast hit that is closer
        // than all remaining entities.
        if ((closestDistance >= 0.0f) &&
            (closestDistance < queryResult[qrIdx].distance))
        {
            break;
        }

        // Get the entity to check.
        Ogre::MovableObject* const pentity = queryResult[qrIdx].movable;

        const bool isEntity = queryResult[qrIdx].movable->getMovableType().compare("Entity") == 0;
        const bool isR2VB   = queryResult[qrIdx].movable->getMovableType().compare(
            factory::R2VBRenderable::FACTORY_TYPE_NAME) == 0;

        // Only check this result if its a hit against an entity.
        if( (pentity != nullptr) && (isEntity || isR2VB) )
        {
            if(!pentity->isVisible())
            {
                continue;
            }

            const Ogre::Vector3 position       = pentity->getParentNode()->_getDerivedPosition();
            const Ogre::Quaternion orientation = pentity->getParentNode()->_getDerivedOrientation();
            const Ogre::Vector3 scale          = pentity->getParentNode()->_getDerivedScale();

            const ::Ogre::MeshPtr mesh = isEntity ?
                                         static_cast< ::Ogre::Entity* >(pentity)->getMesh() :
                                         static_cast< ::fwRenderOgre::R2VBRenderable*>(pentity)->getMesh();

            std::vector< ::Ogre::Vector3> vertices;
            bool addedShared = false;

            for(const Ogre::SubMesh* const submesh : mesh->getSubMeshes())
            {
                // Store vertices one time if data are shared, else the vector is refilled each time.
                if( (!submesh->useSharedVertices) || (submesh->useSharedVertices && !addedShared))
                {
                    if(submesh->useSharedVertices)
                    {
                        addedShared = true;
                        vertices.resize(mesh->sharedVertexData->vertexCount);
                    }
                    else
                    {
                        vertices.clear();
                        vertices.resize(submesh->vertexData->vertexCount);
                    }

                    const Ogre::VertexData* vertexData =
                        submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;
                    const Ogre::VertexElement* const posElem =
                        vertexData->vertexDeclaration->findElementBySemantic(::Ogre::VES_POSITION);
                    const Ogre::HardwareVertexBufferSharedPtr vbuf = vertexData->vertexBufferBinding->getBuffer(
                        posElem->getSource());

                    unsigned char* vertex =
                        static_cast<unsigned char*>(vbuf->lock(::Ogre::HardwareBuffer::HBL_READ_ONLY));
                    float* pReal;

                    for(size_t j = 0; j < vertexData->vertexCount; ++j, vertex += vbuf->getVertexSize())
                    {
                        posElem->baseVertexPointerToElement(vertex, &pReal);
                        Ogre::Vector3 pt(pReal[0], pReal[1], pReal[2]);
                        vertices[j] = (orientation * (pt * scale)) + position;
                    }

                    vbuf->unlock();
                }

                // Retrieve the index buffer to loop over vertices.
                std::vector< ::Ogre::uint32> indices;
                {
                    const Ogre::IndexData* const indexData        = submesh->indexData;
                    const Ogre::HardwareIndexBufferSharedPtr ibuf = indexData->indexBuffer;

                    // Billboars do not define an entity, they do not contain any index buffer.
                    if(ibuf != nullptr)
                    {
                        const bool use32bitindexes = (ibuf->getType() == ::Ogre::HardwareIndexBuffer::IT_32BIT);

                        const ::Ogre::uint32* const pLong =
                            static_cast< const ::Ogre::uint32* >(ibuf->lock(::Ogre::HardwareBuffer::HBL_READ_ONLY));
                        const unsigned short* const pShort = reinterpret_cast< const unsigned short* >(pLong);

                        indices.resize(indexData->indexCount);

                        // Iterate over index buffer.
                        if(use32bitindexes)
                        {
                            for(size_t k = 0; k < indexData->indexCount; ++k)
                            {
                                indices[k] = pLong[k];
                            }
                        }
                        else
                        {
                            for(size_t k = 0; k < indexData->indexCount; ++k)
                            {
                                indices[k] = static_cast< Ogre::uint32 >(pShort[k]);
                            }
                        }

                        ibuf->unlock();
                    }
                }

                // Retrieve the material to check the culling mode.
                const ::Ogre::MaterialPtr material = ::Ogre::MaterialManager::getSingletonPtr()->getByName(
                    submesh->getMaterialName());
                bool positiveSide = true;
                bool negativeSide = true;
                if(material != nullptr)
                {
                    positiveSide = false;
                    negativeSide = false;
                    const auto* tech = material->getBestTechnique();
                    SLM_ASSERT("'" + material->getName() + "' does not have any suitable technique.", tech);
                    const auto& passes = tech->getPasses();
                    SLM_ASSERT(
                        "'" + material->getName() + "' technique '" + tech->getName() + "' does not define any passes.",
                        passes.size() > 0);
                    for(const ::Ogre::Pass* pass : passes)
                    {
                        const ::Ogre::CullingMode cullingMode = pass->getCullingMode();
                        switch (cullingMode)
                        {
                            case ::Ogre::CULL_NONE:
                                positiveSide = true;
                                negativeSide = true;
                                break;
                            case ::Ogre::CULL_CLOCKWISE:
                                positiveSide = true;
                                break;
                            case ::Ogre::CULL_ANTICLOCKWISE:
                                negativeSide = true;
                                break;
                            default:
                                SLM_ERROR("Unsuported culling mode");
                                break;
                        }
                    }
                }

                // Check intersections depending on the render operation type.
                bool newClosestFound = false;
                switch(submesh->operationType)
                {
                    // Points list is used for billboard.
                    case ::Ogre::RenderOperation::OT_POINT_LIST:
                    {
                        // We do not have faces, we simply check all vertices and compute the distance from the picked
                        // point
                        const ::Ogre::Camera* const camera = mSceneMgr->getCamera(
                            ::fwRenderOgre::Layer::DEFAULT_CAMERA_NAME);
                        const ::Ogre::Matrix4 viewMatrix = camera->getViewMatrix();
                        const ::Ogre::Matrix4 projMatrix = camera->getProjectionMatrixWithRSDepth();

                        const ::Ogre::Matrix4 viewProjMatrix = projMatrix * viewMatrix;

                        const ::Ogre::Vector3 resPointWVP = viewProjMatrix * _ray.getPoint(queryResult[qrIdx].distance);
                        const ::Ogre::Vector2 resPointSS  = (resPointWVP.xy() / 2.f) + 0.5f;

                        static const ::Ogre::Real s_TOLERANCE = 0.02f;

                        if(indices.size() > 0)
                        {
                            for (size_t i = 0; i < indices.size(); ++i)
                            {
                                const ::Ogre::Vector3 pointWVP = viewProjMatrix * vertices[indices[i]];
                                const ::Ogre::Vector2 pointSS  = (pointWVP.xy() / 2.f) + 0.5f;

                                if (((closestDistance < 0.0f) || (queryResult[qrIdx].distance < closestDistance)) &&
                                    resPointSS.distance(pointSS) < s_TOLERANCE)
                                {
                                    newClosestFound = true;
                                    closestDistance = vertices[indices[i]].distance(_ray.getOrigin());
                                }
                            }
                        }
                        else
                        {
                            for (size_t i = 0; i < vertices.size(); ++i)
                            {
                                const ::Ogre::Vector3 pointWVP = viewProjMatrix * vertices[i];
                                const ::Ogre::Vector2 pointSS  = (pointWVP.xy() / 2.f) + 0.5f;

                                if (((closestDistance < 0.0f) || (queryResult[qrIdx].distance < closestDistance)) &&
                                    resPointSS.distance(pointSS) < s_TOLERANCE)
                                {
                                    newClosestFound = true;
                                    closestDistance = vertices[i].distance(_ray.getOrigin());
                                }
                            }
                        }
                    }
                    break;
                    // Lines list is used to represent a quad.
                    case ::Ogre::RenderOperation::OT_LINE_LIST:
                        if(indices.size() > 0)
                        {
                            for(size_t i = 0; i < indices.size(); i += 4)
                            {
                                const auto firstHit = intersect(_ray, vertices[indices[i]],  vertices[indices[i+1]],
                                                                vertices[indices[i+2]], closestDistance, positiveSide,
                                                                negativeSide);
                                if(firstHit.first)
                                {
                                    newClosestFound = true;
                                    closestDistance = firstHit.second;
                                }
                                const auto secondHit = intersect(_ray, vertices[indices[i+2]],  vertices[indices[i+3]],
                                                                 vertices[indices[i]], closestDistance, positiveSide,
                                                                 negativeSide);
                                if(secondHit.first)
                                {
                                    newClosestFound = true;
                                    closestDistance = secondHit.second;
                                }
                            }
                        }
                        else
                        {
                            for(size_t i = 0; i < vertices.size(); i += 4)
                            {
                                const auto firstHit = intersect(_ray, vertices[i],  vertices[i+1], vertices[i+2],
                                                                closestDistance, positiveSide, negativeSide);
                                if(firstHit.first)
                                {
                                    newClosestFound = true;
                                    closestDistance = firstHit.second;
                                }
                                const auto secondHit = intersect(_ray, vertices[i+2],  vertices[i+3], vertices[i],
                                                                 closestDistance, positiveSide, negativeSide);
                                if(secondHit.first)
                                {
                                    newClosestFound = true;
                                    closestDistance = secondHit.second;
                                }
                            }
                        }
                        break;
                    // Triangles list is simply a list of triangles.
                    case ::Ogre::RenderOperation::OT_TRIANGLE_LIST:
                        if(indices.size() > 0)
                        {
                            for(size_t i = 0; i < indices.size(); i += 3)
                            {
                                const auto firstHit = intersect(_ray, vertices[indices[i]],  vertices[indices[i+1]],
                                                                vertices[indices[i+2]], closestDistance, positiveSide,
                                                                negativeSide);
                                if(firstHit.first)
                                {
                                    newClosestFound = true;
                                    closestDistance = firstHit.second;
                                }
                            }
                        }
                        else
                        {
                            for(size_t i = 0; i < vertices.size(); i += 3)
                            {
                                const auto firstHit = intersect(_ray, vertices[i],  vertices[i+1], vertices[i+2],
                                                                closestDistance, positiveSide, negativeSide);
                                if(firstHit.first)
                                {
                                    newClosestFound = true;
                                    closestDistance = firstHit.second;
                                }
                            }
                        }
                        break;
                    default:
                        SLM_ERROR("Unsuported operation type");
                        break;
                }

                if(newClosestFound)
                {
                    target        = pentity;
                    closestResult = _ray.getPoint(closestDistance);
                }
            }
        }
    }

    const bool collisionSuccess = (closestDistance >= 0.0f);
    return std::make_tuple(collisionSuccess, closestResult, target, closestDistance);
}

//------------------------------------------------------------------------------

std::pair<bool, float> CollisionTools::intersect(const ::Ogre::Ray& _ray,
                                                 const ::Ogre::Vector3& _a,
                                                 const ::Ogre::Vector3& _b,
                                                 const ::Ogre::Vector3& _c,
                                                 const float _closestDistance,
                                                 bool _positiveSide,
                                                 bool _negativeSide)
{
    const std::pair< bool, Ogre::Real > hit = Ogre::Math::intersects(_ray, _a, _b, _c,  _positiveSide, _negativeSide);
    if(hit.first)
    {
        if((_closestDistance < 0.0f) || (hit.second < _closestDistance))
        {
            return std::pair(true, hit.second);
        }
    }
    return std::pair(false, hit.second);
}

//------------------------------------------------------------------------------

}
