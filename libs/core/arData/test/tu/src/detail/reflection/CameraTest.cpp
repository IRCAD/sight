/************************************************************************
 *
 * Copyright (C) 2014-2020 IRCAD France
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

#include "detail/reflection/CameraTest.hpp"

#include "detail/reflection/DataCampHelper.hpp"

#include <arData/Camera.hpp>

#include <boost/lexical_cast.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::arData::detail::reflection::ut::CameraTest );

namespace arData
{
namespace detail
{
namespace reflection
{
namespace ut
{

//------------------------------------------------------------------------------

void CameraTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void CameraTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void CameraTest::propertiesTest()
{
    const double CX           = rand()%10000/100;
    const double CY           = rand()%10000/100;
    const double FX           = rand()%10000/100;
    const double FY           = rand()%10000/100;
    const double SKEW         = rand()%10000/100;
    const double K1           = rand()%10000/100;
    const double K2           = rand()%10000/100;
    const double P1           = rand()%10000/100;
    const double P2           = rand()%10000/100;
    const double K3           = rand()%10000/100;
    const bool IS_CALIBRATED  = true;
    const std::string DESC    = "My camera";
    const std::string CAM_ID  = "CAM_461384568";
    const float MAX_FRAMERATE = 42.5f;
    const size_t WIDTH        = 1920;
    const size_t HEIGHT       = 1080;
    const double SCALE        = 1.5;

    ::arData::Camera::sptr camera = ::arData::Camera::New();
    camera->setCx(CX);
    camera->setCy(CY);
    camera->setFx(FX);
    camera->setFy(FY);
    camera->setSkew(SKEW);
    camera->setDistortionCoefficient(K1, K2, P1, P2, K3);
    camera->setIsCalibrated(IS_CALIBRATED);
    camera->setDescription(DESC);
    camera->setCameraID(CAM_ID);
    camera->setCameraSource(::arData::Camera::DEVICE);
    camera->setPixelFormat(::arData::Camera::RGBA32);
    camera->setMaximumFrameRate(MAX_FRAMERATE);
    camera->setWidth(WIDTH);
    camera->setHeight(HEIGHT);
    camera->setScale(SCALE);

    namespace dch = ::arData::ut::DataCampHelper;
    const dch::PropertiesNameType dataProperties = {"fields",
                                                    "intrinsic",
                                                    "is_calibrated",
                                                    "skew",
                                                    "distortion_coefficient",
                                                    "width",
                                                    "height",
                                                    "camera_id",
                                                    "description",
                                                    "max_framerate",
                                                    "video_file",
                                                    "stream_url",
                                                    "camera_source",
                                                    "pixel_format",
                                                    "scale"};

    dch::visitProperties(camera->getClassname(), dataProperties);
    dch::compareSimplePropertyValue(camera, "@description", DESC);
    dch::compareSimplePropertyValue(camera, "@camera_id", CAM_ID);
    dch::compareSimplePropertyValue(camera, "@is_calibrated", "1");
    dch::compareSimplePropertyValue(camera, "@skew", ::boost::lexical_cast<std::string>(SKEW));
    dch::compareSimplePropertyValue(camera, "@intrinsic.2", ::boost::lexical_cast<std::string>(CX));
    dch::compareSimplePropertyValue(camera, "@intrinsic.3", ::boost::lexical_cast<std::string>(CY));
    dch::compareSimplePropertyValue(camera, "@intrinsic.0", ::boost::lexical_cast<std::string>(FX));
    dch::compareSimplePropertyValue(camera, "@intrinsic.1", ::boost::lexical_cast<std::string>(FY));
    dch::compareSimplePropertyValue(camera, "@distortion_coefficient.0",
                                    ::boost::lexical_cast<std::string>(K1));
    dch::compareSimplePropertyValue(camera, "@distortion_coefficient.1",
                                    ::boost::lexical_cast<std::string>(K2));
    dch::compareSimplePropertyValue(camera, "@distortion_coefficient.2",
                                    ::boost::lexical_cast<std::string>(P1));
    dch::compareSimplePropertyValue(camera, "@distortion_coefficient.3",
                                    ::boost::lexical_cast<std::string>(P2));
    dch::compareSimplePropertyValue(camera, "@distortion_coefficient.4",
                                    ::boost::lexical_cast<std::string>(K3));
    dch::compareSimplePropertyValue(camera, "@max_framerate",
                                    ::boost::lexical_cast<std::string>(MAX_FRAMERATE));
    dch::compareSimplePropertyValue(camera, "@width",
                                    ::boost::lexical_cast<std::string>(WIDTH));
    dch::compareSimplePropertyValue(camera, "@height",
                                    ::boost::lexical_cast<std::string>(HEIGHT));
    dch::compareSimplePropertyValue(camera, "@scale",
                                    ::boost::lexical_cast<std::string>(SCALE));
}

//------------------------------------------------------------------------------
} // namespace ut
} // namespace reflection
} // namespace detail
} // namespace ardata
