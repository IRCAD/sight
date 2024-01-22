/************************************************************************
 *
 * Copyright (C) 2017-2024 IRCAD France
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

#include <data/matrix4.hpp>

#include <Eigen/Core>

namespace sight::geometry::eigen::helper
{

/**
 * @brief rvec_tvec_t is a std::pair of Eigen::Vector3d that handles Rotation and Translation Vectors (Rvec, Tvec)
 * The first element is the rotation vector (Rvec)
 * The second element is the translation vector (Tvec)
 */
using rvec_tvec_t = std::pair<Eigen::Vector3d, Eigen::Vector3d>;
using EigenMatrix = Eigen::Matrix<double, 4, 4, Eigen::RowMajor>;
/**
 * @brief Convert from an Eigen float 4x4 Matrix to a data::matrix4
 * @param _mat : the eigen matrix
 * @return a pointer to a data::matrix4
 */
GEOMETRY_EIGEN_API data::matrix4::sptr from_eigen(const Eigen::Matrix4f& _mat);

/**
 * @brief Convert from an Eigen double 4x4 Matrix to a data::matrix4
 * @param _mat : the eigen matrix
 * @return a pointer to a data::matrix4
 */
GEOMETRY_EIGEN_API data::matrix4::sptr from_eigen(const Eigen::Matrix4d& _mat);

/**
 * @brief Transform a eigen 4x4 matrix to a rvec tvec representation
 * @param _mat : input matrix
 * @return std::pair of Eigen::Vector3d (see rvec_tvec_t)
 */
GEOMETRY_EIGEN_API rvec_tvec_t eigen_mat_to_rvec_tvec(const Eigen::Matrix4d& _mat);

/**
 * @brief Transform rvec tvec representation to a eigen 4x4 matrix
 * @param _mat : input data::matrix4
 * @return std::pair of Eigen::Vector3d (see rvec_tvec_t)
 */
GEOMETRY_EIGEN_API rvec_tvec_t f4s_mat_to_rvec_tvec(const data::matrix4::csptr _mat);
/**
 * @brief toEigen
 * @param _farray of float (16 values)
 * @return eigen Matrix (double)
 */
GEOMETRY_EIGEN_API EigenMatrix to_eigen(const std::array<float, 16>& _farray);

/**
 * @brief toEigen
 * @param _farray of double (16 values)
 * @return eigen Matrix (double)
 */
GEOMETRY_EIGEN_API EigenMatrix to_eigen(const std::array<double, 16>& _farray);

/**
 * @brief Transform rvec tvec representation to a eigen 4x4 matrix
 * @param _farray input matrix
 * @return std::pair of Eigen::Vector3d (see rvec_tvec_t)
 */
GEOMETRY_EIGEN_API rvec_tvec_t float16_to_rvec_tvec(const std::array<float, 16>& _farray);

/**
 * @brief Transform a data::matrix4::sptr to a eigen 4x4 matrix
 * @param _trf  input matrix to transform
 * @return the corresponding eigen 4x4 matrix
 */
template<class T = double>
Eigen::Matrix<T, 4, 4, Eigen::RowMajor> to_eigen(const data::matrix4::csptr _trf)
{
    Eigen::Matrix<T, 4, 4> mat;
    for(unsigned int r = 0 ; r < 4 ; ++r)
    {
        for(unsigned int c = 0 ; c < 4 ; ++c)
        {
            mat(r, c) = static_cast<T>((*_trf)(r, c));
        }
    }

    return mat;
}

} // namespace sight::geometry::eigen::helper
