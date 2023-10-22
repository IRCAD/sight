/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#pragma once

#include "io/opencv/config.hpp"

#include <data/frame_tl.hpp>

#include <opencv2/core.hpp>

namespace sight::io::opencv
{

/**
 * @brief Helper to manage conversion from/to OpenCV to/from data::frame_tl buffer
 *
 * @note There is no method to move a OpenCV image into a data::frame_tl buffer. This is because it is tough and not
 * really recommended to detach a buffer allocated by a cv::Mat, cause we may not free it in the same way than OpenCV.
 * Instead you can allocate a data::frame_tl buffer of the correct size/type, and then use move_to_cv() to wrap the
 * image with a cv::Mat. This way every modification made to the buffer will be applied to the data::frame_tl
 * buffer, and you don't need an extra stage to get back your modified image.
 */
class IO_OPENCV_CLASS_API frame_tl
{
public:

    /**
     * @brief Convert a timeline buffer into an OpenCV image.
     *
     * The resulting cv::Mat does not allocate a new buffer and points to the data::frame_tl::buffer_t.
     * That's why the source buffer is not const, because the _cvImage may alter the existing buffer.
     */
    IO_OPENCV_API static void move_to_cv(
        const data::frame_tl::csptr& _timeline,
        data::frame_tl::buffer_t::element_t* _buffer,
        cv::Mat& _cv_image
    );

    /**
     * @brief Convert a timeline buffer into an OpenCV image.
     *
     * The resulting cv::Mat does not allocate a new buffer and points to the data::frame_tl::buffer_t.
     * The returned cvMat is const because the input buffer is const as well.
     */
    IO_OPENCV_API static cv::Mat move_to_cv(
        const data::frame_tl::csptr& _timeline,
        const data::frame_tl::buffer_t::element_t* _buffer
    );

    /**
     * @brief Copy a OpenCV image into our internal image data.
     */
    IO_OPENCV_API static void copy_from_cv(
        const data::frame_tl::csptr& _timeline,
        data::frame_tl::buffer_t::element_t* _buffer,
        const cv::Mat& _cv_image
    );

    /**
     * @brief Copy an image data into an OpenCV image.
     *
     * @note The cvImage allocates its own buffer and has the ownership.
     */
    IO_OPENCV_API static void copyToCv(
        const data::frame_tl::csptr& _timeline,
        const data::frame_tl::buffer_t::element_t* _buffer,
        cv::Mat& _cv_image
    );
};

} // namespace sight::io::opencv
