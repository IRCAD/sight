/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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
    m_patternWidth = config.get<double>("patternWidth", m_patternWidth);
    SIGHT_ASSERT("patternWidth setting is set to " << m_patternWidth << " but should be > 0.", m_patternWidth > 0);

    auto inoutCfg = config.equal_range("inout");
    for(auto itCfg = inoutCfg.first ; itCfg != inoutCfg.second ; ++itCfg)
    {
        const auto group = itCfg->second.get<std::string>("<xmlattr>.group");
        if(group == s_MATRIX_INOUT)
        {
            auto keyCfg = itCfg->second.equal_range("key");
            for(auto itKeyCfg = keyCfg.first ; itKeyCfg != keyCfg.second ; ++itKeyCfg)
            {
                const auto key = itKeyCfg->second.get<std::string>("<xmlattr>.id");
                m_matricesTag.push_back(key);
            }

            break;
        }
    }
}

//-----------------------------------------------------------------------------

void pose_from2d::starting()
{
    //3D Points
    const float halfWidth = static_cast<float>(m_patternWidth) * .5F;

    m_3dModel.emplace_back(-halfWidth, halfWidth, 0.F);
    m_3dModel.emplace_back(halfWidth, halfWidth, 0.F);
    m_3dModel.emplace_back(halfWidth, -halfWidth, 0.F);
    m_3dModel.emplace_back(-halfWidth, -halfWidth, 0.F);

    auto pl = m_pointList.lock();
    if(pl)
    {
        for(std::size_t i = 0 ; i < m_3dModel.size() ; ++i)
        {
            const cv::Point3f cvPoint     = m_3dModel.at(i);
            const data::point::sptr point = std::make_shared<data::point>(cvPoint.x, cvPoint.y, cvPoint.z);
            point->setLabel(std::to_string(i));
            pl->pushBack(point);
        }

        auto sig = pl->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
        sig->async_emit();
    }

    this->initialize();
}

//-----------------------------------------------------------------------------

void pose_from2d::stopping()
{
    m_cameras.clear();
    m_3dModel.clear();
    m_lastTimestamp = 0;

    auto pl = m_pointList.lock();
    if(pl)
    {
        pl->clear();
        auto sig = pl->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
        sig->async_emit();
    }
}

//-----------------------------------------------------------------------------

void pose_from2d::updating()
{
    // When working with a frame (newest design), we do not rely on the timetamp
    // So we can just send the current one.
    // When removing timelines from the service then we could get rid of it
    auto timestamp = core::hires_clock::get_time_in_milli_sec();
    this->computeRegistration(timestamp);
}

//-----------------------------------------------------------------------------

