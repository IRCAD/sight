/************************************************************************
 *
 * Copyright (C) 2017-2025 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "pose_from2d.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <geometry/vision/helper.hpp>

#include <io/opencv/camera.hpp>

//-----------------------------------------------------------------------------

namespace sight::module::geometry::vision
{

static const std::string UPDATE_CAMERA_SLOT = "updateCamera";

//-----------------------------------------------------------------------------

pose_from2d::pose_from2d() noexcept
{
    new_slot(UPDATE_CAMERA_SLOT, &pose_from2d::initialize, this);
}

//-----------------------------------------------------------------------------

void pose_from2d::configuring()
{
    service::config_t config = this->get_config();
    m_pattern_width = config.get<double>("pattern_width", m_pattern_width);
    SIGHT_ASSERT("pattern_width setting is set to " << m_pattern_width << " but should be > 0.", m_pattern_width > 0);

    auto inout_cfg = config.equal_range("inout");
    for(auto it_cfg = inout_cfg.first ; it_cfg != inout_cfg.second ; ++it_cfg)
    {
        const auto group = it_cfg->second.get<std::string>("<xmlattr>.group");
        if(group == MATRIX_INOUT)
        {
            auto key_cfg = it_cfg->second.equal_range("key");
            for(auto it_key_cfg = key_cfg.first ; it_key_cfg != key_cfg.second ; ++it_key_cfg)
            {
                const auto key = it_key_cfg->second.get<std::string>("<xmlattr>.id");
                m_matrices_tag.push_back(key);
            }

            break;
        }
    }
}

//-----------------------------------------------------------------------------

void pose_from2d::starting()
{
    //3D Points
    const float half_width = static_cast<float>(m_pattern_width) * .5F;

    m_3d_model.emplace_back(-half_width, half_width, 0.F);
    m_3d_model.emplace_back(half_width, half_width, 0.F);
    m_3d_model.emplace_back(half_width, -half_width, 0.F);
    m_3d_model.emplace_back(-half_width, -half_width, 0.F);

    auto pl = m_point_list.lock();
    if(pl)
    {
        for(std::size_t i = 0 ; i < m_3d_model.size() ; ++i)
        {
            const cv::Point3f cv_point    = m_3d_model.at(i);
            const data::point::sptr point = std::make_shared<data::point>(cv_point.x, cv_point.y, cv_point.z);
            point->set_label(std::to_string(i));
            pl->push_back(point);
        }

        auto sig = pl->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
        sig->async_emit();
    }

    this->initialize();
}

//-----------------------------------------------------------------------------

void pose_from2d::stopping()
{
    m_cameras.clear();
    m_3d_model.clear();
    m_last_timestamp = 0;

    auto pl = m_point_list.lock();
    if(pl)
    {
        pl->clear();
        auto sig = pl->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
        sig->async_emit();
    }
}

//-----------------------------------------------------------------------------

void pose_from2d::updating()
{
    // When working with a frame (newest design), we do not rely on the timetamp
    // So we can just send the current one.
    // When removing timelines from the service then we could get rid of it
    auto timestamp = core::clock::get_time_in_milli_sec();
    this->compute_registration(timestamp);
}

//-----------------------------------------------------------------------------

void pose_from2d::compute_registration(core::clock::type /*timestamp*/)
{
    SIGHT_WARN_IF("Invoking computeRegistration while service is STOPPED", this->stopped());

    if(this->started())
    {
        // For each marker
        unsigned int marker_index = 0;
        for(const auto& marker_key : m_matrices_tag)
        {
            std::vector<marker> markers;

            // For each camera timeline
            for(const auto& marker_map : m_marker_map)
            {
                const auto marker_ptr = marker_map.second->lock();
                const auto* marker    = marker_ptr->get_marker(marker_key);

                if(marker != nullptr)
                {
                    struct marker current_marker;
                    for(std::size_t i = 0 ; i < 4 ; ++i)
                    {
                        current_marker.corners_2d.emplace_back((*marker)[i][0], (*marker)[i][1]);
                    }

                    markers.push_back(current_marker);
                }
            }

            auto matrix = m_matrix[marker_index].lock();
            SIGHT_ASSERT("Matrix " << marker_index << " not found", matrix);

            if(markers.empty())
            {
                SIGHT_WARN("No Markers!")
            }
            else
            {
                cv::Matx44f rt;
                if(markers.size() == 1)
                {
                    rt = this->camera_pose_from_mono(markers[0]);
                }
                else if(markers.size() == 2)
                {
                    rt = this->camera_pose_from_stereo(markers[0], markers[1]);
                }
                else
                {
                    SIGHT_WARN("More than 2 cameras is not handle for the moment");
                    continue;
                }

                for(std::uint8_t i = 0 ; i < 4 ; ++i)
                {
                    for(std::uint8_t j = 0 ; j < 4 ; ++j)
                    {
                        (*matrix)[4 * i + j] = rt(i, j);
                    }
                }
            }

            // Always send the signal even if we did not find anything.
            // This allows to keep updating the whole processing pipeline.
            auto sig = matrix->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
            sig->async_emit();

            ++marker_index;
        }
    }
}

