/************************************************************************
 *
 * Copyright (C) 2019-2021 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include "opDepthMap/SPointCloudFromDepthMap.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>
#include <core/Profiling.hpp>

#include <data/CameraSeries.hpp>
#include <data/mt/ObjectReadLock.hpp>
#include <data/mt/ObjectWriteLock.hpp>

#include <depthMapOp/Projection.hpp>

#include <fwDataTools/TransformationMatrix3D.hpp>

#include <services/macros.hpp>

#include <glm/glm.hpp>

namespace opDepthMap
{

fwServicesRegisterMacro( ::sight::services::IOperator, ::opDepthMap::SPointCloudFromDepthMap)

const core::com::Slots::SlotKeyType SPointCloudFromDepthMap::s_SET_DEPTH_RANGE = "setDepthRange";

//------------------------------------------------------------------------------

SPointCloudFromDepthMap::SPointCloudFromDepthMap() noexcept :
    services::IOperator()
{
    newSlot(s_SET_DEPTH_RANGE, &SPointCloudFromDepthMap::setDepthRange, this);
}

//------------------------------------------------------------------------------

SPointCloudFromDepthMap::~SPointCloudFromDepthMap() noexcept
{
}

//------------------------------------------------------------------------------

void SPointCloudFromDepthMap::starting()
{
}

//------------------------------------------------------------------------------

void SPointCloudFromDepthMap::stopping()
{

}

//------------------------------------------------------------------------------

void SPointCloudFromDepthMap::configuring()
{

}

//------------------------------------------------------------------------------

void SPointCloudFromDepthMap::updating()
{
    auto calibration = this->getInput< data::CameraSeries>("calibration");
    auto depthMap    = this->getInput< data::Image>("depthMap");
    auto pointCloud  = this->getInOut< data::Mesh >("pointCloud");
    SLM_ASSERT("Missing 'pointCloud' inout", pointCloud);
    SLM_ASSERT("Missing 'calibration' input", calibration);
    SLM_ASSERT("Missing 'depthMap' input", depthMap);

    auto depthCalibration = calibration->getCamera(0);

    auto rgbMap = this->getInput< data::Image>("rgbMap");
    data::Camera::csptr colorCalibration;
    data::TransformationMatrix3D::csptr extrinsicMatrix;
    if (rgbMap)
    {
        colorCalibration = calibration->getCamera(1);
        extrinsicMatrix  = calibration->getExtrinsicMatrix(1);
        SLM_ASSERT("Missing extrinsic matrix", extrinsicMatrix);
    }

    // Initialize mesh points memory one time in order to increase performances
    if (pointCloud->getNumberOfPoints() == 0)
    {
        const auto size       = depthMap->getSize2();
        const size_t width    = size[0];
        const size_t height   = size[1];
        const size_t nbPoints = width * height;

        data::mt::ObjectWriteLock meshLock(pointCloud);
        // allocate mesh
        data::Mesh::Attributes attribute = data::Mesh::Attributes::NONE;
        if (rgbMap)
        {
            attribute = data::Mesh::Attributes::POINT_COLORS;
        }
        pointCloud->resize(nbPoints, nbPoints, data::Mesh::CellType::POINT, attribute);

        const auto dumpLock = pointCloud->lock();

        auto itr = pointCloud->begin< data::iterator::CellIterator >();

        // to display the mesh, we need to create cells with one point.
        for( size_t i = 0; i < nbPoints; ++i, ++itr )
        {
            *itr->type       = data::Mesh::CellType::POINT;
            *itr->offset     = i;
            *(itr+1)->offset = i+1; // to be able to iterate through point indices
            itr->pointIdx[0] = i;
        }

        auto sig = pointCloud->signal< data::Mesh::ModifiedSignalType >(data::Mesh::s_MODIFIED_SIG);
        sig->asyncEmit();
    }

    if (rgbMap)
    {
        this->depthMapToPointCloudRGB(depthCalibration, colorCalibration, depthMap, rgbMap, extrinsicMatrix,
                                      pointCloud);

        auto sig =
            pointCloud->signal< data::Mesh::VertexModifiedSignalType >(data::Mesh::s_VERTEX_MODIFIED_SIG);
        sig->asyncEmit();

        auto sig2 = pointCloud->signal< data::Mesh::PointColorsModifiedSignalType >(
            data::Mesh::s_POINT_COLORS_MODIFIED_SIG);
        sig2->asyncEmit();
    }
    else
    {
        this->depthMapToPointCloud(depthCalibration, depthMap, pointCloud);
        auto sig =
            pointCloud->signal< data::Mesh::VertexModifiedSignalType >(data::Mesh::s_VERTEX_MODIFIED_SIG);
        sig->asyncEmit();
    }

    m_sigComputed->asyncEmit();
}

//------------------------------------------------------------------------------

void SPointCloudFromDepthMap::setDepthRange(int _val, std::string _key)
{
    if(_key == "minDepth")
    {
        if(_val >= 0 && _val <= UINT16_MAX)
        {
            m_minDepth = static_cast< std::uint16_t >(_val);
        }
        else
        {
            SLM_ERROR("min Depth should be between [0; 65536]");
        }

    }
    else if(_key == "maxDepth")
    {
        if(_val >= 0 && _val <= UINT16_MAX)
        {
            m_maxDepth = static_cast< std::uint16_t >(_val);
        }
        else
        {
            SLM_ERROR("min Depth should be between [0; 65536]");
        }
    }
    else
    {
        SLM_ERROR("unknown key '" + _key + "' in slot '" + s_SET_DEPTH_RANGE + "'" );
    }
}

//------------------------------------------------------------------------------

void SPointCloudFromDepthMap::depthMapToPointCloud(
    const data::Camera::csptr& depthCamera,
    const data::Image::csptr& depthMap,
    const data::Mesh::sptr& pointCloud)
{
    SLM_INFO("Input RGB map was empty, skipping colors");

    const auto type = depthMap->getType();
    if(type != core::tools::Type::s_UINT16)
    {
        SLM_ERROR("Wrong input depth map format: " << type << ", uint16 is expected.");
        return;
    }

    const auto size     = depthMap->getSize2();
    const size_t width  = size[0];
    const size_t height = size[1];

    data::mt::ObjectReadLock depthMapLock(depthMap);

    const auto depthDumpLock = depthMap->lock();

    auto depthItr = depthMap->begin< std::uint16_t >();

    const double cx = depthCamera->getCx(),
                 cy = depthCamera->getCy(),
                 fx = depthCamera->getFx(),
                 fy = depthCamera->getFy();

    data::mt::ObjectWriteLock meshLock(pointCloud);

    const auto meshDumpLock = pointCloud->lock();

    auto pointsItr = pointCloud->begin< data::iterator::PointIterator >();

    size_t nbRealPoints = 0;
    for(size_t y = 0; y != height; ++y)
    {
        for(size_t x = 0; x != width; ++x)
        {
            const uint16_t depth = *depthItr;
            if(depth >= m_minDepth && depth <= m_maxDepth)
            {
                double px, py, pz;
                ::depthMapOp::Projection::projectPixel<double>(x, y, depth, cx, cy, fx, fy, px, py, pz);
                pointsItr->point->x = static_cast<float>(px);
                pointsItr->point->y = static_cast<float>(py);
                pointsItr->point->z = static_cast<float>(pz);
                ++pointsItr;
                ++nbRealPoints;
            }
            ++depthItr;
        }
    }
    pointCloud->setNumberOfPoints(nbRealPoints);
    pointCloud->setNumberOfCells(nbRealPoints);
    pointCloud->setCellDataSize(nbRealPoints);
}

//------------------------------------------------------------------------------

void SPointCloudFromDepthMap::depthMapToPointCloudRGB(
    const data::Camera::csptr& depthCamera,
    const data::Camera::csptr& colorCamera,
    const data::Image::csptr& depthMap,
    const data::Image::csptr& colorMap,
    const data::TransformationMatrix3D::csptr& extrinsic,
    const data::Mesh::sptr& pointCloud)
{
    SLM_INFO("Input RGB map was supplied, including colors");

    const auto type = depthMap->getType();
    if(type != core::tools::Type::s_UINT16)
    {
        SLM_ERROR("Wrong input depth map format: " << type << ", uint16 is expected.");
        return;
    }

    // Make sure RGB and depth maps are the same size
    const auto size     = depthMap->getSize2();
    const size_t width  = size[0];
    const size_t height = size[1];

    const auto rgbType = colorMap->getType();
    if(rgbType != core::tools::Type::s_UINT8)
    {
        SLM_ERROR("Wrong input rgb format: " << rgbType << ", uint8 is expected.");
        return;
    }
    if(4 != colorMap->getNumberOfComponents())
    {
        SLM_ERROR("Wrong number of components in rgb : " << colorMap->getNumberOfComponents() << ", 4 is expected.");
        return;
    }

    const auto rgbSize     = colorMap->getSize2();
    const size_t rgbWidth  = rgbSize[0];
    const size_t rgbHeight = rgbSize[1];

    if(rgbWidth != width || rgbHeight != height)
    {
        SLM_ERROR("RGB and depth maps must have the same size");
        return;
    }

    data::mt::ObjectReadLock depthMapLock(depthMap);

    const auto depthDumpLock = depthMap->lock();
    auto depthItr            = depthMap->begin< std::uint16_t >();

    data::mt::ObjectReadLock rgbLock(colorMap);

    const auto rgbDumpLock = colorMap->lock();
    const auto rgbBegin    = colorMap->begin< data::iterator::RGBA >();

    const double cx = depthCamera->getCx(),
                 cy = depthCamera->getCy(),
                 fx = depthCamera->getFx(),
                 fy = depthCamera->getFy();

    const double rgbCx = colorCamera->getCx(),
                 rgbCy = colorCamera->getCy(),
                 rgbFx = colorCamera->getFx(),
                 rgbFy = colorCamera->getFy();

    data::mt::ObjectWriteLock meshLock(pointCloud);

    const auto meshDumpLock = pointCloud->lock();

    auto pointsItr = pointCloud->begin< data::iterator::PointIterator >();

    const data::iterator::RGBA defaultColor = {255, 255, 255, 255};

    size_t nbRealPoints     = 0;
    auto glmExtrinsicMatrix = ::fwDataTools::TransformationMatrix3D::getMatrixFromTF3D(extrinsic);

    const auto imageSize = height*width;
    for(size_t y = 0; y != height; ++y)
    {
        for(size_t x = 0; x != width; ++x)
        {
            const uint16_t depth = *depthItr;
            if(depth >= m_minDepth && depth <= m_maxDepth)
            {
                // get the 3D coordinates in the depth world
                double px, py, pz;
                ::depthMapOp::Projection::projectPixel<double>(x, y, depth, cx, cy, fx, fy, px, py, pz);
                pointsItr->point->x = static_cast<float>(px);
                pointsItr->point->y = static_cast<float>(py);
                pointsItr->point->z = static_cast<float>(pz);

                // Transform point to the rgb sensor world
                const ::glm::dvec4 point(px, py, pz, 1.0);
                const ::glm::dvec4 rgbPoint = glmExtrinsicMatrix * point;

                // project point to the rgb image
                size_t rgbPx, rgbPy;
                const bool isProjected = ::depthMapOp::Projection::projectPoint(
                    rgbPoint.x, rgbPoint.y, rgbPoint.z,
                    rgbCx, rgbCy, rgbFx, rgbFy,
                    rgbWidth, rgbHeight, rgbPx, rgbPy);

                if (isProjected)
                {
                    const size_t rgbIdx = rgbPy * rgbWidth + rgbPx;
                    if (rgbIdx < imageSize)
                    {
                        const auto color = rgbBegin + rgbIdx;
                        *pointsItr->rgba = *color;
                    }
                    else
                    {
                        *pointsItr->rgba = defaultColor;
                    }
                }
                else
                {
                    *pointsItr->rgba = defaultColor;
                }
                ++pointsItr;
                ++nbRealPoints;
            }
            ++depthItr;
        }
    }

    // Since we discard points for which the depth map is zero, the mesh buffers are not full
    pointCloud->setNumberOfPoints(nbRealPoints);
    pointCloud->setNumberOfCells(nbRealPoints);
    pointCloud->setCellDataSize(nbRealPoints);
}

//-----------------------------------------------------------------------------

} // namespace opDepthMap
