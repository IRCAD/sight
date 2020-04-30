/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "imageFilterOp/ImageExtruder.hpp"

#include <fwTools/Dispatcher.hpp>
#include <fwTools/TypeKeyTypeMapping.hpp>

#include <glm/gtx/intersect.hpp>
#include <glm/vec2.hpp>

namespace imageFilterOp
{

//------------------------------------------------------------------------------

void ImageExtruder::extrude(const ::fwData::Image::sptr& _image, const ::fwData::Mesh::csptr& _mesh)
{
    SLM_ASSERT("The image must be in 3 dimensions", _image->getNumberOfDimensions() == 3);

    Parameters param;
    param.m_image = _image;
    param.m_mesh  = _mesh;

    // We use a dispatcher because we can't retrieve the image type without a DynamicType.
    ::fwTools::Type type = _image->getType();
    ::fwTools::Dispatcher< ::fwTools::SupportedDispatcherTypes, ImageExtruder >::invoke(type, param);
}

//------------------------------------------------------------------------------

template<typename IMAGE_TYPE>
void ImageExtruder::operator()(Parameters& _param)
{
    // Creates triangles and bounding box of the mesh.
    const auto itPoint   = _param.m_mesh->begin< ::fwData::iterator::ConstPointIterator >();
    const auto itCellEnd = _param.m_mesh->end< ::fwData::iterator::ConstCellIterator >();
    auto itCell          = _param.m_mesh->begin< ::fwData::iterator::ConstCellIterator >();
    const float min      = std::numeric_limits< float >::lowest();
    const float max      = std::numeric_limits< float >::max();

    std::list< Triangle > triangles;
    ::glm::vec3 minBound(max, max, max);
    ::glm::vec3 maxBound(min, min, min);

    const auto addTriangle =
        [&](const ::fwData::iterator::ConstPointIterator& _pa,
            const ::fwData::iterator::ConstPointIterator& _pb,
            const ::fwData::iterator::ConstPointIterator& _pc)
        {
            const float ax = _pa->point->x;
            const float ay = _pa->point->y;
            const float az = _pa->point->z;

            const float bx = _pb->point->x;
            const float by = _pb->point->y;
            const float bz = _pb->point->z;

            const float cx = _pc->point->x;
            const float cy = _pc->point->y;
            const float cz = _pc->point->z;

            const ::glm::vec3 triA(ax, ay, az);
            const ::glm::vec3 triB(bx, by, bz);
            const ::glm::vec3 triC(cx, cy, cz);

            triangles.push_back(Triangle{triA, triB, triC});

            minBound.x = std::min(std::min(std::min(minBound.x, ax), bx), cx);
            minBound.y = std::min(std::min(std::min(minBound.y, ay), by), cy);
            minBound.z = std::min(std::min(std::min(minBound.z, az), bz), cz);

            maxBound.x = std::max(std::max(std::max(maxBound.x, ax), bx), cx);
            maxBound.y = std::max(std::max(std::max(maxBound.y, ay), by), cy);
            maxBound.z = std::max(std::max(std::max(maxBound.z, az), bz), cz);
        };

    for (; itCell != itCellEnd; ++itCell)
    {
        if(itCell->nbPoints < 3)
        {
            SLM_FATAL("The extrusion works only with meshes of at least three points per cells");
        }
        else if(itCell->nbPoints == 3)
        {
            const auto pointA = ::fwData::iterator::ConstPointIterator(itPoint + itCell->pointIdx[0]);
            const auto pointB = ::fwData::iterator::ConstPointIterator(itPoint + itCell->pointIdx[1]);
            const auto pointC = ::fwData::iterator::ConstPointIterator(itPoint + itCell->pointIdx[2]);

            addTriangle(pointA, pointB, pointC);
        }
        else if(itCell->nbPoints == 4)
        {
            const auto pointA = ::fwData::iterator::ConstPointIterator(itPoint + itCell->pointIdx[0]);
            const auto pointB = ::fwData::iterator::ConstPointIterator(itPoint + itCell->pointIdx[1]);
            const auto pointC = ::fwData::iterator::ConstPointIterator(itPoint + itCell->pointIdx[2]);
            const auto pointD = ::fwData::iterator::ConstPointIterator(itPoint + itCell->pointIdx[3]);

            addTriangle(pointA, pointB, pointC);
            addTriangle(pointC, pointD, pointA);
        }
        else
        {
            SLM_FATAL("The extrusion works only with meshes of at most four points per cells");
        }
    }

    // Get images.
    const auto dumpLock = _param.m_image->lock();

    // Get image informations.
    const ::fwData::Image::Origin& origin   = _param.m_image->getOrigin2();
    const ::fwData::Image::Size& size       = _param.m_image->getSize2();
    const ::fwData::Image::Spacing& spacing = _param.m_image->getSpacing2();

    // Loop over the bounding box intersection of the mesh and the image to increase perfomance.
    std::int64_t indexXBeg = 0;
    if(origin[0] < minBound.x)
    {
        indexXBeg = static_cast< std::int64_t >((minBound.x - origin[0])/spacing[0]);
    }
    std::int64_t indexXEnd = static_cast< std::int64_t >(size[0]);
    if(origin[0] + indexXEnd*spacing[0] > maxBound.x)
    {
        indexXEnd = static_cast< std::int64_t >((maxBound.x - origin[0])/spacing[0]);
    }

    std::int64_t indexYBeg = 0;
    if(origin[1] < minBound.y)
    {
        indexYBeg = static_cast< std::int64_t >((minBound.y - origin[1])/spacing[1]);
    }
    std::int64_t indexYEnd = size[1];
    if(origin[1] + indexYEnd*spacing[1] > maxBound.y)
    {
        indexYEnd = static_cast< std::int64_t >((maxBound.y - origin[1])/spacing[1]);
    }

    std::int64_t indexZBeg = 0;
    if(origin[2] < minBound.z)
    {
        indexZBeg = static_cast< std::int64_t >((minBound.z - origin[2])/spacing[2]);
    }
    std::int64_t indexZEnd = size[2];
    if(origin[2] + indexZEnd*spacing[2] > maxBound.z)
    {
        indexZEnd = static_cast< std::int64_t >((maxBound.z - origin[2])/spacing[2]);
    }

    // Check if the ray origin is inside or outside of the mesh and return all found intersections.
    const auto getIntersections =
        [&](const ::glm::vec3& _rayOrig, const ::glm::vec3& _rayDir,
            std::vector< ::glm::vec3 >& _intersections) -> bool
        {
            bool inside = false;
            for(const Triangle& tri : triangles)
            {
                ::glm::vec2 pos;
                float distance;
                if(::glm::intersectRayTriangle(_rayOrig, _rayDir, tri.a, tri.b, tri.c, pos,
                                               distance))
                {
                    if(distance >= 0.f)
                    {
                        const ::glm::vec3 cross = _rayOrig + _rayDir*distance;
                        // Sometime, the ray it the edge of a triangle, we need to take it into account only one time.
                        if(std::find(_intersections.begin(), _intersections.end(), cross) == _intersections.end())
                        {
                            _intersections.push_back(cross);
                            inside = !inside;
                        }
                    }
                }
            }

            // Sort all intersections from nearest to farthest from the origin.
            std::sort(_intersections.begin(), _intersections.end(), [&](const ::glm::vec3& _a,
                                                                        const ::glm::vec3& _b)
            {
                return glm::distance(_rayOrig, _a) < glm::distance(_rayOrig, _b);
            });

            return inside;
        };

    // Check if each voxel are in the mesh and sets them to the lowest value.
    const IMAGE_TYPE emptyValue = std::numeric_limits< IMAGE_TYPE >::lowest();

    // We loop over two dimensions out of three, for each voxel, we launch a ray on the third dimension and get a
    // list of intersections. After that, we iterate over the voxel line on the third dimension and with the
    // intersections list, we know if the voxel is inside or outside of the mesh. So to improve performance, we need
    // to launch the minimum number of rays. The better way is to create three loops and call the right one.
    const auto zLoop =
        [&]()
        {
        #pragma omp parallel for
            for(std::int64_t x = indexXBeg; x < indexXEnd; ++x)
            {
                for(std::int64_t y = indexYBeg; y < indexYEnd; ++y)
                {
                    // For each voxel of the slice, launch a ray to the third axis.
                    const ::glm::vec3 rayOrig(origin[0] + x* spacing[0] + spacing[0]/2.f,
                                              origin[1] + y* spacing[1] + spacing[1]/2.f,
                                              origin[2] + indexZBeg* spacing[2] + spacing[2]/2.f);
                    const ::glm::vec3 rayDirPos(rayOrig.x, rayOrig.y, rayOrig.z + 1);
                    const ::glm::vec3 rayDir = ::glm::normalize(rayDirPos-rayOrig);

                    // Check if the first voxel is inside or not, and stores all intersections.
                    std::vector< ::glm::vec3 > intersections;
                    bool inside = getIntersections(rayOrig, rayDir, intersections);

                    // If there is no intersection, the entire line is visible.
                    if(intersections.size() > 0)
                    {
                        // Iterate over the "ray" and check intersections to know if the voxel is inside
                        // or outside of the mesh.
                        auto nextIntersection      = intersections.begin();
                        const auto intersectionEnd = intersections.end();
                        for(std::int64_t z = indexZBeg; z < indexZEnd; ++z)
                        {
                            const ::glm::vec3 currentVoxelPos(origin[0] + x* spacing[0] + spacing[0]/2.f,
                                                              origin[1] + y* spacing[1] + spacing[1]/2.f,
                                                              origin[2] + z* spacing[2] + spacing[2]/2.f);
                            // While the current ray position is near to the next intersection, set the
                            // voxel to the value if
                            // it's needed.
                            if(glm::distance(rayOrig, currentVoxelPos) < glm::distance(rayOrig, *nextIntersection))
                            {
                                if(inside)
                                {
                                    _param.m_image->at<IMAGE_TYPE>(x, y, z) = emptyValue;
                                }
                            }
                            // Once the intersection reach, get the next one.
                            else
                            {
                                inside = !inside;
                                ++nextIntersection;
                                // Once we found the last intersection, finish the image line.
                                if(nextIntersection == intersectionEnd)
                                {
                                    if(inside)
                                    {
                                        for(std::int64_t zp = z; zp < indexZEnd; ++zp)
                                        {
                                            _param.m_image->at<IMAGE_TYPE>(x, y, zp) = emptyValue;
                                        }
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        };

    const auto yLoop =
        [&]()
        {
        #pragma omp parallel for
            for(std::int64_t x = indexXBeg; x < indexXEnd; ++x)
            {
                for(std::int64_t z = indexZBeg; z < indexZEnd; ++z)
                {
                    const ::glm::vec3 rayOrig(origin[0] + x* spacing[0] + spacing[0]/2.f,
                                              origin[1] + indexYBeg* spacing[1] + spacing[1]/2.f,
                                              origin[2] + z* spacing[2] + spacing[2]/2.f);
                    const ::glm::vec3 rayDirPos(rayOrig.x, rayOrig.y + 1, rayOrig.z);
                    const ::glm::vec3 rayDir = ::glm::normalize(rayDirPos-rayOrig);

                    std::vector< ::glm::vec3 > intersections;
                    bool inside = getIntersections(rayOrig, rayDir, intersections);

                    if(intersections.size() > 0)
                    {
                        auto nextIntersection      = intersections.begin();
                        const auto intersectionEnd = intersections.end();
                        for(std::int64_t y = indexYBeg; y < indexYEnd; ++y)
                        {
                            const ::glm::vec3 currentVoxelPos(origin[0] + x* spacing[0] + spacing[0]/2.f,
                                                              origin[1] + y* spacing[1] + spacing[1]/2.f,
                                                              origin[2] + z* spacing[2] + spacing[2]/2.f);
                            if(glm::distance(rayOrig, currentVoxelPos) < glm::distance(rayOrig, *nextIntersection))
                            {
                                if(inside)
                                {
                                    _param.m_image->at<IMAGE_TYPE>(x, y, z) = emptyValue;
                                }
                            }
                            else
                            {
                                inside = !inside;
                                ++nextIntersection;
                                if(nextIntersection == intersectionEnd)
                                {
                                    if(inside)
                                    {
                                        for(std::int64_t yp = y; yp < indexYEnd; ++yp)
                                        {
                                            _param.m_image->at<IMAGE_TYPE>(x, yp, z) = emptyValue;
                                        }
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        };

    const auto xLoop =
        [&]()
        {
        #pragma omp parallel for
            for(std::int64_t y = indexYBeg; y < indexYEnd; ++y)
            {
                for(std::int64_t z = indexZBeg; z < indexZEnd; ++z)
                {
                    const ::glm::vec3 rayOrig(origin[0] + indexXBeg* spacing[0] + spacing[0]/2.f,
                                              origin[1] + y* spacing[1] + spacing[1]/2.f,
                                              origin[2] + z* spacing[2] + spacing[2]/2.f);
                    const ::glm::vec3 rayDirPos(rayOrig.x + 1, rayOrig.y, rayOrig.z);
                    const ::glm::vec3 rayDir = ::glm::normalize(rayDirPos-rayOrig);

                    std::vector< ::glm::vec3 > intersections;
                    bool inside = getIntersections(rayOrig, rayDir, intersections);

                    if(intersections.size() > 0)
                    {
                        auto nextIntersection      = intersections.begin();
                        const auto intersectionEnd = intersections.end();
                        for(std::int64_t x = indexXBeg; x < indexXEnd; ++x)
                        {
                            const ::glm::vec3 currentVoxelPos(origin[0] + x* spacing[0] + spacing[0]/2.f,
                                                              origin[1] + y* spacing[1] + spacing[1]/2.f,
                                                              origin[2] + z* spacing[2] + spacing[2]/2.f);
                            if(glm::distance(rayOrig, currentVoxelPos) < glm::distance(rayOrig, *nextIntersection))
                            {
                                if(inside)
                                {
                                    _param.m_image->at<IMAGE_TYPE>(x, y, z) = emptyValue;
                                }
                            }
                            else
                            {
                                inside = !inside;
                                ++nextIntersection;
                                if(nextIntersection == intersectionEnd)
                                {
                                    if(inside)
                                    {
                                        for(std::int64_t xp = x; xp < indexXEnd; ++xp)
                                        {
                                            _param.m_image->at<IMAGE_TYPE>(xp, y, z) = emptyValue;
                                        }
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        };

    // Get the smallest dimension in terms of voxel to loop over the minimum of voxel.
    std::uint8_t axis = 2;
    size_t voxel      = (indexXEnd - indexXBeg) * (indexYEnd - indexYBeg);

    size_t voxelXZ = (indexXEnd - indexXBeg) * (indexZEnd - indexZBeg);
    size_t voxelYZ = (indexYEnd - indexYBeg) * (indexZEnd - indexZBeg);
    if(voxelXZ < voxel)
    {
        axis  = 1;
        voxel = voxelXZ;
    }
    if(voxelYZ < voxel)
    {
        axis  = 0;
        voxel = voxelYZ;
    }

    // Call the right loop.
    switch (axis)
    {
        case 2:
            zLoop();
            break;
        case 1:
            yLoop();
            break;
        case 0:
            xLoop();
            break;
    }
}

}
