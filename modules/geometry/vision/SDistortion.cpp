/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

#include "SDistortion.hpp"

#include <io/opencv/Camera.hpp>
#include <io/opencv/Image.hpp>

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#define FW_PROFILING_DISABLED
#include <core/profiling.hpp>

#include <data/Array.hpp>

#include <ui/__/dialog/message.hpp>

#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>

namespace sight::module::geometry::vision
{

// Public slot
const core::com::slots::key_t SDistortion::CHANGE_STATE_SLOT = "changeState";

// Private slot
static const core::com::slots::key_t CALIBRATE_SLOT = "calibrate";

//------------------------------------------------------------------------------
SDistortion::SDistortion() noexcept
{
    new_slot(CHANGE_STATE_SLOT, &SDistortion::changeState, this);
    new_slot(CALIBRATE_SLOT, &SDistortion::calibrate, this);
}

//------------------------------------------------------------------------------

SDistortion::~SDistortion() noexcept =
    default;

// ----------------------------------------------------------------------------

service::connections_t SDistortion::getAutoConnections() const
{
    service::connections_t connections;
    connections.push(s_CAMERA_INPUT, data::Camera::MODIFIED_SIG, CALIBRATE_SLOT);
    connections.push(s_CAMERA_INPUT, data::Camera::INTRINSIC_CALIBRATED_SIG, CALIBRATE_SLOT);
    connections.push(s_IMAGE_INPUT, data::Image::MODIFIED_SIG, service::slots::UPDATE);
    connections.push(s_IMAGE_INPUT, data::Image::BUFFER_MODIFIED_SIG, service::slots::UPDATE);

    return connections;
}

//------------------------------------------------------------------------------

void SDistortion::configuring()
{
    const auto config = this->getConfiguration();

    const auto mode = config.get<std::string>("mode");
    if(mode == "undistort")
    {
        m_distort = false;
    }
    else if(mode != "distort")
    {
        SIGHT_ERROR("Mode should be distort or undistort");
    }
}

//------------------------------------------------------------------------------

void SDistortion::starting()
{
    this->calibrate();
}

//------------------------------------------------------------------------------

void SDistortion::stopping()
{
    m_calibrationMismatch = false;
    m_prevImageSize       = {0, 0, 0};
}

//------------------------------------------------------------------------------

void SDistortion::updating()
{
    const auto inputImage = m_image.lock();
    SIGHT_ASSERT("No '" << s_IMAGE_INPUT << "' found.", inputImage);

    if(inputImage && m_calibrationMismatch)
    {
        const auto inputSize = inputImage->size();
        if(inputSize != m_prevImageSize)
        {
            // Reset the error detection boolean
            m_calibrationMismatch = false;
            m_prevImageSize       = {0, 0, 0};
        }
    }

    if(m_isEnabled)
    {
        const auto camera = m_camera.lock();
        SIGHT_ASSERT("No '" << s_CAMERA_INPUT << "' found.", camera);

        if(camera->getIsCalibrated())
        {
            this->remap();
        }
        else
        {
            SIGHT_WARN("Unable to distort/undistort the image: camera '" + camera->get_id() + "' is not calibrated.");
        }
    }
    else
    {
        // Simple copy of the input image

        auto outputImage = m_output.lock();

        SIGHT_ASSERT("No '" << s_IMAGE_INOUT << "' found.", outputImage);

        if(inputImage && outputImage)
        {
            // Since we shallow copy the input image when no remap is done,
            // we have to notify the output image pointer has changed if it was not shared yet before
            bool reallocated = false;
            {
                reallocated = inputImage->buffer() != outputImage->buffer();
            }

            // Shallow copy the image is faster
            // We only have to take care about reallocating a new buffer when we perform the distortion
            outputImage->shallow_copy(inputImage.get_shared());

            if(reallocated)
            {
                auto sig = outputImage->signal<data::Object::ModifiedSignalType>(data::Object::MODIFIED_SIG);
                {
                    core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
                    sig->async_emit();
                }
            }

            auto sig = outputImage->signal<data::Image::BufferModifiedSignalType>(data::Image::BUFFER_MODIFIED_SIG);
            {
                core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
                sig->async_emit();
            }
        }
    }
}

//------------------------------------------------------------------------------

void SDistortion::remap()
{
    const auto inputImage = m_image.lock();
    SIGHT_ASSERT("No '" << s_IMAGE_INPUT << "' found.", inputImage);
    auto outputImage = m_output.lock();
    SIGHT_ASSERT("No '" << s_IMAGE_INOUT << "' found.", outputImage);

    if(!inputImage || !outputImage || m_calibrationMismatch)
    {
        return;
    }

    FW_PROFILE_AVG("distort", 5);

    auto sig = inputImage->signal<data::Object::ModifiedSignalType>(data::Image::BUFFER_MODIFIED_SIG);

    // Blocking signals early allows to discard any event while we are updating
    core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));

    const auto inputSize = inputImage->size();

    if(inputImage->getSizeInBytes() == 0 || inputImage->numDimensions() < 2)
    {
        SIGHT_WARN("Can not remap this image, it is empty.");
        return;
    }

    const auto camera = m_camera.lock();
    SIGHT_ASSERT("No '" << s_CAMERA_INPUT << "' found.", camera);

    if(inputSize[0] != camera->getWidth() || inputSize[1] != camera->getHeight())
    {
        std::stringstream msg;
        msg << "Can not distort/undistort, the camera calibration resolution ["
        << camera->getWidth() << "x" << camera->getHeight() << "] does not match the input image size ["
        << inputSize[0] << "x" << inputSize[1] << "]";

        sight::ui::dialog::message::show(
            "Error",
            msg.str(),
            sight::ui::dialog::message::CRITICAL
        );

        m_calibrationMismatch = true;
        m_prevImageSize       = inputSize;
        return;
    }

    const auto prevSize = outputImage->size();

    // Since we shallow copy the input image when no remap is done
    // We have to reallocate the output image if it still shares the buffer
    bool realloc = false;
    {
        realloc = inputImage->buffer() == outputImage->buffer();
    }
    if(prevSize != inputSize || realloc)
    {
        data::Image::Size size = {inputSize[0], inputSize[1], 0};

        // Since we may have shared the pointer on the input image, we can't use data::Image::allocate
        // Because it will not give us a new buffer and will thus make us modify both input and output images
        data::Image::sptr tmpImage = std::make_shared<data::Image>();
        outputImage->shallow_copy(tmpImage);
        outputImage->resize(size, inputImage->getType(), inputImage->getPixelFormat());

        const data::Image::Origin origin = {0., 0., 0.};
        outputImage->setOrigin(origin);

        const data::Image::Spacing spacing = {1., 1., 1.};
        outputImage->setSpacing(spacing);
        outputImage->setWindowWidth({1});
        outputImage->setWindowCenter({0});
    }

    const auto newSize = outputImage->size();

    // Get cv::Mat from data::Image
    cv::Mat img = io::opencv::Image::moveToCv(inputImage.get_shared());

    cv::Mat undistortedImage;

