/************************************************************************
 *
 * Copyright (C) 2017-2026 IRCAD France
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

#include "geometry/eigen/helper.hpp"

#include <Eigen/Geometry>

namespace sight::geometry::eigen::helper
{

//-------------------------------------------------------------------------------------------------

data::matrix4::sptr from_eigen(const Eigen::Matrix4f& _mat)
{
    data::matrix4::sptr trf = std::make_shared<data::matrix4>();

    for(unsigned int r = 0 ; r < 4 ; ++r)
    {
        for(unsigned int c = 0 ; c < 4 ; ++c)
        {
            (*trf)(r, c) = static_cast<double>(_mat(r, c));
        }
    }

    return trf;
}

//-------------------------------------------------------------------------------------------------

data::matrix4::sptr from_eigen(const Eigen::Matrix4d& _mat)
{
    data::matrix4::sptr trf = std::make_shared<data::matrix4>();

    for(unsigned int r = 0 ; r < 4 ; ++r)
    {
        for(unsigned int c = 0 ; c < 4 ; ++c)
        {
            (*trf)(r, c) = _mat(r, c);
        }
    }

    return trf;
}

//-------------------------------------------------------------------------------------------------

rvec_tvec_t to_rvec_tvec(const Eigen::Matrix4d& _mat)
{
    Eigen::AngleAxisd angle_axis;

    Eigen::Matrix3d rotation;

    for(unsigned int r = 0 ; r < 3 ; ++r)
    {
        for(unsigned int c = 0 ; c < 3 ; ++c)
        {
            rotation(r, c) = _mat(r, c);
        }
    }

    angle_axis.fromRotationMatrix(rotation);

    Eigen::Vector3d rvec = angle_axis.angle() * angle_axis.axis();
    Eigen::Vector3d tvec = _mat.block<3, 1>(0, 3);

    return std::make_pair(rvec, tvec);
}

//-------------------------------------------------------------------------------------------------

rvec_tvec_t to_eigen_rvec_tvec(const data::matrix4::csptr& _mat)
{
    Eigen::Matrix4d eigen_mat = to_eigen<double>(_mat);
    return to_rvec_tvec(eigen_mat);
}

//-------------------------------------------------------------------------------------------------

} // namespace sight::geometry::eigen::helper
