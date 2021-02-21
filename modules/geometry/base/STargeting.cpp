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

#include "modules/geometry/base/STargeting.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <data/Matrix4.hpp>
#include <data/PointList.hpp>
#include <data/Landmarks.hpp>

#include <geometry/data/Matrix4.hpp>

#include <service/macros.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/intersect.hpp>
#undef GLM_ENABLE_EXPERIMENTAL

namespace sight::module::geometry::base
{

// -----------------------------------------------------------------------------

const service::IService::KeyType s_LANDMARK_INPUT         = "landmark";
const service::IService::KeyType s_MATRIX_INPUT           = "matrix";
const service::IService::KeyType s_POINTLIST_INOUT        = "pointList";
const core::com::Slots::SlotKeyType s_SELECTED_POINT_SLOT = "updateSelectedPoint";
const core::com::Slots::SlotKeyType s_UPDATE_POINT_SLOT   = "updatePoint";
const core::com::Slots::SlotKeyType s_REMOVE_POINT_SLOT   = "removePoint";

// -----------------------------------------------------------------------------

fwServicesRegisterMacro( ::sight::service::IService, ::sight::module::geometry::base::STargeting )

// -----------------------------------------------------------------------------

STargeting::STargeting() noexcept :
    m_targetLandmark(::glm::dvec3(0.0, 0.0, 0.0))
{
    newSlot(s_SELECTED_POINT_SLOT, &STargeting::updateSelectedPoint, this);
    newSlot(s_UPDATE_POINT_SLOT, &STargeting::updatePoint, this);
    newSlot(s_REMOVE_POINT_SLOT, &STargeting::removePoint, this);
}

// -----------------------------------------------------------------------------

STargeting::~STargeting() noexcept
{
}

// -----------------------------------------------------------------------------

void STargeting::starting()
{

}

// -----------------------------------------------------------------------------

void STargeting::stopping()
{
}

// -----------------------------------------------------------------------------

void STargeting::configuring()
{
    const ConfigType configuration = this->getConfigTree();
    m_label = configuration.get< std::string >("label", m_label);
    if(!m_label.empty())
    {
        m_landmarkSelected = true;
    }

    m_width = configuration.get< int >("width", m_width);

}

// -----------------------------------------------------------------------------

void STargeting::updating()
{
    if(m_landmarkSelected)
    {
        const auto landmark = this->getLockedInput< data::Landmarks >(s_LANDMARK_INPUT);
        SLM_ASSERT("Input \"landmark\" is missing.", landmark);
        if(landmark->getGroup(m_label).m_points.size() > 0)
        {
            const data::Landmarks::PointType point = landmark->getPoint(m_label, m_index);
            m_targetLandmark = ::glm::dvec3(point[0], point[1], point[2]);

        }
        else
        {
            return;
        }

    }

    // Get the input matrix for the needle tip
    const auto matrix =
        this->getLockedInput< data::Matrix4 >( s_MATRIX_INPUT );
    SLM_ASSERT("Input \"matrix\" is missing.", matrix);

    const ::glm::dmat4x4 mat = sight::geometry::data::getMatrixFromTF3D(matrix.get_shared());

    const ::glm::dvec4 origin(0.0, 0.0, 0.0, 1.0);

    const ::glm::dvec4 axisX(1.0, 0.0, 0.0, 0.0);
    const ::glm::dvec4 axisZ(0.0, 0.0, 1.0, 0.0);

    // Compute the needle tip position
    const ::glm::dvec3 needleTip = ::glm::dvec3(mat * origin);

    // Compute the needle orientation vectors
    const ::glm::dvec3 needleTipX = ::glm::dvec3(mat * axisX);
    const ::glm::dvec3 needleTipZ = ::glm::dvec3(mat * axisZ);

    // WARNING: this axis must be consistent with the orientation of the input matrix
    // For example:
    // - If you use a camera matrix, this must be the Z axis
    // - If you use an EM sensor from the trakSTAR, this must be the X axis
    const ::glm::dvec3 needleDirection = ::glm::normalize(needleTipZ);

    const ::glm::dvec3 needleTipToLandmark = ::glm::normalize(m_targetLandmark - needleTip);

    // Compute the intersection between the needle (from the tip) and the landmark plane
    double distance  = 0.0;
    double distanceX = 0.0;

    /* Project the needle tip origin and the associated X axis of the matrices on the landmark plane */
    /* To get a coordinate system on this plane (the Y axis will be obtained via a cross product) */
    if(::glm::intersectRayPlane(needleTip, needleDirection, m_targetLandmark, -needleTipToLandmark, distance)
       && ::glm::intersectRayPlane(needleTip + needleTipX, needleDirection, m_targetLandmark, -needleTipToLandmark,
                                   distanceX)
       && !m_label.empty()
       )
    {

        // Compute the 3D position of the intersection between the needle and the landmark plane
        const ::glm::dvec3 projectedNeedleOrigin = needleTip + needleDirection * distance;
        // Shift the needleTip with the axis vector
        const ::glm::dvec3 projectedNeedleX = needleTip + needleTipX + needleDirection * distanceX;

        const ::glm::dvec3 projectedXAxis = ::glm::normalize(projectedNeedleX - projectedNeedleOrigin);
        const ::glm::dvec3 projectedYAxis = ::glm::cross(needleTipToLandmark, projectedXAxis);

        // Compute the matrix from the world coordinates to the landmark plane coordinates
        ::glm::dmat4x4 worldToPlaneMatrix;
        worldToPlaneMatrix[0] = ::glm::dvec4(projectedXAxis, 0.0);
        worldToPlaneMatrix[1] = ::glm::dvec4(projectedYAxis, 0.0);
        worldToPlaneMatrix[2] = ::glm::dvec4(needleTipToLandmark, 0.0);
        worldToPlaneMatrix[3] = ::glm::dvec4(m_targetLandmark, 1.0);

        // Invert the world to landmark plane matrix
        const ::glm::dmat4x4 planeToWorldMatrix = ::glm::affineInverse(worldToPlaneMatrix);

        // Transform in the 2D landmark plane the needle intersection
        ::glm::dvec3 transformedNeedleIntersection =
            ::glm::dvec3(planeToWorldMatrix * ::glm::dvec4(projectedNeedleOrigin, 1.0));
        transformedNeedleIntersection = ::glm::normalize(transformedNeedleIntersection);

        // Get the distance between the projected needle point and the landmark position
        const double projectedNeedleToLandmarkdistance = ::glm::distance(projectedNeedleOrigin, m_targetLandmark);

        // Compute a scale value so that the vector will represent the correct distance value on the view
        const double maxDistance = 50.0;
        double scale             = projectedNeedleToLandmarkdistance / maxDistance * m_width/2.0;
        scale = (projectedNeedleToLandmarkdistance > maxDistance ? m_width/2.0 : scale);

        transformedNeedleIntersection = transformedNeedleIntersection * scale;

        auto pointList = this->getLockedInOut< data::PointList >(s_POINTLIST_INOUT);
        SLM_ASSERT("InOut \"pointList\" is missing.", pointList);
        if(pointList->getPoints().size() > 0)
        {
            pointList->clear();
        }
        const data::Point::sptr point = data::Point::New(transformedNeedleIntersection[0],
                                                         -transformedNeedleIntersection[1],
                                                         0.);

        pointList->pushBack(point);

        auto sig = pointList->signal< data::PointList::PointAddedSignalType >(
            data::PointList::s_POINT_ADDED_SIG);
        sig->asyncEmit(point);
    }

}

// -----------------------------------------------------------------------------

service::IService::KeyConnectionsMap STargeting::getAutoConnections() const
{

    service::IService::KeyConnectionsMap connections;
    connections.push(s_MATRIX_INPUT, data::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
    return connections;
}

// -----------------------------------------------------------------------------

void STargeting::updateSelectedPoint(std::string name, size_t index)
{
    m_label            = name;
    m_landmarkSelected = true;
    m_index            = index;
    this->update();

}
// -----------------------------------------------------------------------------

void STargeting::updatePoint(std::string name)
{
    m_label            = name;
    m_landmarkSelected = true;
    const auto landmark = this->getLockedInput< data::Landmarks >(s_LANDMARK_INPUT);
    SLM_ASSERT("Input \"landmark\" is missing.", landmark);

    const size_t size = landmark->getGroup(m_label).m_points.size();
    m_index = size -1;
    this->update();
}

// -----------------------------------------------------------------------------

void STargeting::removePoint()
{
    // When a point is removed, it's not selected anymore
    m_landmarkSelected = false;

    auto pointList = this->getLockedInOut< data::PointList >(s_POINTLIST_INOUT);
    SLM_ASSERT("InOut \"pointList\" is missing.", pointList);
    auto points = pointList->getPoints(); // copy the points.
    pointList->clear();
    for(const auto& pt : points)
    {
        // Send signals.
        auto sig = pointList->signal< data::PointList::PointRemovedSignalType >(
            data::PointList::s_POINT_REMOVED_SIG);
        sig->asyncEmit(pt);
    }

}

// -----------------------------------------------------------------------------

}// namespace sight::module::geometry::base
