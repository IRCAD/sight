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

#include "opImageFilter/SImageExtruder.hpp"

#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>
#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwTools/Dispatcher.hpp>
#include <fwTools/TypeKeyTypeMapping.hpp>

#include <glm/gtx/intersect.hpp>

namespace opImageFilter
{

static const std::string s_MESHES_INPUT = "meshes";
static const std::string s_IMAGE_INPUT  = "image";

static const std::string s_IMAGE_INOUT = "extrudedImage";

static const ::fwCom::Slots::SlotKeyType s_ADD_RECONSTRUCTIONS_SLOT = "addReconstructions";

//------------------------------------------------------------------------------

SImageExtruder::SImageExtruder()
{
    newSlot(s_ADD_RECONSTRUCTIONS_SLOT, &SImageExtruder::addReconstructions, this);
}

//------------------------------------------------------------------------------

SImageExtruder::~SImageExtruder()
{

}

//------------------------------------------------------------------------------

void SImageExtruder::configuring()
{

}

//------------------------------------------------------------------------------

void SImageExtruder::starting()
{
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SImageExtruder::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push(s_MESHES_INPUT, ::fwMedData::ModelSeries::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_MESHES_INPUT, ::fwMedData::ModelSeries::s_RECONSTRUCTIONS_ADDED_SIG, s_ADD_RECONSTRUCTIONS_SLOT);
    connections.push(s_MESHES_INPUT, ::fwMedData::ModelSeries::s_RECONSTRUCTIONS_REMOVED_SIG, s_UPDATE_SLOT);

    connections.push(s_IMAGE_INPUT, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_INPUT, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void SImageExtruder::updating()
{
    const ::fwData::Image::csptr image = this->getInput< ::fwData::Image >(s_IMAGE_INPUT);
    SLM_ASSERT("Input '" + s_IMAGE_INPUT + "' does not exist.", image);
    ::fwData::mt::ObjectReadLock imageLock(image);

    if(::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image))
    {
        // Copy the image into the output.
        const ::fwData::Image::sptr imageOut = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
        SLM_ASSERT("Inout '" + s_IMAGE_INOUT + "' does not exist.", imageOut);
        {
            const ::fwData::mt::ObjectWriteLock imageOutLock(imageOut);

            SLM_ASSERT("The image must be in 3 dimensions", image->getNumberOfDimensions() == 3);

            imageOut->deepCopy(image);

            const auto sig = imageOut->signal< ::fwData::Image::ModifiedSignalType>(::fwData::Image::s_MODIFIED_SIG);
            sig->asyncEmit();
        }

        imageLock.unlock();

        const ::fwMedData::ModelSeries::csptr meshes = this->getInput< ::fwMedData::ModelSeries >(s_MESHES_INPUT);
        SLM_ASSERT("Input '" + s_MESHES_INPUT + "' does not exist.", meshes);
        ::fwData::mt::ObjectReadLock meshLock(meshes);

        ::fwMedData::ModelSeries::ReconstructionVectorType reconstructions = meshes->getReconstructionDB();

        this->addReconstructions(reconstructions);
    }
}

//------------------------------------------------------------------------------

void SImageExtruder::stopping()
{
}

//------------------------------------------------------------------------------

void SImageExtruder::addReconstructions(::fwMedData::ModelSeries::ReconstructionVectorType _reconstructions) const
{
    const ::fwData::Image::csptr imageOut = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Inout '" + s_IMAGE_INOUT + "' does not exist.", imageOut);
    ::fwData::mt::ObjectReadLock imageOutLock(imageOut);

    if(::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(imageOut))
    {
        imageOutLock.unlock();

        for(const ::fwData::Reconstruction::csptr reconstruction : _reconstructions)
        {
            this->extrudeMesh(reconstruction->getMesh());
        }
    }
}

//------------------------------------------------------------------------------

struct ExtrudeImage
{
    /// Represents a 3D triangle by three points.
    struct Triangle
    {
        const ::glm::vec3 a;
        const ::glm::vec3 b;
        const ::glm::vec3 c;
    };

    /// Stores parameters of the functor.
    struct Parameters
    {
        ::fwData::Image::sptr m_image;
        ::fwData::Mesh::csptr m_mesh;
    };

    //------------------------------------------------------------------------------

    template<typename IMAGE_TYPE>
    void operator()(Parameters& _param)
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
                SLM_FATAL("The extrusion work only with mesh of at least three points per cells");
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
                SLM_FATAL("The extrusion work only with mesh of at most four points per cells");
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

        // Check if the ray origin is inside or ousite of the mesh and return all found intersections.
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
                        if(distance >= 0)
                        {
                            _intersections.push_back(_rayOrig + _rayDir*distance);
                            inside = !inside;
                        }
                    }
                }

