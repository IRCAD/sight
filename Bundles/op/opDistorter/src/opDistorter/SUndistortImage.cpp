/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "opDistorter/SUndistortImage.hpp"

#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

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
}

// ----------------------------------------------------------------------------

void SUndistortImage::undistort()
{
    ::fwData::Image::sptr outputImage = this->getInOut< ::fwData::Image >("output");
    SLM_FATAL_IF("Object 'output' is not found.", !outputImage);

    ::fwData::Image::csptr inputImage = this->getInput< ::fwData::Image> ("input");
    SLM_FATAL_IF("Object 'input' is not found.", !inputImage);

    ::fwDataTools::helper::ImageGetter inputImgHelper(inputImage);

    if (!inputImgHelper.getBuffer())
    {
        return;
    }

    if(!outputImage->getSizeInBytes())
    {
        ::fwData::Image::SizeType size(3);
        size[0] = inputImage->getSize()[0];
        size[1] = inputImage->getSize()[1];
        size[2] = 1;

        outputImage->allocate(size, inputImage->getType(), inputImage->getNumberOfComponents());

        ::fwData::Image::OriginType origin(3, 0);
        outputImage->setOrigin(origin);

        const ::fwData::Image::SpacingType::value_type voxelSize = 1;
        ::fwData::Image::SpacingType spacing(3, voxelSize);
        outputImage->setSpacing(spacing);
        outputImage->setWindowWidth(1);
        outputImage->setWindowCenter(0);
    }

    ::fwDataTools::helper::Image outputImgHelper(outputImage);

    // Get cv::Mat from fwData::Image
    cv::Mat img(static_cast<int>(inputImage->getSize()[1]), static_cast<int>(inputImage->getSize()[0]), CV_8UC4,
                inputImgHelper.getBuffer(), ::cv::Mat::AUTO_STEP);

    cv::Mat undistortedImage;
    if(outputImage != inputImage)
    {
        undistortedImage =
            cv::Mat(static_cast<int>(outputImage->getSize()[1]),
                    static_cast<int>(outputImage->getSize()[0]), CV_8UC4, outputImgHelper.getBuffer(),
                    ::cv::Mat::AUTO_STEP);
    }

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

    cv::undistort(img, undistortedImage, cameraMatrix, distCoefs);

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

    auto sig = outputImage->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Image::s_BUFFER_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

// ----------------------------------------------------------------------------

void SUndistortImage::changeState()
{
    m_isEnabled = !m_isEnabled;
    this->updating();
}

}  // namespace opDistorter