void pose_from2d::computeRegistration(core::hires_clock::type /*timestamp*/)
{
    SIGHT_WARN_IF("Invoking computeRegistration while service is STOPPED", this->stopped());

    if(this->started())
    {
        // For each marker
        unsigned int markerIndex = 0;
        for(const auto& markerKey : m_matricesTag)
        {
            std::vector<Marker> markers;

            // For each camera timeline
            for(const auto& markerMap : m_markerMap)
            {
                const auto marker_ptr = markerMap.second->lock();
                const auto* marker    = marker_ptr->getMarker(markerKey);

                if(marker != nullptr)
                {
                    Marker currentMarker;
                    for(std::size_t i = 0 ; i < 4 ; ++i)
                    {
                        currentMarker.corners2D.emplace_back((*marker)[i][0], (*marker)[i][1]);
                    }

                    markers.push_back(currentMarker);
                }
            }

            auto matrix = m_matrix[markerIndex].lock();
            SIGHT_ASSERT("Matrix " << markerIndex << " not found", matrix);

            if(markers.empty())
            {
                SIGHT_WARN("No Markers!")
            }
            else
            {
                cv::Matx44f Rt;
                if(markers.size() == 1)
                {
                    Rt = this->cameraPoseFromMono(markers[0]);
                }
                else if(markers.size() == 2)
                {
                    Rt = this->cameraPoseFromStereo(markers[0], markers[1]);
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
                        (*matrix)[4 * i + j] = Rt(i, j);
                    }
                }
            }

            // Always send the signal even if we did not find anything.
            // This allows to keep updating the whole processing pipeline.
            auto sig = matrix->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
            sig->async_emit();

            ++markerIndex;
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
        std::tie(cam.intrinsicMat, cam.imageSize, cam.distCoef) = io::opencv::camera::copyToCv(camera.get_shared());

        // set extrinsic matrix only if stereo.
        if(idx == 1)
        {
            auto extrinsicMatrix = m_extrinsic.lock();
            SIGHT_FATAL_IF("Extrinsic matrix with key '" << s_EXTRINSIC_INPUT << "' not found", !extrinsicMatrix);

            m_extrinsicMat.Matrix4x4   = cv::Mat::eye(4, 4, CV_64F);
            m_extrinsicMat.rotation    = cv::Mat::eye(3, 3, CV_64F);
            m_extrinsicMat.translation = cv::Mat::eye(3, 1, CV_64F);

            for(std::uint8_t i = 0 ; i < 3 ; ++i)
            {
                for(std::uint8_t j = 0 ; j < 3 ; ++j)
                {
                    m_extrinsicMat.rotation.at<double>(i, j)  = (*extrinsicMatrix)(i, j);
                    m_extrinsicMat.Matrix4x4.at<double>(i, j) = (*extrinsicMatrix)(i, j);
                }
            }

            m_extrinsicMat.translation.at<double>(0, 0) = (*extrinsicMatrix)(0, 3);
            m_extrinsicMat.translation.at<double>(1, 0) = (*extrinsicMatrix)(1, 3);
            m_extrinsicMat.translation.at<double>(2, 0) = (*extrinsicMatrix)(2, 3);

            m_extrinsicMat.Matrix4x4.at<double>(0, 3) = (*extrinsicMatrix)(0, 3);
            m_extrinsicMat.Matrix4x4.at<double>(1, 3) = (*extrinsicMatrix)(1, 3);
            m_extrinsicMat.Matrix4x4.at<double>(2, 3) = (*extrinsicMatrix)(2, 3);
        }

        m_cameras.push_back(cam);
    }
}

//-----------------------------------------------------------------------------

cv::Matx44f pose_from2d::cameraPoseFromStereo(
    const pose_from2d::Marker& _markerCam1,
    const pose_from2d::Marker& _markerCam2
) const
{
    cv::Matx44f pose = sight::geometry::vision::helper::cameraPoseStereo(
        m_3dModel,
        m_cameras[0].intrinsicMat,
        m_cameras[0].distCoef,
        m_cameras[1].intrinsicMat,
        m_cameras[1].distCoef,
        _markerCam1.corners2D,
        _markerCam2.corners2D,
        m_extrinsicMat.rotation,
        m_extrinsicMat.translation
    );

    return pose;
}

//-----------------------------------------------------------------------------

cv::Matx44f pose_from2d::cameraPoseFromMono(const pose_from2d::Marker& _markerCam1) const
{
    cv::Matx44f pose =
        sight::geometry::vision::helper::cameraPoseMonocular(
            m_3dModel,
            _markerCam1.corners2D,
            m_cameras[0].intrinsicMat,
            m_cameras[0].distCoef
        );
    return pose;
}

//-----------------------------------------------------------------------------

service::connections_t pose_from2d::auto_connections() const
{
    return {
        {s_MARKERMAP_INPUT, data::object::MODIFIED_SIG, service::slots::UPDATE},
        {s_CAMERA_INPUT, data::object::MODIFIED_SIG, UPDATE_CAMERA_SLOT},
        {s_CAMERA_INPUT, data::camera::INTRINSIC_CALIBRATED_SIG, UPDATE_CAMERA_SLOT}
    };
}

//-----------------------------------------------------------------------------

} // namespace sight::module::geometry::vision
