/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "modules/geometry/base/SPointToLandmarkDistance.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <data/Landmarks.hpp>
#include <data/String.hpp>
#include <data/TransformationMatrix3D.hpp>

#include <service/macros.hpp>

#include <geometry/data/TransformationMatrix3D.hpp>

namespace sight::modules::geometry::base
{

// -----------------------------------------------------------------------------

fwServicesRegisterMacro( ::sight::service::IService, ::sight::modules::geometry::base::SPointToLandmarkDistance )

static const core::com::Signals::SignalKeyType DISTANCE_CHANGED_SIG = "distanceChanged";
const core::com::Slots::SlotKeyType s_SELECTED_POINT_SLOT = "updateSelectedPoint";
const core::com::Slots::SlotKeyType s_UPDATE_POINT_SLOT   = "updatePoint";
const core::com::Slots::SlotKeyType s_REMOVE_POINT_SLOT   = "removePoint";

// -----------------------------------------------------------------------------

SPointToLandmarkDistance::SPointToLandmarkDistance() noexcept :
    m_landmarkSelected(false)
{
    newSignal< DistanceChangedSignalType>(DISTANCE_CHANGED_SIG);
    newSlot(s_SELECTED_POINT_SLOT, &SPointToLandmarkDistance::updateSelectedPoint, this);
    newSlot(s_UPDATE_POINT_SLOT, &SPointToLandmarkDistance::updatePoint, this);
    newSlot(s_REMOVE_POINT_SLOT, &SPointToLandmarkDistance::removePoint, this);
}

// -----------------------------------------------------------------------------

SPointToLandmarkDistance::~SPointToLandmarkDistance() noexcept
{
}

// -----------------------------------------------------------------------------

void SPointToLandmarkDistance::configuring()
{
    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>.");
    m_unit      = config.get<std::string>("unit", "");
    m_precision = config.get< int >("precision", 6);
    if(!m_unit.empty())
    {
        m_unit = " " + m_unit;
    }
}

// -----------------------------------------------------------------------------

void SPointToLandmarkDistance::starting()
{

}

// -----------------------------------------------------------------------------

void SPointToLandmarkDistance::stopping()
{
}

// -----------------------------------------------------------------------------

void SPointToLandmarkDistance::updating()
{
    if(m_landmarkSelected)
    {
        const auto pointMat =
            this->getLockedInput< data::TransformationMatrix3D>("pointMatrix");
        auto pointToLandmarkMatLocked =
            this->getLockedInOut< data::TransformationMatrix3D>("pointToLandmarkMatrix");
        auto pointToLandmarkMat          = pointToLandmarkMatLocked.get_shared();
        const auto distanceText          = this->getLockedInOut< data::String>("distanceText");
        const ::glm::dmat4x4 pointMatrix = sight::geometry::data::getMatrixFromTF3D(
            pointMat.get_shared());
        const ::glm::dvec4 originPoint(0.0, 0.0, 0.0, 1.0);
        const ::glm::dvec3 point = ::glm::dvec3(pointMatrix*originPoint);

        const ::glm::dvec3 direction = m_currentLandmark - point;
        const float length           = static_cast<float>(::glm::length(direction));

        std::ostringstream out;
        out.precision(m_precision);
        out << std::fixed << length;
        distanceText->setValue(out.str() + m_unit);

        // notify that distance is modified
        this->signal< DistanceChangedSignalType>(DISTANCE_CHANGED_SIG)->asyncEmit(static_cast<float>(length));
        // notify that text distance is modified
        distanceText->signal< data::Object::ModifiedSignalType>( data::Object::s_MODIFIED_SIG )->asyncEmit();

        // compute the matrix
        ::glm::dmat4x4 cameraMatrix;

        const ::glm::dvec3 front = ::glm::normalize(direction);
        // compute an orthogonal vector to front ( vec(a,b,c) --> vecOrtho(-b,a,0))
        ::glm::dvec3 up = ::glm::dvec3(-front[1], front[0], 0);
        const ::glm::dvec3 right = ::glm::normalize(cross(up, front));
        up = ::glm::cross(front, right);

        cameraMatrix[0] = ::glm::dvec4(right, 0.0);
        cameraMatrix[1] = ::glm::dvec4(up, 0.0);
        cameraMatrix[2] = ::glm::dvec4(front, 0.0);
        cameraMatrix[3] = ::glm::dvec4(point, 1.0);
        sight::geometry::data::setTF3DFromMatrix(pointToLandmarkMat,
                                                 cameraMatrix);
        auto sig =
            pointToLandmarkMat->signal< data::Object::ModifiedSignalType >(data::Object::s_MODIFIED_SIG);
        {
            core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
            sig->asyncEmit();
        }
    }
}

// -----------------------------------------------------------------------------
void SPointToLandmarkDistance::updateSelectedPoint(std::string name, size_t index)
{
    m_landmarkSelected = true;

    const auto landmark = this->getLockedInput< data::Landmarks>("landmark");

    const data::Landmarks::PointType& point = landmark->getPoint(name, index);
    for(int i = 0; i < 3; ++i)
    {
        m_currentLandmark[i] = point[i];
    }
    this->update();

}
// -----------------------------------------------------------------------------
void SPointToLandmarkDistance::updatePoint(std::string name)
{
    m_landmarkSelected = true;

    const auto landmark                     = this->getLockedInput< data::Landmarks>("landmark");
    size_t size                             = landmark->getGroup(name).m_points.size();
    const data::Landmarks::PointType& point = landmark->getPoint(name, size-1);
    for(int i = 0; i < 3; ++i)
    {
        m_currentLandmark[i] = point[i];
    }
    this->update();
}

// -----------------------------------------------------------------------------
void SPointToLandmarkDistance::removePoint()
{
    // When a point is removed, it's not selected anymore
    m_landmarkSelected = false;
    // Notify that distance is modified
    this->signal< DistanceChangedSignalType>(DISTANCE_CHANGED_SIG)->asyncEmit(static_cast<float>(0.0));

    auto distanceText = this->getLockedInOut< data::String>("distanceText");
    distanceText->setValue("");

    // notify that text distance is modified
    distanceText->signal< data::Object::ModifiedSignalType>( data::Object::s_MODIFIED_SIG )->asyncEmit();
}

// -----------------------------------------------------------------------------
} // maths
