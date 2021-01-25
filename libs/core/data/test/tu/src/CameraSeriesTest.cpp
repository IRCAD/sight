/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#include "CameraSeriesTest.hpp"

#include <data/Camera.hpp>
#include <data/CameraSeries.hpp>

#include <core/spyLog.hpp>

#include <utestData/helper/compare.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::sight::data::ut::CameraSeriesTest );

namespace sight::data
{
namespace ut
{

//------------------------------------------------------------------------------

void CameraSeriesTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void CameraSeriesTest::tearDown()
{
    //Hack: force link with arData
    data::Camera::sptr obj = data::Camera::New();
    obj->getClassname();
}

//------------------------------------------------------------------------------

data::CameraSeries::sptr initCameraSeries()
{
    data::CameraSeries::sptr series = data::CameraSeries::New();

    // --------------- Camera 1 ----------------------
    data::Camera::sptr camera1 = data::Camera::New();
    camera1->setCx(3.5);
    camera1->setCy(9.3);
    camera1->setFx(48.2);
    camera1->setFy(7.3);
    camera1->setSkew(1.9);
    camera1->setDistortionCoefficient(1.1, 2.2, 3.3, 4.4, 5.5);
    series->addCamera(camera1);

    // --------------- Camera 2 ----------------------
    data::Camera::sptr camera2 = data::Camera::New();
    camera2->setCx(86.5);
    camera2->setCy(543.);
    camera2->setFx(4.4);
    camera2->setFy(5.14);
    camera2->setSkew(0.19);
    camera2->setDistortionCoefficient(2.4, 5.1, 66., 4.1, 6.4);
    series->addCamera(camera2);

    // --------------- Extrinsic matrix ----------------------
    data::TransformationMatrix3D::sptr mat = data::TransformationMatrix3D::New();
    for (size_t i = 0; i < 4; ++i)
    {
        for (size_t j = 0; j < 4; ++j)
        {
            const auto value = static_cast< data::TransformationMatrix3D::TM3DType >(2*i+j);
            mat->setCoefficient(i, j, value);
        }
    }

    series->setExtrinsicMatrix(1, mat);

    return series;
}

//------------------------------------------------------------------------------

void CameraSeriesTest::cameraTest()
{
    data::CameraSeries::sptr series = data::CameraSeries::New();
    CPPUNIT_ASSERT(series);

    data::TransformationMatrix3D::sptr identity = data::TransformationMatrix3D::New();
    data::TransformationMatrix3D::sptr mat      = data::TransformationMatrix3D::New();
    for (size_t i = 0; i < 4; ++i)
    {
        for (size_t j = 0; j < 4; ++j)
        {
            const auto value = static_cast< data::TransformationMatrix3D::TM3DType >(2*i+j);
            mat->setCoefficient(i, j, value);
        }
    }

    data::Camera::sptr camera1 = data::Camera::New();
    data::Camera::sptr camera2 = data::Camera::New();
    data::Camera::sptr camera3 = data::Camera::New();

    CPPUNIT_ASSERT_NO_THROW(series->addCamera(camera1));
    CPPUNIT_ASSERT_NO_THROW(series->addCamera(camera2));
    CPPUNIT_ASSERT_THROW(series->addCamera(camera2), core::Exception);

    CPPUNIT_ASSERT(series->getExtrinsicMatrix(0));
    CPPUNIT_ASSERT(utestData::helper::compare(identity, series->getExtrinsicMatrix(0)));
    CPPUNIT_ASSERT(!series->getExtrinsicMatrix(1));
    CPPUNIT_ASSERT_NO_THROW(series->setExtrinsicMatrix(1, mat));
    CPPUNIT_ASSERT_THROW(series->setExtrinsicMatrix(2, mat), core::Exception);
    CPPUNIT_ASSERT_THROW(series->getExtrinsicMatrix(2), core::Exception);
    CPPUNIT_ASSERT(series->getExtrinsicMatrix(1) == mat);

    CPPUNIT_ASSERT_EQUAL(size_t(2), series->getNumberOfCameras());

    CPPUNIT_ASSERT(series->getCamera(0) == camera1);
    CPPUNIT_ASSERT(series->getCamera(1) == camera2);
    CPPUNIT_ASSERT_THROW(series->getCamera(2), core::Exception);

    CPPUNIT_ASSERT_NO_THROW(series->addCamera(camera3));
    CPPUNIT_ASSERT(series->getCamera(2) == camera3);

    CPPUNIT_ASSERT_NO_THROW(series->removeCamera(camera1));
    CPPUNIT_ASSERT_EQUAL(size_t(2), series->getNumberOfCameras());
    CPPUNIT_ASSERT(series->getCamera(0) == camera2);
    CPPUNIT_ASSERT_THROW(series->removeCamera(camera1), core::Exception);

    CPPUNIT_ASSERT_NO_THROW(series->removeCamera(camera2));
    CPPUNIT_ASSERT_EQUAL(size_t(1), series->getNumberOfCameras());
    CPPUNIT_ASSERT(series->getCamera(0) == camera3);
    CPPUNIT_ASSERT_NO_THROW(series->removeCamera(camera3));

    CPPUNIT_ASSERT_EQUAL(size_t(0), series->getNumberOfCameras());
}

//------------------------------------------------------------------------------

void CameraSeriesTest::shallowCopyTest()
{
    data::CameraSeries::sptr series  = initCameraSeries();
    data::CameraSeries::sptr series2 = data::CameraSeries::New();

    series2->shallowCopy(series);

    CPPUNIT_ASSERT_EQUAL(series->getNumberOfCameras(), series2->getNumberOfCameras());
    CPPUNIT_ASSERT_EQUAL(series->getCamera(0), series2->getCamera(0));
    CPPUNIT_ASSERT_EQUAL(series->getCamera(1), series2->getCamera(1));
}

//------------------------------------------------------------------------------

void CameraSeriesTest::deepCopyTest()
{
    data::CameraSeries::sptr series = initCameraSeries();
    data::CameraSeries::sptr series2;
    series2 = data::Object::copy< data::CameraSeries >(series);

    CPPUNIT_ASSERT(utestData::helper::compare(series, series2));
}

} //namespace ut
} //namespace sight::data
