/************************************************************************
 *
 * Copyright (C) 2018-2025 IRCAD France
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
    new_signal<distance_changed_signal_t>(DISTANCE_CHANGED_SIG);
    new_slot(SELECTED_POINT_SLOT, &point_to_landmark_distance::update_selected_point, this);
    new_slot(UPDATE_POINT_SLOT, &point_to_landmark_distance::update_point, this);
    new_slot(REMOVE_POINT_SLOT, &point_to_landmark_distance::remove_point, this);
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
    if(m_landmark_selected)
    {
        const auto point_mat            = m_point_matrix.lock();
        auto point_to_landmark_mat      = m_point_to_landmark_matrix.lock();
        const auto distance_text        = m_distance_text.lock();
        const glm::dmat4x4 point_matrix = sight::geometry::data::to_glm_mat(
            *point_mat
        );
        const glm::dvec4 origin_point(0.0, 0.0, 0.0, 1.0);
        const glm::dvec3 point = glm::dvec3(point_matrix * origin_point);

        const glm::dvec3 direction = m_current_landmark - point;
        const auto length          = static_cast<float>(glm::length(direction));

        std::ostringstream out;
        out.precision(m_precision);
        out << std::fixed << length;
        distance_text->set_value(out.str() + m_unit);

        // notify that distance is modified
        this->signal<distance_changed_signal_t>(DISTANCE_CHANGED_SIG)->async_emit(static_cast<float>(length));
        // notify that text distance is modified
        distance_text->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG)->async_emit();

        // compute the matrix
        glm::dmat4x4 camera_matrix;

        const glm::dvec3 front = glm::normalize(direction);
        // compute an orthogonal vector to front ( vec(a,b,c) --> vecOrtho(-b,a,0))
        glm::dvec3 up          = glm::dvec3(-front[1], front[0], 0);
        const glm::dvec3 right = glm::normalize(cross(up, front));
        up = glm::cross(front, right);

        camera_matrix[0] = glm::dvec4(right, 0.0);
        camera_matrix[1] = glm::dvec4(up, 0.0);
        camera_matrix[2] = glm::dvec4(front, 0.0);
        camera_matrix[3] = glm::dvec4(point, 1.0);
        sight::geometry::data::from_glm_mat(
            *point_to_landmark_mat,
            camera_matrix
        );
        auto sig =
            point_to_landmark_mat->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
        {
            core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
            sig->async_emit();
        }
    }
}

// -----------------------------------------------------------------------------
void point_to_landmark_distance::update_selected_point(std::string _name, std::size_t _index)
{
    m_landmark_selected = true;

    const auto landmark = m_landmark.lock();

    const data::landmarks::point_t& point = landmark->get_point(_name, _index);
    for(int i = 0 ; i < 3 ; ++i)
    {
        m_current_landmark[i] = point[std::size_t(i)];
    }

    this->update();
}

// -----------------------------------------------------------------------------
void point_to_landmark_distance::update_point(std::string _name)
{
    m_landmark_selected = true;

    const auto landmark                   = m_landmark.lock();
    std::size_t size                      = landmark->get_group(_name).m_points.size();
    const data::landmarks::point_t& point = landmark->get_point(_name, size - 1);
    for(int i = 0 ; i < 3 ; ++i)
    {
        m_current_landmark[i] = point[std::size_t(i)];
    }

    this->update();
}

// -----------------------------------------------------------------------------
void point_to_landmark_distance::remove_point()
{
    // When a point is removed, it's not selected anymore
    m_landmark_selected = false;
    // Notify that distance is modified
    this->signal<distance_changed_signal_t>(DISTANCE_CHANGED_SIG)->async_emit(static_cast<float>(0.0));

    auto distance_text = m_distance_text.lock();
    distance_text->set_value("");

    // notify that text distance is modified
    distance_text->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG)->async_emit();
}

// -----------------------------------------------------------------------------

} // namespace sight::module::geometry
