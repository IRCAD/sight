/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "opDistorter/SDistortImage.hpp"

#include <arData/Camera.hpp>

#include <cvIO/Camera.hpp>
#include <cvIO/Image.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwCore/Profiling.hpp>

#include <fwData/Array.hpp>
#include <fwData/Image.hpp>
#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwDataTools/helper/Image.hpp>
#include <fwDataTools/helper/ImageGetter.hpp>

#include <opencv2/imgproc.hpp>

namespace opDistorter
{

const ::fwCom::Slots::SlotKeyType SDistortImage::s_CHANGE_STATE_SLOT = "changeState";

const ::fwServices::IService::KeyType s_CAMERA_INPUT = "camera";
const ::fwServices::IService::KeyType s_IMAGE_INPUT  = "input";
const ::fwServices::IService::KeyType s_IMAGE_INOUT  = "output";

//------------------------------------------------------------------------------
SDistortImage::SDistortImage() noexcept :
    m_isEnabled(false)
{
    newSlot(s_CHANGE_STATE_SLOT, &SDistortImage::changeState, this);
}

//------------------------------------------------------------------------------

SDistortImage::~SDistortImage() noexcept
{
}

// ----------------------------------------------------------------------------

fwServices::IService::KeyConnectionsMap SDistortImage::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push( s_IMAGE_INPUT, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT );
    connections.push( s_IMAGE_INPUT, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT );

    return connections;
}

//------------------------------------------------------------------------------

void SDistortImage::configuring()
{
}

//------------------------------------------------------------------------------

void SDistortImage::starting()
{
    m_camera = this->getInput< ::arData::Camera> (s_CAMERA_INPUT);
    SLM_FATAL_IF("Object 'camera' is not found.", !m_camera);

    ::cv::Mat intrinsics;
    ::cv::Mat distCoefs;
    ::cv::Size size;

    std::tie(intrinsics, size, distCoefs) = ::cvIO::Camera::copyToCv(m_camera);

    ::cv::Mat pixel_locations_src      = ::cv::Mat( size, CV_32FC2);
    ::cv::Mat fractional_locations_dst = ::cv::Mat( size, CV_32FC2);

    for (int i = 0; i < size.height; i++)
    {
        for (int j = 0; j < size.width; j++)
        {
            pixel_locations_src.at< ::cv::Point2f>(i, j) = ::cv::Point2f(j, i);
        }
        ::cv::undistortPoints(pixel_locations_src.row(i),
                              fractional_locations_dst.row(i),
                              intrinsics, distCoefs);
    }

    ::cv::Mat pixelLocations = ::cv::Mat( size, CV_32FC2);

    // Output from undistortPoints is normalized point coordinates
    const float fx = static_cast<float>(intrinsics.at<double>(0, 0));
    const float fy = static_cast<float>(intrinsics.at<double>(1, 1));
    const float cx = static_cast<float>(intrinsics.at<double>(0, 2));
    const float cy = static_cast<float>(intrinsics.at<double>(1, 2));

    for (int i = 0; i < size.height; i++)
    {
        for (int j = 0; j < size.width; j++)
        {
            const float x = fractional_locations_dst.at< ::cv::Point2f>(i, j).x*fx + cx;
            const float y = fractional_locations_dst.at< ::cv::Point2f>(i, j).y*fy + cy;
            pixelLocations.at< ::cv::Point2f>(i, j) = ::cv::Point2f(x, y);
        }
    }

    std::vector< ::cv::Mat> xy(2);
    ::cv::split(pixelLocations, xy);

#if OPENCV_CUDA_SUPPORT
    m_mapx = ::cv::cuda::GpuMat(xy[0]);
    m_mapy = ::cv::cuda::GpuMat(xy[1]);
#else
    m_mapx = xy[0];
    m_mapy = xy[1];
#endif // OPENCV_CUDA_SUPPORT
}

//------------------------------------------------------------------------------

void SDistortImage::stopping()
{
}

//------------------------------------------------------------------------------

void SDistortImage::updating()
{
    if (m_isEnabled)
    {
        ::arData::Camera::csptr camera = this->getInput< ::arData::Camera >(s_CAMERA_INPUT);
        SLM_FATAL_IF("Object with id \"" + s_CAMERA_INPUT + "\" is not found.", !camera);
        if (camera->getIsCalibrated())
        {
            this->distort();
        }
        else
        {
            OSLM_WARN("Unable to distort the image: " << camera->getID() << " is not calibrated.");
        }
    }
}

//------------------------------------------------------------------------------

void SDistortImage::distort()
{
    FW_PROFILE("distort");
    ::fwData::Image::sptr outputImage = this->getInOut< ::fwData::Image >( s_IMAGE_INOUT);
    SLM_FATAL_IF("Object 'output' is not found.", !outputImage);

    ::fwData::Image::csptr inputImage = this->getInput< ::fwData::Image> (  s_IMAGE_INPUT);
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

void SDistortImage::changeState()
{
    m_isEnabled = !m_isEnabled;
    this->updating();
}

//------------------------------------------------------------------------------

} // namespace opDistorter
