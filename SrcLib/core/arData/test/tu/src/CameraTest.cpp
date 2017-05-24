/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "CameraTest.hpp"

#include <arData/Camera.hpp>

#include <arDataCamp/Version.hpp>

#include <fwCore/spyLog.hpp>

#include <fwTest/helper/compare.hpp>

#include <fwTools/NumericRoundCast.hxx>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::arData::ut::CameraTest );

namespace arData
{
namespace ut
{

//------------------------------------------------------------------------------

void CameraTest::setUp()
{
    // Set up context before running a test.
    std::srand(::fwTools::numericRoundCast< unsigned int >(std::time(NULL)));
}

//------------------------------------------------------------------------------

void CameraTest::tearDown()
{
    //Force link with arDataCamp
    const int arfVersion = ::arDataCamp::Version::s_CURRENT_VERSION;
}

//------------------------------------------------------------------------------

::arData::Camera::sptr initCamera()
{
    const double CX                                 = rand()%10000/100;
    const double CY                                 = rand()%10000/100;
    const double FX                                 = rand()%10000/100;
    const double FY                                 = rand()%10000/100;
    const double SKEW                               = rand()%10000/100;
    const double K1                                 = rand()%10000/100;
    const double K2                                 = rand()%10000/100;
    const double P1                                 = rand()%10000/100;
    const double P2                                 = rand()%10000/100;
    const double K3                                 = rand()%10000/100;
    const bool IS_CALIBRATED                        = true;
    const std::string DESC                          = "My camera";
    const std::string CAM_ID                        = "CAM_461384568";
    const size_t WIDTH                              = 800;
    const size_t HEIGHT                             = 800;
    const bool ISCALIBRATED                         = true;
    const ::arData::Camera::SourceType CAMERASOURCE = ::arData::Camera::DEVICE;
    const std::string CAMERAID                      = "/dev/video0";
    const float MAXIMUMFRAMERATE                    = 30.f;
    const ::arData::Camera::PixelFormat PIXELFORMAT = ::arData::Camera::RGBA32;
    const std::string VIDEOFILE                     = "/tmp/video.mp4";
    const std::string STREAMURL                     = "rtsp://192.168.0.1/h264.sdp";
    const double SCALE                              = 0.9996;

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
    camera->setWidth(WIDTH);
    camera->setHeight(HEIGHT);
    camera->setCameraSource(CAMERASOURCE);
    camera->setMaximumFrameRate(MAXIMUMFRAMERATE);
    camera->setPixelFormat(PIXELFORMAT);
    camera->setVideoFile(VIDEOFILE);
    camera->setStreamUrl(STREAMURL);
    camera->setScale(SCALE);

    return camera;
}

//------------------------------------------------------------------------------

void CameraTest::paramTest()
{
    const double CX                                 = rand()%10000/100;
    const double CY                                 = rand()%10000/100;
    const double FX                                 = rand()%10000/100;
    const double FY                                 = rand()%10000/100;
    const double SKEW                               = rand()%10000/100;
    const double K1                                 = rand()%10000/100;
    const double K2                                 = rand()%10000/100;
    const double P1                                 = rand()%10000/100;
    const double P2                                 = rand()%10000/100;
    const double K3                                 = rand()%10000/100;
    const bool IS_CALIBRATED                        = true;
    const std::string DESC                          = "My camera";
    const std::string CAM_ID                        = "CAM_461384568";
    const size_t WIDTH                              = 800;
    const size_t HEIGHT                             = 800;
    const bool ISCALIBRATED                         = true;
    const ::arData::Camera::SourceType CAMERASOURCE = ::arData::Camera::DEVICE;
    const std::string CAMERAID                      = "/dev/video0";
    const float MAXIMUMFRAMERATE                    = 30.f;
    const ::arData::Camera::PixelFormat PIXELFORMAT = ::arData::Camera::RGBA32;
    const std::string VIDEOFILE                     = "/tmp/video.mp4";
    const std::string STREAMURL                     = "rtsp://192.168.0.1/h264.sdp";
    const double SCALE                              = 0.9796;

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

    camera->setWidth(WIDTH);
    camera->setHeight(HEIGHT);
    camera->setCameraSource(CAMERASOURCE);
    camera->setMaximumFrameRate(MAXIMUMFRAMERATE);
    camera->setPixelFormat(PIXELFORMAT);
    camera->setVideoFile(VIDEOFILE);
    camera->setStreamUrl(STREAMURL);
    camera->setScale(SCALE);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(CX, camera->getCx(), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(CY, camera->getCy(), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(FX, camera->getFx(), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(FY, camera->getFy(), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(SKEW, camera->getSkew(), 0.00001);

    ::arData::Camera::DistArrayType dist = camera->getDistortionCoefficient();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(K1, dist[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(K2, dist[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(P1, dist[2], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(P2, dist[3], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(K3, dist[4], 0.00001);

    CPPUNIT_ASSERT_EQUAL(IS_CALIBRATED, camera->getIsCalibrated());
    CPPUNIT_ASSERT_EQUAL(DESC, camera->getDescription());
    CPPUNIT_ASSERT_EQUAL(CAM_ID, camera->getCameraID());
    CPPUNIT_ASSERT_EQUAL(WIDTH, camera->getWidth());
    CPPUNIT_ASSERT_EQUAL(HEIGHT, camera->getHeight());
    CPPUNIT_ASSERT_EQUAL(CAMERASOURCE, camera->getCameraSource());
    CPPUNIT_ASSERT_EQUAL(MAXIMUMFRAMERATE, camera->getMaximumFrameRate());
    CPPUNIT_ASSERT_EQUAL(PIXELFORMAT, camera->getPixelFormat());
    CPPUNIT_ASSERT_EQUAL(VIDEOFILE, camera->getVideoFile().string());
    CPPUNIT_ASSERT_EQUAL(STREAMURL, camera->getStreamUrl());
    CPPUNIT_ASSERT_EQUAL(SCALE, camera->getScale());
}

//------------------------------------------------------------------------------

void CameraTest::shallowCopyTest()
{
    ::arData::Camera::sptr camera  = initCamera();
    ::arData::Camera::sptr camera2 = ::arData::Camera::New();

    camera2->shallowCopy(camera);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(camera->getCx(), camera2->getCx(), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(camera->getCy(), camera2->getCy(), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(camera->getFx(), camera2->getFx(), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(camera->getFy(), camera2->getFy(), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(camera->getSkew(), camera2->getSkew(), 0.00001);

    ::arData::Camera::DistArrayType dist  = camera->getDistortionCoefficient();
    ::arData::Camera::DistArrayType dist2 = camera2->getDistortionCoefficient();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(dist[0], dist2[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(dist[1], dist2[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(dist[2], dist2[2], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(dist[3], dist2[3], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(dist[4], dist2[4], 0.00001);

    CPPUNIT_ASSERT_EQUAL(camera->getIsCalibrated(), camera2->getIsCalibrated());
    CPPUNIT_ASSERT_EQUAL(camera->getDescription(), camera2->getDescription());
    CPPUNIT_ASSERT_EQUAL(camera->getCameraID(), camera2->getCameraID());
    CPPUNIT_ASSERT_EQUAL(camera->getWidth(), camera2->getWidth());
    CPPUNIT_ASSERT_EQUAL(camera->getHeight(), camera2->getHeight());
    CPPUNIT_ASSERT_EQUAL(camera->getCameraSource(), camera2->getCameraSource());
    CPPUNIT_ASSERT_EQUAL(camera->getMaximumFrameRate(), camera2->getMaximumFrameRate());
    CPPUNIT_ASSERT_EQUAL(camera->getPixelFormat(), camera2->getPixelFormat());
    CPPUNIT_ASSERT_EQUAL(camera->getVideoFile(), camera2->getVideoFile());
    CPPUNIT_ASSERT_EQUAL(camera->getStreamUrl(), camera2->getStreamUrl());
    CPPUNIT_ASSERT_EQUAL(camera->getScale(), camera2->getScale());
}

//------------------------------------------------------------------------------

void CameraTest::deepCopyTest()
{
    ::arData::Camera::sptr camera = initCamera();
    ::arData::Camera::sptr camera2;
    camera2 = ::fwData::Object::copy(camera);
    CPPUNIT_ASSERT(::fwTest::helper::compare(camera, camera2));
}

} //namespace ut
} //namespace arData
