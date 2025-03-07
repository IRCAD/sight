/************************************************************************
 *
 * Copyright (C) 2018-2025 IRCAD France
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

#include "point_list.hpp"

namespace sight::io::opencv
{

//------------------------------------------------------------------------------

void point_list::copy_to_cv(const data::point::csptr& _src, cv::Point2d& _dst)
{
    _dst.x = (*_src)[0];
    _dst.y = (*_src)[1];
}

//------------------------------------------------------------------------------

void point_list::copy_to_cv(const data::point_list::csptr& _src, std::vector<cv::Point2d>& _dst)
{
    _dst.clear();
    for(const auto& point : _src->get_points())
    {
        cv::Point2d cv_point;
        copy_to_cv(point, cv_point);
        _dst.push_back(cv_point);
    }
}

//------------------------------------------------------------------------------

void point_list::copy_from_cv(const std::vector<cv::Point2d>& _src, const data::point_list::sptr& _dst)
{
    _dst->get_points().clear();
    for(const auto& cv_point : _src)
    {
        _dst->get_points().push_back(std::make_shared<data::point>(cv_point.x, cv_point.y, 0.));
    }
}

//------------------------------------------------------------------------------

} // namespace sight::io::opencv
