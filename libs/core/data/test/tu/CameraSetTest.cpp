/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
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

#include "CameraSetTest.hpp"

#include <core/spyLog.hpp>

#include <data/Camera.hpp>
#include <data/CameraSet.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::CameraSetTest);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void CameraSetTest::setUp()
{
}

//------------------------------------------------------------------------------

void CameraSetTest::tearDown()
{
}

//------------------------------------------------------------------------------

CameraSet::sptr initCameraSet()
{
    auto cameraSet = std::make_shared<CameraSet>();

    // --------------- Camera 1 ----------------------
    auto camera1 = std::make_shared<Camera>();
    camera1->setCx(3.5);
    camera1->setCy(9.3);
    camera1->setFx(48.2);
    camera1->setFy(7.3);
    camera1->setSkew(1.9);
    camera1->setDistortionCoefficient(1.1, 2.2, 3.3, 4.4, 5.5);
    cameraSet->add_camera(camera1);

    // --------------- Camera 2 ----------------------
    auto camera2 = std::make_shared<Camera>();
    camera2->setCx(86.5);
    camera2->setCy(543.);
    camera2->setFx(4.4);
    camera2->setFy(5.14);
    camera2->setSkew(0.19);
    camera2->setDistortionCoefficient(2.4, 5.1, 66., 4.1, 6.4);
    cameraSet->add_camera(camera2);

    // --------------- Extrinsic matrix ----------------------
    auto mat = std::make_shared<Matrix4>();
    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        for(std::size_t j = 0 ; j < 4 ; ++j)
        {
            const auto value = static_cast<data::Matrix4::TM3DType>(2 * i + j);
            mat->setCoefficient(i, j, value);
        }
    }

    cameraSet->set_extrinsic_matrix(1, mat);

    return cameraSet;
}

//------------------------------------------------------------------------------

void CameraSetTest::cameraTest()
{
    auto cameraSet = std::make_shared<CameraSet>();
    CPPUNIT_ASSERT(cameraSet);

    auto identity = std::make_shared<Matrix4>();
    auto mat      = std::make_shared<Matrix4>();
    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        for(std::size_t j = 0 ; j < 4 ; ++j)
        {
            const auto value = static_cast<data::Matrix4::TM3DType>(2 * i + j);
            mat->setCoefficient(i, j, value);
        }
    }

    auto camera1 = std::make_shared<Camera>();
    auto camera2 = std::make_shared<Camera>();
    auto camera3 = std::make_shared<Camera>();

    CPPUNIT_ASSERT_NO_THROW(cameraSet->add_camera(camera1));
    CPPUNIT_ASSERT_NO_THROW(cameraSet->add_camera(camera2));
    CPPUNIT_ASSERT_THROW(cameraSet->add_camera(camera2), core::Exception);

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
    CPPUNIT_ASSERT_THROW(cameraSet->remove_camera(camera1), core::Exception);

    CPPUNIT_ASSERT_NO_THROW(cameraSet->remove_camera(camera2));
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), cameraSet->size());
    CPPUNIT_ASSERT(cameraSet->get_camera(0) == camera3);
    CPPUNIT_ASSERT_NO_THROW(cameraSet->remove_camera(camera3));

    CPPUNIT_ASSERT_EQUAL(std::size_t(0), cameraSet->size());
}

//------------------------------------------------------------------------------

void CameraSetTest::shallowCopyTest()
{
    auto cameraSet  = initCameraSet();
    auto cameraSet2 = std::make_shared<CameraSet>();

    cameraSet2->shallowCopy(cameraSet);

    CPPUNIT_ASSERT_EQUAL(cameraSet->size(), cameraSet2->size());
    CPPUNIT_ASSERT_EQUAL(cameraSet->get_camera(0), cameraSet2->get_camera(0));
    CPPUNIT_ASSERT_EQUAL(cameraSet->get_camera(1), cameraSet2->get_camera(1));
}

//------------------------------------------------------------------------------

void CameraSetTest::deepCopyTest()
{
    CameraSet::sptr cameraSet = initCameraSet();
    CameraSet::sptr cameraSet2;
    cameraSet2 = data::Object::copy<CameraSet>(cameraSet);

    CPPUNIT_ASSERT(*cameraSet == *cameraSet2);
}

} // namespace sight::data::ut
