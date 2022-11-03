/************************************************************************
 *
 * Copyright (C) 2017-2022 IRCAD France
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

#include "SReprojectionError.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <geometry/vision/helper.hpp>

#include <io/opencv/Camera.hpp>
#include <io/opencv/Image.hpp>

#include <opencv2/calib3d.hpp>
#include <opencv2/opencv.hpp>

namespace sight::module::geometry::vision
{

const core::com::Slots::SlotKeyType SReprojectionError::s_COMPUTE_SLOT       = "compute";
const core::com::Slots::SlotKeyType SReprojectionError::s_SET_PARAMETER_SLOT = "setParameter";

static const core::com::Signals::SignalKeyType s_ERROR_COMPUTED_SIG = "errorComputed";

//-----------------------------------------------------------------------------

SReprojectionError::SReprojectionError() :

    m_cvColor(cv::Scalar(255, 255, 255, 255))
{
    newSignal<ErrorComputedSignalType>(s_ERROR_COMPUTED_SIG);

    newSlot(s_COMPUTE_SLOT, &SReprojectionError::compute, this);
    newSlot(s_SET_PARAMETER_SLOT, &SReprojectionError::setParameter, this);
}

//-----------------------------------------------------------------------------

void SReprojectionError::configuring()
{
    service::IService::ConfigType config = this->getConfigTree();
    m_patternWidth = config.get<double>("patternWidth", m_patternWidth);
    SIGHT_ASSERT("patternWidth setting is set to " << m_patternWidth << " but should be > 0.", m_patternWidth > 0);

    auto inCfg = config.equal_range("in");
    for(auto itCfg = inCfg.first ; itCfg != inCfg.second ; ++itCfg)
    {
        const auto group = itCfg->second.get<std::string>("<xmlattr>.group", "");
        if(group == s_MATRIX_INPUT)
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

void SReprojectionError::starting()
{
    //3D Points
    const float halfWidth = static_cast<float>(m_patternWidth) * .5F;

    m_objectPoints.emplace_back(-halfWidth, halfWidth, 0.F);
    m_objectPoints.emplace_back(halfWidth, halfWidth, 0.F);
    m_objectPoints.emplace_back(halfWidth, -halfWidth, 0.F);
    m_objectPoints.emplace_back(-halfWidth, -halfWidth, 0.F);

    //TODO: Add an option to use a chessboard instead of a marker
    // --> configure height, width and square size(in mm)

    auto camera = m_camera.lock();
    SIGHT_ASSERT("Camera is not found", camera);

    cv::Size imgSize;
    std::tie(m_cameraMatrix, imgSize, m_distorsionCoef) = io::opencv::Camera::copyToCv(camera.get_shared());

    m_cvExtrinsic = cv::Mat::eye(4, 4, CV_64F);

    auto extrinsic = m_extrinsic.lock();
    if(extrinsic)
    {
        for(std::uint8_t i = 0 ; i < 4 ; ++i)
        {
            for(std::uint8_t j = 0 ; j < 4 ; ++j)
            {
                m_cvExtrinsic.at<double>(i, j) = (*extrinsic)(i, j);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void SReprojectionError::stopping()
{
    m_objectPoints.clear();
}

//-----------------------------------------------------------------------------

void SReprojectionError::compute(core::HiResClock::HiResClockType timestamp)
{
    if(!this->isStarted())
    {
        return;
    }

    if(timestamp > m_lastTimestamp)
    {
        std::vector<sight::geometry::vision::helper::ErrorAndPointsType> errors;

        {
            auto markerMap = m_markerMap.lock();

            // For each matrix
            unsigned int i = 0;
            for(const auto& markerKey : m_matricesTag)
            {
                const auto* marker = markerMap->getMarker(markerKey);

                if(marker != nullptr)
                {
                    std::vector<cv::Point2f> points2D;

                    cv::Mat mat = cv::Mat::eye(4, 4, CV_64F);
                    {
                        auto matrix = m_matrix[i].lock();
                        for(std::uint8_t r = 0 ; r < 4 ; ++r)
                        {
                            for(std::uint8_t c = 0 ; c < 4 ; ++c)
                            {
                                mat.at<double>(r, c) = static_cast<double>((*matrix)(r, c));
                            }
                        }
                    }

                    const cv::Mat pose = m_cvExtrinsic * mat;

                    cv::Mat rot = pose(cv::Rect(0, 0, 3, 3));

                    cv::Mat tvec = cv::Mat(3, 1, CV_64F);
                    tvec.at<double>(0) = pose.at<double>(0, 3);
                    tvec.at<double>(1) = pose.at<double>(1, 3);
                    tvec.at<double>(2) = pose.at<double>(2, 3);

                    cv::Mat rvec;

                    cv::Rodrigues(rot, rvec);

                    for(const auto& p : *marker)
                    {
                        points2D.emplace_back(p[0], p[1]);
                    }

                    sight::geometry::vision::helper::ErrorAndPointsType errP =
                        sight::geometry::vision::helper::computeReprojectionError(
                            m_objectPoints,
                            points2D,
                            rvec,
                            tvec,
                            m_cameraMatrix,
                            m_distorsionCoef
                        );

                    this->signal<ErrorComputedSignalType>(s_ERROR_COMPUTED_SIG)->asyncEmit(errP.first);

                    errors.push_back(errP);
                }

                ++i;
            }
        }

        // draw reprojected points
        if(m_display)
        {
            for(const auto& err : errors)
            {
                auto frame = m_frame.lock();
                SIGHT_ASSERT("The input " << s_FRAME_INOUT << " is not valid.", frame);

                if(frame->getSizeInBytes() > 0)
                {
                    cv::Mat cvImage = io::opencv::Image::moveToCv(frame.get_shared());

                    std::vector<cv::Point2f> reprojectedP = err.second;

                    for(auto& j : reprojectedP)
                    {
                        cv::circle(cvImage, j, 7, m_cvColor, 1, cv::LINE_8);
                    }
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

void SReprojectionError::setParameter(sight::ui::base::parameter_t _val, std::string _key)
{
    if(_key == "display")
    {
        m_display = std::get<bool>(_val);
    }
    else if(_key == "color")
    {
        const auto color = std::get<sight::ui::base::color_t>(_val);
        m_cvColor = cv::Scalar(color[0], color[1], color[2], 255);
    }
    else
    {
        SIGHT_ERROR("the key '" + _key + "' is not handled");
    }
}

//-----------------------------------------------------------------------------

void SReprojectionError::updating()
{
    // When working with a frame (newest design), we do not rely on the timetamp
    // So we can just send the current one.
    // When removing timelines from the service then we could get rid of it
    auto timestamp = core::HiResClock::getTimeInMilliSec();
    this->compute(timestamp);
}

//-----------------------------------------------------------------------------

service::IService::KeyConnectionsMap SReprojectionError::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_MATRIX_INPUT, data::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
    return connections;
}

//-----------------------------------------------------------------------------

} //namespace sight::module::geometry::vision
