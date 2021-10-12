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

#include "SPointCloudFromDepthMap.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>
#include <core/Profiling.hpp>

#include <filter/vision/Projection.hpp>

#include <geometry/data/Matrix4.hpp>

#include <glm/glm.hpp>

#include <service/macros.hpp>

namespace sight::module::filter::vision
{

const core::com::Slots::SlotKeyType SPointCloudFromDepthMap::s_SET_DEPTH_RANGE = "setDepthRange";

//------------------------------------------------------------------------------

SPointCloudFromDepthMap::SPointCloudFromDepthMap() noexcept :
    service::IFilter()
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
    const auto calibration = m_calibration.lock();
    const auto depthMap    = m_depthMap.lock();
    const auto pointCloud  = m_pointCloud.lock();

    SIGHT_ASSERT("Missing 'pointCloud' inout", pointCloud);
    SIGHT_ASSERT("Missing 'calibration' input", calibration);
    SIGHT_ASSERT("Missing 'depthMap' input", depthMap);

    const auto depthCalibration = calibration->getCamera(0);

    const auto rgbMap = m_rgbMap.lock();

    data::Camera::csptr colorCalibration;
    data::Matrix4::csptr extrinsicMatrix;

    if(rgbMap)
    {
        colorCalibration = calibration->getCamera(1);
        extrinsicMatrix  = calibration->getExtrinsicMatrix(1);
        SIGHT_ASSERT("Missing extrinsic matrix", extrinsicMatrix);
    }

    // Initialize mesh points memory one time in order to increase performances
    if(pointCloud->numPoints() == 0)
    {
        const auto size       = depthMap->getSize();
        const size_t width    = size[0];
        const size_t height   = size[1];
        const size_t nbPoints = width * height;

        // allocate mesh
        data::Mesh::Attributes attribute = data::Mesh::Attributes::NONE;
        if(rgbMap)
        {
            attribute = data::Mesh::Attributes::POINT_COLORS;
        }

        pointCloud->resize(nbPoints, nbPoints, data::Mesh::CellType::POINT, attribute);

        const auto dumpLock = pointCloud->lock();

        auto itr = pointCloud->begin<data::iterator::cell::point>();

        // to display the mesh, we need to create cells with one point.
        for(size_t i = 0 ; i < nbPoints ; ++i, ++itr)
        {
            itr->pt = i;
        }

        auto sig = pointCloud->signal<data::Mesh::ModifiedSignalType>(data::Mesh::s_MODIFIED_SIG);
        sig->asyncEmit();
    }

    if(rgbMap)
    {
        this->depthMapToPointCloudRGB(
            depthCalibration,
            colorCalibration,
            depthMap.get_shared(),
            rgbMap.get_shared(),
            extrinsicMatrix,
            pointCloud.get_shared()
        );

        auto sig =
            pointCloud->signal<data::Mesh::signal_t>(data::Mesh::s_VERTEX_MODIFIED_SIG);
        sig->asyncEmit();

        auto sig2 = pointCloud->signal<data::Mesh::signal_t>(data::Mesh::s_POINT_COLORS_MODIFIED_SIG);
        sig2->asyncEmit();
    }
    else
    {
        this->depthMapToPointCloud(depthCalibration, depthMap.get_shared(), pointCloud.get_shared());
        auto sig =
            pointCloud->signal<data::Mesh::signal_t>(data::Mesh::s_VERTEX_MODIFIED_SIG);
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
            m_minDepth = static_cast<std::uint16_t>(_val);
        }
        else
        {
            SIGHT_ERROR("min Depth should be between [0; 65536]");
        }
    }
    else if(_key == "maxDepth")
    {
        if(_val >= 0 && _val <= UINT16_MAX)
        {
            m_maxDepth = static_cast<std::uint16_t>(_val);
        }
        else
        {
            SIGHT_ERROR("min Depth should be between [0; 65536]");
        }
    }
    else
    {
        SIGHT_ERROR("unknown key '" + _key + "' in slot '" + s_SET_DEPTH_RANGE + "'");
    }
}

//------------------------------------------------------------------------------

void SPointCloudFromDepthMap::depthMapToPointCloud(
    const data::Camera::csptr& depthCamera,
    const data::Image::csptr& depthMap,
    const data::Mesh::sptr& pointCloud
)
{
    SIGHT_INFO("Input RGB map was empty, skipping colors");

    const auto type = depthMap->getType();
    if(type != core::tools::Type::s_UINT16)
    {
        SIGHT_ERROR("Wrong input depth map format: " << type << ", uint16 is expected.");
        return;
    }

    const auto size     = depthMap->getSize();
    const size_t width  = size[0];
    const size_t height = size[1];

    const auto depthDumpLock = depthMap->lock();

    auto depthItr = depthMap->begin<std::uint16_t>();

    const double cx = depthCamera->getCx(),
                 cy = depthCamera->getCy(),
                 fx = depthCamera->getFx(),
                 fy = depthCamera->getFy();

    const auto meshDumpLock = pointCloud->lock();

    auto pointsItr = pointCloud->begin<data::iterator::point::xyz>();

    size_t nbRealPoints = 0;
    for(size_t y = 0 ; y != height ; ++y)
    {
        for(size_t x = 0 ; x != width ; ++x)
        {
            const uint16_t depth = *depthItr;
            if(depth >= m_minDepth && depth <= m_maxDepth)
            {
                double px, py, pz;
                sight::filter::vision::Projection::projectPixel<double>(x, y, depth, cx, cy, fx, fy, px, py, pz);
                pointsItr->x = static_cast<float>(px);
                pointsItr->y = static_cast<float>(py);
                pointsItr->z = static_cast<float>(pz);
                ++pointsItr;
                ++nbRealPoints;
            }

            ++depthItr;
        }
    }
}

