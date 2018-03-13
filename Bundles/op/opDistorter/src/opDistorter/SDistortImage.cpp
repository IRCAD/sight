/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "opDistorter/SDistortImage.hpp"

#include <arData/Camera.hpp>

#include <cvIO/Image.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Array.hpp>
#include <fwData/Image.hpp>
#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwDataTools/helper/Array.hpp>
#include <fwDataTools/helper/Image.hpp>

#include <fwServices/macros.hpp>

#include <opencv2/imgproc.hpp>

namespace opDistorter
{

fwServicesRegisterMacro( ::fwServices::IController, ::opDistorter::SDistortImage, ::fwData::Image);

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
    connections.push( "input", ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT );
    connections.push( "input", ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT );

    return connections;
}

//------------------------------------------------------------------------------

void SDistortImage::starting()
{
}

//------------------------------------------------------------------------------

void SDistortImage::stopping()
{
}

//------------------------------------------------------------------------------

void SDistortImage::configuring()
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
    ::arData::Camera::csptr camera = this->getInput< ::arData::Camera >(s_CAMERA_INPUT);
    SLM_FATAL_IF("Camera with id \"" + s_CAMERA_INPUT + "\" is not found.", !camera);

    ::fwData::Image::csptr image = this->getInput< ::fwData::Image >(s_IMAGE_INPUT);
    SLM_FATAL_IF("Image with id \"" + s_IMAGE_INPUT + "\" is not found.", !image);

    ::fwData::mt::ObjectReadLock lock(image);
    ::fwData::Array::sptr arraySrc = image->getDataArray();
    ::fwDataTools::helper::Array arraySrcHelper(arraySrc);

    if (!arraySrcHelper.getBuffer())
    {
        SLM_WARN("Empty image buffer");
        return;
    }

    ::fwData::Array::sptr arrayDst = ::fwData::Array::New();
    arrayDst->resize(image->getType(), image->getSize(), image->getNumberOfComponents(), true);

    ::fwDataTools::helper::Array arrayDstHelper(arrayDst);

    ::cv::Mat src =
        ::cv::Mat(::cv::Size(static_cast<int>(image->getSize()[0]), static_cast<int>(image->getSize()[1])),
                  CV_8UC4, (void*)arraySrcHelper.getBuffer(), ::cv::Mat::AUTO_STEP);

    ::cv::Mat cameraMatrix = ::cv::Mat::eye(3, 3, CV_64F);
    ::cv::Mat distCoeffs   = ::cv::Mat::eye(4, 1, CV_64F);

    // 3x3 matrix (fx 0 cx, 0 fy cy, 0 0 1)
    cameraMatrix.at<double>(0, 0) = camera->getFx();
    cameraMatrix.at<double>(1, 1) = camera->getFy();
    cameraMatrix.at<double>(0, 2) = camera->getCx();
    cameraMatrix.at<double>(1, 2) = camera->getCy();

    //4x1 matrix (k1,k2,p1,p2)
    for (unsigned int i = 0; i < 4; ++i)
    {
        distCoeffs.at<double>(static_cast<int>(i), 0) = camera->getDistortionCoefficient()[i];
    }

    ::cv::Mat pixel_locations_src      = ::cv::Mat(src.size(), CV_32FC2);
    ::cv::Mat fractional_locations_dst = ::cv::Mat(src.size(), CV_32FC2);

    for (int i = 0; i < src.size().height; i++)
    {
        for (int j = 0; j < src.size().width; j++)
        {
            pixel_locations_src.at< ::cv::Point2f>(i, j) = ::cv::Point2f(j, i);
        }
        ::cv::undistortPoints(pixel_locations_src.row(i),
                              fractional_locations_dst.row(i),
                              cameraMatrix, distCoeffs);
    }

    ::cv::Mat pixel_locations_dst = ::cv::Mat(src.size(), CV_32FC2);

    // Output from undistortPoints is normalized point coordinates
    const double fx = cameraMatrix.at<double>(0, 0);
    const double fy = cameraMatrix.at<double>(1, 1);
    const double cx = cameraMatrix.at<double>(0, 2);
    const double cy = cameraMatrix.at<double>(1, 2);
    // TODO: is there a faster way to do this?
    for (int i = 0; i < src.size().height; i++)
    {
        for (int j = 0; j < src.size().width; j++)
        {
            const float x = static_cast<float>(fractional_locations_dst.at< ::cv::Point2f>(i, j).x*fx + cx);
            const float y = static_cast<float>(fractional_locations_dst.at< ::cv::Point2f>(i, j).y*fy + cy);
            pixel_locations_dst.at< ::cv::Point2f>(i, j) = ::cv::Point2f(x, y);
        }
    }

    ::fwData::Image::sptr outputImage = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_FATAL_IF("Image with id \"" + s_IMAGE_INOUT + "\" is not found.", !outputImage);

    ::fwData::mt::ObjectWriteLock outputLock(outputImage);

    if(!outputImage->getSizeInBytes())
    {
        ::fwData::Image::SizeType size(2);
        size[0] = image->getSize()[0];
        size[1] = image->getSize()[1];

        outputImage->allocate(size, image->getType(), image->getNumberOfComponents());

        ::fwData::Image::OriginType origin(2, 0);
        outputImage->setOrigin(origin);

        const ::fwData::Image::SpacingType::value_type voxelSize = 1;
        ::fwData::Image::SpacingType spacing(2, voxelSize);
        outputImage->setSpacing(spacing);
        outputImage->setWindowWidth(1);
        outputImage->setWindowCenter(0);
    }

    ::cv::Mat dst = ::cvIO::Image::moveToCv(outputImage);

    ::cv::remap(src, dst, pixel_locations_dst, ::cv::Mat(), CV_INTER_LINEAR);

    auto sig = outputImage->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Image::s_BUFFER_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

// ----------------------------------------------------------------------------

void SDistortImage::changeState()
{
    m_isEnabled = !m_isEnabled;
    this->updating();
}

//------------------------------------------------------------------------------

} // namespace opDistorter
