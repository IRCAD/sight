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

#include <fwData/Patient.hpp>
#include <fwData/Study.hpp>
#include "PatientTest.h"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwData::ut::PatientTest );

namespace fwData
{
namespace ut
{

//------------------------------------------------------------------------------

void PatientTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void PatientTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void PatientTest::methodes1()
{
    // set up
    const std::string NOM       ="NomPatient1" ;
    const std::string FIRSTNAME ="PrenomPatient1" ;
    const std::string IDDICOM   ="IDDICOM" ;
    const std::string BIRTHDATE ="2007-Feb-21 15:00:00";
    const bool SEX              =true;
    const ::boost::int32_t  DBID = 457;
    // process
    ::fwData::Patient::NewSptr p1;
    p1->setCRefName(        NOM);
    p1->setCRefFirstname(   FIRSTNAME);
    p1->setCRefIDDicom(     IDDICOM);
    p1->setCRefBirthdate( ::boost::posix_time::time_from_string(BIRTHDATE));
    p1->setIsMale(  SEX);
    p1->setDbID(    DBID);
    // check
    CPPUNIT_ASSERT_EQUAL(NOM, p1->getName());
    CPPUNIT_ASSERT_EQUAL(NOM, p1->getCRefName());
    CPPUNIT_ASSERT_EQUAL(NOM, p1->getRefName());

    CPPUNIT_ASSERT_EQUAL(FIRSTNAME,        p1->getFirstname());
    CPPUNIT_ASSERT_EQUAL(FIRSTNAME,    p1->getCRefFirstname());
    CPPUNIT_ASSERT_EQUAL(FIRSTNAME,     p1->getRefFirstname());

    CPPUNIT_ASSERT_EQUAL(IDDICOM,      p1->getIDDicom());
    CPPUNIT_ASSERT_EQUAL(IDDICOM,  p1->getCRefIDDicom());
    CPPUNIT_ASSERT_EQUAL(IDDICOM,   p1->getRefIDDicom());

    CPPUNIT_ASSERT_EQUAL(BIRTHDATE ,    ::boost::posix_time::to_simple_string(p1->getBirthdate()));
    CPPUNIT_ASSERT_EQUAL(BIRTHDATE ,::boost::posix_time::to_simple_string(p1->getCRefBirthdate()));
    CPPUNIT_ASSERT_EQUAL(BIRTHDATE , ::boost::posix_time::to_simple_string(p1->getRefBirthdate()));

    CPPUNIT_ASSERT_EQUAL(SEX,       p1->getIsMale());
    CPPUNIT_ASSERT_EQUAL(SEX,   p1->getCRefIsMale());
    CPPUNIT_ASSERT_EQUAL(SEX,    p1->getRefIsMale());

    CPPUNIT_ASSERT_EQUAL(DBID ,     p1->getDbID()) ;
    CPPUNIT_ASSERT_EQUAL(DBID , p1->getCRefDbID()) ;
    CPPUNIT_ASSERT_EQUAL(DBID ,  p1->getRefDbID()) ;
}

//------------------------------------------------------------------------------

void PatientTest::methodes2()
{
    // set up
    const std::string NOM       = "NomPatient1" ;
    const std::string FIRSTNAME = "PrenomPatient1" ;
    const std::string IDDICOM   = "IDDICOM2343" ;
    const std::string BIRTHDATE = "2005-Feb-20 15:00:00";
    const bool SEX              = false;
    const ::boost::int32_t  DBID    = 47;
    // process
    ::fwData::Patient::NewSptr p1;


    p1->setCRefName( NOM );
    p1->setCRefFirstname( FIRSTNAME );
    p1->setCRefIDDicom( IDDICOM );
    p1->setCRefBirthdate(boost::posix_time::time_from_string( BIRTHDATE ));
    //test l'autre setter
    p1->setCRefIsMale( SEX );
    //test l'autre setter
    p1->setCRefDbID( DBID );
    // check
    CPPUNIT_ASSERT_EQUAL(NOM, p1->getName());
    CPPUNIT_ASSERT_EQUAL(NOM, p1->getCRefName());
    CPPUNIT_ASSERT_EQUAL(NOM, p1->getRefName());

    CPPUNIT_ASSERT_EQUAL(FIRSTNAME, p1->getFirstname());
    CPPUNIT_ASSERT_EQUAL(FIRSTNAME, p1->getCRefFirstname());
    CPPUNIT_ASSERT_EQUAL(FIRSTNAME, p1->getRefFirstname());

    CPPUNIT_ASSERT_EQUAL(IDDICOM, p1->getIDDicom());
    CPPUNIT_ASSERT_EQUAL(IDDICOM, p1->getCRefIDDicom());
    CPPUNIT_ASSERT_EQUAL(IDDICOM, p1->getRefIDDicom());

    CPPUNIT_ASSERT_EQUAL(BIRTHDATE , ::boost::posix_time::to_simple_string(p1->getBirthdate()));
    CPPUNIT_ASSERT_EQUAL(BIRTHDATE ,::boost::posix_time::to_simple_string(p1->getCRefBirthdate()));
    CPPUNIT_ASSERT_EQUAL(BIRTHDATE , ::boost::posix_time::to_simple_string(p1->getRefBirthdate()));

    CPPUNIT_ASSERT_EQUAL(SEX, p1->getIsMale());
    CPPUNIT_ASSERT_EQUAL(SEX, p1->getCRefIsMale());
    CPPUNIT_ASSERT_EQUAL(SEX, p1->getRefIsMale());

    CPPUNIT_ASSERT_EQUAL(DBID, p1->getDbID()) ;
    CPPUNIT_ASSERT_EQUAL(DBID, p1->getCRefDbID()) ;
    CPPUNIT_ASSERT_EQUAL(DBID, p1->getRefDbID()) ;
}

//------------------------------------------------------------------------------

void PatientTest::addStudy()
{
    Patient::StudyContainerType::size_type NBSTUDIES = 0 ;
    ::fwData::Patient::NewSptr p1;

    //test : addStudy()
    ::fwData::Study::NewSptr s1;
    CPPUNIT_ASSERT_EQUAL(NBSTUDIES, p1->getNumberOfStudies());
    p1->addStudy(s1) ;
    NBSTUDIES = 1;
    CPPUNIT_ASSERT_EQUAL(NBSTUDIES, p1->getNumberOfStudies());
    //end addStudy

    ::fwData::Patient::sptr p2 = ::fwData::Object::copy(p1);
    CPPUNIT_ASSERT_EQUAL(NBSTUDIES, p2->getNumberOfStudies());
}

} //namespace ut
} //namespace fwData
