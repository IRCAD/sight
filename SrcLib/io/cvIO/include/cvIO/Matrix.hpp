/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "cvIO/config.hpp"

#include <fwData/TransformationMatrix3D.hpp>

#include <opencv2/core.hpp>

namespace cvIO
{
/**
 * @brief The Matrix class
 */
class CVIO_CLASS_API Matrix
{
public:

    /**
     * @brief copyFromCv: copies values from OpenCV structure to fw4spl data.
     * @param _src [::cv::Matx44f]: Opencv 4x4 float matrix.
     * @param _dst [::fwData::TransformationMatrix3D::sptr]: TranformationMatrix (need to be initialized).
     */
    CVIO_API static void copyFromCv( const ::cv::Matx44f& _src, ::fwData::TransformationMatrix3D::sptr& _dst );

    /**
     * @brief copyToCv: copies values from fw4spl TransformationMatrix3D to OpenCV structure.
     * @param _src [::fwData::TransformationMatrix3D::csptr]: fw4spl matrix to be copied.
     * @param _dst [::cv::Matx44f]: OpenCV 4x4 float matrix.
     */
    CVIO_API static void copyToCv(const ::fwData::TransformationMatrix3D::csptr& _src, ::cv::Matx44f& _dst);
};

} //namespace cvIO
