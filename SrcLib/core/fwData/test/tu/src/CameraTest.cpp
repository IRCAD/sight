/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <exception>
#include <vector>
#include <ostream>
#include <map>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "fwData/Camera.hpp"
#include "CameraTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( CameraTest );

void CameraTest::setUp()
{
	// Set up context before running a test.

}
void CameraTest::tearDown()
{
	// Clean up after the test run.
}

void CameraTest::methode1() //test des attributs
{
	//-----------test values
	const double CX = 3.0;
	const double CY = 2.0;
	const double FX = 0.5;
	const double FY = 1.3;

	::fwData::Camera::NewSptr camera;

	camera->setCx(CX);
	camera->setCy(CY);
	camera->setFx(FX);
	camera->setFy(FY);

	CPPUNIT_ASSERT_EQUAL(camera->getCx(),  CX ) ;
	CPPUNIT_ASSERT_EQUAL(camera->getCy(),  CY ) ;
	CPPUNIT_ASSERT_EQUAL(camera->getFx(),  FX ) ;
	CPPUNIT_ASSERT_EQUAL(camera->getFy(),  FY ) ;
}


