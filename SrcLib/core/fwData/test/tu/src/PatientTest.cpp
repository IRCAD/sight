/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <exception>
#include <vector>
#include <ostream>
#include <map>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "fwData/Patient.hpp"
#include "fwData/Study.hpp"
#include "PatientTest.h"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( PatientTest );

//------------------------------------------------------------------------------

void PatientTest::setUp()
{
	// Set up context before running a test.
	//ptCubeDeTest =  new Cube();
}

//------------------------------------------------------------------------------

void PatientTest::tearDown()
{
	// Clean up after the test run.
	//delete ptCubeDeTest;
}

//------------------------------------------------------------------------------

void PatientTest::methodes1()
{

	// set up
	const std::string NOM		="NomPatient1" ;
	const std::string FIRSTNAME ="PrenomPatient1" ;
	const std::string IDDICOM	="IDDICOM" ;
	const std::string BIRTHDATE	="2007-Feb-21 15:00:00";
	const bool SEX				=true;
	const ::boost::int32_t  DBID = '457';
	// process
	::fwData::Patient::NewSptr p1;
	p1->setCRefName(		NOM);
	p1->setCRefFirstname(	FIRSTNAME);
	p1->setCRefIDDicom(		IDDICOM);
	p1->setCRefBirthdate( ::boost::posix_time::time_from_string(BIRTHDATE));
	p1->setIsMale(	SEX);
	p1->setDbID(	DBID);
	// check
	CPPUNIT_ASSERT_EQUAL(p1->getName(), NOM);
	CPPUNIT_ASSERT_EQUAL(p1->getCRefName(), NOM);
	CPPUNIT_ASSERT_EQUAL(p1->getRefName(), NOM);

	CPPUNIT_ASSERT_EQUAL(p1->getFirstname(),		FIRSTNAME);
	CPPUNIT_ASSERT_EQUAL(p1->getCRefFirstname(),	FIRSTNAME);
	CPPUNIT_ASSERT_EQUAL(p1->getRefFirstname(),		FIRSTNAME);

	CPPUNIT_ASSERT_EQUAL(p1->getIDDicom(),		IDDICOM);
	CPPUNIT_ASSERT_EQUAL(p1->getCRefIDDicom(),	IDDICOM);
	CPPUNIT_ASSERT_EQUAL(p1->getRefIDDicom(),	IDDICOM);

	CPPUNIT_ASSERT_EQUAL( ::boost::posix_time::to_simple_string(p1->getBirthdate()),	BIRTHDATE);
	CPPUNIT_ASSERT_EQUAL( ::boost::posix_time::to_simple_string(p1->getCRefBirthdate()),BIRTHDATE);
	CPPUNIT_ASSERT_EQUAL( ::boost::posix_time::to_simple_string(p1->getRefBirthdate()),	BIRTHDATE);

	CPPUNIT_ASSERT_EQUAL(p1->getIsMale(),		SEX);
	CPPUNIT_ASSERT_EQUAL(p1->getCRefIsMale(),	SEX);
	CPPUNIT_ASSERT_EQUAL(p1->getRefIsMale(),	SEX);

	CPPUNIT_ASSERT_EQUAL(p1->getDbID(),		DBID ) ;
	CPPUNIT_ASSERT_EQUAL(p1->getCRefDbID(),	DBID ) ;
	CPPUNIT_ASSERT_EQUAL(p1->getRefDbID(),	DBID ) ;


}

//------------------------------------------------------------------------------

void PatientTest::methodes2()
{
	// set up
	const std::string NOM		= "NomPatient1" ;
	const std::string FIRSTNAME = "PrenomPatient1" ;
	const std::string IDDICOM	= "IDDICOM2343" ;
	const std::string BIRTHDATE	= "2005-Feb-20 15:00:00";
	const bool SEX				= false;
	const ::boost::int32_t  DBID	= 47;
	// process
	::fwData::Patient::NewSptr p1;


	p1->setCRefName(		NOM);
	p1->setCRefFirstname(	FIRSTNAME);
	p1->setCRefIDDicom(		IDDICOM);
	p1->setCRefBirthdate(boost::posix_time::time_from_string(BIRTHDATE));
	//test l'autre setter
	p1->setCRefIsMale( SEX);
	//test l'autre setter
	p1->setCRefDbID( DBID);
	// check
	CPPUNIT_ASSERT_EQUAL(p1->getName(), NOM);
	CPPUNIT_ASSERT_EQUAL(p1->getCRefName(), NOM);
	CPPUNIT_ASSERT_EQUAL(p1->getRefName(), NOM);

	CPPUNIT_ASSERT_EQUAL(p1->getFirstname(),		FIRSTNAME);
	CPPUNIT_ASSERT_EQUAL(p1->getCRefFirstname(),	FIRSTNAME);
	CPPUNIT_ASSERT_EQUAL(p1->getRefFirstname(),		FIRSTNAME);

	CPPUNIT_ASSERT_EQUAL(p1->getIDDicom(),		IDDICOM);
	CPPUNIT_ASSERT_EQUAL(p1->getCRefIDDicom(),	IDDICOM);
	CPPUNIT_ASSERT_EQUAL(p1->getRefIDDicom(),	IDDICOM);

	CPPUNIT_ASSERT_EQUAL( ::boost::posix_time::to_simple_string(p1->getBirthdate()),	BIRTHDATE);
	CPPUNIT_ASSERT_EQUAL( ::boost::posix_time::to_simple_string(p1->getCRefBirthdate()),BIRTHDATE);
	CPPUNIT_ASSERT_EQUAL( ::boost::posix_time::to_simple_string(p1->getRefBirthdate()),	BIRTHDATE);

	CPPUNIT_ASSERT_EQUAL(p1->getIsMale(),		SEX);
	CPPUNIT_ASSERT_EQUAL(p1->getCRefIsMale(),		SEX);
	CPPUNIT_ASSERT_EQUAL(p1->getRefIsMale(),		SEX);

	CPPUNIT_ASSERT_EQUAL(p1->getDbID(),			DBID ) ;
	CPPUNIT_ASSERT_EQUAL(p1->getCRefDbID(),			DBID ) ;
	CPPUNIT_ASSERT_EQUAL(p1->getRefDbID(),			DBID ) ;
}

//------------------------------------------------------------------------------

void PatientTest::addStudy()
{
	::boost::uint32_t  NBSTUDIES = 0 ;
	::fwData::Patient::NewSptr p1;

	//test : addStudy()
	::fwData::Study::NewSptr s1;
	CPPUNIT_ASSERT_EQUAL(p1->getStudySize(),  NBSTUDIES );
	p1->addStudy(s1) ;
	NBSTUDIES = 1;
	CPPUNIT_ASSERT_EQUAL(p1->getStudySize(),  NBSTUDIES );
	//end addStudy
}
