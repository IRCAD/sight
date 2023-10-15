/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

#include "modules/geometry/__/point_to_landmark_distance.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <geometry/data/matrix4.hpp>
namespace sight::module::geometry
{

// -----------------------------------------------------------------------------

static const core::com::signals::key_t DISTANCE_CHANGED_SIG = "distanceChanged";
const core::com::slots::key_t SELECTED_POINT_SLOT           = "updateSelectedPoint";
const core::com::slots::key_t UPDATE_POINT_SLOT             = "updatePoint";
const core::com::slots::key_t REMOVE_POINT_SLOT             = "removePoint";

// -----------------------------------------------------------------------------

point_to_landmark_distance::point_to_landmark_distance() noexcept
{
    new_signal<DistanceChangedSignalType>(DISTANCE_CHANGED_SIG);
    new_slot(SELECTED_POINT_SLOT, &point_to_landmark_distance::updateSelectedPoint, this);
    new_slot(UPDATE_POINT_SLOT, &point_to_landmark_distance::updatePoint, this);
    new_slot(REMOVE_POINT_SLOT, &point_to_landmark_distance::removePoint, this);
}

// -----------------------------------------------------------------------------

point_to_landmark_distance::~point_to_landmark_distance() noexcept =
    default;

// -----------------------------------------------------------------------------

void point_to_landmark_distance::configuring()
{
    const config_t config = this->get_config().get_child("config.<xmlattr>.");
    m_unit      = config.get<std::string>("unit", "");
    m_precision = config.get<int>("precision", 6);
    if(!m_unit.empty())
    {
        m_unit = " " + m_unit;
    }
}

// -----------------------------------------------------------------------------

void point_to_landmark_distance::starting()
{
}

// -----------------------------------------------------------------------------

void point_to_landmark_distance::stopping()
{
}

// -----------------------------------------------------------------------------

void point_to_landmark_distance::updating()
{
    if(m_landmarkSelected)
    {
        const auto pointMat            = m_pointMatrix.lock();
        auto pointToLandmarkMat        = m_pointToLandmarkMatrix.lock();
        const auto distanceText        = m_distanceText.lock();
        const glm::dmat4x4 pointMatrix = sight::geometry::data::getMatrixFromTF3D(
            *pointMat
        );
        const glm::dvec4 originPoint(0.0, 0.0, 0.0, 1.0);
        const glm::dvec3 point = glm::dvec3(pointMatrix * originPoint);

        const glm::dvec3 direction = m_currentLandmark - point;
        const auto length          = static_cast<float>(glm::length(direction));

        std::ostringstream out;
        out.precision(m_precision);
        out << std::fixed << length;
        distanceText->setValue(out.str() + m_unit);

        // notify that distance is modified
        this->signal<DistanceChangedSignalType>(DISTANCE_CHANGED_SIG)->async_emit(static_cast<float>(length));
        // notify that text distance is modified
        distanceText->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG)->async_emit();

        // compute the matrix
        glm::dmat4x4 cameraMatrix;

        const glm::dvec3 front = glm::normalize(direction);
        // compute an orthogonal vector to front ( vec(a,b,c) --> vecOrtho(-b,a,0))
        glm::dvec3 up          = glm::dvec3(-front[1], front[0], 0);
        const glm::dvec3 right = glm::normalize(cross(up, front));
        up = glm::cross(front, right);

        cameraMatrix[0] = glm::dvec4(right, 0.0);
        cameraMatrix[1] = glm::dvec4(up, 0.0);
        cameraMatrix[2] = glm::dvec4(front, 0.0);
        cameraMatrix[3] = glm::dvec4(point, 1.0);
        sight::geometry::data::setTF3DFromMatrix(
            *pointToLandmarkMat,
            cameraMatrix
        );
        auto sig =
            pointToLandmarkMat->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
        {
            core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
            sig->async_emit();
        }
    }
}

// -----------------------------------------------------------------------------
void point_to_landmark_distance::updateSelectedPoint(std::string name, std::size_t index)
{
    m_landmarkSelected = true;

    const auto landmark = m_landmark.lock();

    const data::landmarks::PointType& point = landmark->getPoint(name, index);
    for(int i = 0 ; i < 3 ; ++i)
    {
        m_currentLandmark[i] = point[std::size_t(i)];
    }

    this->update();
}

// -----------------------------------------------------------------------------
void point_to_landmark_distance::updatePoint(std::string name)
{
    m_landmarkSelected = true;

    const auto landmark                     = m_landmark.lock();
    std::size_t size                        = landmark->getGroup(name).m_points.size();
    const data::landmarks::PointType& point = landmark->getPoint(name, size - 1);
    for(int i = 0 ; i < 3 ; ++i)
    {
        m_currentLandmark[i] = point[std::size_t(i)];
    }

    this->update();
}

// -----------------------------------------------------------------------------
void point_to_landmark_distance::removePoint()
{
    // When a point is removed, it's not selected anymore
    m_landmarkSelected = false;
    // Notify that distance is modified
    this->signal<DistanceChangedSignalType>(DISTANCE_CHANGED_SIG)->async_emit(static_cast<float>(0.0));

    auto distanceText = m_distanceText.lock();
    distanceText->setValue("");

    // notify that text distance is modified
    distanceText->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG)->async_emit();
}

// -----------------------------------------------------------------------------

} // namespace sight::module::geometry
