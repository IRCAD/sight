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

#include "module/geometry/__/point_to_landmark_vector.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <geometry/data/matrix4.hpp>

#include <glm/glm.hpp>

namespace sight::module::geometry
{

// -----------------------------------------------------------------------------

static const core::com::signals::key_t LENGTH_CHANGED_SIG     = "lengthChanged";
static const core::com::signals::key_t LENGTH_STR_CHANGED_SIG = "lengthChangedStr";
static const core::com::signals::key_t SAME_SLICE_SIG         = "sameSlice";

// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

point_to_landmark_vector::point_to_landmark_vector() noexcept
{
    new_signal<length_changed_signal_t>(LENGTH_CHANGED_SIG);
    new_signal<length_str_changed_signal_t>(LENGTH_STR_CHANGED_SIG);
    new_signal<same_slice_signal_t>(SAME_SLICE_SIG);
}

// -----------------------------------------------------------------------------

point_to_landmark_vector::~point_to_landmark_vector() noexcept =
    default;

// -----------------------------------------------------------------------------

void point_to_landmark_vector::starting()
{
    auto computed_landmark = m_computed_landmark.lock();
    computed_landmark->add_group(m_group_label);
}

// -----------------------------------------------------------------------------

void point_to_landmark_vector::stopping()
{
}

// -----------------------------------------------------------------------------

void point_to_landmark_vector::configuring()
{
    const config_t configuration = this->get_config();
    m_origin_label     = configuration.get<std::string>("originLabel", m_origin_label);
    m_end_label        = configuration.get<std::string>("endLabel", m_end_label);
    m_group_label      = configuration.get<std::string>("computedLandmarkLabel", m_group_label);
    m_tolerance        = configuration.get<double>("sameAxialSliceTolerance", m_tolerance);
    m_same_slice_label = configuration.get<std::string>("sameAxialSliceLabel", m_same_slice_label);
}

// -----------------------------------------------------------------------------

void point_to_landmark_vector::updating()
{
    auto transform          = m_transform.lock();
    auto translation_matrix = m_translation_matrix.lock();
    const auto landmark     = m_landmark.lock();
    std::array<double, 3> source_point {};
    std::array<double, 3> target_point {};
    if(landmark->get_group(m_origin_label).m_size >= 1)
    {
        source_point = landmark->get_point(m_origin_label, 0);
    }

    if(landmark->get_group(m_end_label).m_size >= 1)
    {
        target_point = landmark->get_point(m_end_label, 0);
    }

    if(std::abs(source_point[2] - target_point[2]) < m_tolerance)
    {
        this->signal<same_slice_signal_t>(SAME_SLICE_SIG)->async_emit(m_same_slice_label + ": Yes");
    }
    else
    {
        this->signal<same_slice_signal_t>(SAME_SLICE_SIG)->async_emit(m_same_slice_label + ": No");
    }

    // Compute the vector and put the result in the translation part of the matrix.
    const glm::dvec3 source_pt(source_point[0], source_point[1], source_point[2]);
    const glm::dvec3 target_pt(target_point[0], target_point[1], target_point[2]);
    const glm::dvec3 point_to_target = target_pt - source_pt;
    const auto length                = static_cast<float>(glm::length(point_to_target));
    this->signal<length_changed_signal_t>(LENGTH_CHANGED_SIG)->async_emit(length);
    const std::string length_str = std::to_string(length) + " mm";
    this->signal<length_str_changed_signal_t>(LENGTH_STR_CHANGED_SIG)->async_emit(length_str);

    glm::dmat4x4 point_to_target_mat(1.0);
    const glm::dvec3 front = glm::normalize(point_to_target);
    // compute an orthogonal vector to front ( vec(a,b,c) --> vecOrtho(-b,a,0))
    glm::dvec3 up          = glm::dvec3(-front[1], front[0], 0);
    const glm::dvec3 right = glm::normalize(cross(up, front));
    up = glm::cross(front, right);

    point_to_target_mat[0] = glm::dvec4(right, 0.0);
    point_to_target_mat[1] = glm::dvec4(up, 0.0);
    point_to_target_mat[2] = glm::dvec4(front, 0.0);
    point_to_target_mat[3] = glm::dvec4(source_pt, 1.0);

    sight::geometry::data::from_glm_mat(*transform, point_to_target_mat);
    auto sig = transform->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
    sig->async_emit();

    // Create the computed landmark containing the position of the target point

    auto computed_landmark = m_computed_landmark.lock();
    if(computed_landmark->get_group(m_group_label).m_size > 0)
    {
        computed_landmark->clear_points(m_group_label);
    }

    computed_landmark->add_point(m_group_label, target_point);

    auto sig1 = computed_landmark->signal<data::landmarks::point_added_signal_t>(data::landmarks::POINT_ADDED_SIG);
    sig1->async_emit(m_group_label);

    (*translation_matrix)(0, 3) = point_to_target[0];
    (*translation_matrix)(1, 3) = point_to_target[1];
    (*translation_matrix)(2, 3) = point_to_target[2];

    auto sig2 = translation_matrix->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
    sig2->async_emit();
}

// -----------------------------------------------------------------------------

service::connections_t point_to_landmark_vector::auto_connections() const
{
    return {{LANDMARK_INPUT, data::landmarks::POINT_ADDED_SIG, service::slots::UPDATE}};
}

// -----------------------------------------------------------------------------

} // namespace sight::module::geometry
