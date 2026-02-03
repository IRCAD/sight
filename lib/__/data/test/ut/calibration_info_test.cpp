/************************************************************************
 *
 * Copyright (C) 2014-2026 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include <core/spy_log.hpp>
#include <core/type.hpp>

#include <data/calibration_info.hpp>
#include <data/point.hpp>
#include <data/point_list.hpp>

#include <utest_data/generator/image.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::data::calibration_info")
{
//------------------------------------------------------------------------------

    TEST_CASE("calibration")
    {
        sight::data::calibration_info::sptr cal_info = std::make_shared<sight::data::calibration_info>();

        sight::data::image::sptr img = std::make_shared<sight::data::image>();
        sight::utest_data::generator::image::generate_random_image(img, sight::core::type::INT16);

        auto pl  = std::make_shared<sight::data::point_list>();
        auto pt1 = std::make_shared<sight::data::point>(1.0, 2.0, 3.0);
        auto pt2 = std::make_shared<sight::data::point>(4.0, 5.0, 6.0);
        auto pt3 = std::make_shared<sight::data::point>(7.0, 8.0, 9.0);

        pl->get_points().push_back(pt1);
        pl->get_points().push_back(pt2);
        pl->get_points().push_back(pt3);

        cal_info->add_record(img, pl);

        //Testing values

        const auto img_list = cal_info->get_image_container();

        CHECK_EQ(std::size_t(1), img_list.size());
        CHECK_EQ(img, img_list.front());

        const auto pl_list = cal_info->get_point_list_container();

        CHECK_EQ(std::size_t(1), pl_list.size());
        CHECK_EQ(pl, pl_list.front());

        sight::data::point_list::csptr pl1 = cal_info->get_point_list(img_list.front());
        CHECK_EQ(sight::data::point_list::csptr(pl), pl1);

        sight::data::image::csptr img1 = cal_info->get_image(pl_list.front());
        CHECK_EQ(sight::data::image::csptr(img), img1);

        cal_info->remove_record(0);

        sight::data::point_list::csptr pl2 = cal_info->get_point_list(img);
        CHECK_EQ(sight::data::point_list::csptr(), pl2);

        sight::data::image::csptr img2 = cal_info->get_image(pl);
        CHECK_EQ(sight::data::image::csptr(), img2);

        CHECK(cal_info->get_image_container().empty());
        CHECK(cal_info->get_point_list_container().empty());
    }

//------------------------------------------------------------------------------

    TEST_CASE("shallow_copy")
    {
        sight::data::calibration_info::sptr cal_info = std::make_shared<sight::data::calibration_info>();

        sight::data::image::sptr img = std::make_shared<sight::data::image>();
        sight::utest_data::generator::image::generate_random_image(img, sight::core::type::INT16);

        auto pl  = std::make_shared<sight::data::point_list>();
        auto pt1 = std::make_shared<sight::data::point>(1.0, 2.0, 3.0);
        auto pt2 = std::make_shared<sight::data::point>(4.0, 5.0, 6.0);
        auto pt3 = std::make_shared<sight::data::point>(7.0, 8.0, 9.0);

        pl->get_points().push_back(pt1);
        pl->get_points().push_back(pt2);
        pl->get_points().push_back(pt3);

        cal_info->add_record(img, pl);

        sight::data::calibration_info::sptr cal_info2 = std::make_shared<sight::data::calibration_info>();
        cal_info2->shallow_copy(cal_info);

        CHECK(cal_info->get_image_container() == cal_info2->get_image_container());
        CHECK(cal_info->get_point_list_container() == cal_info2->get_point_list_container());
    }

//------------------------------------------------------------------------------

    TEST_CASE("deep_copy")
    {
        sight::data::calibration_info::sptr cal_info1 = std::make_shared<sight::data::calibration_info>();

        sight::data::image::sptr img = std::make_shared<sight::data::image>();
        sight::utest_data::generator::image::generate_random_image(img, sight::core::type::INT16);

        auto pl  = std::make_shared<sight::data::point_list>();
        auto pt1 = std::make_shared<sight::data::point>(1.0, 2.0, 3.0);
        auto pt2 = std::make_shared<sight::data::point>(4.0, 5.0, 6.0);
        auto pt3 = std::make_shared<sight::data::point>(7.0, 8.0, 9.0);

        pl->get_points().push_back(pt1);
        pl->get_points().push_back(pt2);
        pl->get_points().push_back(pt3);

        cal_info1->add_record(img, pl);

        sight::data::calibration_info::sptr cal_info2 = std::make_shared<sight::data::calibration_info>();

        // == operator test
        CHECK(*cal_info1 != *cal_info2);

        cal_info2->deep_copy(cal_info1);

        CHECK_EQ(cal_info2->get_image_container().size(), cal_info2->get_point_list_container().size());

        CHECK_EQ(cal_info1->get_image_container().size(), cal_info2->get_image_container().size());
        CHECK_EQ(cal_info1->get_point_list_container().size(), cal_info2->get_point_list_container().size());

        const auto cal_info1_img_list = cal_info1->get_image_container();
        const auto cal_info2_img_list = cal_info2->get_image_container();

        auto iter_img1 = cal_info1_img_list.begin();
        auto iter_img2 = cal_info2_img_list.begin();

        const auto cal_info1_point_list = cal_info1->get_point_list_container();
        const auto cal_info2_point_list = cal_info2->get_point_list_container();

        auto iter_pl1 = cal_info1_point_list.begin();
        auto iter_pl2 = cal_info2_point_list.begin();

        while(iter_img1 != cal_info1_img_list.end())
        {
            CHECK(*iter_img1 != *iter_img2);
            CHECK(*iter_pl1 != *iter_pl2);

            CHECK(**iter_img1 == **iter_img2);
            CHECK(**iter_pl1 == **iter_pl2);

            ++iter_pl1;
            ++iter_pl2;

            ++iter_img1;
            ++iter_img2;
        }

        // == operator test
        CHECK(*cal_info1 == *cal_info2);
    }

//------------------------------------------------------------------------------

    TEST_CASE("get_image")
    {
        auto cal_info = std::make_shared<sight::data::calibration_info>();

        auto img1 = std::make_shared<sight::data::image>();
        sight::utest_data::generator::image::generate_random_image(img1, sight::core::type::INT16);
        auto pl1 = std::make_shared<sight::data::point_list>();
        pl1->set_points(
            {std::make_shared<sight::data::point>(1., 2., 3.), std::make_shared<sight::data::point>(4., 5., 6.),
             std::make_shared<sight::data::point>(.7, 8., 9.)
            });
        cal_info->add_record(img1, pl1);

        auto img2 = std::make_shared<sight::data::image>();
        sight::utest_data::generator::image::generate_random_image(img2, sight::core::type::INT16);
        auto pl2 = std::make_shared<sight::data::point_list>();
        pl2->set_points(
            {std::make_shared<sight::data::point>(10., 11., 12.), std::make_shared<sight::data::point>(
                 13.,
                 14.,
                 15.
             ),
             std::make_shared<sight::data::point>(16., 17., 18.)
            });
        cal_info->add_record(img2, pl2);

        CHECK_EQ(std::const_pointer_cast<const sight::data::image>(img1), cal_info->get_image(pl1));
        CHECK_EQ(std::const_pointer_cast<const sight::data::image>(img2), cal_info->get_image(pl2));
        CHECK_EQ(img1, cal_info->get_image(0));
        CHECK_EQ(img2, cal_info->get_image(1));
    }
} // TEST_SUITE("sight::data::calibration_info")
