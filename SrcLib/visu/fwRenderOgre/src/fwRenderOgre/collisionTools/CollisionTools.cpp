/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
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

namespace fwRenderOgre
{

#ifdef ETM_TERRAIN
CollisionTools::CollisionTools(::Ogre::SceneManager* sceneMgr, const ET::TerrainInfo* terrainInfo)
{
    mRaySceneQuery = sceneMgr->createRayQuery(::Ogre::Ray());
    if (NULL == mRaySceneQuery)
    {
        // LOG_ERROR << "Failed to create Ogre::RaySceneQuery instance" << ENDLOG;
        return;
    }
    mRaySceneQuery->setSortByDistance(true);

    mTSMRaySceneQuery = NULL;

    mTerrainInfo = terrainInfo;

    _heightAdjust = 0.0f;
}
#endif

CollisionTools::CollisionTools(::Ogre::SceneManager* sceneMgr)
{
    mSceneMgr = sceneMgr;

    mRaySceneQuery = mSceneMgr->createRayQuery(::Ogre::Ray());
    if (NULL == mRaySceneQuery)
    {
        // LOG_ERROR << "Failed to create Ogre::RaySceneQuery instance" << ENDLOG;
        return;
    }
    mRaySceneQuery->setSortByDistance(true);

    mTSMRaySceneQuery = mSceneMgr->createRayQuery(::Ogre::Ray());

    _heightAdjust = 0.0f;
}

CollisionTools::~CollisionTools()
{
    if (mRaySceneQuery != NULL)
    {
        delete mRaySceneQuery;
    }

    if (mTSMRaySceneQuery != NULL)
    {
        delete mTSMRaySceneQuery;
    }
}

//------------------------------------------------------------------------------

bool CollisionTools::raycastFromCamera(::Ogre::RenderWindow* rw, Ogre::Camera* camera, const Ogre::Vector2& mousecoords,
                                       Ogre::Vector3& result, Ogre::Entity*& target, float& closest_distance,
                                       const Ogre::uint32 queryMask)
{
    return raycastFromCamera(rw, camera, mousecoords, result, (::Ogre::MovableObject*&)target, closest_distance,
                             queryMask);
}

//------------------------------------------------------------------------------

bool CollisionTools::raycastFromCamera(::Ogre::RenderWindow* rw, Ogre::Camera* camera, const Ogre::Vector2& mousecoords,
                                       Ogre::Vector3& result, Ogre::MovableObject*& target, float& closest_distance,
                                       const Ogre::uint32 queryMask)
{
    // Create the ray to test
    Ogre::Real tx = mousecoords.x / (::Ogre::Real) rw->getWidth();
    Ogre::Real ty = mousecoords.y / (::Ogre::Real) rw->getHeight();
    Ogre::Ray ray = camera->getCameraToViewportRay(tx, ty);

    return raycast(ray, result, target, closest_distance, queryMask);
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

    Ogre::Vector3 myResult(0, 0, 0);
    Ogre::MovableObject* myObject = NULL;
    float distToColl              = 0.0f;

    if (raycastFromPoint(fromPointAdj, normal, myResult, myObject, distToColl, queryMask))
    {
        distToColl -= collisionRadius;
        return (distToColl <= distToDest);
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------

float CollisionTools::getTSMHeightAt(const float x, const float z)
{
    float y = 0.0f;

    static Ogre::Ray updateRay;

    updateRay.setOrigin(::Ogre::Vector3(x, 9999, z));
    updateRay.setDirection(::Ogre::Vector3::NEGATIVE_UNIT_Y);

    mTSMRaySceneQuery->setRay(updateRay);
    Ogre::RaySceneQueryResult& qryResult = mTSMRaySceneQuery->execute();

    Ogre::RaySceneQueryResult::iterator i = qryResult.begin();
    if (i != qryResult.end() && i->worldFragment)
    {
        y = i->worldFragment->singleIntersection.y;
    }
    return y;
}

//------------------------------------------------------------------------------

void CollisionTools::calculateY(::Ogre::SceneNode* n, const bool doTerrainCheck, const bool doGridCheck,
                                const float gridWidth,
                                const Ogre::uint32 queryMask)
{
    Ogre::Vector3 pos = n->getPosition();

    float x = pos.x;
    float z = pos.z;
    float y = pos.y;

    Ogre::Vector3 myResult(0, 0, 0);
    Ogre::MovableObject* myObject = NULL;
    float distToColl              = 0.0f;

    float terrY = 0, colY = 0, colY2 = 0;

    if( raycastFromPoint(::Ogre::Vector3(x, y, z), Ogre::Vector3::NEGATIVE_UNIT_Y, myResult, myObject, distToColl,
                         queryMask))
    {
        if (myObject != NULL)
        {
            colY = myResult.y;
        }
        else
        {
            colY = -99999;
        }
    }

    //if doGridCheck is on, repeat not to fall through small holes for example when crossing a hangbridge
    if (doGridCheck)
    {
        if( raycastFromPoint(::Ogre::Vector3(x, y,
                                             z)+
                             (n->getOrientation()*
                              Ogre::Vector3(0, 0,
                                            gridWidth)), Ogre::Vector3::NEGATIVE_UNIT_Y, myResult, myObject, distToColl,
                             queryMask))
        {
            if (myObject != NULL)
            {
                colY = myResult.y;
            }
            else
            {
                colY = -99999;
            }
        }
        if (colY < colY2)
        {
            colY = colY2;
        }
    }

    // set the parameter to false if you are not using ETM or TSM
    if (doTerrainCheck)
    {

#ifdef ETM_TERRAIN
        // ETM height value
        terrY = mTerrainInfo->getHeightAt(x, z);
#else
        // TSM height value
        terrY = getTSMHeightAt(x, z);
#endif

        if(terrY < colY )
        {
            n->setPosition(x, colY+_heightAdjust, z);
        }
        else
        {
            n->setPosition(x, terrY+_heightAdjust, z);
        }
    }
    else
    {
        if (!doTerrainCheck && colY == -99999)
        {
            colY = y;
        }
        n->setPosition(x, colY+_heightAdjust, z);
    }
}

// raycast from a point in to the scene.
// returns success or failure.
// on success the point is returned in the result.
bool CollisionTools::raycastFromPoint(const Ogre::Vector3& point,
                                      const Ogre::Vector3& normal,
                                      Ogre::Vector3& result, Ogre::Entity*& target,
                                      float& closest_distance,
                                      const Ogre::uint32 queryMask)
{
    return raycastFromPoint(point, normal, result, (::Ogre::MovableObject*&)target, closest_distance, queryMask);
}

//------------------------------------------------------------------------------

bool CollisionTools::raycastFromPoint(const Ogre::Vector3& point,
                                      const Ogre::Vector3& normal,
                                      Ogre::Vector3& result, Ogre::MovableObject*& target,
                                      float& closest_distance,
                                      const Ogre::uint32 queryMask)
{
    // create the ray to test
    static Ogre::Ray ray;
    ray.setOrigin(point);
    ray.setDirection(normal);

    return raycast(ray, result, target, closest_distance, queryMask);
}

//------------------------------------------------------------------------------

bool CollisionTools::raycast(const Ogre::Ray& ray, Ogre::Vector3& result, Ogre::Entity*& target,
                             float& closest_distance,
                             const Ogre::uint32 queryMask)
{
    return raycast(ray, result, (::Ogre::MovableObject*&)target, closest_distance, queryMask);
}

//------------------------------------------------------------------------------

bool CollisionTools::raycast(const Ogre::Ray& ray, Ogre::Vector3& result, Ogre::MovableObject*& target,
                             float& closest_distance,
                             const Ogre::uint32 queryMask)
{
    target = NULL;

    // check we are initialised
    if (mRaySceneQuery != NULL)
    {
        // create a query object
        mRaySceneQuery->setRay(ray);
        mRaySceneQuery->setSortByDistance(true);
        mRaySceneQuery->setQueryMask(queryMask);
        // execute the query, returns a vector of hits
        if (mRaySceneQuery->execute().size() <= 0)
        {
            // raycast did not hit an objects bounding box
            return (false);
        }
    }
    else
    {
        //LOG_ERROR << "Cannot raycast without RaySceneQuery instance" << ENDLOG;
        return (false);
    }

    // at this point we have raycast to a series of different objects bounding boxes.
    // we need to test these different objects to see which is the first polygon hit.
    // there are some minor optimizations (distance based) that mean we wont have to
    // check all of the objects most of the time, but the worst case scenario is that
    // we need to test every triangle of every object.
    //Ogre::Ogre::Real closest_distance = -1.0f;
    closest_distance = -1.0f;
    Ogre::Vector3 closest_result;
    Ogre::RaySceneQueryResult& query_result = mRaySceneQuery->getLastResults();
    for (size_t qr_idx = 0; qr_idx < query_result.size(); qr_idx++)
    {
        // stop checking if we have found a raycast hit that is closer
        // than all remaining entities
        if ((closest_distance >= 0.0f) &&
            (closest_distance < query_result[qr_idx].distance))
        {
            break;
        }

        // only check this result if its a hit against an entity
        if ((query_result[qr_idx].movable != NULL)  &&
            (query_result[qr_idx].movable->getMovableType().compare("Entity") == 0))
        {
            // get the entity to check
            Ogre::MovableObject* pentity = static_cast<Ogre::MovableObject*>(query_result[qr_idx].movable);

            // mesh data to retrieve
            size_t vertex_count;
            size_t index_count;
            Ogre::Vector3* vertices;
            Ogre::uint32* indices;

            // get the mesh information
            GetMeshInformation(
                (static_cast<Ogre::Entity*>(pentity))->getMesh(), vertex_count, vertices, index_count, indices,
                pentity->getParentNode()->_getDerivedPosition(),
                pentity->getParentNode()->_getDerivedOrientation(),
                pentity->getParentNode()->_getDerivedScale());

            // test for hitting individual triangles on the mesh
            bool new_closest_found = false;
            for (size_t i = 0; i < index_count; i += 3)
            {
                // check for a hit against this triangle
                std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(ray, vertices[indices[i]],
                                                                         vertices[indices[i+1]], vertices[indices[i+2]],
                                                                         true,
                                                                         false);

                // if it was a hit check if its the closest
                if (hit.first)
                {
                    if ((closest_distance < 0.0f) ||
                        (hit.second < closest_distance))
                    {
                        // this is the closest so far, save it off
                        closest_distance  = hit.second;
                        new_closest_found = true;
                    }
                }
            }

            // free the verticies and indicies memory
            delete[] vertices;
            delete[] indices;

            // if we found a new closest raycast for this object, update the
            // closest_result before moving on to the next object.
            if (new_closest_found)
            {
                target         = pentity;
                closest_result = ray.getPoint(closest_distance);
            }
        }
    }

    // return the result
    if (closest_distance >= 0.0f)
    {
        // raycast success
        result = closest_result;
        return (true);
    }
    else
    {
        // raycast failed
        return (false);
    }
}

// Get the mesh information for the given mesh.
// Code found on this forum link: http://www.ogre3d.org/wiki/index.php/RetrieveVertexData
void CollisionTools::GetMeshInformation(const Ogre::MeshPtr mesh,
                                        size_t& vertex_count,
                                        Ogre::Vector3*& vertices,
                                        size_t& index_count,
                                        Ogre::uint32*& indices,
                                        const Ogre::Vector3& position,
                                        const Ogre::Quaternion& orient,
                                        const Ogre::Vector3& scale)
{
    bool added_shared     = false;
    size_t current_offset = 0;
    size_t shared_offset  = 0;
    size_t next_offset    = 0;
    size_t index_offset   = 0;

    vertex_count = index_count = 0;

    // Calculate how many vertices and indices we're going to need
    for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        Ogre::SubMesh* submesh = mesh->getSubMesh( i );

        // We only need to add the shared vertices once
        if(submesh->useSharedVertices)
        {
            if( !added_shared )
            {
                vertex_count += mesh->sharedVertexData->vertexCount;
                added_shared  = true;
            }
        }
        else
        {
            vertex_count += submesh->vertexData->vertexCount;
        }

        // Add the indices
        index_count += submesh->indexData->indexCount;
    }

    // Allocate space for the vertices and indices
    vertices = new Ogre::Vector3[vertex_count];
    indices  = new Ogre::uint32[index_count];

    added_shared = false;

    // Run through the submeshes again, adding the data into the arrays
    for ( unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        Ogre::SubMesh* submesh = mesh->getSubMesh(i);

        Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;

        if((!submesh->useSharedVertices)||(submesh->useSharedVertices && !added_shared))
        {
            if(submesh->useSharedVertices)
            {
                added_shared  = true;
                shared_offset = current_offset;
            }

            const Ogre::VertexElement* posElem =
                vertex_data->vertexDeclaration->findElementBySemantic(::Ogre::VES_POSITION);

            Ogre::HardwareVertexBufferSharedPtr vbuf =
                vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());

            unsigned char* vertex =
                static_cast<unsigned char*>(vbuf->lock(::Ogre::HardwareBuffer::HBL_READ_ONLY));

            // There is _no_ baseVertexPointerToElement() which takes an Ogre::Ogre::Real or a double
            //  as second argument. So make it float, to avoid trouble when Ogre::Ogre::Real will
            //  be comiled/typedefed as double:
            //      Ogre::Ogre::Real* pOgre::Real;
            float* pReal;

            for( size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
            {
                posElem->baseVertexPointerToElement(vertex, &pReal);

                Ogre::Vector3 pt(pReal[0], pReal[1], pReal[2]);

                vertices[current_offset + j] = (orient * (pt * scale)) + position;
            }

            vbuf->unlock();
            next_offset += vertex_data->vertexCount;
        }

        Ogre::IndexData* index_data             = submesh->indexData;
        size_t numTris                          = index_data->indexCount / 3;
        Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;

        /*********************** FIX LUIS ***********************/

        // When a mesh i picked, a billboard is created (for printing purposes), this line ensure theses billboars are
        // not picked by the same picker as they do not define an entity.
        if(index_data->indexCount == 0)
        {
            return;
        }

        /*********************** END FIX LUIS ***********************/

        bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

        Ogre::uint32*  pLong   = static_cast<Ogre::uint32*>(ibuf->lock(::Ogre::HardwareBuffer::HBL_READ_ONLY));
        unsigned short* pShort = reinterpret_cast<unsigned short*>(pLong);

        size_t offset = (submesh->useSharedVertices) ? shared_offset : current_offset;

        if ( use32bitindexes )
        {
            for ( size_t k = 0; k < numTris*3; ++k)
            {
                indices[index_offset++] = pLong[k] + static_cast<Ogre::uint32>(offset);
            }
        }
        else
        {
            for ( size_t k = 0; k < numTris*3; ++k)
            {
                indices[index_offset++] = static_cast<Ogre::uint32>(pShort[k]) +
                                          static_cast<Ogre::uint32>(offset);
            }
        }

        ibuf->unlock();
        current_offset = next_offset;
    }
}

//------------------------------------------------------------------------------

void CollisionTools::setHeightAdjust(const float heightadjust)
{
    _heightAdjust = heightadjust;
}

//------------------------------------------------------------------------------

float CollisionTools::getHeightAdjust(void)
{
    return _heightAdjust;
}

}
