/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/TransformationMatrix3D.hpp>

#include "TransformationMatrix3DTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( TransformationMatrix3DTest );

void TransformationMatrix3DTest::setUp()
{
	// Set up context before running a test.

}

//------------------------------------------------------------------------------

void TransformationMatrix3DTest::tearDown()
{
	// Clean up after the test run.
}

//------------------------------------------------------------------------------

void TransformationMatrix3DTest::methode1()
{

	double COEFFICIENTS = 1 ;

	// process
	::fwData::TransformationMatrix3D::NewSptr p1;

	// check
	CPPUNIT_ASSERT_EQUAL(p1->getCoefficients().back(),	COEFFICIENTS);
	CPPUNIT_ASSERT_EQUAL(p1->getCRefCoefficients().back(),	COEFFICIENTS);
	CPPUNIT_ASSERT_EQUAL(p1->getRefCoefficients().back(),	COEFFICIENTS);

}

//------------------------------------------------------------------------------

void TransformationMatrix3DTest::methode2()
{
	double COEFFICIENTS = 4 ;
	std::vector<double> VECTORCOEFFICIENTS ;
	VECTORCOEFFICIENTS.push_back( COEFFICIENTS ) ;

	// process
	::fwData::TransformationMatrix3D::NewSptr p1;
	p1->setCRefCoefficients(VECTORCOEFFICIENTS) ;

	// check
	CPPUNIT_ASSERT_EQUAL(p1->getCoefficients().back(),	COEFFICIENTS) ;
	CPPUNIT_ASSERT_EQUAL(p1->getCRefCoefficients().back(),	COEFFICIENTS) ;
	CPPUNIT_ASSERT_EQUAL(p1->getRefCoefficients().back(),	COEFFICIENTS) ;
}
