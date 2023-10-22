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

#include "distortion.hpp"

#include <io/opencv/camera.hpp>
#include <io/opencv/image.hpp>

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#define FW_PROFILING_DISABLED
#include <core/profiling.hpp>

#include <data/array.hpp>

#include <ui/__/dialog/message.hpp>

#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>

namespace sight::module::geometry::vision
{

// Public slot
const core::com::slots::key_t distortion::CHANGE_STATE_SLOT = "changeState";

// Private slot
static const core::com::slots::key_t CALIBRATE_SLOT = "calibrate";

//------------------------------------------------------------------------------
distortion::distortion() noexcept
{
    new_slot(CHANGE_STATE_SLOT, &distortion::changeState, this);
    new_slot(CALIBRATE_SLOT, &distortion::calibrate, this);
}

//------------------------------------------------------------------------------

distortion::~distortion() noexcept =
    default;

// ----------------------------------------------------------------------------

service::connections_t distortion::auto_connections() const
{
    service::connections_t connections;
    connections.push(s_CAMERA_INPUT, data::camera::MODIFIED_SIG, CALIBRATE_SLOT);
    connections.push(s_CAMERA_INPUT, data::camera::INTRINSIC_CALIBRATED_SIG, CALIBRATE_SLOT);
    connections.push(s_IMAGE_INPUT, data::image::MODIFIED_SIG, service::slots::UPDATE);
    connections.push(s_IMAGE_INPUT, data::image::BUFFER_MODIFIED_SIG, service::slots::UPDATE);

    return connections;
}

//------------------------------------------------------------------------------

void distortion::configuring()
{
    const auto config = this->get_config();

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

void distortion::starting()
{
    this->calibrate();
}

//------------------------------------------------------------------------------

void distortion::stopping()
{
    m_calibrationMismatch = false;
    m_prevImageSize       = {0, 0, 0};
}

//------------------------------------------------------------------------------

void distortion::updating()
{
    const auto input_image = m_image.lock();
    SIGHT_ASSERT("No '" << s_IMAGE_INPUT << "' found.", input_image);

    if(input_image && m_calibrationMismatch)
    {
        const auto input_size = input_image->size();
        if(input_size != m_prevImageSize)
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

        auto output_image = m_output.lock();

        SIGHT_ASSERT("No '" << s_IMAGE_INOUT << "' found.", output_image);

        if(input_image && output_image)
        {
            // Since we shallow copy the input image when no remap is done,
            // we have to notify the output image pointer has changed if it was not shared yet before
            bool reallocated = false;
            {
                reallocated = input_image->buffer() != output_image->buffer();
            }

            // Shallow copy the image is faster
            // We only have to take care about reallocating a new buffer when we perform the distortion
            output_image->shallow_copy(input_image.get_shared());

            if(reallocated)
            {
                auto sig = output_image->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
                {
                    core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
                    sig->async_emit();
                }
            }

            auto sig = output_image->signal<data::image::buffer_modified_signal_t>(data::image::BUFFER_MODIFIED_SIG);
            {
                core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
                sig->async_emit();
            }
        }
    }
}

//------------------------------------------------------------------------------

void distortion::remap()
{
    const auto input_image = m_image.lock();
    SIGHT_ASSERT("No '" << s_IMAGE_INPUT << "' found.", input_image);
    auto output_image = m_output.lock();
    SIGHT_ASSERT("No '" << s_IMAGE_INOUT << "' found.", output_image);

    if(!input_image || !output_image || m_calibrationMismatch)
    {
        return;
    }

    FW_PROFILE_AVG("distort", 5);

    auto sig = input_image->signal<data::object::modified_signal_t>(data::image::BUFFER_MODIFIED_SIG);

    // Blocking signals early allows to discard any event while we are updating
    core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));

    const auto input_size = input_image->size();

    if(input_image->getSizeInBytes() == 0 || input_image->numDimensions() < 2)
    {
        SIGHT_WARN("Can not remap this image, it is empty.");
        return;
    }

    const auto camera = m_camera.lock();
    SIGHT_ASSERT("No '" << s_CAMERA_INPUT << "' found.", camera);

    if(input_size[0] != camera->getWidth() || input_size[1] != camera->getHeight())
    {
        std::stringstream msg;
        msg << "Can not distort/undistort, the camera calibration resolution ["
        << camera->getWidth() << "x" << camera->getHeight() << "] does not match the input image size ["
        << input_size[0] << "x" << input_size[1] << "]";

        sight::ui::dialog::message::show(
            "Error",
            msg.str(),
            sight::ui::dialog::message::CRITICAL
        );

        m_calibrationMismatch = true;
        m_prevImageSize       = input_size;
        return;
    }

    const auto prev_size = output_image->size();

