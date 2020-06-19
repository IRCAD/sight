/************************************************************************
 *
 * Copyright (C) 2017-2018 IRCAD France
 * Copyright (C) 2017-2018 IHU Strasbourg
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

#include "videoCalibration/SReprojectionError.hpp"

#include <arData/Camera.hpp>
#include <arData/FrameTL.hpp>
#include <arData/MarkerTL.hpp>
#include <arData/MatrixTL.hpp>

#include <calibration3d/helper.hpp>

#include <cvIO/Camera.hpp>
#include <cvIO/Image.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwServices/macros.hpp>

#include <opencv2/calib3d.hpp>
#include <opencv2/opencv.hpp>

fwServicesRegisterMacro(::fwServices::IController, ::videoCalibration::SReprojectionError);

namespace videoCalibration
{

const ::fwCom::Slots::SlotKeyType SReprojectionError::s_COMPUTE_SLOT = "compute";

const ::fwCom::Slots::SlotKeyType SReprojectionError::s_SET_BOOL_PARAMETER_SLOT  = "setBoolParameter";
const ::fwCom::Slots::SlotKeyType SReprojectionError::s_SET_COLOR_PARAMETER_SLOT = "setColorParameter";

static const ::fwCom::Signals::SignalKeyType s_ERROR_COMPUTED_SIG = "errorComputed";

const ::fwServices::IService::KeyType s_MARKERTL_INPUT  = "markerTL";
const ::fwServices::IService::KeyType s_MARKERMAP_INPUT = "markerMap";
const ::fwServices::IService::KeyType s_CAMERA_INPUT    = "camera";
const ::fwServices::IService::KeyType s_EXTRINSIC_INPUT = "extrinsic";
const ::fwServices::IService::KeyType s_MATRIXTL_INPUT  = "matrixTL";
const ::fwServices::IService::KeyType s_MATRIX_INPUT    = "matrix";
const ::fwServices::IService::KeyType s_FRAMETL_INOUT   = "frameTL";
const ::fwServices::IService::KeyType s_FRAME_INOUT     = "frame";

//-----------------------------------------------------------------------------

SReprojectionError::SReprojectionError() :
    m_lastTimestamp(0),
    m_patternWidth(80),
    m_cvColor(::cv::Scalar(255, 255, 255, 255)),
    m_display(true)
{
    newSignal<ErrorComputedSignalType>(s_ERROR_COMPUTED_SIG);
    newSlot(s_COMPUTE_SLOT, &SReprojectionError::compute, this);

    newSlot(s_SET_BOOL_PARAMETER_SLOT, &SReprojectionError::setBoolParameter, this);
    newSlot(s_SET_COLOR_PARAMETER_SLOT, &SReprojectionError::setColorParameter, this);
}

//-----------------------------------------------------------------------------

SReprojectionError::~SReprojectionError()
{
}

//-----------------------------------------------------------------------------

void SReprojectionError::configuring()
{
    ::fwServices::IService::ConfigType config = this->getConfigTree();
    m_patternWidth                            = config.get<double>("patternWidth", m_patternWidth);
    OSLM_ASSERT("patternWidth setting is set to " << m_patternWidth << " but should be > 0.", m_patternWidth > 0);

    auto inCfg = config.equal_range("in");
    for (auto itCfg = inCfg.first; itCfg != inCfg.second; ++itCfg)
    {
        const auto group = itCfg->second.get<std::string>("<xmlattr>.group", "");
        if(group == s_MATRIX_INPUT)
        {
            auto keyCfg = itCfg->second.equal_range("key");
            for (auto itKeyCfg = keyCfg.first; itKeyCfg != keyCfg.second; ++itKeyCfg)
            {
                const ::arData::MarkerMap::KeyType key = itKeyCfg->second.get<std::string>("<xmlattr>.id");
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
    const float halfWidth = static_cast<float>(m_patternWidth) * .5f;

    m_objectPoints.push_back( ::cv::Point3f(-halfWidth, halfWidth, 0) );
    m_objectPoints.push_back( ::cv::Point3f(halfWidth, halfWidth, 0) );
    m_objectPoints.push_back( ::cv::Point3f(halfWidth, -halfWidth, 0) );
    m_objectPoints.push_back( ::cv::Point3f(-halfWidth, -halfWidth, 0) );

    //TODO: Add an option to use a chessboard instead of a marker
    // --> configure height, width and square size(in mm)

    auto camera = this->getInput< ::arData::Camera >(s_CAMERA_INPUT);
    SLM_ASSERT("Camera is not found", camera);

    ::cv::Size imgSize;
    std::tie(m_cameraMatrix, imgSize, m_distorsionCoef) = ::cvIO::Camera::copyToCv(camera);

    m_extrinsic = ::cv::Mat::eye(4, 4, CV_64F);

    auto extrinsic = this->getInput< ::fwData::TransformationMatrix3D>(s_EXTRINSIC_INPUT);
    if(extrinsic != nullptr)
    {
        for(std::uint8_t i = 0; i < 4; ++i )
        {
            for(std::uint8_t j = 0; j < 4; ++j)
            {
                m_extrinsic.at<double>(i, j) = extrinsic->getCoefficient(i, j);
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

void SReprojectionError::compute(fwCore::HiResClock::HiResClockType timestamp)
{
    if(!this->isStarted())
    {
        return;
    }

    if(timestamp > m_lastTimestamp)
    {
        auto markerTL = this->getInput< ::arData::MarkerTL >(s_MARKERTL_INPUT);
        if(markerTL)
        {
            auto matrixTL = this->getInput< ::arData::MatrixTL >(s_MATRIXTL_INPUT);
            ::fwCore::HiResClock::HiResClockType ts = matrixTL->getNewerTimestamp();
            if(ts <= 0)
            {
                OSLM_WARN("No matrix found in a timeline for timestamp '"<<ts<<"'.");
                return;
            }

            const CSPTR(::arData::MatrixTL::BufferType) matBuff = matrixTL->getClosestBuffer(ts);
            const CSPTR(::arData::MarkerTL::BufferType) buffer  = markerTL->getClosestBuffer(ts);

            if(matBuff == nullptr || buffer == nullptr)
            {
                return;
            }

            const size_t nbMatrices = matBuff->getMaxElementNum();
            const size_t nbMarkers  = buffer->getMaxElementNum();

            SLM_ASSERT("Number of matrices should be equal to the number of marker", nbMatrices == nbMarkers);

            for(unsigned int i = 0; i < nbMatrices; ++i)
            {
                const float* marker = buffer->getElement(i);
                const float* matrix = matBuff->getElement(i);
                std::vector< ::cv::Point2f >points2D;

                ::cv::Mat rot = ::cv::Mat(3, 3, CV_64F);
                ::cv::Mat mat = ::cv::Mat::eye(4, 4, CV_64F);

                for(int r = 0; r < 4; ++r)
                {
                    for(int c = 0; c < 4; ++c)
                    {
                        mat.at<double>(r, c) = static_cast<double>(matrix[r * 4 + c]);
                    }
                }

                ::cv::Mat pose = m_extrinsic * mat;

                rot = pose( ::cv::Rect(0, 0, 3, 3));

                ::cv::Mat tvec     = ::cv::Mat(3, 1, CV_64F);
                tvec.at<double>(0) = pose.at<double>(0, 3);
                tvec.at<double>(1) = pose.at<double>(1, 3);
                tvec.at<double>(2) = pose.at<double>(2, 3);

                ::cv::Mat rvec;

                ::cv::Rodrigues(rot, rvec);

                for(size_t p = 0; p < 4; ++p)
                {
                    points2D.push_back(::cv::Point2f(marker[p*2], marker[p*2 +1]));
                }

                ::calibration3d::helper::ErrorAndPointsType errP =
                    ::calibration3d::helper::computeReprojectionError(m_objectPoints, points2D, rvec, tvec,
                                                                      m_cameraMatrix, m_distorsionCoef);

                this->signal<ErrorComputedSignalType>(s_ERROR_COMPUTED_SIG)->asyncEmit(errP.first);

                if(m_display) //draw reprojected points
                {
                    auto frameTL = this->getInOut< ::arData::FrameTL >(s_FRAMETL_INOUT);
                    SLM_ASSERT("The input "+ s_FRAMETL_INOUT +" is not valid.", frameTL);

                    CSPTR(::arData::FrameTL::BufferType) bufferFrame = frameTL->getClosestBuffer(ts);

                    if(bufferFrame != nullptr)
                    {
                        const std::uint8_t* frameData = &bufferFrame->getElement(0);
                        const int width               = static_cast< int >( frameTL->getWidth() );
                        const int height              = static_cast< int >( frameTL->getHeight() );
                        ::cv::Mat img( ::cv::Size( width, height ), CV_8UC4 );
                        img.data = const_cast< uchar*>(frameData);

                        std::vector< ::cv::Point2f > reprojectedP = errP.second;

                        for(size_t i = 0; i < reprojectedP.size(); ++i)
                        {
                            ::cv::circle(img, reprojectedP[i], 7, m_cvColor, 1, ::cv::LINE_8);
                        }
                    }
                }
            }
        }
        else
        {
            auto markerMap = this->getInput< ::arData::MarkerMap >(s_MARKERMAP_INPUT);

            // For each matrix
            unsigned int i = 0;
            for(auto markerKey : m_matricesTag)
            {
                auto matrix = this->getInput< ::fwData::TransformationMatrix3D >(s_MATRIX_INPUT, i);

                const auto* marker = markerMap->getMarker(markerKey);

                if(marker)
                {
                    std::vector< ::cv::Point2f >points2D;

                    ::cv::Mat rot = ::cv::Mat(3, 3, CV_64F);
                    ::cv::Mat mat = ::cv::Mat::eye(4, 4, CV_64F);

                    for(std::uint8_t r = 0; r < 4; ++r)
                    {
                        for(std::uint8_t c = 0; c < 4; ++c)
                        {
                            mat.at<double>(r, c) = static_cast<double>(matrix->getCoefficient(r, c));
                        }
                    }

                    const ::cv::Mat pose = m_extrinsic * mat;

                    rot = pose( ::cv::Rect(0, 0, 3, 3));

                    ::cv::Mat tvec     = ::cv::Mat(3, 1, CV_64F);
                    tvec.at<double>(0) = pose.at<double>(0, 3);
                    tvec.at<double>(1) = pose.at<double>(1, 3);
                    tvec.at<double>(2) = pose.at<double>(2, 3);

                    ::cv::Mat rvec;

                    ::cv::Rodrigues(rot, rvec);

                    for(const auto& p : *marker)
                    {
                        points2D.push_back(::cv::Point2f(p[0], p[1]));
                    }

                    ::calibration3d::helper::ErrorAndPointsType errP =
                        ::calibration3d::helper::computeReprojectionError(m_objectPoints, points2D, rvec, tvec,
                                                                          m_cameraMatrix, m_distorsionCoef);

                    this->signal<ErrorComputedSignalType>(s_ERROR_COMPUTED_SIG)->asyncEmit(errP.first);

                    if(m_display) //draw reprojected points
                    {
                        auto frame = this->getInOut< ::fwData::Image >(s_FRAME_INOUT);
                        SLM_ASSERT("The input "+ s_FRAMETL_INOUT +" is not valid.", frame);

                        ::fwData::mt::ObjectWriteLock lock(frame);

                        if(frame->getSizeInBytes() > 0)
                        {
                            ::cv::Mat cvImage = ::cvIO::Image::moveToCv(frame);

                            std::vector< ::cv::Point2f > reprojectedP = errP.second;

                            for(size_t i = 0; i < reprojectedP.size(); ++i)
                            {
                                ::cv::circle(cvImage, reprojectedP[i], 7, m_cvColor, 1, ::cv::LINE_8);
                            }
                        }
                    }
                }
                ++i;
            }
        }
    }
}

//-----------------------------------------------------------------------------

void SReprojectionError::setBoolParameter(bool _val, std::string _key)
{
    if(_key == "display")
    {
        m_display = _val;
    }
    else
    {
        SLM_ERROR("the key '" + _key + "' is not handled");
    }
}

//-----------------------------------------------------------------------------

void SReprojectionError::setColorParameter(std::array<uint8_t, 4> _val, std::string _key)
{
    if(_key == "color")
    {
        m_cvColor = ::cv::Scalar(_val[0], _val[1], _val[2], 255);
    }
    else
    {
        SLM_ERROR("the key '" + _key + "' is not handled");
    }
}

//-----------------------------------------------------------------------------

void SReprojectionError::updating()
{
    // When working with a frame (newest design), we do not rely on the timetamp
    // So we can just send the current one.
    // When removing timelines from the service then we could get rid of it
    auto timestamp = ::fwCore::HiResClock::getTimeInMilliSec();
    this->compute(timestamp);
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SReprojectionError::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push( s_MATRIXTL_INPUT, ::arData::TimeLine::s_OBJECT_PUSHED_SIG, s_COMPUTE_SLOT );
    connections.push( s_MATRIX_INPUT, ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT );

    return connections;
}

//-----------------------------------------------------------------------------

} //namespace videoCalibration