//-----------------------------------------------------------------------------

void pose_from2d::initialize()
{
    m_cameras.clear();
    for(std::size_t idx = 0 ; idx < m_camera.size() ; ++idx)
    {
        auto camera = m_camera[idx].lock();
        SIGHT_FATAL_IF("Camera[" << idx << "] not found", !camera);

        struct camera cam;
        std::tie(
            cam.intrinsic_mat,
            cam.image_size,
            cam.dist_coef
        ) = io::opencv::camera::copy_to_cv(camera.get_shared());

        // set extrinsic matrix only if stereo.
        if(idx == 1)
        {
            auto extrinsic_matrix = m_extrinsic.lock();
            SIGHT_FATAL_IF("Extrinsic matrix with key '" << EXTRINSIC_INPUT << "' not found", !extrinsic_matrix);

            m_extrinsic_mat.matrix4x4   = cv::Mat::eye(4, 4, CV_64F);
            m_extrinsic_mat.rotation    = cv::Mat::eye(3, 3, CV_64F);
            m_extrinsic_mat.translation = cv::Mat::eye(3, 1, CV_64F);

            for(std::uint8_t i = 0 ; i < 3 ; ++i)
            {
                for(std::uint8_t j = 0 ; j < 3 ; ++j)
                {
                    m_extrinsic_mat.rotation.at<double>(i, j)  = (*extrinsic_matrix)(i, j);
                    m_extrinsic_mat.matrix4x4.at<double>(i, j) = (*extrinsic_matrix)(i, j);
                }
            }

            m_extrinsic_mat.translation.at<double>(0, 0) = (*extrinsic_matrix)(0, 3);
            m_extrinsic_mat.translation.at<double>(1, 0) = (*extrinsic_matrix)(1, 3);
            m_extrinsic_mat.translation.at<double>(2, 0) = (*extrinsic_matrix)(2, 3);

            m_extrinsic_mat.matrix4x4.at<double>(0, 3) = (*extrinsic_matrix)(0, 3);
            m_extrinsic_mat.matrix4x4.at<double>(1, 3) = (*extrinsic_matrix)(1, 3);
            m_extrinsic_mat.matrix4x4.at<double>(2, 3) = (*extrinsic_matrix)(2, 3);
        }

        m_cameras.push_back(cam);
    }
}

//-----------------------------------------------------------------------------

cv::Matx44f pose_from2d::camera_pose_from_stereo(
    const pose_from2d::marker& _marker_cam1,
    const pose_from2d::marker& _marker_cam2
) const
{
    cv::Matx44f pose = sight::geometry::vision::helper::camera_pose_stereo(
        m_3d_model,
        m_cameras[0].intrinsic_mat,
        m_cameras[0].dist_coef,
        m_cameras[1].intrinsic_mat,
        m_cameras[1].dist_coef,
        _marker_cam1.corners_2d,
        _marker_cam2.corners_2d,
        m_extrinsic_mat.rotation,
        m_extrinsic_mat.translation
    );

    return pose;
}

//-----------------------------------------------------------------------------

cv::Matx44f pose_from2d::camera_pose_from_mono(const pose_from2d::marker& _marker_cam1) const
{
    cv::Matx44f pose =
        sight::geometry::vision::helper::camera_pose_monocular(
            m_3d_model,
            _marker_cam1.corners_2d,
            m_cameras[0].intrinsic_mat,
            m_cameras[0].dist_coef
        );
    return pose;
}

//-----------------------------------------------------------------------------

service::connections_t pose_from2d::auto_connections() const
{
    return {
        {marker_map_INPUT, data::object::MODIFIED_SIG, service::slots::UPDATE},
        {CAMERA_INPUT, data::object::MODIFIED_SIG, UPDATE_CAMERA_SLOT},
        {CAMERA_INPUT, data::camera::INTRINSIC_CALIBRATED_SIG, UPDATE_CAMERA_SLOT}
    };
}

//-----------------------------------------------------------------------------

} // namespace sight::module::geometry::vision