    // Since we shallow copy the input image when no remap is done
    // We have to reallocate the output image if it still shares the buffer
    bool realloc = false;
    {
        realloc = input_image->buffer() == output_image->buffer();
    }
    if(prev_size != input_size || realloc)
    {
        data::image::Size size = {input_size[0], input_size[1], 0};

        // Since we may have shared the pointer on the input image, we can't use data::image::allocate
        // Because it will not give us a new buffer and will thus make us modify both input and output images
        data::image::sptr tmp_image = std::make_shared<data::image>();
        output_image->shallow_copy(tmp_image);
        output_image->resize(size, input_image->getType(), input_image->getPixelFormat());

        const data::image::Origin origin = {0., 0., 0.};
        output_image->setOrigin(origin);

        const data::image::Spacing spacing = {1., 1., 1.};
        output_image->setSpacing(spacing);
        output_image->setWindowWidth({1});
        output_image->setWindowCenter({0});
    }

    const auto new_size = output_image->size();

    // Get cv::Mat from data::image
    cv::Mat img = io::opencv::image::move_to_cv(input_image.get_shared());

    cv::Mat undistorted_image;

#ifndef OPENCV_CUDA_SUPPORT
    if(output_image.get_shared() != input_image.get_shared())
    {
        undistorted_image = io::opencv::image::move_to_cv(output_image.get_shared());
    }
#endif

    {
#ifdef OPENCV_CUDA_SUPPORT
        FW_PROFILE_AVG("cv::cuda::remap", 5);

        cv::cuda::GpuMat image_gpu(img);
        cv::cuda::GpuMat image_gpu_rect(undistortedImage);
        cv::cuda::remap(image_gpu, image_gpu_rect, m_map_x, m_map_y, cv::INTER_LINEAR, cv::BORDER_CONSTANT);
        undistortedImage = cv::Mat(image_gpu_rect);

        io::opencv::image::copy_from_cv(outputImage.get_shared(), undistortedImage);
#else
        FW_PROFILE_AVG("cv::remap", 5);

        cv::remap(img, undistorted_image, m_map_x, m_map_y, cv::INTER_LINEAR, cv::BORDER_CONSTANT);

        const auto out_dump_lock = output_image->dump_lock();
        if(output_image.get_shared() == input_image.get_shared())
        {
            // Copy new image.
            // According to OpenCv's doc, if img and undistortedImage have
            // the same size and type, no reallocation will be done. i.e:
            // this call should copy the undistorted image to the video's
            // frameBuffer.
            undistorted_image.copyTo(img);
            SIGHT_ASSERT("OpenCV did something wrong.", img.data == input_image->buffer());
        }
        else
        {
            SIGHT_ASSERT("OpenCV did something wrong.", undistorted_image.data == output_image->buffer());
        }
#endif // OPENCV_CUDA_SUPPORT
    }

    if(prev_size != new_size)
    {
        auto sig_modified = output_image->signal<data::image::modified_signal_t>(data::image::MODIFIED_SIG);
        {
            core::com::connection::blocker another_block(sig_modified->get_connection(slot(service::slots::UPDATE)));
            sig_modified->async_emit();
        }
    }

    auto sig_out = output_image->signal<data::object::modified_signal_t>(data::image::BUFFER_MODIFIED_SIG);
    sig_out->async_emit();
}

// ----------------------------------------------------------------------------

void distortion::changeState()
{
    // Reset the error detection boolean
    m_calibrationMismatch = false;
    m_prevImageSize       = {0, 0, 0};

    m_isEnabled = !m_isEnabled;
}

// ----------------------------------------------------------------------------

void distortion::calibrate()
{
    // Reset the error detection boolean
    m_calibrationMismatch = false;
    m_prevImageSize       = {0, 0, 0};

    const auto camera = m_camera.lock();
    SIGHT_ASSERT("Object 'camera' is not found.", camera);

    cv::Mat intrinsics;
    cv::Mat dist_coefs;
    cv::Size size;

    std::tie(intrinsics, size, dist_coefs) = io::opencv::camera::copyToCv(camera.get_shared());

    std::vector<cv::Mat> xy_maps(2);

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
                dist_coefs
            );
        }

        cv::Mat pixel_locations = cv::Mat(size, CV_32FC2);

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
                pixel_locations.at<cv::Point2f>(i, j) = cv::Point2f(x, y);
            }
        }

        cv::split(pixel_locations, xy_maps);
    }
    else
    {
        cv::initUndistortRectifyMap(
            intrinsics,
            dist_coefs,
            cv::Mat(),
            intrinsics,
            size,
            CV_32FC1,
            xy_maps[0],
            xy_maps[1]
        );
    }

    auto map = m_map.lock();

    if(map)
    {
        cv::Mat cv_map;
        cv::merge(xy_maps, cv_map);

        io::opencv::image::copy_from_cv(*map, cv_map);

        auto sig_modified = map->signal<data::image::modified_signal_t>(data::image::MODIFIED_SIG);
        sig_modified->async_emit();
    }
    else
    {
#if OPENCV_CUDA_SUPPORT
        m_map_x = cv::cuda::GpuMat(xyMaps[0]);
        m_map_y = cv::cuda::GpuMat(xyMaps[1]);
#else
        m_map_x = xy_maps[0];
        m_map_y = xy_maps[1];
#endif // OPENCV_CUDA_SUPPORT
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::geometry::vision