                // Sort all intersection from the nearest one of the origin, to the lastest.
                std::sort(_intersections.begin(), _intersections.end(), [&](const ::glm::vec3& _a,
                                                                            const ::glm::vec3& _b)
                {
                    return glm::distance(_rayOrig, _a) < glm::distance(_rayOrig, _b);
                });

                return inside;
            };

        // Check if each voxel are in the mesh and sets them to -1000.
        const IMAGE_TYPE air = static_cast<IMAGE_TYPE>(-1000);

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
                        const ::glm::vec3 rayOrig(origin[0] + x*spacing[0],
                                                  origin[1] + y*spacing[1],
                                                  origin[2] + indexZBeg*spacing[2]);
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
                                const ::glm::vec3 currentVoxelPos(origin[0] + x*spacing[0],
                                                                  origin[1] + y*spacing[1],
                                                                  origin[2] + z*spacing[2]);
                                // While the current ray position is near to the next intersection, set the
                                // voxel to the value if
                                // it's needed.
                                if(glm::distance(rayOrig, currentVoxelPos) < glm::distance(rayOrig, *nextIntersection))
                                {
                                    if(inside)
                                    {
                                        _param.m_image->at<IMAGE_TYPE>(x, y, z) = air;
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
                                                _param.m_image->at<IMAGE_TYPE>(x, y, zp) = air;
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
                        const ::glm::vec3 rayOrig(origin[0] + x*spacing[0],
                                                  origin[1] + indexYBeg*spacing[1],
                                                  origin[2] + z*spacing[2]);
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
                                const ::glm::vec3 currentVoxelPos(origin[0] + x*spacing[0],
                                                                  origin[1] + y*spacing[1],
                                                                  origin[2] + z*spacing[2]);
                                if(glm::distance(rayOrig, currentVoxelPos) < glm::distance(rayOrig, *nextIntersection))
                                {
                                    if(inside)
                                    {
                                        _param.m_image->at<IMAGE_TYPE>(x, y, z) = air;
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
                                                _param.m_image->at<IMAGE_TYPE>(x, yp, z) = air;
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
                        const ::glm::vec3 rayOrig(origin[0] + indexXBeg*spacing[0],
                                                  origin[1] + y*spacing[1],
                                                  origin[2] + z*spacing[2]);
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
                                const ::glm::vec3 currentVoxelPos(origin[0] + x*spacing[0],
                                                                  origin[1] + y*spacing[1],
                                                                  origin[2] + z*spacing[2]);
                                if(glm::distance(rayOrig, currentVoxelPos) < glm::distance(rayOrig, *nextIntersection))
                                {
                                    if(inside)
                                    {
                                        _param.m_image->at<IMAGE_TYPE>(x, y, z) = air;
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
                                                _param.m_image->at<IMAGE_TYPE>(xp, y, z) = air;
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

        // Send signals.
        const auto sig =
            _param.m_image->signal< ::fwData::Image::BufferModifiedSignalType>(::fwData::Image::s_BUFFER_MODIFIED_SIG);
        sig->asyncEmit();
    }
};

//------------------------------------------------------------------------------

void SImageExtruder::extrudeMesh(const ::fwData::Mesh::csptr _mesh) const
{
    const ::fwData::Image::sptr imageOut = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Inout '" + s_IMAGE_INOUT + "' does not exist.", imageOut);
    const ::fwData::mt::ObjectWriteLock imageOutLock(imageOut);

    ExtrudeImage::Parameters param;
    param.m_image = imageOut;
    param.m_mesh  = _mesh;

    // We use a dispatcher because we can't retrieve the image type without a DynamicType.
    ::fwTools::Type type = imageOut->getType();
    ::fwTools::Dispatcher< ::fwTools::SupportedDispatcherTypes, ExtrudeImage >::invoke(type, param);

    m_sigComputed->asyncEmit();
}

} // namespace opImageFilter.
