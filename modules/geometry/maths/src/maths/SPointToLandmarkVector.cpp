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

#include "maths/SPointToLandmarkVector.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <data/PointList.hpp>
#include <data/tools/TransformationMatrix3D.hpp>
#include <data/TransformationMatrix3D.hpp>

#include <glm/glm.hpp>

#include <services/macros.hpp>

namespace maths
{

// -----------------------------------------------------------------------------

const services::IService::KeyType s_LANDMARK_INPUT                    = "landmark";
const services::IService::KeyType s_TRANSFORM_INOUT                   = "transform";
const services::IService::KeyType s_TRANSLATION_INOUT                 = "translationMatrix";
const services::IService::KeyType s_COMPUTED_LANDMARK_INOUT           = "computedLandmark";
static const core::com::Signals::SignalKeyType LENGTH_CHANGED_SIG     = "lengthChanged";
static const core::com::Signals::SignalKeyType LENGTH_STR_CHANGED_SIG = "lengthChangedStr";
static const core::com::Signals::SignalKeyType SAME_SLICE_SIG         = "sameSlice";

// -----------------------------------------------------------------------------

fwServicesRegisterMacro( ::sight::services::IService, ::maths::SPointToLandmarkVector )

// -----------------------------------------------------------------------------

SPointToLandmarkVector::SPointToLandmarkVector() noexcept
{
    newSignal< LengthChangedSignalType >(LENGTH_CHANGED_SIG);
    newSignal< LengthStrChangedSignalType >(LENGTH_STR_CHANGED_SIG);
    newSignal< SameSliceSignalType >(SAME_SLICE_SIG);
}

// -----------------------------------------------------------------------------

SPointToLandmarkVector::~SPointToLandmarkVector() noexcept
{
}

// -----------------------------------------------------------------------------

void SPointToLandmarkVector::starting()
{
    auto computedLandmarkLocked = this->getLockedInOut< data::Landmarks >(s_COMPUTED_LANDMARK_INOUT);
    m_computedLandmark = computedLandmarkLocked.get_shared();
    m_computedLandmark->addGroup(m_groupLabel);
}

// -----------------------------------------------------------------------------

void SPointToLandmarkVector::stopping()
{
}

// -----------------------------------------------------------------------------

void SPointToLandmarkVector::configuring()
{
    const ConfigType configuration = this->getConfigTree();
    m_originLabel    = configuration.get< std::string >("originLabel", m_originLabel);
    m_endLabel       = configuration.get< std::string >("endLabel", m_endLabel);
    m_groupLabel     = configuration.get< std::string >("computedLandmarkLabel", m_groupLabel);
    m_tolerance      = configuration.get< double >("sameAxialSliceTolerance", m_tolerance);
    m_sameSliceLabel = configuration.get< std::string >("sameAxialSliceLabel", m_sameSliceLabel);
}

// -----------------------------------------------------------------------------

void SPointToLandmarkVector::updating()
{
    auto transformLocked = this->getLockedInOut< data::TransformationMatrix3D >(s_TRANSFORM_INOUT);
    auto transform = transformLocked.get_shared();
    auto translationMatrix = this->getLockedInOut< data::TransformationMatrix3D >(s_TRANSLATION_INOUT);
    const auto landmark = this->getLockedInput< data::Landmarks >(s_LANDMARK_INPUT);
    std::array< double, 3> sourcePoint, targetPoint;
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
        this->signal< SameSliceSignalType >(SAME_SLICE_SIG)->asyncEmit(m_sameSliceLabel + ": Yes");
    }
    else
    {
        this->signal< SameSliceSignalType >(SAME_SLICE_SIG)->asyncEmit(m_sameSliceLabel + ": No");
    }

    // Compute the vector and put the result in the translation part of the matrix.
    const ::glm::dvec3 sourcePt(sourcePoint[0], sourcePoint[1], sourcePoint[2]);
    const ::glm::dvec3 targetPt(targetPoint[0], targetPoint[1], targetPoint[2]);
    const ::glm::dvec3 pointToTarget = targetPt - sourcePt;
    const float length               = static_cast<float>(::glm::length(pointToTarget));
    this->signal< LengthChangedSignalType>(LENGTH_CHANGED_SIG)->asyncEmit(length);
    const std::string lengthStr = std::to_string(length) + " mm";
    this->signal< LengthStrChangedSignalType>(LENGTH_STR_CHANGED_SIG)->asyncEmit(lengthStr);

    ::glm::dmat4x4 pointToTargetMat(1.0);
    const ::glm::dvec3 front = ::glm::normalize(pointToTarget);
    // compute an orthogonal vector to front ( vec(a,b,c) --> vecOrtho(-b,a,0))
    ::glm::dvec3 up = ::glm::dvec3(-front[1], front[0], 0);
    const ::glm::dvec3 right = ::glm::normalize(cross(up, front));
    up = ::glm::cross(front, right);

    pointToTargetMat[0] = ::glm::dvec4(right, 0.0);
    pointToTargetMat[1] = ::glm::dvec4(up, 0.0);
    pointToTargetMat[2] = ::glm::dvec4(front, 0.0);
    pointToTargetMat[3] = ::glm::dvec4(sourcePt, 1.0);

    data::tools::TransformationMatrix3D::setTF3DFromMatrix(transform, pointToTargetMat);
    auto sig = transform->signal< data::Object::ModifiedSignalType >(data::Object::s_MODIFIED_SIG);
    sig->asyncEmit();

    // Create the computed landmark containing the position of the target point
    if(m_computedLandmark->getGroup(m_groupLabel).m_size > 0)
    {
        m_computedLandmark->clearPoints(m_groupLabel);
    }

    m_computedLandmark->addPoint(m_groupLabel, targetPoint);

    auto sig1 = m_computedLandmark->signal< data::Landmarks::PointAddedSignalType >(
        data::Landmarks::s_POINT_ADDED_SIG);
    sig1->asyncEmit(m_groupLabel);

    translationMatrix->setCoefficient(0, 3, pointToTarget[0]);
    translationMatrix->setCoefficient(1, 3, pointToTarget[1]);
    translationMatrix->setCoefficient(2, 3, pointToTarget[2]);

    auto sig2 = translationMatrix->signal< data::Object::ModifiedSignalType >(data::Object::s_MODIFIED_SIG);
    sig2->asyncEmit();

}

// -----------------------------------------------------------------------------

services::IService::KeyConnectionsMap SPointToLandmarkVector::getAutoConnections() const
{

    services::IService::KeyConnectionsMap connections;
    connections.push(s_LANDMARK_INPUT, data::Landmarks::s_POINT_ADDED_SIG, s_UPDATE_SLOT);
    return connections;
}

// -----------------------------------------------------------------------------

} // maths