#ifndef OPENCV_CUDA_SUPPORT
    if(outputImage.get_shared() != inputImage.get_shared())
    {
        undistortedImage = io::opencv::Image::moveToCv(outputImage.get_shared());
    }
#endif

    {
#ifdef OPENCV_CUDA_SUPPORT
        FW_PROFILE_AVG("cv::cuda::remap", 5);

        cv::cuda::GpuMat image_gpu(img);
        cv::cuda::GpuMat image_gpu_rect(undistortedImage);
        cv::cuda::remap(image_gpu, image_gpu_rect, m_map_x, m_map_y, cv::INTER_LINEAR, cv::BORDER_CONSTANT);
        undistortedImage = cv::Mat(image_gpu_rect);

        io::opencv::Image::copyFromCv(outputImage.get_shared(), undistortedImage);
#else
        FW_PROFILE_AVG("cv::remap", 5);

        cv::remap(img, undistortedImage, m_map_x, m_map_y, cv::INTER_LINEAR, cv::BORDER_CONSTANT);

        const auto outDumpLock = outputImage->dump_lock();
        if(outputImage.get_shared() == inputImage.get_shared())
        {
            // Copy new image.
            // According to OpenCv's doc, if img and undistortedImage have
            // the same size and type, no reallocation will be done. i.e:
            // this call should copy the undistorted image to the video's
            // frameBuffer.
            undistortedImage.copyTo(img);
            SIGHT_ASSERT("OpenCV did something wrong.", img.data == inputImage->buffer());
        }
        else
        {
            SIGHT_ASSERT("OpenCV did something wrong.", undistortedImage.data == outputImage->buffer());
        }
#endif // OPENCV_CUDA_SUPPORT
    }

    if(prevSize != newSize)
    {
        auto sigModified = outputImage->signal<data::Image::ModifiedSignalType>(data::Image::MODIFIED_SIG);
        {
            core::com::connection::blocker anotherBlock(sigModified->get_connection(slot(service::slots::UPDATE)));
            sigModified->async_emit();
        }
    }

    auto sigOut = outputImage->signal<data::Object::ModifiedSignalType>(data::Image::BUFFER_MODIFIED_SIG);
    sigOut->async_emit();
}

