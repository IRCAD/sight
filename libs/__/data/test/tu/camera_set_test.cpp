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

#include "camera_set_test.hpp"

#include <core/spy_log.hpp>

#include <data/camera.hpp>
#include <data/camera_set.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::camera_set_test);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void camera_set_test::setUp()
{
}

//------------------------------------------------------------------------------

void camera_set_test::tearDown()
{
}

//------------------------------------------------------------------------------

camera_set::sptr initCameraSet()
{
    auto cameraSet = std::make_shared<camera_set>();

    // --------------- Camera 1 ----------------------
    auto camera1 = std::make_shared<camera>();
    camera1->setCx(3.5);
    camera1->setCy(9.3);
    camera1->setFx(48.2);
    camera1->setFy(7.3);
    camera1->setSkew(1.9);
    camera1->setDistortionCoefficient(1.1, 2.2, 3.3, 4.4, 5.5);
    cameraSet->add_camera(camera1);

    // --------------- Camera 2 ----------------------
    auto camera2 = std::make_shared<camera>();
    camera2->setCx(86.5);
    camera2->setCy(543.);
    camera2->setFx(4.4);
    camera2->setFy(5.14);
    camera2->setSkew(0.19);
    camera2->setDistortionCoefficient(2.4, 5.1, 66., 4.1, 6.4);
    cameraSet->add_camera(camera2);

    // --------------- Extrinsic matrix ----------------------
    auto mat = std::make_shared<matrix4>();
    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        for(std::size_t j = 0 ; j < 4 ; ++j)
        {
            const auto value = static_cast<data::matrix4::value_type>(2 * i + j);
            (*mat)(i, j) = value;
        }
    }

    cameraSet->set_extrinsic_matrix(1, mat);

    return cameraSet;
}

//------------------------------------------------------------------------------

void camera_set_test::cameraTest()
{
    auto cameraSet = std::make_shared<camera_set>();
    CPPUNIT_ASSERT(cameraSet);

    auto identity = std::make_shared<matrix4>();
    auto mat      = std::make_shared<matrix4>();
    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        for(std::size_t j = 0 ; j < 4 ; ++j)
        {
            const auto value = static_cast<data::matrix4::value_type>(2 * i + j);
            (*mat)(i, j) = value;
        }
    }

    auto camera1 = std::make_shared<camera>();
    auto camera2 = std::make_shared<camera>();
    auto camera3 = std::make_shared<camera>();

    CPPUNIT_ASSERT_NO_THROW(cameraSet->add_camera(camera1));
    CPPUNIT_ASSERT_NO_THROW(cameraSet->add_camera(camera2));
    CPPUNIT_ASSERT_THROW(cameraSet->add_camera(camera2), core::exception);

    CPPUNIT_ASSERT(cameraSet->get_extrinsic_matrix(0));
    CPPUNIT_ASSERT(*identity == *cameraSet->get_extrinsic_matrix(0));
    CPPUNIT_ASSERT(!cameraSet->get_extrinsic_matrix(1));
    CPPUNIT_ASSERT_NO_THROW(cameraSet->set_extrinsic_matrix(1, mat));
    CPPUNIT_ASSERT_THROW(cameraSet->set_extrinsic_matrix(2, mat), std::out_of_range);
    CPPUNIT_ASSERT_THROW(cameraSet->get_extrinsic_matrix(2), std::out_of_range);
    CPPUNIT_ASSERT(cameraSet->get_extrinsic_matrix(1) == mat);

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), cameraSet->size());

    CPPUNIT_ASSERT(cameraSet->get_camera(0) == camera1);
    CPPUNIT_ASSERT(cameraSet->get_camera(1) == camera2);
    CPPUNIT_ASSERT_THROW(cameraSet->get_camera(2), std::out_of_range);

    CPPUNIT_ASSERT_NO_THROW(cameraSet->add_camera(camera3));
    CPPUNIT_ASSERT(cameraSet->get_camera(2) == camera3);

    CPPUNIT_ASSERT_NO_THROW(cameraSet->remove_camera(camera1));
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), cameraSet->size());
    CPPUNIT_ASSERT(cameraSet->get_camera(0) == camera2);
    CPPUNIT_ASSERT_THROW(cameraSet->remove_camera(camera1), core::exception);

    CPPUNIT_ASSERT_NO_THROW(cameraSet->remove_camera(camera2));
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), cameraSet->size());
    CPPUNIT_ASSERT(cameraSet->get_camera(0) == camera3);
    CPPUNIT_ASSERT_NO_THROW(cameraSet->remove_camera(camera3));

    CPPUNIT_ASSERT_EQUAL(std::size_t(0), cameraSet->size());
}

//------------------------------------------------------------------------------

void camera_set_test::shallow_copyTest()
{
    auto cameraSet  = initCameraSet();
    auto cameraSet2 = std::make_shared<camera_set>();

    cameraSet2->shallow_copy(cameraSet);

    CPPUNIT_ASSERT_EQUAL(cameraSet->size(), cameraSet2->size());
    CPPUNIT_ASSERT_EQUAL(cameraSet->get_camera(0), cameraSet2->get_camera(0));
    CPPUNIT_ASSERT_EQUAL(cameraSet->get_camera(1), cameraSet2->get_camera(1));
}

//------------------------------------------------------------------------------

void camera_set_test::deep_copyTest()
{
    camera_set::sptr cameraSet = initCameraSet();
    camera_set::sptr cameraSet2;
    cameraSet2 = data::object::copy<camera_set>(cameraSet);

    CPPUNIT_ASSERT(*cameraSet == *cameraSet2);
}

} // namespace sight::data::ut
