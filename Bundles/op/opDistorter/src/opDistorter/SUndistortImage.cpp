/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "opDistorter/SUndistortImage.hpp"

#include <cvIO/Image.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwCore/Profiling.hpp>

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwDataTools/helper/Image.hpp>
#include <fwDataTools/helper/ImageGetter.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/macros.hpp>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

fwServicesRegisterMacro(::fwServices::IController, ::opDistorter::SUndistortImage, ::fwData::Image);

namespace opDistorter
{

const ::fwCom::Slots::SlotKeyType SUndistortImage::s_CHANGE_STATE_SLOT = "changeState";

// ----------------------------------------------------------------------------

SUndistortImage::SUndistortImage() noexcept :
    m_isEnabled(false)
{
    newSlot(s_CHANGE_STATE_SLOT, &SUndistortImage::changeState, this);
}

// ----------------------------------------------------------------------------

void SUndistortImage::configuring()
{
}

// ----------------------------------------------------------------------------

void SUndistortImage::starting()
{
    m_camera = this->getInput< ::arData::Camera> ("camera");
    SLM_FATAL_IF("Object 'camera' is not found.", !m_camera);

    // 3x3 matrix (fx 0 cx, 0 fy cy, 0 0 1)
    ::cv::Mat cameraMatrix        = ::cv::Mat::eye(3, 3, CV_64F);
    cameraMatrix.at<double>(0, 0) = m_camera->getFx();
    cameraMatrix.at<double>(1, 1) = m_camera->getFy();
    cameraMatrix.at<double>(0, 2) = m_camera->getCx();
    cameraMatrix.at<double>(1, 2) = m_camera->getCy();

    //4x1 matrix (k1,k2,p1,p2)
    ::cv::Mat distCoefs = ::cv::Mat::eye(4, 1, CV_64F);
    for (unsigned int i = 0; i < 4; ++i)
    {
        distCoefs.at<double>(static_cast<int>(i), 0) = m_camera->getDistortionCoefficient()[i];
    }

    ::cv::Size size(static_cast<int>(m_camera->getWidth()), static_cast<int>(m_camera->getHeight()) );

    ::cv::Mat mx;
    ::cv::Mat my;
    ::cv::initUndistortRectifyMap(cameraMatrix, distCoefs, ::cv::Mat(), cameraMatrix, size, CV_32FC1, mx, my);

#ifdef OPENCV_CUDA_SUPPORT
    m_mapx = ::cv::cuda::GpuMat(mx);
    m_mapy = ::cv::cuda::GpuMat(my);
#else
    m_mapx = mx;
    m_mapy = my;
#endif // OPENCV_CUDA_SUPPORT

}

// ----------------------------------------------------------------------------

void SUndistortImage::stopping()
{
}

// ----------------------------------------------------------------------------

fwServices::IService::KeyConnectionsMap SUndistortImage::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push( "input", ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT );
    connections.push( "input", ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT );

    return connections;
}

// ----------------------------------------------------------------------------

void SUndistortImage::updating()
{
    if (m_isEnabled)
    {
        if (m_camera->getIsCalibrated())
        {
            this->undistort();
        }
        else
        {
            OSLM_WARN("Unable to undistort the image: " << m_camera->getID() << " is not calibrated.");
        }
    }
    else
    {

        ::fwData::Image::sptr outputImage = this->getInOut< ::fwData::Image >("output");
        SLM_FATAL_IF("Object 'output' is not found.", !outputImage);

        ::fwData::Image::csptr inputImage = this->getInput< ::fwData::Image> ("input");
        SLM_FATAL_IF("Object 'input' is not found.", !inputImage);

        outputImage->deepCopy(inputImage);

        auto sig = outputImage->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Image::s_BUFFER_MODIFIED_SIG);
        {
            ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
            sig->asyncEmit();
        }
    }
}

// ----------------------------------------------------------------------------

