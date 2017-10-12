/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CVIO_IMAGE_HPP__
#define __CVIO_IMAGE_HPP__

#include "cvIO/config.hpp"

#include <fwData/Image.hpp>

#include <opencv2/core.hpp>

namespace cvIO
{
/**
 * @brief Helper to manage conversion from/to OpenCV to/from ::fwData::Image
 *
 * @note There is no method to move a OpenCV image into a ::fwData::Image. This is because it is tough and not
 * really recommended to detach a buffer allocated by a ::cv::Mat, cause we may not free it in the same way than OpenCV.
 * Instead you can allocate a ::fwData::Image of the correct size/type, and then use moveToCv() to wrap the image
 * with a ::cv::Mat. This way every modification made to the buffer will be applied to the ::fwData::Image, and you
 * don't need an extra stage to get back your modified image.
 */
class CVIO_CLASS_API Image
{

public:

    /**
     * @brief Convert an image data into an OpenCV image.
     *
     * The resulting ::cv::Mat does not allocate to a new buffer and points to the buffer of the ::fwData::Image.
     * That's why the source image is not const, because the _cvImage may alter the existing buffer.
     */
    CVIO_API static ::cv::Mat moveToCv(::fwData::Image::sptr& _image);

    /**
     * @brief Copy a OpenCV image into our internal image data.
     */
    CVIO_API static void copyFromCv(::fwData::Image::sptr& _image, const ::cv::Mat& _cvImage);

    /**
     * @brief Copy an image data into an OpenCV image.
     *
     * @note The cvImage allocates its own buffer and has the ownership.
     */
    CVIO_API static ::cv::Mat copyToCv(const ::fwData::Image::csptr& _image);

};

}// namespace cvIO

#endif //__CVIO_IMAGE_HPP__