//------------------------------------------------------------------------------

void SPointCloudFromDepthMap::depthMapToPointCloudRGB(
    const data::Camera::csptr& depthCamera,
    const data::Camera::csptr& colorCamera,
    const data::Image::csptr& depthMap,
    const data::Image::csptr& colorMap,
    const data::Matrix4::csptr& extrinsic,
    const data::Mesh::sptr& pointCloud
)
{
    SIGHT_INFO("Input RGB map was supplied, including colors");

    const auto type = depthMap->getType();
    if(type != core::tools::Type::s_UINT16)
    {
        SIGHT_ERROR("Wrong input depth map format: " << type << ", uint16 is expected.");
        return;
    }

    // Make sure RGB and depth maps are the same size
    const auto size     = depthMap->getSize();
    const size_t width  = size[0];
    const size_t height = size[1];

    const auto rgbType = colorMap->getType();
    if(rgbType != core::tools::Type::s_UINT8)
    {
        SIGHT_ERROR("Wrong input rgb format: " << rgbType << ", uint8 is expected.");
        return;
    }

    if(4 != colorMap->numComponents())
    {
        SIGHT_ERROR("Wrong number of components in rgb : " << colorMap->numComponents() << ", 4 is expected.");
        return;
    }

    const auto rgbSize     = colorMap->getSize();
    const size_t rgbWidth  = rgbSize[0];
    const size_t rgbHeight = rgbSize[1];

    if(rgbWidth != width || rgbHeight != height)
    {
        SIGHT_ERROR("RGB and depth maps must have the same size");
        return;
    }

    const auto depthDumpLock = depthMap->lock();
    auto depthItr            = depthMap->begin<std::uint16_t>();

    const auto rgbDumpLock = colorMap->lock();
    const auto rgbBegin    = colorMap->begin<data::iterator::rgba>();

    const double cx = depthCamera->getCx(),
                 cy = depthCamera->getCy(),
                 fx = depthCamera->getFx(),
                 fy = depthCamera->getFy();

    const double rgbCx = colorCamera->getCx(),
                 rgbCy = colorCamera->getCy(),
                 rgbFx = colorCamera->getFx(),
                 rgbFy = colorCamera->getFy();

    const auto meshDumpLock = pointCloud->lock();

    auto pointsItr = pointCloud->zip_range<data::iterator::point::xyz, data::iterator::point::rgba>().begin();

    const data::iterator::rgba defaultColor = {255, 255, 255, 255};

    unsigned int nbRealPoints = 0;
    auto glmExtrinsicMatrix   = geometry::data::getMatrixFromTF3D(*extrinsic);

    const auto imageSize = height * width;
    for(size_t y = 0 ; y != height ; ++y)
    {
        for(size_t x = 0 ; x != width ; ++x)
        {
            const uint16_t depth = *depthItr;
            if(depth >= m_minDepth && depth <= m_maxDepth)
            {
                auto&& [p, c] = *pointsItr;

                // get the 3D coordinates in the depth world
                double px, py, pz;
                sight::filter::vision::Projection::projectPixel<double>(x, y, depth, cx, cy, fx, fy, px, py, pz);
                p.x = static_cast<float>(px);
                p.y = static_cast<float>(py);
                p.z = static_cast<float>(pz);

                // Transform point to the rgb sensor world
                const ::glm::dvec4 point(px, py, pz, 1.0);
                const ::glm::dvec4 rgbPoint = glmExtrinsicMatrix * point;

                // project point to the rgb image
                size_t rgbPx, rgbPy;
                const bool isProjected = sight::filter::vision::Projection::projectPoint(
                    rgbPoint.x,
                    rgbPoint.y,
                    rgbPoint.z,
                    rgbCx,
                    rgbCy,
                    rgbFx,
                    rgbFy,
                    rgbWidth,
                    rgbHeight,
                    rgbPx,
                    rgbPy
                );

                if(isProjected)
                {
                    const size_t rgbIdx = rgbPy * rgbWidth + rgbPx;
                    if(rgbIdx < imageSize)
                    {
                        const auto color = rgbBegin + rgbIdx;
                        c = *color;
                    }
                    else
                    {
                        c = defaultColor;
                    }
                }
                else
                {
                    c = defaultColor;
                }

                ++pointsItr;
                ++nbRealPoints;
            }

            ++depthItr;
        }
    }

    // Since we discard points for which the depth map is zero, the mesh buffers are not full
    pointCloud->truncate(nbRealPoints, nbRealPoints);
}

//-----------------------------------------------------------------------------

} // namespace sight::module::filter::vision
