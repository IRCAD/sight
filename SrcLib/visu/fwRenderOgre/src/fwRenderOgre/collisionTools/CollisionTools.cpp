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

#include "fwRenderOgre/collisionTools/CollisionTools.hpp"

#include "fwRenderOgre/factory/R2VBRenderable.hpp"
#include "fwRenderOgre/Layer.hpp"
#include "fwRenderOgre/R2VBRenderable.hpp"

namespace fwRenderOgre
{

CollisionTools::CollisionTools(::Ogre::SceneManager* sceneMgr)
{
    mSceneMgr = sceneMgr;

    mRaySceneQuery = mSceneMgr->createRayQuery(::Ogre::Ray());
    if (nullptr == mRaySceneQuery)
    {
        // LOG_ERROR << "Failed to create Ogre::RaySceneQuery instance" << ENDLOG;
        return;
    }
    mRaySceneQuery->setSortByDistance(true);

    mTSMRaySceneQuery = mSceneMgr->createRayQuery(::Ogre::Ray());
}

CollisionTools::~CollisionTools()
{
    if (mRaySceneQuery != nullptr)
    {
        delete mRaySceneQuery;
    }

    if (mTSMRaySceneQuery != nullptr)
    {
        delete mTSMRaySceneQuery;
    }
}

//------------------------------------------------------------------------------

bool CollisionTools::collidesWithEntity(const Ogre::Vector3& fromPoint, const Ogre::Vector3& toPoint,
                                        const float collisionRadius, const float rayHeightLevel,
                                        const Ogre::uint32 queryMask)
{
    Ogre::Vector3 fromPointAdj(fromPoint.x, fromPoint.y + rayHeightLevel, fromPoint.z);
    Ogre::Vector3 toPointAdj(toPoint.x, toPoint.y + rayHeightLevel, toPoint.z);
    Ogre::Vector3 normal = toPointAdj - fromPointAdj;
    float distToDest     = normal.normalise();
    float distToColl     = 0.0f;

    std::tuple<bool, Ogre::Vector3, Ogre::MovableObject*,
               float> res = raycastFromPoint(fromPointAdj, normal, queryMask);

    if (std::get<0>(res))
    {
        distToColl  = std::get<3>(res);
        distToColl -= collisionRadius;
        return (distToColl <= distToDest);
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------

std::tuple<bool, Ogre::Vector3, Ogre::MovableObject*, float> CollisionTools::raycastFromCamera(::Ogre::RenderWindow* rw,
                                                                                               Ogre::Camera* camera,
                                                                                               const Ogre::Vector2& mousecoords,
                                                                                               const Ogre::uint32 queryMask)
{
    // Create the ray to test
    Ogre::Real tx = mousecoords.x / static_cast< ::Ogre::Real>(rw->getWidth());
    Ogre::Real ty = mousecoords.y / static_cast< ::Ogre::Real>(rw->getHeight());
    Ogre::Ray ray = camera->getCameraToViewportRay(tx, ty);

    return raycast(ray, queryMask);
}

//------------------------------------------------------------------------------

std::tuple<bool, Ogre::Vector3, Ogre::MovableObject*, float> CollisionTools::raycastFromPoint(
    const Ogre::Vector3& point, const Ogre::Vector3& normal, const Ogre::uint32 queryMask)
{
    // create the ray to test
    static Ogre::Ray ray;
    ray.setOrigin(point);
    ray.setDirection(normal);

    return raycast(ray, queryMask);
}

//------------------------------------------------------------------------------

std::tuple<bool, Ogre::Vector3, Ogre::MovableObject*, float> CollisionTools::raycast(const Ogre::Ray& ray,
                                                                                     const Ogre::uint32 queryMask)
{
    Ogre::Vector3 result;
    Ogre::MovableObject* target = nullptr;
    float closestDistance;

    // check we are initialised
    if (mRaySceneQuery != nullptr)
    {
        // create a query object
        mRaySceneQuery->setRay(ray);
        mRaySceneQuery->setSortByDistance(true);
        mRaySceneQuery->setQueryMask(queryMask);
        // execute the query, returns a vector of hits
        if (mRaySceneQuery->execute().size() <= 0)
        {
            // raycast did not hit an objects bounding box
            return std::make_tuple(false, result, target, closestDistance);
        }
    }
    else
    {
        //LOG_ERROR << "Cannot raycast without RaySceneQuery instance" << ENDLOG;
        return std::make_tuple(false, result, target, closestDistance);
    }

    // at this point we have raycast to a series of different objects bounding boxes.
    // we need to test these different objects to see which is the first polygon hit.
    // there are some minor optimizations (distance based) that mean we wont have to
    // check all of the objects most of the time, but the worst case scenario is that
    // we need to test every triangle of every object.
    //Ogre::Ogre::Real closestDistance = -1.0f;
    closestDistance = -1.0f;
    Ogre::Vector3 closestResult;
    Ogre::RaySceneQueryResult& queryResult = mRaySceneQuery->getLastResults();
    for (size_t qrIdx = 0; qrIdx < queryResult.size(); qrIdx++)
    {
        // stop checking if we have found a raycast hit that is closer
        // than all remaining entities
        if ((closestDistance >= 0.0f) &&
            (closestDistance < queryResult[qrIdx].distance))
        {
            break;
        }

        // get the entity to check
        Ogre::MovableObject* pentity = queryResult[qrIdx].movable;

        const bool isEntity = queryResult[qrIdx].movable->getMovableType().compare("Entity") == 0;
        const bool isR2VB   = queryResult[qrIdx].movable->getMovableType().compare(
            factory::R2VBRenderable::FACTORY_TYPE_NAME) == 0;

        // only check this result if its a hit against an entity
        if ((pentity != nullptr) && (isEntity || isR2VB) )
        {
            if(!pentity->isVisible())
            {
                continue;
            }

            auto mesh = isEntity ?
                        static_cast< ::Ogre::Entity*>(pentity)->getMesh() :
                        static_cast< ::fwRenderOgre::R2VBRenderable*>(pentity)->getMesh();

            // mesh data to retrieve
            std::vector< ::Ogre::Vector3> vertices;
            std::vector< ::Ogre::uint32> indicesTri;
            std::vector< ::Ogre::uint32> indicesQuad;

            // get the mesh information
            std::tie( vertices, indicesTri, indicesQuad) =
                GetMeshInformation(mesh,
                                   pentity->getParentNode()->_getDerivedPosition(),
                                   pentity->getParentNode()->_getDerivedOrientation(),
                                   pentity->getParentNode()->_getDerivedScale());

            // test for hitting individual triangles on the mesh
            bool newClosestFound = false;
            if(indicesQuad.size())
            {
                for (size_t i = 0; i < indicesQuad.size(); i += 4)
                {
                    // check for a hit against this triangle
                    std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(ray, vertices[indicesQuad[i]],
                                                                             vertices[indicesQuad[i+1]],
                                                                             vertices[indicesQuad[i+3]], true, false);

                    // if it was a hit check if its the closest
                    if (hit.first)
                    {
                        if ((closestDistance < 0.0f) || (hit.second < closestDistance))
                        {
                            // this is the closest so far, save it off
                            closestDistance = hit.second;
                            newClosestFound = true;
                        }
                    }

                    // check for a hit against this triangle
                    hit = Ogre::Math::intersects(ray, vertices[indicesQuad[i+1]],
                                                 vertices[indicesQuad[i+2]],
                                                 vertices[indicesQuad[i+3]], true, false);

                    // if it was a hit check if its the closest
                    if (hit.first)
                    {
                        if ((closestDistance < 0.0f) || (hit.second < closestDistance))
                        {
                            // this is the closest so far, save it off
                            closestDistance = hit.second;
                            newClosestFound = true;
                        }
                    }
                }
            }
            else if(indicesTri.size())
            {
                for (size_t i = 0; i < indicesTri.size(); i += 3)
                {
                    // check for a hit against this triangle
                    std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(ray, vertices[indicesTri[i]],
                                                                             vertices[indicesTri[i+1]],
                                                                             vertices[indicesTri[i+2]], true, false);

                    // if it was a hit check if its the closest
                    if (hit.first)
                    {
                        if ((closestDistance < 0.0f) || (hit.second < closestDistance))
                        {
                            // this is the closest so far, save it off
                            closestDistance = hit.second;
                            newClosestFound = true;
                        }
                    }
                }
            }
            else
            {
                // We do not have faces, we simply check all vertices and compute the distance from the picked point
                const auto* camera               = mSceneMgr->getCamera(::fwRenderOgre::Layer::DEFAULT_CAMERA_NAME);
                const ::Ogre::Matrix4 viewMatrix = camera->getViewMatrix();
                const ::Ogre::Matrix4 projMatrix = camera->getProjectionMatrixWithRSDepth();

                const ::Ogre::Matrix4 viewProjMatrix = projMatrix * viewMatrix;

                const ::Ogre::Vector3 resPointWVP = viewProjMatrix * ray.getPoint(queryResult[qrIdx].distance);
                const ::Ogre::Vector2 resPointSS  = (resPointWVP.xy() / 2.f) + 0.5f;

                for (size_t i = 0; i < vertices.size(); ++i)
                {
                    const ::Ogre::Vector3 pointWVP = viewProjMatrix * vertices[i];
                    const ::Ogre::Vector2 pointSS  = (pointWVP.xy() / 2.f) + 0.5f;

                    static const ::Ogre::Real s_TOLERANCE = 0.02f;
                    if (((closestDistance < 0.0f) || (queryResult[qrIdx].distance < closestDistance)) &&
                        resPointSS.distance(pointSS) < s_TOLERANCE)
                    {
                        newClosestFound = true;
                        closestDistance = vertices[i].distance(ray.getOrigin());
                    }
                }
            }

            // if we found a new closest raycast for this object, update the
            // closest_result before moving on to the next object.
            if (newClosestFound)
            {
                target        = pentity;
                closestResult = ray.getPoint(closestDistance);
            }
        }
    }

    // return the result
    if (closestDistance >= 0.0f)
    {
        // raycast success
        result = closestResult;
        return std::make_tuple(true, result, target, closestDistance);
    }
    else
    {
        // raycast failed
        return std::make_tuple(false, result, target, closestDistance);
    }
}

// Get the mesh information for the given mesh.
// Code found on this forum link: http://www.ogre3d.org/wiki/index.php/RetrieveVertexData
std::tuple< std::vector< Ogre::Vector3>,
            std::vector< Ogre::uint32>,
            std::vector< Ogre::uint32> >
CollisionTools::GetMeshInformation(const Ogre::MeshPtr mesh,
                                   const Ogre::Vector3& position,
                                   const Ogre::Quaternion& orient,
                                   const Ogre::Vector3& scale)
{
    bool addedShared     = false;
    size_t currentOffset = 0;
    size_t sharedOffset  = 0;
    size_t nextOffset    = 0;

    size_t vertexCount     = 0;
    size_t indexTriCount   = 0;
    size_t indexTriOffset  = 0;
    size_t indexQuadCount  = 0;
    size_t indexQuadOffset = 0;

    // Calculate how many vertices and indices we're going to need
    for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        const Ogre::SubMesh* submesh = mesh->getSubMesh( i );

        // We only need to add the shared vertices once
        if(submesh->useSharedVertices)
        {
            if( !addedShared )
            {
                vertexCount += mesh->sharedVertexData->vertexCount;
                addedShared  = true;
            }
        }
        else
        {
            vertexCount += submesh->vertexData->vertexCount;
        }

        // Add the indices
        if(submesh->operationType == ::Ogre::RenderOperation::OT_TRIANGLE_LIST)
        {
            indexTriCount += submesh->indexData->indexCount;
        }
        else if(submesh->operationType == ::Ogre::RenderOperation::OT_LINE_LIST)
        {
            indexQuadCount += submesh->indexData->indexCount;
        }
    }

    // Allocate space for the vertices and indices
    std::vector< ::Ogre::Vector3> vertices;
    vertices.resize(vertexCount);
    std::vector< ::Ogre::uint32> indicesTri;
    indicesTri.resize(indexTriCount);
    std::vector< ::Ogre::uint32> indicesQuad;
    indicesQuad.resize(indexQuadCount);

    addedShared = false;

    // Run through the submeshes again, adding the data into the arrays
    for ( unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        const Ogre::SubMesh* submesh = mesh->getSubMesh(i);

        const Ogre::VertexData* vertexData = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;

        if((!submesh->useSharedVertices)||(submesh->useSharedVertices && !addedShared))
        {
            if(submesh->useSharedVertices)
            {
                addedShared  = true;
                sharedOffset = currentOffset;
            }

            const Ogre::VertexElement* posElem =
                vertexData->vertexDeclaration->findElementBySemantic(::Ogre::VES_POSITION);

            const Ogre::HardwareVertexBufferSharedPtr vbuf =
                vertexData->vertexBufferBinding->getBuffer(posElem->getSource());

            unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(::Ogre::HardwareBuffer::HBL_READ_ONLY));

            // There is _no_ baseVertexPointerToElement() which takes an Ogre::Ogre::Real or a double
            //  as second argument. So make it float, to avoid trouble when Ogre::Ogre::Real will
            //  be compiled/typedefed as double:
            //      Ogre::Ogre::Real* pOgre::Real;
            float* pReal;

            for( size_t j = 0; j < vertexData->vertexCount; ++j, vertex += vbuf->getVertexSize())
            {
                posElem->baseVertexPointerToElement(vertex, &pReal);

                Ogre::Vector3 pt(pReal[0], pReal[1], pReal[2]);

                vertices[currentOffset + j] = (orient * (pt * scale)) + position;
            }

            vbuf->unlock();
            nextOffset += vertexData->vertexCount;
        }

        const Ogre::IndexData* indexData              = submesh->indexData;
        const Ogre::HardwareIndexBufferSharedPtr ibuf = indexData->indexBuffer;

        /*********************** FIX LUIS ***********************/

        // When a mesh is picked, a billboard is created (for printing purposes), this line ensure theses billboars are
        // not picked by the same picker as they do not define an entity.
        if(indexData->indexCount == 0)
        {
            return std::tuple<std::vector< Ogre::Vector3>,
                              std::vector< Ogre::uint32>,
                              std::vector< Ogre::uint32> >(vertices, indicesTri, indicesQuad);
        }

        /*********************** END FIX LUIS ***********************/

        bool use32bitindexes = (ibuf->getType() == ::Ogre::HardwareIndexBuffer::IT_32BIT);

        auto pLong  = static_cast< const ::Ogre::uint32*>(ibuf->lock(::Ogre::HardwareBuffer::HBL_READ_ONLY));
        auto pShort = reinterpret_cast<const unsigned short*>(pLong);

        size_t offset = (submesh->useSharedVertices) ? sharedOffset : currentOffset;

        std::vector< ::Ogre::uint32>* indices;
        size_t* indexOffset;

        if(submesh->operationType == ::Ogre::RenderOperation::OT_TRIANGLE_LIST)
        {
            indexOffset = &indexTriOffset;
            indices     = &indicesTri;
        }
        else
        {
            indexOffset = &indexQuadOffset;
            indices     = &indicesQuad;

        }

        if ( use32bitindexes )
        {
            for ( size_t k = 0; k < indexData->indexCount; ++k)
            {
                (*indices)[(*indexOffset)++] = pLong[k] + static_cast<Ogre::uint32>(offset);
            }
        }
        else
        {
            for ( size_t k = 0; k < indexData->indexCount; ++k)
            {
                (*indices)[(*indexOffset)++] = static_cast<Ogre::uint32>(pShort[k]) + static_cast<Ogre::uint32>(offset);
            }
        }

        ibuf->unlock();
        currentOffset = nextOffset;
    }
    return std::tuple < std::vector< Ogre::Vector3>,
                        std::vector< Ogre::uint32>,
                        std::vector< Ogre::uint32> >(vertices, indicesTri, indicesQuad);
}

//------------------------------------------------------------------------------

}
