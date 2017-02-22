/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoCalibration/SReprojectionError.hpp"

#include <arData/Camera.hpp>
#include <arData/FrameTL.hpp>
#include <arData/MarkerTL.hpp>
#include <arData/MatrixTL.hpp>

#include <calibration3d/helper.hpp>

#include <opencv2/calib3d.hpp>
#include <opencv2/opencv.hpp>

fwServicesRegisterMacro(::fwServices::IController, ::videoCalibration::SReprojectionError);

namespace videoCalibration
{

const ::fwCom::Slots::SlotKeyType SReprojectionError::s_COMPUTE_SLOT = "compute";

static const ::fwCom::Signals::SignalKeyType s_ERROR_COMPUTED_SIG = "errorComputed";

const ::fwServices::IService::KeyType s_MARKERTL_INPUT  = "markerTL";
const ::fwServices::IService::KeyType s_CAMERA_INPUT    = "camera";
const ::fwServices::IService::KeyType s_EXTRINSIC_INPUT = "extrinsic";
const ::fwServices::IService::KeyType s_MATRIXTL_INPUT  = "matrixTL";
const ::fwServices::IService::KeyType s_FRAMETL_INPUT   = "frameTL";

//-----------------------------------------------------------------------------

SReprojectionError::SReprojectionError() :
    m_lastTimestamp(0),
    m_patternWidth(80),
    m_display(true)
{
    newSignal<ErrorComputedSignalType>(s_ERROR_COMPUTED_SIG);
    newSlot(s_COMPUTE_SLOT, &SReprojectionError::compute, this);
}

//-----------------------------------------------------------------------------

SReprojectionError::~SReprojectionError()
{

}

//-----------------------------------------------------------------------------

void SReprojectionError::configuring() throw (::fwTools::Failed)
{
    // gets pattern width
    ::fwRuntime::ConfigurationElement::sptr cfgPatternWidth = m_configuration->findConfigurationElement("patternWidth");
    if (cfgPatternWidth)
    {
        m_patternWidth = std::stod(cfgPatternWidth->getValue());
    }
}

//-----------------------------------------------------------------------------

void SReprojectionError::starting() throw(::fwTools::Failed)
{
    //3D Points
    const float halfWidth = static_cast<float>(m_patternWidth) * .5f;

    m_objectPoints.push_back( ::cv::Point3f(-halfWidth, halfWidth, 0) );
    m_objectPoints.push_back( ::cv::Point3f(halfWidth, halfWidth, 0) );
    m_objectPoints.push_back( ::cv::Point3f(halfWidth, -halfWidth, 0) );
    m_objectPoints.push_back( ::cv::Point3f(-halfWidth, -halfWidth, 0) );

    auto camera = this->getInput< ::arData::Camera >(s_CAMERA_INPUT);

    m_cameraMatrix                  = ::cv::Mat::eye(3, 3, CV_64F);
    m_cameraMatrix.at<double>(0, 0) = camera->getFx();
    m_cameraMatrix.at<double>(1, 1) = camera->getFy();
    m_cameraMatrix.at<double>(0, 2) = camera->getCx();
    m_cameraMatrix.at<double>(1, 2) = camera->getCy();

    m_distorsionCoef = ::cv::Mat(5, 1, CV_64F);
    for (int i = 0; i < 5; ++i)
    {
        m_distorsionCoef.at<double>(i) = camera->getDistortionCoefficient()[i];
    }

}

//-----------------------------------------------------------------------------

void SReprojectionError::stopping() throw(::fwTools::Failed)
{

}

//-----------------------------------------------------------------------------

void SReprojectionError::compute(fwCore::HiResClock::HiResClockType timestamp)
{
    if(timestamp > m_lastTimestamp)
    {
        ::fwCore::HiResClock::HiResClockType newerTimestamp =
            std::numeric_limits< ::fwCore::HiResClock::HiResClockType >::max();

        auto matrixTL = this->getInput< ::arData::MatrixTL >(s_MATRIXTL_INPUT);
        auto markerTL = this->getInput< ::arData::MarkerTL >(s_MARKERTL_INPUT);
        ::fwCore::HiResClock::HiResClockType ts = matrixTL->getNewerTimestamp();
        if(ts <= 0)
        {
            OSLM_WARN("No matrix found in a timeline for timestamp '"<<ts<<"'.");
            return;
        }

        const CSPTR(::arData::MatrixTL::BufferType) matBuff = matrixTL->getBuffer(ts);
        const CSPTR(::arData::MarkerTL::BufferType) buffer  = markerTL->getBuffer(ts);

        const size_t nbMatrices = matBuff->getMaxElementNum();
        const size_t nbMarkers  = buffer->getMaxElementNum();

        SLM_ASSERT("Number of matrices should be equal to the number of marker", nbMatrices == nbMarkers);

        for(unsigned int i = 0; i < nbMatrices; ++i)
        {
            const float* marker = buffer->getElement(i);
            const float* matrix = matBuff->getElement(i);
            std::vector< ::cv::Point2f >points2D;

            ::cv::Mat rot = ::cv::Mat(3, 3, CV_64F);

            for(int r = 0; r < 3; ++r)
            {
                for(int c = 0; c < 3; ++c)
                {
                    rot.at<double>(r, c) = static_cast<double>(matrix[r * 4 + c]);
                }
            }

            ::cv::Mat tvec     = ::cv::Mat(3, 1, CV_64F);
            tvec.at<double>(0) = static_cast<double>(matrix[3]);
            tvec.at<double>(1) = static_cast<double>(matrix[7]);
            tvec.at<double>(2) = static_cast<double>(matrix[11]);

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

            if(m_display)
            {
                auto frameTL = this->getInput< ::arData::FrameTL >(s_FRAMETL_INPUT);
                SLM_ASSERT("The input "+ s_FRAMETL_INPUT +" is not valid.", frameTL);

                CSPTR(::arData::FrameTL::BufferType) bufferFrame = frameTL->getBuffer(ts);
                if(bufferFrame != nullptr)
                {
                    const ::boost::uint8_t* frameData = &bufferFrame->getElement(0);
                    const int width                   = static_cast< int >( frameTL->getWidth() );
                    const int height                  = static_cast< int >( frameTL->getHeight() );
                    ::cv::Mat img( ::cv::Size( width, height ), CV_8UC4 );
                    img.data = const_cast< uchar*>(frameData);

                    std::vector< ::cv::Point2f > reprojectedP = errP.second;

                    for(size_t i = 0; i < reprojectedP.size(); ++i)
                    {
                        ::cv::circle(img, reprojectedP[i], 7, ::cv::Scalar(0, 255, 255, 255), 1, ::cv::LINE_8);
                    }

                }

            }
        }

    }
}

//-----------------------------------------------------------------------------

void SReprojectionError::updating() throw(::fwTools::Failed)
{

}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SReprojectionError::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push( "matrixTL", ::arData::TimeLine::s_OBJECT_PUSHED_SIG, s_COMPUTE_SLOT );

    return connections;
}

//-----------------------------------------------------------------------------

} //namespace videoCalibration
