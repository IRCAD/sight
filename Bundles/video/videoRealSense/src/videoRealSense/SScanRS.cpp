/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "videoRealSense/SScanRS.hpp"

#include <arData/Camera.hpp>
#include <arData/CameraSeries.hpp>
#include <arData/FrameTL.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwCore/base.hpp>

#include <fwData/Composite.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwDataTools/helper/Image.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/Type.hpp>

#include <boost/bind/placeholders.hpp>

#include <librealsense/rs.hpp>

namespace videoRealSense
{

const ::fwServices::IService::KeyType s_CAMERA_SERIES_INOUT = "cameraSeries";

//-----------------------------------------------------------------------------

SScanRS::SScanRS() noexcept
{

}

//-----------------------------------------------------------------------------

SScanRS::~SScanRS() noexcept
{
}

//-----------------------------------------------------------------------------

void SScanRS::starting()
{
    m_depthTimeline = this->getInOut< ::arData::FrameTL>(s_DEPTHTL_INOUT);
    m_colorTimeline = this->getInOut< ::arData::FrameTL>(s_FRAMETL_INOUT);
}

//-----------------------------------------------------------------------------

void SScanRS::stopping()
{
    this->stopCamera();
}

//-----------------------------------------------------------------------------

void SScanRS::configuring()
{
}

//-----------------------------------------------------------------------------

void SScanRS::updating()
{
    this->stopCamera();
}

//-----------------------------------------------------------------------------

void SScanRS::onRgbaPointCloud(const pcl::PointCloud<pcl::PointXYZRGBA>::ConstPtr& _pc)
{
    //TODO: use the pointCloud
}

//-----------------------------------------------------------------------------

void SScanRS::onCameraImage(const uint8_t* _buffer)
{
    // Filling timeline's buffer
    ::fwCore::HiResClock::HiResClockType timestamp  = ::fwCore::HiResClock::getTimeInMilliSec();
    SPTR(::arData::FrameTL::BufferType) colorBuffer = m_colorTimeline->createBuffer(timestamp);

    uint32_t* destColorBuffer = reinterpret_cast< uint32_t* >( colorBuffer->addElement(0) );

    const ::realsenseTools::RsGrabber::Mode actualMode = m_rsgrabber->getMode();

    const unsigned int width  = actualMode.color_width;
    const unsigned int height = actualMode.color_height;

    const unsigned int sizeBuffer = width * height * 3;

    for (unsigned int idx = 0; idx < sizeBuffer; idx += 3)
    {
        *destColorBuffer++ = (_buffer[idx] & 0xFF)
                             | (_buffer[idx+1] & 0xFF) << 8
                             | (_buffer[idx+2] & 0xFF) << 16
                             | (255 & 0xFF) << 24;
    }

    // Push buffer to timeline and notify
    m_colorTimeline->pushObject(colorBuffer);
    ::arData::TimeLine::ObjectPushedSignalType::sptr sig;
    sig = m_colorTimeline->signal< ::arData::TimeLine::ObjectPushedSignalType >(
        ::arData::TimeLine::s_OBJECT_PUSHED_SIG );
    sig->asyncEmit(timestamp);
}

//-----------------------------------------------------------------------------

void SScanRS::onCameraImageDepth(const uint16_t* _buffer)
{
    // Filling the depth image buffer in the timeline
    ::fwCore::HiResClock::HiResClockType timestamp = ::fwCore::HiResClock::getTimeInMilliSec();
    SPTR(::arData::FrameTL::BufferType) depthTL    = m_depthTimeline->createBuffer(timestamp);

    const ::realsenseTools::RsGrabber::Mode actualMode = m_rsgrabber->getMode();

    const unsigned int width  = actualMode.depth_width;
    const unsigned int height = actualMode.depth_height;

    uint16_t* depthBuffer         = reinterpret_cast< uint16_t* >( depthTL->addElement(0) );
    const unsigned int sizeBuffer = width * height;

    for (unsigned int idx = 0; idx < sizeBuffer; ++idx)
    {
        depthBuffer[idx] = _buffer[idx];
    }

    // Push buffer to timeline and notify
    m_depthTimeline->pushObject(depthTL);
    ::arData::TimeLine::ObjectPushedSignalType::sptr sig;
    sig = m_depthTimeline->signal< ::arData::TimeLine::ObjectPushedSignalType >(
        ::arData::TimeLine::s_OBJECT_PUSHED_SIG );
    sig->asyncEmit(timestamp);
}

//-----------------------------------------------------------------------------

void SScanRS::startCamera()
{
    //Check if realsense is connected
    const bool available = ::realsenseTools::RsGrabber::isAvailable();
    if(!available)
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog(
            "RealSense Error",
            "RealSense device is not available. Please check if it is plugged in.",
            ::fwGui::dialog::IMessageDialog::CRITICAL);
        return;
    }

    m_rsgrabber = std::unique_ptr< ::realsenseTools::RsGrabber >(new ::realsenseTools::RsGrabber());
    std::vector< ::realsenseTools::RsGrabber::Mode > modes = m_rsgrabber->getAvailableModes();

    if (modes.empty())
    {
        SLM_ERROR("No available mode for realsense.");
        return;
    }

    const ::realsenseTools::RsGrabber::Mode actualMode = modes[0];

    m_colorTimeline = this->getInOut< ::arData::FrameTL>(s_FRAMETL_INOUT);