// ----------------------------------------------------------------------------

void SDistortion::changeState()
{
    // Reset the error detection boolean
    m_calibrationMismatch = false;
    m_prevImageSize       = {0, 0, 0};

    m_isEnabled = !m_isEnabled;
}

// ----------------------------------------------------------------------------

void SDistortion::calibrate()
{
    // Reset the error detection boolean
    m_calibrationMismatch = false;
    m_prevImageSize       = {0, 0, 0};

    const auto camera = m_camera.lock();
    SIGHT_ASSERT("Object 'camera' is not found.", camera);

    cv::Mat intrinsics;
    cv::Mat distCoefs;
    cv::Size size;

    std::tie(intrinsics, size, distCoefs) = io::opencv::Camera::copyToCv(camera.get_shared());

    std::vector<cv::Mat> xyMaps(2);

    if(m_distort)
    {
        cv::Mat pixel_locations_src      = cv::Mat(size, CV_32FC2);
        cv::Mat fractional_locations_dst = cv::Mat(size, CV_32FC2);

        for(int i = 0 ; i < size.height ; i++)
        {
            for(int j = 0 ; j < size.width ; j++)
            {
                pixel_locations_src.at<cv::Point2f>(i, j) = cv::Point2f(float(j), float(i));
            }

            cv::undistortPoints(
                pixel_locations_src.row(i),
                fractional_locations_dst.row(i),
                intrinsics,
                distCoefs
            );
        }

        cv::Mat pixelLocations = cv::Mat(size, CV_32FC2);

        // Output from undistortPoints is normalized point coordinates
        const auto fx = static_cast<float>(intrinsics.at<double>(0, 0));
        const auto fy = static_cast<float>(intrinsics.at<double>(1, 1));
        const auto cx = static_cast<float>(intrinsics.at<double>(0, 2));
        const auto cy = static_cast<float>(intrinsics.at<double>(1, 2));

        for(int i = 0 ; i < size.height ; i++)
        {
            for(int j = 0 ; j < size.width ; j++)
            {
                const float x = fractional_locations_dst.at<cv::Point2f>(i, j).x * fx + cx;
                const float y = fractional_locations_dst.at<cv::Point2f>(i, j).y * fy + cy;
                pixelLocations.at<cv::Point2f>(i, j) = cv::Point2f(x, y);
            }
        }

        cv::split(pixelLocations, xyMaps);
    }
    else
    {
        cv::initUndistortRectifyMap(
            intrinsics,
            distCoefs,
            cv::Mat(),
            intrinsics,
            size,
            CV_32FC1,
            xyMaps[0],
            xyMaps[1]
        );
    }

    auto map = m_map.lock();

    if(map)
    {
        cv::Mat cvMap;
        cv::merge(xyMaps, cvMap);

        io::opencv::Image::copyFromCv(*map, cvMap);

        auto sigModified = map->signal<data::Image::ModifiedSignalType>(data::Image::MODIFIED_SIG);
        sigModified->async_emit();
    }
    else
    {
#if OPENCV_CUDA_SUPPORT
        m_map_x = cv::cuda::GpuMat(xyMaps[0]);
        m_map_y = cv::cuda::GpuMat(xyMaps[1]);
#else
        m_map_x = xyMaps[0];
        m_map_y = xyMaps[1];
#endif // OPENCV_CUDA_SUPPORT
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::geometry::vision
