/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include "module/geometry/__/camera_path_to_view.hpp"

#include <data/fiducials_series.hpp>

#include <geometry/data/matrix4.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <algorithm>
#include <cmath>
#include <vector>
// cspell:ignore slerp lerp

namespace sight::module::geometry
{

namespace
{

struct path_segment
{
    glm::dvec3 direction;
    glm::dquat rotation;
    double length;
};

struct path_pose
{
    glm::dvec3 position;
    glm::dquat rotation;
};

} // namespace

//-----------------------------------------------------------------------------

static path_pose sample_path(const std::vector<glm::dvec3>& _points, double _distance)
{
    std::vector<path_segment> segments;
    segments.reserve(_points.size() - 1);

    double total_length = 0.0;
    for(std::size_t i = 1 ; i < _points.size() ; ++i)
    {
        const glm::dvec3 offset    = _points[i] - _points[i - 1];
        const double length        = glm::length(offset);
        const glm::dvec3 direction = offset / length;
        const glm::dquat rotation  = segments.empty()
                                     ? glm::rotation(glm::dvec3(0.0, 0.0, 1.0), direction)
                                     : glm::normalize(
            glm::rotation(segments.back().direction, direction)
            * segments.back().rotation
                                     );
        segments.push_back({.direction = direction, .rotation = rotation, .length = length});
        total_length                  += length;
    }

    double remaining = std::fmod(_distance, total_length);
    if(remaining < 0.0)
    {
        remaining += total_length;
    }

    std::size_t index = 0;
    while(index + 1 < segments.size() && remaining > segments[index].length)
    {
        remaining -= segments[index].length;
        ++index;
    }

    const auto& segment       = segments[index];
    const glm::dquat rotation = index + 1 < segments.size()
                                ? glm::slerp(segment.rotation, segments[index + 1].rotation, remaining / segment.length)
                                : segment.rotation;

    return {.position = _points[index] + segment.direction * remaining, .rotation = rotation};
}

//-----------------------------------------------------------------------------

camera_path_to_view::camera_path_to_view()
{
    new_slot(slots::PLAY, &camera_path_to_view::play, this);
    new_slot(slots::PAUSE, &camera_path_to_view::pause, this);
}

//-----------------------------------------------------------------------------

void camera_path_to_view::configuring()
{
    m_path_group = this->get_config().get<std::string>("config.<xmlattr>.path_group", m_path_group);
}

//-----------------------------------------------------------------------------

void camera_path_to_view::starting()
{
    m_timer = core::thread::get_default_worker()->create_timer();
    m_timer->set_function([this]{this->advance();});
    m_timer->set_duration(std::chrono::milliseconds(20));

    this->updating();
}

//-----------------------------------------------------------------------------

void camera_path_to_view::stopping()
{
    if(m_timer)
    {
        m_timer->stop();
        m_timer.reset();
    }
}

//-----------------------------------------------------------------------------

void camera_path_to_view::play()
{
    if(m_timer && !m_timer->is_running())
    {
        m_last_tick = std::chrono::steady_clock::now();
        m_timer->start();
    }
}

//-----------------------------------------------------------------------------

void camera_path_to_view::pause()
{
    if(m_timer && m_timer->is_running())
    {
        m_timer->stop();
    }
}

//-----------------------------------------------------------------------------

void camera_path_to_view::advance()
{
    const auto now     = std::chrono::steady_clock::now();
    const double sign  = *m_direction == direction::backward ? -1.0 : 1.0;
    const double speed = static_cast<double>(std::max<std::int64_t>(0, *m_speed_mm_s));
    {
        auto distance = m_distance.lock();
        distance->set_value(
            distance->value() + sign * speed * std::chrono::duration<double>(now - m_last_tick).count()
        );
        distance->async_emit(this, data::signals::MODIFIED);
    }

    m_last_tick = now;

    this->updating();
}

//-----------------------------------------------------------------------------

void camera_path_to_view::updating()
{
    const auto image = m_image.const_lock();
    if(!image || !image->get_fiducials())
    {
        return;
    }

    // Use only 3D point fiducials from the path group, preserving their order.
    const auto fiducials = image->get_fiducials()->query_fiducials(
        [](const data::fiducials_series::query_result& _result)
        {
            return _result.m_contour_data && _result.m_contour_data->size() == 3;
        },
        data::fiducials_series::shape::point,
        m_path_group
    );

    std::vector<glm::dvec3> points;
    points.reserve(fiducials.size());
    for(const auto& fiducial : fiducials)
    {
        const auto& position = *fiducial.m_contour_data;
        const glm::dvec3 point {position[0], position[1], position[2]};
        if(points.empty() || glm::distance(points.back(), point) > 0.0)
        {
            points.push_back(point);
        }
    }

    if(points.size() < 2)
    {
        return;
    }

    // Convert the distance along the path into the camera transform.
    const auto distance     = m_distance.const_lock();
    const auto sampled_pose = sample_path(points, distance->value());
    glm::dmat4 pose         = glm::mat4_cast(sampled_pose.rotation);
    pose[3] = glm::dvec4(sampled_pose.position, 1.0);

    auto transform = m_transform.lock();
    sight::geometry::data::from_glm_mat(*transform, pose);
    transform->async_emit(this, data::signals::MODIFIED);
}

//-----------------------------------------------------------------------------

service::connections_t camera_path_to_view::auto_connections() const
{
    return {{m_distance, data::signals::MODIFIED, service::slots::UPDATE}};
}

} // namespace sight::module::geometry
