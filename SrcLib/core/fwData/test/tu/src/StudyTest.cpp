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

#include "fwData/Study.hpp"
#include "fwData/Patient.hpp"
#include "fwData/Acquisition.hpp"
#include "StudyTest.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwData::ut::StudyTest );

namespace fwData
{
namespace ut
{

void StudyTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void StudyTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void StudyTest::methode1() //test des setters et getters
{
    const std::string HOSPITAL      ="hopital" ;
    const std::string MODALITY ="modality" ;
    const std::string ZONE  ="IDDICOM" ;
    const std::string RISID ="risid569" ;
    const std::string UID   ="UID569" ;
    const boost::int32_t  DBID  =156 ;

    // process
    ::fwData::Study::NewSptr p1;
    p1->setCRefHospital        ( HOSPITAL );
    p1->setCRefModality        ( MODALITY );
    p1->setCRefAcquisitionZone ( ZONE     );
    p1->setCRefRISId           ( RISID    );
    p1->setCRefUID             ( UID      );
    p1->setDbID                ( DBID     );
    // check
    CPPUNIT_ASSERT_EQUAL(p1->getRefHospital()        , HOSPITAL);
    CPPUNIT_ASSERT_EQUAL(p1->getCRefHospital()       , HOSPITAL);
    CPPUNIT_ASSERT_EQUAL(p1->getRefHospital()        , HOSPITAL);

    CPPUNIT_ASSERT_EQUAL(p1->getModality()           , MODALITY);
    CPPUNIT_ASSERT_EQUAL(p1->getCRefModality()       , MODALITY);
    CPPUNIT_ASSERT_EQUAL(p1->getRefModality()        , MODALITY);

    CPPUNIT_ASSERT_EQUAL(p1->getAcquisitionZone()    , ZONE);
    CPPUNIT_ASSERT_EQUAL(p1->getCRefAcquisitionZone(), ZONE);
    CPPUNIT_ASSERT_EQUAL(p1->getRefAcquisitionZone() , ZONE);

    CPPUNIT_ASSERT_EQUAL(p1->getRefRISId()           , RISID);
    CPPUNIT_ASSERT_EQUAL(p1->getCRefRISId()          , RISID);
    CPPUNIT_ASSERT_EQUAL(p1->getRefRISId()           , RISID);

    CPPUNIT_ASSERT_EQUAL(p1->getUID()                , UID);
    CPPUNIT_ASSERT_EQUAL(p1->getCRefUID()            , UID);
    CPPUNIT_ASSERT_EQUAL(p1->getRefUID()             , UID);

    CPPUNIT_ASSERT_EQUAL(p1->getDbID()               , DBID ) ;
    CPPUNIT_ASSERT_EQUAL(p1->getCRefDbID()           , DBID ) ;
    CPPUNIT_ASSERT_EQUAL(p1->getRefDbID()            , DBID ) ;
}

//------------------------------------------------------------------------------

void StudyTest::methode2()
{
    const std::string HOSPITAL ="hopital" ;
    const std::string MODALITY ="modality" ;
    const std::string ZONE  ="IDDICOM" ;
    const std::string RISID ="risid569" ;
    const std::string UID   ="UID569" ;
    const boost::int32_t  DBID  =156 ;

    // process
    ::fwData::Study::NewSptr p1;
    p1->setCRefHospital        ( HOSPITAL );
    p1->setCRefModality        ( MODALITY );
    p1->setCRefAcquisitionZone ( ZONE     );
    p1->setCRefRISId           ( RISID    );
    p1->setCRefUID             ( UID      );
    //test autre setter DbID
    p1->setCRefDbID            ( DBID     );
    // check
    CPPUNIT_ASSERT_EQUAL(p1->getRefHospital()        , HOSPITAL);
    CPPUNIT_ASSERT_EQUAL(p1->getCRefHospital()       , HOSPITAL);
    CPPUNIT_ASSERT_EQUAL(p1->getRefHospital()        , HOSPITAL);

    CPPUNIT_ASSERT_EQUAL(p1->getModality()           , MODALITY);
    CPPUNIT_ASSERT_EQUAL(p1->getCRefModality()       , MODALITY);
    CPPUNIT_ASSERT_EQUAL(p1->getRefModality()        , MODALITY);

    CPPUNIT_ASSERT_EQUAL(p1->getAcquisitionZone()    , ZONE);
    CPPUNIT_ASSERT_EQUAL(p1->getCRefAcquisitionZone(), ZONE);
    CPPUNIT_ASSERT_EQUAL(p1->getRefAcquisitionZone() , ZONE);

    CPPUNIT_ASSERT_EQUAL(p1->getRefRISId()           , RISID);
    CPPUNIT_ASSERT_EQUAL(p1->getCRefRISId()          , RISID);
    CPPUNIT_ASSERT_EQUAL(p1->getRefRISId()           , RISID);

    CPPUNIT_ASSERT_EQUAL(p1->getUID()                , UID);
    CPPUNIT_ASSERT_EQUAL(p1->getCRefUID()            , UID);
    CPPUNIT_ASSERT_EQUAL(p1->getRefUID()             , UID);

    CPPUNIT_ASSERT_EQUAL(p1->getDbID()               , DBID ) ;
    CPPUNIT_ASSERT_EQUAL(p1->getCRefDbID()           , DBID ) ;
    CPPUNIT_ASSERT_EQUAL(p1->getRefDbID()            , DBID ) ;
}

//------------------------------------------------------------------------------

void StudyTest::addAcquisition()
{
    boost::uint32_t  NB = 0 ;

    ::fwData::Study::NewSptr s1;
    ::fwData::Acquisition::NewSptr a1;

    //check
    CPPUNIT_ASSERT_EQUAL(s1->getAcquisitionSize(),  NB );

    s1->addAcquisition(a1) ;
    NB = 1;
    //check
    CPPUNIT_ASSERT_EQUAL(s1->getAcquisitionSize(),  NB);
}

} //namespace ut
} //namespace fwData
