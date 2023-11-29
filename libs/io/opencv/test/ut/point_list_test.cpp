/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

#include "point_list_test.hpp"

#include <data/point_list.hpp>

#include <io/opencv/point_list.hpp>

#include <opencv2/core.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::opencv::ut::point_list_test);

namespace sight::io::opencv::ut
{

//------------------------------------------------------------------------------

static void assert2d_point_equality(const data::point::csptr& _pt, const cv::Point2d& _cv_pt)
{
    const auto& coords = _pt->get_coord();
    CPPUNIT_ASSERT_EQUAL(coords[0], _cv_pt.x);
    CPPUNIT_ASSERT_EQUAL(coords[1], _cv_pt.y);
    CPPUNIT_ASSERT_EQUAL(coords[2], 0.);
}

//-----------------------------------------------------------------------------

void point_list_test::setUp()
{
}

//-----------------------------------------------------------------------------

void point_list_test::tearDown()
{
}

//-----------------------------------------------------------------------------

void point_list_test::copy_from_cv()
{
    std::vector<cv::Point2d> cv_point_list;

    for(std::uint16_t i = 0 ; i < 512 ; ++i)
    {
        cv_point_list.emplace_back(double(i), double(i * 2 + 3));
    }

    data::point_list::sptr pl = std::make_shared<data::point_list>();
    io::opencv::point_list::copy_from_cv(cv_point_list, pl);

    for(std::uint16_t i = 0 ; i < 512 ; ++i)
    {
        data::point::csptr pt = pl->get_points()[i];
        assert2d_point_equality(pt, cv_point_list[i]);
        CPPUNIT_ASSERT_EQUAL(i, std::uint16_t(pt->get_coord()[0]));
        CPPUNIT_ASSERT_EQUAL(std::uint16_t(i * 2 + 3), std::uint16_t(pt->get_coord()[1]));
        CPPUNIT_ASSERT_EQUAL(0., pt->get_coord()[2]);
    }
}

//-----------------------------------------------------------------------------

void point_list_test::copy_to_cv()
{
    data::point_list::sptr pl = std::make_shared<data::point_list>();

    for(std::uint16_t i = 0 ; i < 512 ; ++i)
    {
        data::point::sptr point = std::make_shared<data::point>(double(i), double(i * 3 + 5), 0.);
        pl->get_points().push_back(point);
    }

    std::vector<cv::Point2d> cv_point_list;
    io::opencv::point_list::copy_to_cv(pl, cv_point_list);

    for(std::uint16_t i = 0 ; i < 512 ; ++i)
    {
        assert2d_point_equality(pl->get_points()[i], cv_point_list[i]);
        CPPUNIT_ASSERT_EQUAL(i, std::uint16_t(cv_point_list[i].x));
        CPPUNIT_ASSERT_EQUAL(std::uint16_t(i * 3 + 5), std::uint16_t(cv_point_list[i].y));
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::io::opencv::ut
