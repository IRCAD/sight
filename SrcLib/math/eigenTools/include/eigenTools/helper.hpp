/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "eigenTools/config.hpp"

#include <fwData/TransformationMatrix3D.hpp>

#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Eigen>

namespace eigenTools
{
namespace helper
{
/**
 * @brief RvecTvecType is a std::pair of ::Eigen::Vector3d that handles Rotation and Translation Vectors (Rvec, Tvec)
 * The first element is the rotation vector (Rvec)
 * The second element is the translation vector (Tvec)
 */
typedef std::pair< ::Eigen::Vector3d, ::Eigen::Vector3d> RvecTvecType;
typedef ::Eigen::Matrix<double, 4, 4, ::Eigen::RowMajor> EigenMatrix;
/**
 * @brief Convert from an Eigen float 4x4 Matrix to a fwData::TransformationMatrix3D
 * @param _mat : the eigen matrix
 * @return a pointer to a ::fwData::TransformationMatrix3D
 */
EIGENTOOLS_API ::fwData::TransformationMatrix3D::sptr toF4s( const ::Eigen::Matrix4f& _mat);

/**
 * @brief Convert from an Eigen double 4x4 Matrix to a fwData::TransformationMatrix3D
 * @param _mat : the eigen matrix
 * @return a pointer to a ::fwData::TransformationMatrix3D
 */
EIGENTOOLS_API ::fwData::TransformationMatrix3D::sptr toF4s( const ::Eigen::Matrix4d& _mat);

/**
 * @brief Transform a eigen 4x4 matrix to a rvec tvec representation
 * @param _mat : input matrix
 * @return std::pair of ::Eigen::Vector3d (see RvecTvecType)
 */
EIGENTOOLS_API RvecTvecType eigenMatToRvecTvec( const ::Eigen::Matrix4d& _mat);

/**
 * @brief Transform rvec tvec representation to a eigen 4x4 matrix
 * @param _mat : input ::fwData::TransformationMatrix3D
 * @return std::pair of ::Eigen::Vector3d (see RvecTvecType)
 */
EIGENTOOLS_API RvecTvecType f4sMatToRvecTvec( const ::fwData::TransformationMatrix3D::csptr _mat);
/**
 * @brief toEigen
 * @param array of float (16 values)
 * @return eigen Matrix (double)
 */
EIGENTOOLS_API EigenMatrix toEigen(const std::array<float, 16>& _farray);

/**
 * @brief toEigen
 * @param array of double (16 values)
 * @return eigen Matrix (double)
 */
EIGENTOOLS_API EigenMatrix toEigen(const std::array<double, 16>& _farray);

/**
 * @brief Transform rvec tvec representation to a eigen 4x4 matrix
 * @param _mat : input matrix
 * @return std::pair of ::Eigen::Vector3d (see RvecTvecType)
 */
EIGENTOOLS_API RvecTvecType float16ToRvecTvec(const std::array<float, 16>& _farray);

/**
 * @brief Transform a ::fwData::TransformationMatrix3D::sptr to a eigen 4x4 matrix
 * @param _tvec : input translation vector
 * @param _mat : output matrix
 */
template< class T>
::Eigen::Matrix<T, 4, 4, ::Eigen::RowMajor> toEigen( const ::fwData::TransformationMatrix3D::csptr _trf)
{
    ::Eigen::Matrix<T, 4, 4> mat;
    for(unsigned int r = 0; r < 4; ++r)
    {
        for(unsigned int c = 0; c < 4; ++c)
        {
            mat(r, c) = _trf->getCoefficient(r, c);
        }
    }

    return mat;
}

} //namespace helper
} //namespace eigenTools
