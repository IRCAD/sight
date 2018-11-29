/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "cvIO/config.hpp"

#include <fwData/TransformationMatrix3D.hpp>

#include <opencv2/core.hpp>

namespace cvIO
{
/**
 * @brief The Matrix helper class contains static method to convert ::fwData::TransformationMatrix3D to opencv Mat.
 */
class CVIO_CLASS_API Matrix
{
public:

    /**
     * @brief copyFromCv: copies values from OpenCV structure to fw4spl data.
     * @param _src [::cv::Matx44f]: OpenCV 4x4 double matrix.
     * @param _dst [::fwData::TransformationMatrix3D::sptr]: TransformationMatrix (need to be initialized).
     */
    CVIO_API static void copyFromCv( const ::cv::Matx44d& _src, ::fwData::TransformationMatrix3D::sptr& _dst );

    /**
     * @brief copyToCv: copies values from fw4spl TransformationMatrix3D to OpenCV structure.
     * @param _src [::fwData::TransformationMatrix3D::csptr]: fw4spl matrix to be copied.
     * @param _dst [::cv::Matx44d]: OpenCV 4x4 double matrix.
     */
    CVIO_API static void copyToCv(const ::fwData::TransformationMatrix3D::csptr& _src, ::cv::Matx44d& _dst);

    /**
     * @brief copyFromCv: copies values from OpenCV structure to fw4spl data.
     * @param _src [::cv::Matx44f]: OpenCV 4x4 float matrix.
     * @param _dst [::fwData::TransformationMatrix3D::sptr]: TransformationMatrix (need to be initialized).
     */
    CVIO_API static void copyFromCv( const ::cv::Matx44f& _src, ::fwData::TransformationMatrix3D::sptr& _dst );

    /**
     * @brief copyToCv: copies values from fw4spl TransformationMatrix3D to OpenCV structure.
     * @param _src [::fwData::TransformationMatrix3D::csptr]: fw4spl matrix to be copied.
     * @param _dst [::cv::Matx44f]: OpenCV 4x4 float matrix.
     */
    CVIO_API static void copyToCv(const ::fwData::TransformationMatrix3D::csptr& _src, ::cv::Matx44f& _dst);

    /**
     * @brief copyFromCv: copies values from OpenCV structure (rotation & translation matrix) to fw4spl data.
     * @param _rvec [::cv::Mat]: OpenCV 1x3 rotation vector (cast in CV_64F).
     * @param _tvec [::cv::Mat]: OpenCV 1x3 translation vector (cast in CV_64F).
     * @param _dst [::fwData::TransformationMatrix3D::sptr]: TransformationMatrix (need to be initialized).
     */
    CVIO_API static void copyFromCv(const ::cv::Mat& _rvec, const ::cv::Mat& _tvec,
                                    ::fwData::TransformationMatrix3D::sptr& _dst );

    /**
     * @brief copyToCv copies values from fw4spl TransformationMatrix3D to OpenCV structures.
     * @param _src [::fwData::TransformationMatrix3D::csptr]: fw4spl matrix to be copied.
     * @param _rvec [::cv::Mat]: OpenCV 1x3 rotation vector of type CV_64F.
     * @param _tvec [::cv::Mat]: OpenCV 1x3 translation vector of type CV_64F.
     */
    CVIO_API static void copyToCv(const ::fwData::TransformationMatrix3D::csptr& _src, ::cv::Mat& _rvec,
                                  ::cv::Mat& _tvec);

    /**
     * @brief copyFromCv copies values from OpenCV structure to fw4spl data.
     * @param _src [::cv::Mat] OpenCV 4x4 matrix (cast in CV_64F).
     * @param _dst [::fwData::TransformationMatrix3D::sptr]:TransformationMatrix (need to be initialized).
     */
    CVIO_API static void copyFromCv(const ::cv::Mat& _src, ::fwData::TransformationMatrix3D::sptr& _dst);

    /**
     * @brief copyToCv: copies values from fw4spl TransformationMatrix3D to OpenCV structure.
     * @param _src [::fwData::TransformationMatrix3D::csptr]: fw4spl matrix to be copied.
     * @param _dst [::cv::Mat]: OpenCV 4x4 matrix of type CV_64F.
     */
    CVIO_API static void copyToCv(const ::fwData::TransformationMatrix3D::csptr& _src, ::cv::Mat& _dst);

};

} //namespace cvIO
