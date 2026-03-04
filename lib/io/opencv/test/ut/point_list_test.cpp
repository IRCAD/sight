/************************************************************************
 *
 * Copyright (C) 2018-2026 IRCAD France
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

#include <data/point_list.hpp>

#include <io/opencv/point_list.hpp>

#include <doctest/doctest.h>

#include <opencv2/core.hpp>

//------------------------------------------------------------------------------

static void assert2d_point_equality(const sight::data::point::csptr& _pt, const cv::Point2d& _cv_pt)
{
    CHECK_EQ((*_pt)[0], _cv_pt.x);
    CHECK_EQ((*_pt)[1], _cv_pt.y);
    CHECK_EQ((*_pt)[2], 0.);
}

TEST_SUITE("sight::io::opencv::point_list")
{
    TEST_CASE("copy_from_cv")
    {
        std::vector<cv::Point2d> cv_point_list;

        cv_point_list.reserve(512);
        for(std::uint16_t i = 0 ; i < 512 ; ++i)
        {
            cv_point_list.emplace_back(double(i), double(i * 2 + 3));
        }

        sight::data::point_list::sptr pl = std::make_shared<sight::data::point_list>();
        sight::io::opencv::point_list::copy_from_cv(cv_point_list, pl);

        for(std::uint16_t i = 0 ; i < 512 ; ++i)
        {
            sight::data::point::csptr pt = (*pl)[i];
            assert2d_point_equality(pt, cv_point_list[i]);
            CHECK_EQ(i, std::uint16_t((*pt)[0]));
            CHECK_EQ(std::uint16_t(i * 2 + 3), std::uint16_t((*pt)[1]));
            CHECK_EQ(0., (*pt)[2]);
        }
    }

    TEST_CASE("copy_to_cv")
    {
        sight::data::point_list::sptr pl = std::make_shared<sight::data::point_list>();

        for(std::uint16_t i = 0 ; i < 512 ; ++i)
        {
            sight::data::point::sptr point = std::make_shared<sight::data::point>(double(i), double(i * 3 + 5), 0.);
            pl->push_back(point);
        }

        std::vector<cv::Point2d> cv_point_list;
        sight::io::opencv::point_list::copy_to_cv(pl, cv_point_list);

        for(std::uint16_t i = 0 ; i < 512 ; ++i)
        {
            assert2d_point_equality((*pl)[i], cv_point_list[i]);
            CHECK_EQ(i, std::uint16_t(cv_point_list[i].x));
            CHECK_EQ(std::uint16_t(i * 3 + 5), std::uint16_t(cv_point_list[i].y));
        }
    }
}
