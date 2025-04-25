/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include "calibration_info_test.hpp"

#include <core/spy_log.hpp>
#include <core/type.hpp>

#include <data/calibration_info.hpp>
#include <data/point.hpp>
#include <data/point_list.hpp>

#include <utest_data/generator/image.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::calibration_info_test);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void calibration_info_test::setUp()
{
}

//------------------------------------------------------------------------------

void calibration_info_test::tearDown()
{
}

//------------------------------------------------------------------------------

void calibration_info_test::calibration_test()
{
    data::calibration_info::sptr cal_info = std::make_shared<data::calibration_info>();

    data::image::sptr img = std::make_shared<data::image>();
    utest_data::generator::image::generate_random_image(img, core::type::INT16);

    auto pl  = std::make_shared<data::point_list>();
    auto pt1 = std::make_shared<data::point>(1.0, 2.0, 3.0);
    auto pt2 = std::make_shared<data::point>(4.0, 5.0, 6.0);
    auto pt3 = std::make_shared<data::point>(7.0, 8.0, 9.0);

    pl->get_points().push_back(pt1);
    pl->get_points().push_back(pt2);
    pl->get_points().push_back(pt3);

    cal_info->add_record(img, pl);

    //Testing values

    const auto img_list = cal_info->get_image_container();

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), img_list.size());
    CPPUNIT_ASSERT_EQUAL(img, img_list.front());

    const auto pl_list = cal_info->get_point_list_container();

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), pl_list.size());
    CPPUNIT_ASSERT_EQUAL(pl, pl_list.front());

    data::point_list::csptr pl1 = cal_info->get_point_list(img_list.front());
    CPPUNIT_ASSERT_EQUAL(data::point_list::csptr(pl), pl1);

    data::image::csptr img1 = cal_info->get_image(pl_list.front());
    CPPUNIT_ASSERT_EQUAL(data::image::csptr(img), img1);

    cal_info->remove_record(0);

    data::point_list::csptr pl2 = cal_info->get_point_list(img);
    CPPUNIT_ASSERT_EQUAL(data::point_list::csptr(), pl2);

    data::image::csptr img2 = cal_info->get_image(pl);
    CPPUNIT_ASSERT_EQUAL(data::image::csptr(), img2);

    CPPUNIT_ASSERT(cal_info->get_image_container().empty());
    CPPUNIT_ASSERT(cal_info->get_point_list_container().empty());
}

//------------------------------------------------------------------------------

void calibration_info_test::shallow_copy_test()
{
    data::calibration_info::sptr cal_info = std::make_shared<data::calibration_info>();

    data::image::sptr img = std::make_shared<data::image>();
    utest_data::generator::image::generate_random_image(img, core::type::INT16);

    auto pl  = std::make_shared<data::point_list>();
    auto pt1 = std::make_shared<data::point>(1.0, 2.0, 3.0);
    auto pt2 = std::make_shared<data::point>(4.0, 5.0, 6.0);
    auto pt3 = std::make_shared<data::point>(7.0, 8.0, 9.0);

    pl->get_points().push_back(pt1);
    pl->get_points().push_back(pt2);
    pl->get_points().push_back(pt3);

    cal_info->add_record(img, pl);

    data::calibration_info::sptr cal_info2 = std::make_shared<data::calibration_info>();
    cal_info2->shallow_copy(cal_info);

    CPPUNIT_ASSERT(cal_info->get_image_container() == cal_info2->get_image_container());
    CPPUNIT_ASSERT(cal_info->get_point_list_container() == cal_info2->get_point_list_container());
}

//------------------------------------------------------------------------------

void calibration_info_test::deep_copy_test()
{
    data::calibration_info::sptr cal_info = std::make_shared<data::calibration_info>();

    data::image::sptr img = std::make_shared<data::image>();
    utest_data::generator::image::generate_random_image(img, core::type::INT16);

    auto pl  = std::make_shared<data::point_list>();
    auto pt1 = std::make_shared<data::point>(1.0, 2.0, 3.0);
    auto pt2 = std::make_shared<data::point>(4.0, 5.0, 6.0);
    auto pt3 = std::make_shared<data::point>(7.0, 8.0, 9.0);

    pl->get_points().push_back(pt1);
    pl->get_points().push_back(pt2);
    pl->get_points().push_back(pt3);

    cal_info->add_record(img, pl);

    data::calibration_info::sptr cal_info2 = std::make_shared<data::calibration_info>();

    // == operator test
    CPPUNIT_ASSERT(*cal_info != *cal_info2);

    cal_info2->deep_copy(cal_info);

    CPPUNIT_ASSERT_EQUAL(cal_info2->get_image_container().size(), cal_info2->get_point_list_container().size());

    CPPUNIT_ASSERT_EQUAL(cal_info->get_image_container().size(), cal_info2->get_image_container().size());
    CPPUNIT_ASSERT_EQUAL(cal_info->get_point_list_container().size(), cal_info2->get_point_list_container().size());

    std::list<data::image::sptr>::const_iterator iter_img1;
    std::list<data::image::sptr>::const_iterator iter_img2;
    iter_img1 = cal_info->get_image_container().begin();
    iter_img2 = cal_info2->get_image_container().begin();

    std::list<data::point_list::sptr>::const_iterator iter_pl1;
    std::list<data::point_list::sptr>::const_iterator iter_pl2;
    iter_pl1 = cal_info->get_point_list_container().begin();
    iter_pl2 = cal_info2->get_point_list_container().begin();

    /* TODO: fix
       while(iterImg1 != calInfo->getImageContainer().end())
       {
        CPPUNIT_ASSERT(*iterImg1 != *iterImg2);
        CPPUNIT_ASSERT(*iterPl1 != *iterPl2);

        CPPUNIT_ASSERT(**iterImg1 == **iterImg2);
        CPPUNIT_ASSERT(**iterPl1 == **iterPl2);

     ++iterPl1;
     ++iterPl2;

     ++iterImg1;
     ++iterImg2;
       }
     */

    // == operator test
    CPPUNIT_ASSERT(*cal_info == *cal_info2);
}

//------------------------------------------------------------------------------

void calibration_info_test::get_image_test()
{
    auto cal_info = std::make_shared<data::calibration_info>();

    auto img1 = std::make_shared<data::image>();
    utest_data::generator::image::generate_random_image(img1, core::type::INT16);
    auto pl1 = std::make_shared<data::point_list>();
    pl1->set_points(
        {std::make_shared<data::point>(1., 2., 3.), std::make_shared<data::point>(4., 5., 6.),
         std::make_shared<data::point>(.7, 8., 9.)
        });
    cal_info->add_record(img1, pl1);

    auto img2 = std::make_shared<data::image>();
    utest_data::generator::image::generate_random_image(img2, core::type::INT16);
    auto pl2 = std::make_shared<data::point_list>();
    pl2->set_points(
        {std::make_shared<data::point>(10., 11., 12.), std::make_shared<data::point>(
             13.,
             14.,
             15.
         ),
         std::make_shared<data::point>(16., 17., 18.)
        });
    cal_info->add_record(img2, pl2);

    CPPUNIT_ASSERT_EQUAL(std::const_pointer_cast<const data::image>(img1), cal_info->get_image(pl1));
    CPPUNIT_ASSERT_EQUAL(std::const_pointer_cast<const data::image>(img2), cal_info->get_image(pl2));
    CPPUNIT_ASSERT_EQUAL(img1, cal_info->get_image(0));
    CPPUNIT_ASSERT_EQUAL(img2, cal_info->get_image(1));
}

} // namespace sight::data::ut
