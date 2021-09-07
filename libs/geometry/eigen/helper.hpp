/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2018 IHU Strasbourg
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

#include "geometry/eigen/config.hpp"

#include <data/Matrix4.hpp>

#include <Eigen/Core>

namespace sight::geometry::eigen
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
 * @brief Convert from an Eigen float 4x4 Matrix to a data::Matrix4
 * @param _mat : the eigen matrix
 * @return a pointer to a data::Matrix4
 */
GEOMETRY_EIGEN_API data::Matrix4::sptr toF4s(const ::Eigen::Matrix4f& _mat);

/**
 * @brief Convert from an Eigen double 4x4 Matrix to a data::Matrix4
 * @param _mat : the eigen matrix
 * @return a pointer to a data::Matrix4
 */
GEOMETRY_EIGEN_API data::Matrix4::sptr toF4s(const ::Eigen::Matrix4d& _mat);

/**
 * @brief Transform a eigen 4x4 matrix to a rvec tvec representation
 * @param _mat : input matrix
 * @return std::pair of ::Eigen::Vector3d (see RvecTvecType)
 */
GEOMETRY_EIGEN_API RvecTvecType eigenMatToRvecTvec(const ::Eigen::Matrix4d& _mat);

/**
 * @brief Transform rvec tvec representation to a eigen 4x4 matrix
 * @param _mat : input data::Matrix4
 * @return std::pair of ::Eigen::Vector3d (see RvecTvecType)
 */
GEOMETRY_EIGEN_API RvecTvecType f4sMatToRvecTvec(const data::Matrix4::csptr _mat);
/**
 * @brief toEigen
 * @param array of float (16 values)
 * @return eigen Matrix (double)
 */
GEOMETRY_EIGEN_API EigenMatrix toEigen(const std::array<float, 16>& _farray);

/**
 * @brief toEigen
 * @param array of double (16 values)
 * @return eigen Matrix (double)
 */
GEOMETRY_EIGEN_API EigenMatrix toEigen(const std::array<double, 16>& _farray);

/**
 * @brief Transform rvec tvec representation to a eigen 4x4 matrix
 * @param _mat : input matrix
 * @return std::pair of ::Eigen::Vector3d (see RvecTvecType)
 */
GEOMETRY_EIGEN_API RvecTvecType float16ToRvecTvec(const std::array<float, 16>& _farray);

/**
 * @brief Transform a data::Matrix4::sptr to a eigen 4x4 matrix
 * @param _tvec : input translation vector
 * @param _mat : output matrix
 */
template<class T>
::Eigen::Matrix<T, 4, 4, ::Eigen::RowMajor> toEigen(const data::Matrix4::csptr _trf)
{
    ::Eigen::Matrix<T, 4, 4> mat;
    for(unsigned int r = 0 ; r < 4 ; ++r)
    {
        for(unsigned int c = 0 ; c < 4 ; ++c)
        {
            mat(r, c) = static_cast<T>(_trf->getCoefficient(r, c));
        }
    }

    return mat;
}

} //namespace helper

} //namespace sight::geometry::eigen
