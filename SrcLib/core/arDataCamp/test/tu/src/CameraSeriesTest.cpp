/************************************************************************
 *
 * Copyright (C) 2014-2015 IRCAD France
 * Copyright (C) 2014-2015 IHU Strasbourg
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

#include "DataCampHelper.hpp"
#include "CameraSeriesTest.hpp"

#include <arData/CameraSeries.hpp>
#include <arData/Camera.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::arDataCamp::ut::CameraSeriesTest );

namespace arDataCamp
{
namespace ut
{

void CameraSeriesTest::setUp()
{
    // Set up context before running a test.
}

void CameraSeriesTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void CameraSeriesTest::propertiesTest()
{
    ::arData::CameraSeries::sptr series = ::arData::CameraSeries::New();

    ::fwData::TransformationMatrix3D::sptr mat = ::fwData::TransformationMatrix3D::New();
    for (int i = 0; i<4; ++i)
    {
        for (int j = 0; j<4; ++j)
        {
            mat->setCoefficient(i, j, 2*i+j);
        }
    }

    // --------------- Camera 1 ----------------------
    ::arData::Camera::sptr camera1 = ::arData::Camera::New();
    camera1->setCx(3.5);
    camera1->setCy(9.3);
    camera1->setFx(48.2);
    camera1->setFy(7.3);
    camera1->setSkew(1.9);
    camera1->setDistortionCoefficient(1.1, 2.2, 3.3, 4.4, 5.5);

    // --------------- Camera 2 ----------------------
    ::arData::Camera::sptr camera2 = ::arData::Camera::New();
    camera2->setCx(86.5);
    camera2->setCy(543.);
    camera2->setFx(4.4);
    camera2->setFy(5.14);
    camera2->setSkew(0.19);
    camera2->setDistortionCoefficient(2.4, 5.1, 66., 4.1, 6.4);


    series->addCamera(camera1);
    series->addCamera(camera2);
    series->setExtrinsicMatrix(1, mat);

    const DataCampHelper::PropertiesNameType dataProperties = {"fields",
                                                               "patient",
                                                               "study",
                                                               "equipment",
                                                               "instance_uid",
                                                               "modality",
                                                               "date",
                                                               "time",
                                                               "performing_physicians_name",
                                                               "description",
                                                               "cameras",
                                                               "extrinsic_matrices"};

    DataCampHelper::visitProperties(series->getClassname(), dataProperties);
    DataCampHelper::compareObjectPropertyValue(series, "@cameras.0", camera1);
    DataCampHelper::compareObjectPropertyValue(series, "@cameras.1", camera2);
    DataCampHelper::compareObjectPropertyValue(series, "@extrinsic_matrices.1", mat);
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace arDataCamp
