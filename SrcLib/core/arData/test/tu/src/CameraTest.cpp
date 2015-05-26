/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/NumericRoundCast.hxx>

#include <fwTest/helper/compare.hpp>

#include <arData/Camera.hpp>

#include <arDataCamp/Version.hpp>

#include <fwCore/spyLog.hpp>

#include "CameraTest.hpp"


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
    ::arData::Camera::sptr camera = ::arData::Camera::New();
    camera->setCx(rand()%10000/100.);
    camera->setCy(rand()%10000/100.);
    camera->setFx(rand()%10000/100.);
    camera->setFy(rand()%10000/100.);
    camera->setSkew(rand()%10000/100.);
    camera->setDistortionCoefficient(rand()%10000/100., rand()%10000/100., rand()%10000/100.,
                                     rand()%10000/100., rand()%10000/100.);

    return camera;
}

//------------------------------------------------------------------------------

void CameraTest::paramTest()
{
    const double CX = rand()%10000/100;
    const double CY = rand()%10000/100;
    const double FX = rand()%10000/100;
    const double FY = rand()%10000/100;
    const double SKEW = rand()%10000/100;
    const double K1 = rand()%10000/100;
    const double K2 = rand()%10000/100;
    const double P1 = rand()%10000/100;
    const double P2 = rand()%10000/100;
    const double K3 = rand()%10000/100;
    const bool IS_CALIBRATED = true;
    const std::string DESC = "My camera";
    const std::string CAM_ID = "CAM_461384568";

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
}

//------------------------------------------------------------------------------

void CameraTest::shallowCopyTest()
{
    ::arData::Camera::sptr camera = initCamera();
    ::arData::Camera::sptr camera2 = ::arData::Camera::New();

    camera2->shallowCopy(camera);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(camera->getCx(), camera2->getCx(), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(camera->getCy(), camera2->getCy(), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(camera->getFx(), camera2->getFx(), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(camera->getFy(), camera2->getFy(), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(camera->getSkew(), camera2->getSkew(), 0.00001);

    ::arData::Camera::DistArrayType dist = camera->getDistortionCoefficient();
    ::arData::Camera::DistArrayType dist2 = camera2->getDistortionCoefficient();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(dist[0], dist2[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(dist[1], dist2[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(dist[2], dist2[2], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(dist[3], dist2[3], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(dist[4], dist2[4], 0.00001);

    CPPUNIT_ASSERT_EQUAL(camera->getIsCalibrated(), camera2->getIsCalibrated());
    CPPUNIT_ASSERT_EQUAL(camera->getDescription(), camera2->getDescription());
    CPPUNIT_ASSERT_EQUAL(camera->getCameraID(), camera2->getCameraID());
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