    std::stringstream str;
    str << "-- fps : "<<actualMode.fps<<std::endl;
    str << "-- depth w : "<<actualMode.depth_width<<std::endl;
    str << "-- depth h : "<<actualMode.depth_height<<std::endl;
    str << "-- color w : "<<actualMode.color_width<<std::endl;
    str << "-- color h : "<<actualMode.color_height<<std::endl;

    SLM_DEBUG("Actual mode : \n" + str.str());

    //set the first mode
    m_rsgrabber->setMode(actualMode);

    if(m_rsgrabber->providesCallback< ::realsenseTools::RsGrabber::sig_cb_real_sense_depth_image > () &&
       m_rsgrabber->providesCallback< ::realsenseTools::RsGrabber::sig_cb_real_sense_rgb_image > () )
    {
        m_depthTimeline->initPoolSize(actualMode.depth_width, actualMode.depth_height, ::fwTools::Type::s_UINT16, 1);
        m_colorTimeline->initPoolSize(actualMode.color_width, actualMode.color_height, ::fwTools::Type::s_UINT8, 4);
        m_depthTimeline->setMaximumSize(50);
        m_colorTimeline->setMaximumSize(50);

        // callbacks for rgb and depth frames.
        ::boost::function< void (const uint16_t*)> depthCallback =
            ::boost::bind(&SScanRS::onCameraImageDepth, this, _1);

        ::boost::function< void (const uint8_t* ) > rgbCallback =
            ::boost::bind(&SScanRS::onCameraImage, this, _1);

        m_rsgrabber->registerCallback(depthCallback);

        m_rsgrabber->registerCallback(rgbCallback);
    }

    if (!m_rsgrabber->isRunning())
    {
        m_rsgrabber->start();
    }

    // Get camera information
    ::arData::CameraSeries::sptr cameraSeries = this->getInOut< ::arData::CameraSeries>(s_CAMERA_SERIES_INOUT);

    ::arData::Camera::sptr depthCamera = ::arData::Camera::New();
    ::arData::Camera::sptr colorCamera = ::arData::Camera::New();

    ::fwData::TransformationMatrix3D::sptr matrix = ::fwData::TransformationMatrix3D::New();
    cameraSeries->addCamera(depthCamera);
    cameraSeries->addCamera(colorCamera);

    ::rs::intrinsics depthIntrinsics = m_rsgrabber->getDepthIntrinsics();
    ::rs::intrinsics colorIntrinsics = m_rsgrabber->getColorIntrinsics();
    ::rs::extrinsics extrinsic       = m_rsgrabber->getExtrinsics();

    depthCamera->setDescription("Depth camera");
    depthCamera->setWidth(actualMode.depth_width);
    depthCamera->setHeight(actualMode.depth_height);
    depthCamera->setFx(depthIntrinsics.fx);
    depthCamera->setFy(depthIntrinsics.fy);
    depthCamera->setCx(depthIntrinsics.ppx);
    depthCamera->setCy(depthIntrinsics.ppy);
    depthCamera->setDistortionCoefficient(depthIntrinsics.coeffs[0], depthIntrinsics.coeffs[1],
                                          depthIntrinsics.coeffs[2], depthIntrinsics.coeffs[3],
                                          depthIntrinsics.coeffs[4]);
    depthCamera->setIsCalibrated(true);

    colorCamera->setDescription("Color camera");
    colorCamera->setWidth(actualMode.color_width);
    colorCamera->setHeight(actualMode.color_height);
    colorCamera->setFx(colorIntrinsics.fx);
    colorCamera->setFy(colorIntrinsics.fy);
    colorCamera->setCx(colorIntrinsics.ppx);
    colorCamera->setCy(colorIntrinsics.ppy);
    colorCamera->setDistortionCoefficient(colorIntrinsics.coeffs[0], colorIntrinsics.coeffs[1],
                                          colorIntrinsics.coeffs[2], colorIntrinsics.coeffs[3],
                                          colorIntrinsics.coeffs[4]);
    colorCamera->setIsCalibrated(true);

    size_t index = 0;
    for (size_t i = 0; i < 3; ++i)
    {
        matrix->setCoefficient(i, 3, extrinsic.translation[i]);
        for (size_t j = 0; j < 3; ++j)
        {
            matrix->setCoefficient(i, j, extrinsic.rotation[index]);
            ++index;
        }
    }
    cameraSeries->setExtrinsicMatrix(1, matrix);

    auto sig = cameraSeries->signal< ::arData::CameraSeries::ModifiedSignalType >(
        ::arData::CameraSeries::s_MODIFIED_SIG );
    sig->asyncEmit();

    auto sigStarted = this->signal< ::arServices::IGrabber::CameraStartedSignalType >(
        ::arServices::IGrabber::s_CAMERA_STARTED_SIG);
    sigStarted->asyncEmit();
}

//-----------------------------------------------------------------------------

void SScanRS::stopCamera()
{
    if(m_rsgrabber)
    {
        if ( m_rsgrabber->isRunning())
        {
            m_rsgrabber->stop();
        }
    }

    auto sig = this->signal< ::arServices::IGrabber::CameraStoppedSignalType >(
        ::arServices::IGrabber::s_CAMERA_STOPPED_SIG);
    sig->asyncEmit();
}

//-----------------------------------------------------------------------------

void SScanRS::pauseCamera()
{
    //TODO
}

//-----------------------------------------------------------------------------

} // namespace videoRealSense
