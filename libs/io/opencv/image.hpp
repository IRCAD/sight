/************************************************************************
 *
 * Copyright (C) 2017-2024 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include <sight/io/opencv/config.hpp>

#include <data/image.hpp>

#include <opencv2/core.hpp>

namespace sight::io::opencv
{

/**
 * @brief Helper to manage conversion from/to OpenCV to/from data::image
 *
 * @note There is no method to move an OpenCV image into a data::image. This is because it is tough and not
 * really recommended to detach a buffer allocated by a cv::Mat, cause we may not free it in the same way than OpenCV.
 * Instead you can allocate a data::image of the correct size/type, and then use move_to_cv() to wrap the image
 * with a cv::Mat. This way every modification made to the buffer will be applied to the data::image, and you
 * don't need an extra stage to get back your modified image.
 */
class SIGHT_IO_OPENCV_CLASS_API image
{
public:

    /**
     * @brief Convert an image data into an OpenCV image.
     *
     * The resulting cv::Mat does not allocate a new buffer and points to the buffer of the data::image.
     * That's why the source image is not const, because the _cvImage may alter the existing buffer.
     */
    SIGHT_IO_OPENCV_API static cv::Mat move_to_cv(data::image::sptr& _image);

    /**
     * @brief Convert an image data into an OpenCV image.
     *
     * The resulting cv::Mat does not allocate a new buffer and points to the buffer of the data::image.
     * This is the const-overload of the above function. As such it returns a non-modifiable cv::Mat.
     */
    SIGHT_IO_OPENCV_API static cv::Mat move_to_cv(const data::image::csptr& _image);

    /**
     * @brief Copy an OpenCV image into our internal image data.
     * @warning The image's pixelFormat is not changed, openCV image does not give this information.
     */
    SIGHT_IO_OPENCV_API static void copy_from_cv(data::image& _image, const cv::Mat& _cv_image);

    /**
     * @brief Copy an image data into an OpenCV image.
     *
     * @note The cvImage allocates its own buffer and has the ownership.
     */
    SIGHT_IO_OPENCV_API static cv::Mat copy_to_cv(const data::image::csptr& _image);
};

} // namespace sight::io::opencv
