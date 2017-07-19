/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "registrationCV/SPoseFrom2d.hpp"

#include <arData/Camera.hpp>
#include <arData/FrameTL.hpp>
#include <arData/MarkerTL.hpp>
#include <arData/MatrixTL.hpp>

#include <calibration3d/helper.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwData/TransformationMatrix3D.hpp>

fwServicesRegisterMacro(::arServices::IRegisterer, ::registrationCV::SPoseFrom2d);

//-----------------------------------------------------------------------------

namespace registrationCV
{

//-----------------------------------------------------------------------------

const ::fwServices::IService::KeyType s_MARKERTL_INPUT  = "markerTL";
const ::fwServices::IService::KeyType s_CAMERA_INPUT    = "camera";
const ::fwServices::IService::KeyType s_EXTRINSIC_INPUT = "extrinsic";
const ::fwServices::IService::KeyType s_MATRIXTL_INOUT  = "matrixTL";

//-----------------------------------------------------------------------------

SPoseFrom2d::SPoseFrom2d() noexcept :
    m_lastTimestamp(0),
    m_patternWidth(80),
    m_isInitialized(false)
{

}

//-----------------------------------------------------------------------------

SPoseFrom2d::~SPoseFrom2d() noexcept
{
}

//-----------------------------------------------------------------------------

void SPoseFrom2d::configuring()
{
    ::fwServices::IService::ConfigType config = this->getConfigTree().get_child("service");
    m_patternWidth                            = config.get<double>("patternWidth", 80);
    OSLM_ASSERT("patternWidth setting is set to " << m_patternWidth << " but should be > 0.", m_patternWidth > 0);
}

//-----------------------------------------------------------------------------

void SPoseFrom2d::starting()
{
    //3D Points
    const float halfWidth = static_cast<float>(m_patternWidth) * .5f;

    m_3dModel.push_back( ::cv::Point3f(-halfWidth, halfWidth, 0));
    m_3dModel.push_back( ::cv::Point3f(halfWidth, halfWidth, 0));
    m_3dModel.push_back( ::cv::Point3f(halfWidth, -halfWidth, 0));
    m_3dModel.push_back( ::cv::Point3f(-halfWidth, -halfWidth, 0));
}

//-----------------------------------------------------------------------------

void SPoseFrom2d::stopping()
{

    m_cameras.clear();
    m_3dModel.clear();
    m_lastTimestamp = 0;
    m_isInitialized = false;
}

//-----------------------------------------------------------------------------

void SPoseFrom2d::updating()
{
}

//-----------------------------------------------------------------------------

void SPoseFrom2d::computeRegistration(::fwCore::HiResClock::HiResClockType timestamp)
{

    SLM_WARN_IF("Invoking doRegistration while service is STOPPED", this->isStopped() );

    if(!m_isInitialized)
    {
        this->initialize();
    }

    if (this->isStarted())
    {
        if (timestamp > m_lastTimestamp)
        {
            ::fwCore::HiResClock::HiResClockType newerTimestamp =
                std::numeric_limits< ::fwCore::HiResClock::HiResClockType >::max();
            for(size_t i = 0; i < this->getKeyGroupSize(s_MARKERTL_INPUT); ++i)
            {
                auto markerTL = this->getInput< ::arData::MarkerTL >(s_MARKERTL_INPUT, i);
                ::fwCore::HiResClock::HiResClockType timestamp = markerTL->getNewerTimestamp();
                if(timestamp == 0)
                {
                    OSLM_WARN("No marker found in a timeline for timestamp '"<<timestamp<<"'.");
                    return;
                }
                newerTimestamp = std::min(timestamp, newerTimestamp);
            }

            m_lastTimestamp = newerTimestamp;

            // We consider that all timeline have the same number of elements
            // This is WRONG if we have more than two cameras
            auto markerTL = this->getInput< ::arData::MarkerTL >(s_MARKERTL_INPUT, 0);
            const CSPTR(::arData::MarkerTL::BufferType) buffer = markerTL->getClosestBuffer(newerTimestamp);
            const unsigned int numMarkers = buffer->getMaxElementNum();

            ::arData::MatrixTL::sptr matrixTL = this->getInOut< ::arData::MatrixTL >(s_MATRIXTL_INOUT);

            // Push matrix in timeline
            SPTR(::arData::MatrixTL::BufferType) matrixBuf;

            // For each marker
            bool matrixBufferCreated = false;
            for(unsigned int markerIndex = 0; markerIndex < numMarkers; ++markerIndex)
            {
                std::vector< Marker > markers;
                size_t indexTL = 0;

                // For each camera timeline
                for(size_t i = 0; i < this->getKeyGroupSize(s_MARKERTL_INPUT); ++i)
                {
                    auto markerTL = this->getInput< ::arData::MarkerTL >(s_MARKERTL_INPUT, i);
                    const CSPTR(::arData::MarkerTL::BufferType) buffer = markerTL->getClosestBuffer(newerTimestamp);

                    if(buffer->isPresent(markerIndex))
                    {
                        const float* registrationCVBuffer = buffer->getElement(markerIndex);

                        Marker currentMarker;
                        for(size_t i = 0; i < 4; ++i)
                        {
                            currentMarker.corners2D.push_back( ::cv::Point2f(registrationCVBuffer[i*2],
                                                                             registrationCVBuffer[i*2+1]));
                        }
                        markers.push_back(currentMarker);
                    }
                    ++indexTL;
                }

                if(markers.empty())
                {
                    SLM_WARN("No Markers!")
                    continue;
                }

                float matrixValues[16];

                if(markers.size() == 1)
                {
                    const ::cv::Matx44f Rt = this->cameraPoseFromMono(markers[0]);

                    for (unsigned int i = 0; i < 4; ++i)
                    {
                        for (unsigned int j = 0; j < 4; ++j)
                        {
                            matrixValues[4*i+j] = Rt(i, j);
                        }
                    }
                }
                else
                {
                    SLM_WARN("More than 2 cameras is not handle for the moment");
                    continue;
                }

                if(!matrixBufferCreated)
                {
                    matrixBuf = matrixTL->createBuffer(newerTimestamp);
                    matrixTL->pushObject(matrixBuf);
                    matrixBufferCreated = true;
                }

                matrixBuf->setElement(matrixValues, markerIndex);

            }

            if(matrixBufferCreated)
            {
                //Notify
                ::arData::TimeLine::ObjectPushedSignalType::sptr sig;
                sig = matrixTL->signal< ::arData::TimeLine::ObjectPushedSignalType >(
                    ::arData::TimeLine::s_OBJECT_PUSHED_SIG );
                sig->asyncEmit(timestamp);

            }

        }
    }
}

//-----------------------------------------------------------------------------

void SPoseFrom2d::initialize()
{
    // Initialization of timelines

    ::arData::MarkerTL::csptr firstTimeline = this->getInput< ::arData::MarkerTL >(s_MARKERTL_INPUT, 0);

    const unsigned int maxElementNum = firstTimeline->getMaxElementNum();

    for(size_t i = 0; i < this->getKeyGroupSize(s_MARKERTL_INPUT); ++i)
    {
        ::arData::MarkerTL::csptr timeline = this->getInput< ::arData::MarkerTL >(s_MARKERTL_INPUT, 0);

        SLM_ASSERT("Timelines should have the same maximum number of elements",
                   maxElementNum == timeline->getMaxElementNum());
    }

    ::arData::MatrixTL::sptr matrixTL = this->getInOut< ::arData::MatrixTL >(s_MATRIXTL_INOUT);
    // initialized matrix timeline
    matrixTL->initPoolSize(maxElementNum);

    for(size_t idx = 0; idx < this->getKeyGroupSize(s_CAMERA_INPUT); ++idx)
    {
        ::arData::Camera::csptr camera = this->getInput< ::arData::Camera >(s_CAMERA_INPUT, idx);
        OSLM_FATAL_IF("Camera[" << idx << "] not found", !camera);

        Camera cam;
        cam.intrinsicMat = ::cv::Mat::eye(3, 3, CV_64F);

        cam.intrinsicMat.at<double>(0, 0) = camera->getFx();
        cam.intrinsicMat.at<double>(1, 1) = camera->getFy();
        cam.intrinsicMat.at<double>(0, 2) = camera->getCx();
        cam.intrinsicMat.at<double>(1, 2) = camera->getCy();

        cam.distCoef = ::cv::Mat::zeros(5, 1, CV_64F);

        for (size_t i = 0; i < 5; ++i)
        {
            cam.distCoef.at<double>(static_cast<int>(i)) = camera->getDistortionCoefficient()[i];
        }

        // set extrinsic matrix only if stereo.
        if (idx == 1)
        {
            auto extrinsicMatrix = this->getInput< ::fwData::TransformationMatrix3D >(s_EXTRINSIC_INPUT);

            SLM_FATAL_IF("Extrinsic matrix with key '" + s_EXTRINSIC_INPUT + "' not found", !extrinsicMatrix);

            m_extrinsic.Matrix4x4   = ::cv::Mat::eye(4, 4, CV_64F);
            m_extrinsic.rotation    = ::cv::Mat::eye(3, 3, CV_64F);
            m_extrinsic.translation = ::cv::Mat::eye(3, 1, CV_64F);

            for (unsigned int i = 0; i < 3; ++i)
            {
                for (unsigned int j = 0; j < 3; ++j)
                {
                    m_extrinsic.rotation.at<double>(i, j)  = extrinsicMatrix->getCoefficient(i, j);
                    m_extrinsic.Matrix4x4.at<double>(i, j) = extrinsicMatrix->getCoefficient(i, j);
                }
            }

            m_extrinsic.translation.at<double>(0, 0) = extrinsicMatrix->getCoefficient(0, 3);
            m_extrinsic.translation.at<double>(1, 0) = extrinsicMatrix->getCoefficient(1, 3);
            m_extrinsic.translation.at<double>(2, 0) = extrinsicMatrix->getCoefficient(2, 3);

            m_extrinsic.Matrix4x4.at<double>(0, 3) = extrinsicMatrix->getCoefficient(0, 3);
            m_extrinsic.Matrix4x4.at<double>(1, 3) = extrinsicMatrix->getCoefficient(1, 3);
            m_extrinsic.Matrix4x4.at<double>(2, 3) = extrinsicMatrix->getCoefficient(2, 3);

        }
        m_cameras.push_back(cam);
    }
    m_isInitialized = true;
}

//-----------------------------------------------------------------------------

const cv::Matx44f SPoseFrom2d::cameraPoseFromStereo(const SPoseFrom2d::Marker& _markerCam1,
                                                    const SPoseFrom2d::Marker& _markerCam2) const
{
    SLM_ERROR("Not implemented!");
    ::cv::Matx44f pose;
    return pose;
}

//-----------------------------------------------------------------------------

const cv::Matx44f SPoseFrom2d::cameraPoseFromMono(const SPoseFrom2d::Marker& _markerCam1) const
{

    ::cv::Matx44f pose =
        ::calibration3d::helper::cameraPoseMonocular(m_3dModel, _markerCam1.corners2D,
                                                     m_cameras[0].intrinsicMat, m_cameras[0].distCoef);
    return pose;
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SPoseFrom2d::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push( "markerTL", ::arData::TimeLine::s_OBJECT_PUSHED_SIG, s_COMPUTE_REGISTRATION_SLOT );

    return connections;
}

//-----------------------------------------------------------------------------

} // namespace registrationCV

