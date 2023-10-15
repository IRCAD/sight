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

void calibration_info_test::calibrationInfoTest()
{
    data::calibration_info::sptr calInfo = std::make_shared<data::calibration_info>();

    data::image::sptr img = std::make_shared<data::image>();
    utest_data::generator::image::generateRandomImage(img, core::type::INT16);

    auto pl  = std::make_shared<data::point_list>();
    auto pt1 = std::make_shared<data::point>(1.0, 2.0, 3.0);
    auto pt2 = std::make_shared<data::point>(4.0, 5.0, 6.0);
    auto pt3 = std::make_shared<data::point>(7.0, 8.0, 9.0);

    pl->getPoints().push_back(pt1);
    pl->getPoints().push_back(pt2);
    pl->getPoints().push_back(pt3);

    calInfo->addRecord(img, pl);

    //Testing values

    const auto imgList = calInfo->getImageContainer();

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), imgList.size());
    CPPUNIT_ASSERT_EQUAL(img, imgList.front());

    const auto plList = calInfo->getPointListContainer();

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), plList.size());
    CPPUNIT_ASSERT_EQUAL(pl, plList.front());

    data::point_list::csptr pl1 = calInfo->getPointList(imgList.front());
    CPPUNIT_ASSERT_EQUAL(data::point_list::csptr(pl), pl1);

    data::image::csptr img1 = calInfo->getImage(plList.front());
    CPPUNIT_ASSERT_EQUAL(data::image::csptr(img), img1);

    calInfo->removeRecord(0);

    data::point_list::csptr pl2 = calInfo->getPointList(img);
    CPPUNIT_ASSERT_EQUAL(data::point_list::csptr(), pl2);

    data::image::csptr img2 = calInfo->getImage(pl);
    CPPUNIT_ASSERT_EQUAL(data::image::csptr(), img2);

    CPPUNIT_ASSERT(calInfo->getImageContainer().empty());
    CPPUNIT_ASSERT(calInfo->getPointListContainer().empty());
}

//------------------------------------------------------------------------------

void calibration_info_test::shallow_copyTest()
{
    data::calibration_info::sptr calInfo = std::make_shared<data::calibration_info>();

    data::image::sptr img = std::make_shared<data::image>();
    utest_data::generator::image::generateRandomImage(img, core::type::INT16);

    auto pl  = std::make_shared<data::point_list>();
    auto pt1 = std::make_shared<data::point>(1.0, 2.0, 3.0);
    auto pt2 = std::make_shared<data::point>(4.0, 5.0, 6.0);
    auto pt3 = std::make_shared<data::point>(7.0, 8.0, 9.0);

    pl->getPoints().push_back(pt1);
    pl->getPoints().push_back(pt2);
    pl->getPoints().push_back(pt3);

    calInfo->addRecord(img, pl);

    data::calibration_info::sptr calInfo2 = std::make_shared<data::calibration_info>();
    calInfo2->shallow_copy(calInfo);

    CPPUNIT_ASSERT(calInfo->getImageContainer() == calInfo2->getImageContainer());
    CPPUNIT_ASSERT(calInfo->getPointListContainer() == calInfo2->getPointListContainer());
}

//------------------------------------------------------------------------------

void calibration_info_test::deep_copyTest()
{
    data::calibration_info::sptr calInfo = std::make_shared<data::calibration_info>();

    data::image::sptr img = std::make_shared<data::image>();
    utest_data::generator::image::generateRandomImage(img, core::type::INT16);

    auto pl  = std::make_shared<data::point_list>();
    auto pt1 = std::make_shared<data::point>(1.0, 2.0, 3.0);
    auto pt2 = std::make_shared<data::point>(4.0, 5.0, 6.0);
    auto pt3 = std::make_shared<data::point>(7.0, 8.0, 9.0);

    pl->getPoints().push_back(pt1);
    pl->getPoints().push_back(pt2);
    pl->getPoints().push_back(pt3);

    calInfo->addRecord(img, pl);

    data::calibration_info::sptr calInfo2 = std::make_shared<data::calibration_info>();

    // == operator test
    CPPUNIT_ASSERT(*calInfo != *calInfo2);

    calInfo2->deep_copy(calInfo);

    CPPUNIT_ASSERT_EQUAL(calInfo2->getImageContainer().size(), calInfo2->getPointListContainer().size());

    CPPUNIT_ASSERT_EQUAL(calInfo->getImageContainer().size(), calInfo2->getImageContainer().size());
    CPPUNIT_ASSERT_EQUAL(calInfo->getPointListContainer().size(), calInfo2->getPointListContainer().size());

    std::list<data::image::sptr>::const_iterator iterImg1;
    std::list<data::image::sptr>::const_iterator iterImg2;
    iterImg1 = calInfo->getImageContainer().begin();
    iterImg2 = calInfo2->getImageContainer().begin();

    std::list<data::point_list::sptr>::const_iterator iterPl1;
    std::list<data::point_list::sptr>::const_iterator iterPl2;
    iterPl1 = calInfo->getPointListContainer().begin();
    iterPl2 = calInfo2->getPointListContainer().begin();

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
    CPPUNIT_ASSERT(*calInfo == *calInfo2);
}

//------------------------------------------------------------------------------

void calibration_info_test::getImageTest()
{
    auto calInfo = std::make_shared<data::calibration_info>();

    auto img1 = std::make_shared<data::image>();
    utest_data::generator::image::generateRandomImage(img1, core::type::INT16);
    auto pl1 = std::make_shared<data::point_list>();
    pl1->setPoints(
        {std::make_shared<data::point>(1., 2., 3.), std::make_shared<data::point>(4., 5., 6.),
         std::make_shared<data::point>(.7, 8., 9.)
        });
    calInfo->addRecord(img1, pl1);

    auto img2 = std::make_shared<data::image>();
    utest_data::generator::image::generateRandomImage(img2, core::type::INT16);
    auto pl2 = std::make_shared<data::point_list>();
    pl2->setPoints(
        {std::make_shared<data::point>(10., 11., 12.), std::make_shared<data::point>(
             13.,
             14.,
             15.
         ),
         std::make_shared<data::point>(16., 17., 18.)
        });
    calInfo->addRecord(img2, pl2);

    CPPUNIT_ASSERT_EQUAL(std::const_pointer_cast<const data::image>(img1), calInfo->getImage(pl1));
    CPPUNIT_ASSERT_EQUAL(std::const_pointer_cast<const data::image>(img2), calInfo->getImage(pl2));
    CPPUNIT_ASSERT_EQUAL(img1, calInfo->getImage(0));
    CPPUNIT_ASSERT_EQUAL(img2, calInfo->getImage(1));
}

} // namespace sight::data::ut