void SUndistortImage::undistort()
{
    FW_PROFILE_AVG("undistort", 5);
    ::fwData::Image::sptr outputImage = this->getInOut< ::fwData::Image >("output");
    SLM_FATAL_IF("Object 'output' is not found.", !outputImage);

    ::fwData::Image::csptr inputImage = this->getInput< ::fwData::Image> ("input");
    SLM_FATAL_IF("Object 'input' is not found.", !inputImage);

    auto sig = outputImage->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Image::s_BUFFER_MODIFIED_SIG);
    // Blocking signals early allows to discard any event  while we are updating
    ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));

    ::fwData::mt::ObjectReadLock inputLock(inputImage);

    ::fwDataTools::helper::ImageGetter inputImgHelper(inputImage);

    if (!inputImgHelper.getBuffer())
    {
        return;
    }

    if(!outputImage->getSizeInBytes())
    {
        ::fwData::mt::ObjectWriteLock outputLock(outputImage);
        ::fwData::Image::SizeType size(2);
        size[0] = inputImage->getSize()[0];
        size[1] = inputImage->getSize()[1];

        outputImage->allocate(size, inputImage->getType(), inputImage->getNumberOfComponents());

        ::fwData::Image::OriginType origin(2, 0);
        outputImage->setOrigin(origin);

        const ::fwData::Image::SpacingType::value_type voxelSize = 1;
        ::fwData::Image::SpacingType spacing(2, voxelSize);
        outputImage->setSpacing(spacing);
        outputImage->setWindowWidth(1);
        outputImage->setWindowCenter(0);
    }

    ::fwDataTools::helper::Image outputImgHelper(outputImage);

    // Get ::cv::Mat from fwData::Image
    auto inImage = std::const_pointer_cast< ::fwData::Image>(inputImage);
    ::cv::Mat img = ::cvIO::Image::moveToCv(inImage);

    ::cv::Mat undistortedImage;

#ifndef OPENCV_CUDA_SUPPORT
    ::fwData::mt::ObjectWriteLock outputLock(outputImage);
    if(outputImage != inputImage)
    {
        undistortedImage = ::cvIO::Image::moveToCv(outputImage);
    }
#endif

    {

#ifdef OPENCV_CUDA_SUPPORT
        FW_PROFILE_AVG("cv::cuda::remap", 5);

        ::cv::cuda::GpuMat image_gpu(img);
        ::cv::cuda::GpuMat image_gpu_rect(undistortedImage);
        ::cv::cuda::remap(image_gpu, image_gpu_rect, m_mapx, m_mapy, ::cv::INTER_LINEAR, ::cv::BORDER_CONSTANT);
        undistortedImage = ::cv::Mat(image_gpu_rect);

        ::fwData::mt::ObjectWriteLock outputLock(outputImage);
        ::cvIO::Image::copyFromCv(outputImage, undistortedImage);

#else
        FW_PROFILE_AVG("cv::remap", 5);

        ::cv::remap(img, undistortedImage, m_mapx, m_mapy, ::cv::INTER_LINEAR, ::cv::BORDER_CONSTANT);

        if(outputImage == inputImage)
        {
            // Copy new image.
            // According to OpenCv's doc, if img and undistortedImage have
            // the same size and type, no reallocation will be done. i.e:
            // this call should copy the undistorted image to the video's
            // frameBuffer.
            undistortedImage.copyTo(img);
            SLM_ASSERT("OpenCV did something wrong.", img.data == inputImgHelper.getBuffer());
        }
        else
        {
            SLM_ASSERT("OpenCV did something wrong.", undistortedImage.data == outputImgHelper.getBuffer());
        }
#endif // OPENCV_CUDA_SUPPORT
    }

    sig->asyncEmit();
}

// ----------------------------------------------------------------------------

void SUndistortImage::changeState()
{
    m_isEnabled = !m_isEnabled;
    this->updating();
}

}  // namespace opDistorter
