/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include <arData/Camera.hpp>
#include <arData/CameraSeries.hpp>

#include <arDataCamp/Version.hpp>

#include <fwCore/spyLog.hpp>

#include <fwDataCamp/Version.hpp>

#include <fwMedDataCamp/Version.hpp>

#include <fwTest/helper/compare.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::arData::ut::CameraSeriesTest );

namespace arData
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
    ::arData::Camera::sptr obj = ::arData::Camera::New();
    obj->getClassname();

    //Force link with fwDataCamp
    m_fwDataVersion = ::fwDataCamp::Version::s_CURRENT_VERSION;
    FwCoreNotUsedMacro(m_fwDataVersion);
    //Force link with fwMedDataCamp
    m_fwMedDataVersion = ::fwMedDataCamp::Version::s_CURRENT_VERSION;
    FwCoreNotUsedMacro(m_fwMedDataVersion);
    //Force link with arDataCamp
    m_arDataVersion = ::arDataCamp::Version::s_CURRENT_VERSION;
    FwCoreNotUsedMacro(m_arDataVersion);
}

//------------------------------------------------------------------------------

::arData::CameraSeries::sptr initCameraSeries()
{
    ::arData::CameraSeries::sptr series = ::arData::CameraSeries::New();

    // --------------- Camera 1 ----------------------
    ::arData::Camera::sptr camera1 = ::arData::Camera::New();
    camera1->setCx(3.5);
    camera1->setCy(9.3);
    camera1->setFx(48.2);
    camera1->setFy(7.3);
    camera1->setSkew(1.9);
    camera1->setDistortionCoefficient(1.1, 2.2, 3.3, 4.4, 5.5);
    series->addCamera(camera1);

    // --------------- Camera 2 ----------------------
    ::arData::Camera::sptr camera2 = ::arData::Camera::New();
    camera2->setCx(86.5);
    camera2->setCy(543.);
    camera2->setFx(4.4);
    camera2->setFy(5.14);
    camera2->setSkew(0.19);
    camera2->setDistortionCoefficient(2.4, 5.1, 66., 4.1, 6.4);
    series->addCamera(camera2);

    // --------------- Extrinsic matrix ----------------------
    ::fwData::TransformationMatrix3D::sptr mat = ::fwData::TransformationMatrix3D::New();
    for (size_t i = 0; i < 4; ++i)
    {
        for (size_t j = 0; j < 4; ++j)
        {
            mat->setCoefficient(i, j, 2*i+j);
        }
    }

    series->setExtrinsicMatrix(1, mat);

    return series;
}

//------------------------------------------------------------------------------

void CameraSeriesTest::cameraTest()
{
    ::arData::CameraSeries::sptr series = ::arData::CameraSeries::New();
    CPPUNIT_ASSERT(series);

    ::fwData::TransformationMatrix3D::sptr identity = ::fwData::TransformationMatrix3D::New();
    ::fwData::TransformationMatrix3D::sptr mat      = ::fwData::TransformationMatrix3D::New();
    for (size_t i = 0; i < 4; ++i)
    {
        for (size_t j = 0; j < 4; ++j)
        {
            mat->setCoefficient(i, j, 2*i+j);
        }
    }

    ::arData::Camera::sptr camera1 = ::arData::Camera::New();
    ::arData::Camera::sptr camera2 = ::arData::Camera::New();
    ::arData::Camera::sptr camera3 = ::arData::Camera::New();

    CPPUNIT_ASSERT_NO_THROW(series->addCamera(camera1));
    CPPUNIT_ASSERT_NO_THROW(series->addCamera(camera2));
    CPPUNIT_ASSERT_THROW(series->addCamera(camera2), ::fwCore::Exception);

    CPPUNIT_ASSERT(series->getExtrinsicMatrix(0));
    CPPUNIT_ASSERT(::fwTest::helper::compare(identity, series->getExtrinsicMatrix(0)));
    CPPUNIT_ASSERT(!series->getExtrinsicMatrix(1));
    CPPUNIT_ASSERT_NO_THROW(series->setExtrinsicMatrix(1, mat));
    CPPUNIT_ASSERT_THROW(series->setExtrinsicMatrix(2, mat), ::fwCore::Exception);
    CPPUNIT_ASSERT_THROW(series->getExtrinsicMatrix(2), ::fwCore::Exception);
    CPPUNIT_ASSERT(series->getExtrinsicMatrix(1) == mat);

    CPPUNIT_ASSERT_EQUAL(size_t(2), series->getNumberOfCameras());

    CPPUNIT_ASSERT(series->getCamera(0) == camera1);
    CPPUNIT_ASSERT(series->getCamera(1) == camera2);
    CPPUNIT_ASSERT_THROW(series->getCamera(2), ::fwCore::Exception);

    CPPUNIT_ASSERT_NO_THROW(series->addCamera(camera3));
    CPPUNIT_ASSERT(series->getCamera(2) == camera3);

    CPPUNIT_ASSERT_NO_THROW(series->removeCamera(camera1));
    CPPUNIT_ASSERT_EQUAL(size_t(2), series->getNumberOfCameras());
    CPPUNIT_ASSERT(series->getCamera(0) == camera2);
    CPPUNIT_ASSERT_THROW(series->removeCamera(camera1), ::fwCore::Exception);

    CPPUNIT_ASSERT_NO_THROW(series->removeCamera(camera2));
    CPPUNIT_ASSERT_EQUAL(size_t(1), series->getNumberOfCameras());
    CPPUNIT_ASSERT(series->getCamera(0) == camera3);
    CPPUNIT_ASSERT_NO_THROW(series->removeCamera(camera3));

    CPPUNIT_ASSERT_EQUAL(size_t(0), series->getNumberOfCameras());
}

//------------------------------------------------------------------------------

void CameraSeriesTest::shallowCopyTest()
{
    ::arData::CameraSeries::sptr series  = initCameraSeries();
    ::arData::CameraSeries::sptr series2 = ::arData::CameraSeries::New();

    series2->shallowCopy(series);

    CPPUNIT_ASSERT_EQUAL(series->getNumberOfCameras(), series2->getNumberOfCameras());
    CPPUNIT_ASSERT_EQUAL(series->getCamera(0), series2->getCamera(0));
    CPPUNIT_ASSERT_EQUAL(series->getCamera(1), series2->getCamera(1));
}

//------------------------------------------------------------------------------

void CameraSeriesTest::deepCopyTest()
{
    ::arData::CameraSeries::sptr series = initCameraSeries();
    ::arData::CameraSeries::sptr series2;
    series2 = ::fwData::Object::copy< ::arData::CameraSeries >(series);

    CPPUNIT_ASSERT(::fwTest::helper::compare(series, series2));
}

} //namespace ut
} //namespace arData
