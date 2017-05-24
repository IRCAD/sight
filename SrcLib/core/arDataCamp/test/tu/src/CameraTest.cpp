/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "CameraTest.hpp"

#include "DataCampHelper.hpp"

#include <arData/Camera.hpp>

#include <boost/lexical_cast.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::arDataCamp::ut::CameraTest );

namespace arDataCamp
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
    const size_t SCALE        = 1.0450;

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

    const DataCampHelper::PropertiesNameType dataProperties = {"fields",
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

    DataCampHelper::visitProperties(camera->getClassname(), dataProperties);
    DataCampHelper::compareSimplePropertyValue(camera, "@description", DESC);
    DataCampHelper::compareSimplePropertyValue(camera, "@camera_id", CAM_ID);
    DataCampHelper::compareSimplePropertyValue(camera, "@is_calibrated", "1");
    DataCampHelper::compareSimplePropertyValue(camera, "@skew", ::boost::lexical_cast<std::string>(SKEW));
    DataCampHelper::compareSimplePropertyValue(camera, "@intrinsic.2", ::boost::lexical_cast<std::string>(CX));
    DataCampHelper::compareSimplePropertyValue(camera, "@intrinsic.3", ::boost::lexical_cast<std::string>(CY));
    DataCampHelper::compareSimplePropertyValue(camera, "@intrinsic.0", ::boost::lexical_cast<std::string>(FX));
    DataCampHelper::compareSimplePropertyValue(camera, "@intrinsic.1", ::boost::lexical_cast<std::string>(FY));
    DataCampHelper::compareSimplePropertyValue(camera, "@distortion_coefficient.0",
                                               ::boost::lexical_cast<std::string>(K1));
    DataCampHelper::compareSimplePropertyValue(camera, "@distortion_coefficient.1",
                                               ::boost::lexical_cast<std::string>(K2));
    DataCampHelper::compareSimplePropertyValue(camera, "@distortion_coefficient.2",
                                               ::boost::lexical_cast<std::string>(P1));
    DataCampHelper::compareSimplePropertyValue(camera, "@distortion_coefficient.3",
                                               ::boost::lexical_cast<std::string>(P2));
    DataCampHelper::compareSimplePropertyValue(camera, "@distortion_coefficient.4",
                                               ::boost::lexical_cast<std::string>(K3));
    DataCampHelper::compareSimplePropertyValue(camera, "@max_framerate",
                                               ::boost::lexical_cast<std::string>(MAX_FRAMERATE));
    DataCampHelper::compareSimplePropertyValue(camera, "@width",
                                               ::boost::lexical_cast<std::string>(WIDTH));
    DataCampHelper::compareSimplePropertyValue(camera, "@height",
                                               ::boost::lexical_cast<std::string>(HEIGHT));
    DataCampHelper::compareSimplePropertyValue(camera, "@scale",
                                               ::boost::lexical_cast<std::string>(SCALE));
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace arDataCamp
