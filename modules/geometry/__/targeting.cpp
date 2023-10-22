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

#include "modules/geometry/__/targeting.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <geometry/data/matrix4.hpp>

#include <service/macros.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/intersect.hpp>
#undef GLM_ENABLE_EXPERIMENTAL

namespace sight::module::geometry
{

// -----------------------------------------------------------------------------

const core::com::slots::key_t SELECTED_POINT_SLOT = "updateSelectedPoint";
const core::com::slots::key_t UPDATE_POINT_SLOT   = "updatePoint";
const core::com::slots::key_t REMOVE_POINT_SLOT   = "removePoint";

// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

targeting::targeting() noexcept :
    m_targetLandmark(glm::dvec3(0.0, 0.0, 0.0))
{
    new_slot(SELECTED_POINT_SLOT, &targeting::updateSelectedPoint, this);
    new_slot(UPDATE_POINT_SLOT, &targeting::updatePoint, this);
    new_slot(REMOVE_POINT_SLOT, &targeting::removePoint, this);
}

// -----------------------------------------------------------------------------

targeting::~targeting() noexcept =
    default;

// -----------------------------------------------------------------------------

void targeting::starting()
{
}

// -----------------------------------------------------------------------------

void targeting::stopping()
{
}

// -----------------------------------------------------------------------------

void targeting::configuring()
{
    const config_t configuration = this->get_config();
    m_label = configuration.get<std::string>("label", m_label);
    if(!m_label.empty())
    {
        m_landmarkSelected = true;
    }

    m_width = configuration.get<int>("width", m_width);
}

// -----------------------------------------------------------------------------

void targeting::updating()
{
    if(m_landmarkSelected)
    {
        const auto landmark = m_landmark.lock();
        SIGHT_ASSERT("Input \"landmark\" is missing.", landmark);
        if(!landmark->getGroup(m_label).m_points.empty())
        {
            const data::landmarks::point_t point = landmark->getPoint(m_label, m_index);
            m_targetLandmark = glm::dvec3(point[0], point[1], point[2]);
        }
        else
        {
            return;
        }
    }

    // Get the input matrix for the needle tip
    const auto matrix = m_matrix.lock();
    SIGHT_ASSERT("Input \"matrix\" is missing.", matrix);

    const glm::dmat4x4 mat = sight::geometry::data::to_glm_mat(*matrix);

    const glm::dvec4 origin(0.0, 0.0, 0.0, 1.0);

    const glm::dvec4 axis_x(1.0, 0.0, 0.0, 0.0);
    const glm::dvec4 axis_z(0.0, 0.0, 1.0, 0.0);

    // Compute the needle tip position
    const glm::dvec3 needle_tip = glm::dvec3(mat * origin);

    // Compute the needle orientation vectors
    const glm::dvec3 needle_tip_x = glm::dvec3(mat * axis_x);
    const glm::dvec3 needle_tip_z = glm::dvec3(mat * axis_z);

    // WARNING: this axis must be consistent with the orientation of the input matrix
    // For example:
    // - If you use a camera matrix, this must be the Z axis
    // - If you use an EM sensor from the trakSTAR, this must be the X axis
    const glm::dvec3 needle_direction = glm::normalize(needle_tip_z);

    const glm::dvec3 needle_tip_to_landmark = glm::normalize(m_targetLandmark - needle_tip);

    // Compute the intersection between the needle (from the tip) and the landmark plane
    double distance   = 0.0;
    double distance_x = 0.0;

    /* Project the needle tip origin and the associated X axis of the matrices on the landmark plane */
    /* To get a coordinate system on this plane (the Y axis will be obtained via a cross product) */
    if(glm::intersectRayPlane(needle_tip, needle_direction, m_targetLandmark, -needle_tip_to_landmark, distance)
       && glm::intersectRayPlane(
           needle_tip + needle_tip_x,
           needle_direction,
           m_targetLandmark,
           -needle_tip_to_landmark,
           distance_x
       )
       && !m_label.empty())
    {
        // Compute the 3D position of the intersection between the needle and the landmark plane
        const glm::dvec3 projected_needle_origin = needle_tip + needle_direction * distance;
        // Shift the needleTip with the axis vector
        const glm::dvec3 projected_needle_x = needle_tip + needle_tip_x + needle_direction * distance_x;

        const glm::dvec3 projected_x_axis = glm::normalize(projected_needle_x - projected_needle_origin);
        const glm::dvec3 projected_y_axis = glm::cross(needle_tip_to_landmark, projected_x_axis);

        // Compute the matrix from the world coordinates to the landmark plane coordinates
        glm::dmat4x4 world_to_plane_matrix;
        world_to_plane_matrix[0] = glm::dvec4(projected_x_axis, 0.0);
        world_to_plane_matrix[1] = glm::dvec4(projected_y_axis, 0.0);
        world_to_plane_matrix[2] = glm::dvec4(needle_tip_to_landmark, 0.0);
        world_to_plane_matrix[3] = glm::dvec4(m_targetLandmark, 1.0);

        // Invert the world to landmark plane matrix
        const glm::dmat4x4 plane_to_world_matrix = glm::affineInverse(world_to_plane_matrix);

        // Transform in the 2D landmark plane the needle intersection
        glm::dvec3 transformed_needle_intersection =
            glm::dvec3(plane_to_world_matrix * glm::dvec4(projected_needle_origin, 1.0));
        transformed_needle_intersection = glm::normalize(transformed_needle_intersection);

        // Get the distance between the projected needle point and the landmark position
        const double projected_needle_to_landmarkdistance = glm::distance(projected_needle_origin, m_targetLandmark);

        // Compute a scale value so that the vector will represent the correct distance value on the view
        const double max_distance = 50.0;
        double scale              = projected_needle_to_landmarkdistance / max_distance * m_width / 2.0;
        scale = (projected_needle_to_landmarkdistance > max_distance ? m_width / 2.0 : scale);

        transformed_needle_intersection = transformed_needle_intersection * scale;

        auto point_list = m_pointList.lock();
        SIGHT_ASSERT("InOut \"pointList\" is missing.", point_list);
        if(!point_list->getPoints().empty())
        {
            point_list->clear();
        }

        const data::point::sptr point = std::make_shared<data::point>(
            transformed_needle_intersection[0],
            -transformed_needle_intersection[1],
            0.
        );

        point_list->pushBack(point);

        auto sig = point_list->signal<data::point_list::point_added_signal_t>(
            data::point_list::POINT_ADDED_SIG
        );
        sig->async_emit(point);
    }
}

// -----------------------------------------------------------------------------

service::connections_t targeting::auto_connections() const
{
    return {{s_MATRIX_INPUT, data::object::MODIFIED_SIG, service::slots::UPDATE}};
}

// -----------------------------------------------------------------------------

void targeting::updateSelectedPoint(std::string _name, std::size_t _index)
{
    m_label            = _name;
    m_landmarkSelected = true;
    m_index            = _index;
    this->update();
}

// -----------------------------------------------------------------------------

void targeting::updatePoint(std::string _name)
{
    m_label            = _name;
    m_landmarkSelected = true;
    {
        const auto landmark = m_landmark.lock();
        SIGHT_ASSERT("Input \"landmark\" is missing.", landmark);

        const std::size_t size = landmark->getGroup(m_label).m_points.size();
        m_index = size - 1;
    }
    this->update();
}

// -----------------------------------------------------------------------------

void targeting::removePoint()
{
    // When a point is removed, it's not selected anymore
    m_landmarkSelected = false;

    auto point_list = m_pointList.lock();
    SIGHT_ASSERT("InOut \"pointList\" is missing.", point_list);
    auto points = point_list->getPoints(); // copy the points.
    point_list->clear();
    for(const auto& pt : points)
    {
        // Send signals.
        auto sig = point_list->signal<data::point_list::point_removed_signal_t>(
            data::point_list::POINT_REMOVED_SIG
        );
        sig->async_emit(pt);
    }
}

// -----------------------------------------------------------------------------

} // namespace sight::module::geometry
