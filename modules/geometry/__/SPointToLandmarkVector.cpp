/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
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

#include "modules/geometry/__/SPointToLandmarkVector.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <geometry/data/Matrix4.hpp>

#include <glm/glm.hpp>

namespace sight::module::geometry
{

// -----------------------------------------------------------------------------

static const core::com::signals::key_t LENGTH_CHANGED_SIG     = "lengthChanged";
static const core::com::signals::key_t LENGTH_STR_CHANGED_SIG = "lengthChangedStr";
static const core::com::signals::key_t SAME_SLICE_SIG         = "sameSlice";

// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

SPointToLandmarkVector::SPointToLandmarkVector() noexcept
{
    new_signal<LengthChangedSignalType>(LENGTH_CHANGED_SIG);
    new_signal<LengthStrChangedSignalType>(LENGTH_STR_CHANGED_SIG);
    new_signal<SameSliceSignalType>(SAME_SLICE_SIG);
}

// -----------------------------------------------------------------------------

SPointToLandmarkVector::~SPointToLandmarkVector() noexcept =
    default;

// -----------------------------------------------------------------------------

void SPointToLandmarkVector::starting()
{
    auto computedLandmark = m_computedLandmark.lock();
    computedLandmark->addGroup(m_groupLabel);
}

// -----------------------------------------------------------------------------

void SPointToLandmarkVector::stopping()
{
}

// -----------------------------------------------------------------------------

void SPointToLandmarkVector::configuring()
{
    const ConfigType configuration = this->getConfiguration();
    m_originLabel    = configuration.get<std::string>("originLabel", m_originLabel);
    m_endLabel       = configuration.get<std::string>("endLabel", m_endLabel);
    m_groupLabel     = configuration.get<std::string>("computedLandmarkLabel", m_groupLabel);
    m_tolerance      = configuration.get<double>("sameAxialSliceTolerance", m_tolerance);
    m_sameSliceLabel = configuration.get<std::string>("sameAxialSliceLabel", m_sameSliceLabel);
}

// -----------------------------------------------------------------------------

void SPointToLandmarkVector::updating()
{
    auto transform         = m_transform.lock();
    auto translationMatrix = m_translationMatrix.lock();
    const auto landmark    = m_landmark.lock();
    std::array<double, 3> sourcePoint {};
    std::array<double, 3> targetPoint {};
    if(landmark->getGroup(m_originLabel).m_size >= 1)
    {
        sourcePoint = landmark->getPoint(m_originLabel, 0);
    }

    if(landmark->getGroup(m_endLabel).m_size >= 1)
    {
        targetPoint = landmark->getPoint(m_endLabel, 0);
    }

    if(std::abs(sourcePoint[2] - targetPoint[2]) < m_tolerance)
    {
        this->signal<SameSliceSignalType>(SAME_SLICE_SIG)->async_emit(m_sameSliceLabel + ": Yes");
    }
    else
    {
        this->signal<SameSliceSignalType>(SAME_SLICE_SIG)->async_emit(m_sameSliceLabel + ": No");
    }

    // Compute the vector and put the result in the translation part of the matrix.
    const glm::dvec3 sourcePt(sourcePoint[0], sourcePoint[1], sourcePoint[2]);
    const glm::dvec3 targetPt(targetPoint[0], targetPoint[1], targetPoint[2]);
    const glm::dvec3 pointToTarget = targetPt - sourcePt;
    const auto length              = static_cast<float>(glm::length(pointToTarget));
    this->signal<LengthChangedSignalType>(LENGTH_CHANGED_SIG)->async_emit(length);
    const std::string lengthStr = std::to_string(length) + " mm";
    this->signal<LengthStrChangedSignalType>(LENGTH_STR_CHANGED_SIG)->async_emit(lengthStr);

    glm::dmat4x4 pointToTargetMat(1.0);
    const glm::dvec3 front = glm::normalize(pointToTarget);
    // compute an orthogonal vector to front ( vec(a,b,c) --> vecOrtho(-b,a,0))
    glm::dvec3 up          = glm::dvec3(-front[1], front[0], 0);
    const glm::dvec3 right = glm::normalize(cross(up, front));
    up = glm::cross(front, right);

    pointToTargetMat[0] = glm::dvec4(right, 0.0);
    pointToTargetMat[1] = glm::dvec4(up, 0.0);
    pointToTargetMat[2] = glm::dvec4(front, 0.0);
    pointToTargetMat[3] = glm::dvec4(sourcePt, 1.0);

    sight::geometry::data::setTF3DFromMatrix(*transform, pointToTargetMat);
    auto sig = transform->signal<data::Object::ModifiedSignalType>(data::Object::MODIFIED_SIG);
    sig->async_emit();

    // Create the computed landmark containing the position of the target point

    auto computedLandmark = m_computedLandmark.lock();
    if(computedLandmark->getGroup(m_groupLabel).m_size > 0)
    {
        computedLandmark->clearPoints(m_groupLabel);
    }

    computedLandmark->addPoint(m_groupLabel, targetPoint);

    auto sig1 = computedLandmark->signal<data::Landmarks::PointAddedSignalType>(data::Landmarks::POINT_ADDED_SIG);
    sig1->async_emit(m_groupLabel);

    (*translationMatrix)(0, 3) = pointToTarget[0];
    (*translationMatrix)(1, 3) = pointToTarget[1];
    (*translationMatrix)(2, 3) = pointToTarget[2];

    auto sig2 = translationMatrix->signal<data::Object::ModifiedSignalType>(data::Object::MODIFIED_SIG);
    sig2->async_emit();
}

// -----------------------------------------------------------------------------

service::connections_t SPointToLandmarkVector::getAutoConnections() const
{
    return {{s_LANDMARK_INPUT, data::Landmarks::POINT_ADDED_SIG, service::slots::UPDATE}};
}

// -----------------------------------------------------------------------------

} // namespace sight::module::geometry
